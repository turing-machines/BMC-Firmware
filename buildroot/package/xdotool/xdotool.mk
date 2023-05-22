################################################################################
#
# xdotool
#
################################################################################

XDOTOOL_VERSION = 3.20211022.1
XDOTOOL_SITE = https://github.com/jordansissel/xdotool/releases/download/v$(XDOTOOL_VERSION)
XDOTOOL_LICENSE = BSD-3-Clause
XDOTOOL_LICENSE_FILES = COPYRIGHT
XDOTOOL_DEPENDENCIES = xlib_libXtst xlib_libXinerama libxkbcommon xlib_libX11

define XDOTOOL_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef

# Avoid 'install' target to skip 'post-install' which runs ldconfig on host
define XDOTOOL_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D) \
		pre-install installlib installprog installheader \
		PREFIX="$(TARGET_DIR)/usr"
endef

$(eval $(generic-package))
