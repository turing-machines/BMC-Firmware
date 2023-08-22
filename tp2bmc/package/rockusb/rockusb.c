// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver to access the flash storage of a Rockchip device in USB download mode
 * as a block device.
 *
 * Primarily intended for imaging the Turing Machines RK1's eMMC, but should be
 * useful enough in similar contexts as well.
 *
 * Rockchip's "Rockusb" protocol is just USB Mass Storage, but with an entirely
 * different command set at the SCSI CDB level. This driver implements a
 * translation layer that maps standard SCSI commands into Rockusb commands.
 *
 * (c) 2023 Sam Edwards <CFSworks@gmail.com>
 */

#include <linux/module.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>

#include "usb.h"
#include "transport.h"
#include "protocol.h"
#include "debug.h"
#include "scsiglue.h"

#define DRV_NAME "rockusb"

MODULE_DESCRIPTION("Driver for block access to Rockchip devices in USB mode");
MODULE_AUTHOR("Sam Edwards <CFSworks@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_IMPORT_NS(USB_STORAGE);

struct flash_info {
	__u32 flash_size;
	__u16 block_size;
	__u8  page_size;
	__u8  ecc_bits;
	__u8  access_time;
	__u8  manuf_code;
	__u8  flash_cs;
} __attribute__((__packed__));

#define OP_READ_LBA		0x14
#define OP_WRITE_LBA		0x15
#define OP_READ_FLASH_INFO	0x1A

static void rockusb_command(struct scsi_cmnd *srb, struct us_data *us)
{
	scsi_set_resid(srb, 0);

	switch (srb->cmnd[0]) {
	case INQUIRY:
	{
		static unsigned char inquiry_response[36] = {
			0x00, 0x80, 0x00, 0x01, 0x1F, 0x00, 0x00, 0x00
		};
		memcpy(us->iobuf, inquiry_response, sizeof(inquiry_response));
		fill_inquiry_response(us, us->iobuf, 36);
		break;
	}

	case READ_CAPACITY:
	{
		unsigned long capacity;
		struct flash_info flinfo;
		struct scsi_data_buffer sdb;
		struct scatterlist sgl;

		/* Rewrite into a READ FLASH INFO command */
		srb->cmnd[0] = OP_READ_FLASH_INFO;
		srb->cmd_len = 0x06;

		/* Temporarily replace data pointer with &flinfo */
		sdb = srb->sdb;
		memset(&srb->sdb, 0, sizeof(srb->sdb));
		sg_init_one(&sgl, &flinfo, sizeof(flinfo));
		srb->sdb.table.sgl = &sgl;
		srb->sdb.table.nents = srb->sdb.table.orig_nents = 1;
		srb->sdb.length = sizeof(flinfo);

		if (usb_stor_Bulk_transport(srb, us) != USB_STOR_TRANSPORT_GOOD) {
			srb->sdb = sdb;
			srb->result = DID_ERROR << 16;
			break;
		}

		srb->sdb = sdb;

		capacity = le32_to_cpu(flinfo.flash_size);

		((__be32 *) us->iobuf)[0] = cpu_to_be32(capacity - 1);
		((__be32 *) us->iobuf)[1] = cpu_to_be32(srb->device->sector_size);

		usb_stor_set_xfer_buf(us->iobuf, 8, srb);
		srb->result = SAM_STAT_GOOD;
		break;
	}

	/* Cases that require no/little fixup */
	case TEST_UNIT_READY:
	case READ_10:
	case WRITE_10:
	{
		/* Translate opcode */
		if (srb->cmnd[0] == READ_10)
			srb->cmnd[0] = OP_READ_LBA;
		else if (srb->cmnd[0] == WRITE_10)
			srb->cmnd[0] = OP_WRITE_LBA;

		if (usb_stor_Bulk_transport(srb, us) != USB_STOR_TRANSPORT_GOOD) {
			srb->result = DID_ERROR << 16;
			break;
		}

		srb->result = SAM_STAT_GOOD;
		break;
	}

	/* Nothing we can really do with these, so let them succeed silently */
	case MODE_SENSE:
	{
		static unsigned char senseData[8] = {0};

		usb_stor_set_xfer_buf(senseData, sizeof(senseData), srb);

		srb->result = SAM_STAT_GOOD;
		break;
	}

	case ALLOW_MEDIUM_REMOVAL:
		srb->result = SAM_STAT_GOOD;
		break;
	
	default:
		usb_stor_dbg(us, "Unknown SCSI command: %d (0x%02x)\n",
			     srb->cmnd[0], srb->cmnd[0]);
		srb->result = DID_ERROR << 16;
		break;
	}
}

static struct scsi_host_template rockusb_host_template;
static struct us_unusual_dev rockusb_unusual_dev = {
	.vendorName = "Rockchip",
	.productName = "Rockusb device",
	.useProtocol = USB_SC_DEVICE,
	.useTransport = USB_PR_BULK,
};

static int rockusb_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	struct us_data *us;
	int result;
	struct usb_device *udev = interface_to_usbdev(intf);

	/*
	 * If the device is in "maskrom" mode, it won't understand Rockusb, and
	 * we don't want to bind to it. Detecting this is tricky, because the
	 * USB descriptor used by the maskrom is extremely similar to the one
	 * used by Rockchip's "usbplug" stub. The only difference is that the
	 * maskrom descriptor has no device strings, while the "usbplug" stub
	 * (and U-Boot bootloaders in Rockusb mode) populate at least one of
	 * them.
	 */
	if ((!udev->product) && (!udev->manufacturer) && (!udev->serial))
		return -ENODEV;

	result = usb_stor_probe1(&us, intf, id,
				 &rockusb_unusual_dev,
				 &rockusb_host_template);
	if (result)
		return result;

	us->protocol_name = "Rockusb";
	us->proto_handler = rockusb_command;

	return usb_stor_probe2(us);
}

static struct usb_device_id rockusb_usb_ids[] = {
	{
		USB_VENDOR_AND_INTERFACE_INFO(0x2207, 0xff, 0x06, 0x05),
		.driver_info = US_FL_SINGLE_LUN,
	},
	{ }
};

static struct usb_driver rockusb_driver = {
	.name =		DRV_NAME,
	.probe =	rockusb_probe,
	.disconnect =	usb_stor_disconnect,
	.suspend =	usb_stor_suspend,
	.resume =	usb_stor_resume,
	.reset_resume =	usb_stor_reset_resume,
	.pre_reset =	usb_stor_pre_reset,
	.post_reset =	usb_stor_post_reset,
	.id_table =	rockusb_usb_ids,
	.soft_unbind =	1,
	.no_dynamic_id = 1,
};

module_usb_stor_driver(rockusb_driver, rockusb_host_template, DRV_NAME);
