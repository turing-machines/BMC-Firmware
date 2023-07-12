###########################################################
#
# tpi_rs
#
###########################################################

TPI_RS_VERSION = 1.2.0
TPI_RS_SITE = $(call github,turing-machines,tpi_rs,v$(TPI_RS_VERSION))
TPI_RS_LICENSE = Apache-2.0
TPI_RS_LICENSE_FILES = LICENSE
TPI_RS_INSTALL_STAGING = YES
TPI_RS_INSTALL_TARGET = NO
TPI_RS_CARGO_ENV := PKG_CONFIG_ALLOW_CROSS=1 # for `libusb1-sys`
TPI_RS_CARGO_ENV += CC_armv7_unknown_linux_gnueabi="arm-linux-gcc" # for `ring`
TPI_RS_DEPENDENCIES = libusb

# This build step is the default build command (see pkg-cargo.mk), but with TARGET_CONFIGURE_OPTS
# variable removed, as providing environment variable CC="…/arm-buildroot-linux-gnueabi-gcc" causes
# compilation to fail.
define TPI_RS_BUILD_CMDS
	cd $(TPI_RS_SRCDIR) && \
	$(TARGET_MAKE_ENV) \
		$(PKG_CARGO_ENV) \
		$(TPI_RS_CARGO_ENV) \
		cargo build \
			--offline \
			$(if $(BR2_ENABLE_DEBUG),,--release) \
			--manifest-path Cargo.toml \
			--locked \
			$(TPI_RS_CARGO_BUILD_OPTS)
endef

define TPI_RS_INSTALL_STAGING_CMDS
 $(INSTALL) -D -m 0644 $(@D)/target/$(RUSTC_TARGET_NAME)/$(if $(BR2_ENABLE_DEBUG),debug,release)/libtpi_rs.a $(STAGING_DIR)/lib
 $(INSTALL) -D -m 0644 $(@D)/tpi_rs.h $(STAGING_DIR)/usr/include
endef

$(eval $(cargo-package))
