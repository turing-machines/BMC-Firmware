###########################################################
#
# tpi_rs
#
###########################################################

TPI_RS_VERSION = 0.0.0
TPI_RS_SITE = $(TOPDIR)/../tp2bmc/package/tpi_rs
TPI_RS_SITE_METHOD = local
TPI_RS_INSTALL_STAGING = YES
TPI_RS_INSTALL_TARGET = NO

# This build step is derived from the default build cmd. Removed the
# HOST_CONFIGURE_OPTS and $HOST_PKG_CARGO_ENV environment as some of the
# compiler flags where causing cross compilation to fail. Secondly, Removed the
# --offline requirement, having this package referenced locally causes some
#  issues downloading all cargo dependencies as well
define TPI_RS_BUILD_CMDS
	cd $(TPI_RS_SRCDIR) && \
	$(TARGET_MAKE_ENV) \
		$$(PKG_CARGO_ENV) \
		cargo build \
			$(if $(BR2_ENABLE_DEBUG),,--release) \
			--manifest-path Cargo.toml \
			--target $(RUSTC_TARGET_NAME)
			--locked
endef

define TPI_RS_INSTALL_STAGING_CMDS
 $(INSTALL) -D -m 0644 $(@D)/target/$(RUSTC_TARGET_NAME)/release/libtpi_rs.a $(STAGING_DIR)/lib
 $(INSTALL) -D -m 0644 $(@D)/tpi_rs.h $(STAGING_DIR)/usr/include
endef


$(eval $(cargo-package))
