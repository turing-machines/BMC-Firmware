################################################################################
#
# sun8iw20p1-spl
#
################################################################################

SUN8IW20P1_SPL_VERSION = 771192d0b3737798d7feca87263c8fa74a449787
SUN8IW20P1_SPL_SITE = $(call github,smaeul,sun20i_d1_spl,$(SUN8IW20P1_SPL_VERSION))
SUN8IW20P1_SPL_INSTALL_TARGET = NO
SUN8IW20P1_SPL_INSTALL_IMAGES = YES
SUN8IW20P1_SPL_LICENSE = GPL-2.0+

define SUN8IW20P1_SPL_BUILD_CMDS
	$(MAKE) -C $(@D) CROSS_COMPILE="$(TARGET_CROSS)" p=sun20iw1p1 mmc
endef

define SUN8IW20P1_SPL_INSTALL_IMAGES_CMDS
	$(INSTALL) -D -m 0644 $(@D)/nboot/boot0_sdcard_sun20iw1p1.bin \
		$(BINARIES_DIR)/boot0_sdcard_sun20iw1p1.bin
endef

$(eval $(generic-package))
