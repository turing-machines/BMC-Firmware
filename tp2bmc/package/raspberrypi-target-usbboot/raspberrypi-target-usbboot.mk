################################################################################
#
# raspberrypi-target-usbboot
#
################################################################################

RASPBERRYPI_TARGET_USBBOOT_VERSION = 2021.07.01
RASPBERRYPI_TARGET_USBBOOT_SITE = \
	$(call github,raspberrypi,usbboot,v$(RASPBERRYPI_TARGET_USBBOOT_VERSION))
RASPBERRYPI_TARGET_USBBOOT_LICENSE = Apache-2.0
RASPBERRYPI_TARGET_USBBOOT_LICENSE_FILES = LICENSE

RASPBERRYPI_TARGET_USBBOOT_DEPENDENCIES = libusb

define RASPBERRYPI_TARGET_USBBOOT_BUILD_CMDS
	# bin2c should be compiled on the host first in order to let it process blob files
	$(HOST_MAKE_ENV) $(MAKE) $(HOST_CONFIGURE_OPTS) -C $(@D) bin2c
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define RASPBERRYPI_TARGET_USBBOOT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rpiboot $(TARGET_DIR)/bin/rpiboot
endef

$(eval $(generic-package))
$(eval $(host-generic-package))
