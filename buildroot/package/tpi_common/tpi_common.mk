###########################################################
#
# tpi_common
#
###########################################################

TPI_COMMON_VERSION = 0.0.0
TPI_COMMON_SITE = $(TOPDIR)/../app/tpi_common
TPI_COMMON_SITE_METHOD = local
TPI_COMMON_INSTALL_STAGING = YES
TPI_COMMON_INSTALL_TARGET = NO

define TPI_COMMON_INSTALL_STAGING_CMDS
 $(INSTALL) -D -m 0644 $(@D)/target/$(RUSTC_TARGET_NAME)/release/libtpi_common.a $(STAGING_DIR)/lib
 $(INSTALL) -D -m 0644 $(@D)/tpi_common.h $(STAGING_DIR)/usr/include
endef

$(eval $(cargo-package))
