################################################################################
#
# fmlib
#
################################################################################

FMLIB_VERSION = LSDK-21.08
FMLIB_SITE = $(call github,nxp-qoriq,fmlib,$(FMLIB_VERSION))
FMLIB_LICENSE = BSD-3-Clause
FMLIB_LICENSE_FILES = COPYING
FMLIB_DEPENDENCIES = linux
FMLIB_INSTALL_STAGING = YES

# This package installs a static library only, so there's
# nothing to install to the target
FMLIB_INSTALL_TARGET = NO

FMLIB_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	KERNEL_SRC="$(LINUX_DIR)" \
	PREFIX="$(STAGING_DIR)/usr"

FMLIB_ARCHTYPE = $(call qstrip,$(BR2_PACKAGE_FMLIB_ARCHTYPE))
FMLIB_PLATFORM = $(call qstrip,$(BR2_PACKAGE_FMLIB_PLATFORM))

define FMLIB_BUILD_CMDS
	$(SED) "s:P4080:$(FMLIB_PLATFORM):g" $(@D)/Makefile
	$(TARGET_MAKE_ENV) $(MAKE) $(FMLIB_MAKE_OPTS) -C $(@D) libfm-$(FMLIB_ARCHTYPE).a
endef

define FMLIB_INSTALL_STAGING_CMDS
	$(RM) $(STAGING_DIR)/usr/lib/libfm.a
	$(TARGET_MAKE_ENV) $(MAKE) $(FMLIB_MAKE_OPTS) -C $(@D) install-libfm-$(FMLIB_ARCHTYPE)
endef

$(eval $(generic-package))
