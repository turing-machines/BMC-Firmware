###########################################################
#
# bmcd
#
###########################################################

BMCD_VERSION = bb3636350509b09fcb4b4637b8fd669719206d5a
BMCD_SITE = $(call github,turing-machines,bmcd,$(BMCD_VERSION))
BMCD_LICENSE = Apache-2.0
BMCD_LICENSE_FILES = LICENSE
BMCD_CARGO_ENV := PKG_CONFIG_ALLOW_CROSS=1
BMCD_CARGO_ENV += CC_armv7_unknown_linux_gnueabi="arm-linux-gcc"

define BMCD_BUILD_CMDS
	cd $(BMCD_SRCDIR) && \
	$(TARGET_MAKE_ENV) \
		$(PKG_CARGO_ENV) \
		$(BMCD_CARGO_ENV) \
		cargo build \
			--offline \
			$(if $(BR2_ENABLE_DEBUG),,--release) \
			--manifest-path Cargo.toml \
			--locked \
			$(BMCD_CARGO_BUILD_OPTS)
endef

# A copy of default build commands but with --path amended, since we have a virtual manifest.
# For the same reasons as in tpi_rs.mk, TARGET_CONFIGURE_OPTS are removed.
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
endef

define BMCD_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(BR2_EXTERNAL_TP2BMC_PATH)/package/bmcd/S94bmcd \
		$(TARGET_DIR)/etc/init.d/S94bmcd
endef

$(eval $(cargo-package))
