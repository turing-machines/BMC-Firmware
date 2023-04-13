################################################################################
#
# raspberrypi-usbboot
#
################################################################################

RASPBERRYPI_USBBOOT_VERSION = 2021.07.01
RASPBERRYPI_USBBOOT_SITE = \
	$(call github,raspberrypi,usbboot,v$(RASPBERRYPI_USBBOOT_VERSION))
RASPBERRYPI_USBBOOT_LICENSE = Apache-2.0
RASPBERRYPI_USBBOOT_LICENSE_FILES = LICENSE
RASPBERRYPI_USBBOOT_INSTALL_STAGING = YES
RASPBERRYPI_USBBOOT_INSTALL_TARGET = YES

RASPBERRYPI_USBBOOT_DEPENDENCIES = libusb

HOST_RASPBERRYPI_USBBOOT_DEPENDENCIES = host-libusb

define RASPBERRYPI_USBBOOT_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" -C $(@D) rpiboot
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" -C $(@D) librpiboot.a
endef

define HOST_RASPBERRYPI_USBBOOT_BUILD_CMDS
	$(HOST_MAKE_ENV) $(MAKE) $(HOST_CONFIGURE_OPTS) -C $(@D)
endef

define RASPBERRYPI_USBBOOT_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/librpiboot.a $(STAGING_DIR)/usr/lib/librpiboot.a
endef

define RASPBERRYPI_USBBOOT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rpiboot $(TARGET_DIR)/usr/bin/rpiboot
endef

define HOST_RASPBERRYPI_USBBOOT_INSTALL_CMDS
	$(INSTALL) -D -m 0755 $(@D)/rpiboot $(HOST_DIR)/bin/rpiboot
endef

$(eval $(generic-package))
$(eval $(host-generic-package))
