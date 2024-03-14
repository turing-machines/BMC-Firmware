################################################################################
#
# ifupdown-ng
#
################################################################################

IFUPDOWN_NG_VERSION = 0.12.1
IFUPDOWN_NG_SITE = $(call github,ifupdown-ng,ifupdown-ng,$(IFUPDOWN_NG_VERSION))
IFUPDOWN_NG_LICENSE = GPL-2.0+
IFUPDOWN_NG_LICENSE_FILES = COPYING

# Makefile-allow-passing-custom-CFLAGS-CPPFLAGS.patch

ifeq ($(BR2_PACKAGE_LIBBSD),y)
IFUPDOWN_NG_DEPENDENCIES += libbsd host-pkgconf
define IFUPDOWN_NG_EXTRA_FLAGS
	LIBBSD_CFLAGS="$(shell $(HOST_DIR)/bin/pkg-config --cflags libbsd-overlay)" \
	LIBBSD_LIBS="$(shell $(HOST_DIR)/bin/pkg-config --cflags --libs libbsd-overlay)"
endef
endif

define IFUPDOWN_NG_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) \
		$(IFUPDOWN_NG_EXTRA_FLAGS) \
		-C $(@D)
endef

define IFUPDOWN_NG_INSTALL_TARGET_CMDS
	$(RM) $(TARGET_DIR)/sbin/{ifctrstat,ifdown,ifparse,ifquery,ifup}
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) install \
		DESTDIR=$(TARGET_DIR)
endef

$(eval $(generic-package))
