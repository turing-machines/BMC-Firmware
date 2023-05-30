###########################################################
#
# tpi_rs
#
###########################################################

TPI_RS_VERSION = 0.0.0
TPI_RS_SITE = $(TOPDIR)/../tp2bmc/package/tpi_rs
TPI_RS_SITE_METHOD = local
TPI_RS_LICENSE = Apache-2.0
TPI_RS_LICENSE_FILES = LICENSE
TPI_RS_INSTALL_STAGING = YES
TPI_RS_INSTALL_TARGET = NO
TPI_RS_CARGO_ENV = PKG_CONFIG_ALLOW_CROSS=1
TPI_RS_DEPENDENCIES = libusb

# This build step is derived from the default build cmd. Removed the TARGET_CONFIGURE_OPTS variable
# as some of the compiler flags where causing cross compilation to fail. Secondly, Removed the
# --offline requirement, having this package referenced locally causes some issues downloading all
# cargo dependencies as well
define TPI_RS_BUILD_CMDS
	cd $(TPI_RS_SRCDIR) && \
	$(TARGET_MAKE_ENV) \
		$(PKG_CARGO_ENV) \
		$(TPI_RS_CARGO_ENV) \
		env CC_armv7-unknown-linux-gnueabi="arm-linux-gcc" \
		cargo build \
			$(if $(BR2_ENABLE_DEBUG),,--release) \
			--manifest-path Cargo.toml
endef

define TPI_RS_INSTALL_STAGING_CMDS
 $(INSTALL) -D -m 0644 $(@D)/target/$(RUSTC_TARGET_NAME)/$(if $(BR2_ENABLE_DEBUG),debug,release)/libtpi_rs.a $(STAGING_DIR)/lib
 $(INSTALL) -D -m 0644 $(@D)/tpi_rs.h $(STAGING_DIR)/usr/include
endef

$(eval $(cargo-package))
