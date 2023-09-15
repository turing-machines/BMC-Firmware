###########################################################
#
# bmcd
###########################################################

BMCD_VERSION = d8a002a096954c3193beb72e50e52730342312f2
BMCD_SITE = $(call github,turing-machines,bmcd,$(BMCD_VERSION))
BMCD_LICENSE = Apache-2.0
BMCD_LICENSE_FILES = LICENSE
BMCD_DEPENDENCIES += libopenssl
BMCD_CARGO_ENV := PKG_CONFIG_ALLOW_CROSS=1
BMCD_CARGO_ENV += CC_armv7_unknown_linux_gnueabi="arm-linux-gcc"

# A copy of default build commands but with --path amended, since we have a virtual manifest.
define BMCD_INSTALL_TARGET_CMDS
	cd $(BMCD_SRCDIR) && \
	$(TARGET_MAKE_ENV) \
		$(PKG_CARGO_ENV) \
		$(BMCD_CARGO_ENV) \
		cargo install \
			--offline \
			--root $(TARGET_DIR)/usr/ \
			--bins \
			--path bmcd \
			--force \
			--locked \
			-Z target-applies-to-host \
			$(BMCD_CARGO_INSTALL_OPTS)

	$(INSTALL) -D -m 744 $(BR2_EXTERNAL_TP2BMC_PATH)/package/bmcd/generate_self_signedx509.sh \
		$(TARGET_DIR)/etc/bmcd/generate_self_signedx509.sh

	$(INSTALL) -D -m 755 $(BR2_EXTERNAL_TP2BMC_PATH)/package/bmcd/default_config.yaml \
		$(TARGET_DIR)/etc/bmcd/config.yaml
endef

define BMCD_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(BR2_EXTERNAL_TP2BMC_PATH)/package/bmcd/S94bmcd \
		$(TARGET_DIR)/etc/init.d/S94bmcd
endef

$(eval $(cargo-package))
