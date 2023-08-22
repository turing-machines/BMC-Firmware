################################################################################
#
# Rockusb driver
#
################################################################################

ROCKUSB_LICENSE = GPL-2.0

ROCKUSB_MODULE_MAKE_OPTS = \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR) \
	USER_EXTRA_CFLAGS="-Wno-error"
 
define ROCKUSB_EXTRACT_CMDS
	cp $(ROCKUSB_PKGDIR)/{Makefile,rockusb.c} $(@D)
endef
 
$(eval $(kernel-module))
$(eval $(generic-package))
