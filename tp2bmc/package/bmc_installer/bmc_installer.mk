###########################################################
#
# bmc_installer
#
###########################################################

BMC_INSTALLER_VERSION = eef33d0f72728831650ab4d04b5225993f002b31
BMC_INSTALLER_SITE = $(call github,turing-machines,BMC-Installer,$(BMC_INSTALLER_VERSION))
BMC_INSTALLER_LICENSE = Apache-2.0
BMC_INSTALLER_LICENSE_FILES = LICENSE
BMC_INSTALLER_INSTALL_STAGING = YES
BMC_INSTALLER_INSTALL_TARGET = NO
BMC_INSTALLER_CARGO_BUILD_OPTS = --bin=sdcard --bin=sdcard_userspace

define BMC_INSTALLER_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/target/$(RUSTC_TARGET_NAME)/$(if $(BR2_ENABLE_DEBUG),debug,release)/sdcard $(STAGING_DIR)/initramfs/install/init
	$(INSTALL) -D -m 0755 $(@D)/target/$(RUSTC_TARGET_NAME)/$(if $(BR2_ENABLE_DEBUG),debug,release)/sdcard_userspace $(BINARIES_DIR)/factory_overlay/upper/factory/install_firmware
endef

$(eval $(cargo-package))
