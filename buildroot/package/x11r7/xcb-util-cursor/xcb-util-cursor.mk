################################################################################
#
# xcb-util-cursor
#
################################################################################

XCB_UTIL_CURSOR_VERSION = 0.1.4
XCB_UTIL_CURSOR_SITE = https://xorg.freedesktop.org/archive/individual/lib
XCB_UTIL_CURSOR_SOURCE = xcb-util-cursor-$(XCB_UTIL_CURSOR_VERSION).tar.xz
XCB_UTIL_CURSOR_LICENSE = MIT
XCB_UTIL_CURSOR_LICENSE_FILES = COPYING
XCB_UTIL_CURSOR_INSTALL_STAGING = YES
XCB_UTIL_CURSOR_DEPENDENCIES = xcb-util xcb-util-image xcb-util-renderutil

$(eval $(autotools-package))
