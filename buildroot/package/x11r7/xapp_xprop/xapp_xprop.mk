################################################################################
#
# xapp_xprop
#
################################################################################

XAPP_XPROP_VERSION = 1.2.6
XAPP_XPROP_SOURCE = xprop-$(XAPP_XPROP_VERSION).tar.xz
XAPP_XPROP_SITE = https://xorg.freedesktop.org/archive/individual/app
XAPP_XPROP_LICENSE = MIT
XAPP_XPROP_LICENSE_FILES = COPYING
XAPP_XPROP_DEPENDENCIES = xlib_libX11 xlib_libXmu

$(eval $(autotools-package))
