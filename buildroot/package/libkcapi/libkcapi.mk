################################################################################
#
# libkcapi
#
################################################################################

LIBKCAPI_VERSION = 1.4.0
LIBKCAPI_SOURCE = libkcapi-$(LIBKCAPI_VERSION).tar.xz
LIBKCAPI_SITE = http://www.chronox.de/libkcapi
LIBKCAPI_AUTORECONF = YES
LIBKCAPI_INSTALL_STAGING = YES
LIBKCAPI_LICENSE = BSD-3-Clause (library), BSD-3-Clause or GPL-2.0 (programs)
LIBKCAPI_LICENSE_FILES = COPYING COPYING.gplv2 COPYING.bsd
LIBKCAPI_CONF_ENV = \
	ac_cv_path_DB2PDF="" \
	ac_cv_path_DB2PS="" \
	ac_cv_path_XMLTO=""
LIBKCAPI_CONF_OPTS = --disable-werror
LIBKCAPI_MAKE_OPTS = CPPFLAGS="$(TARGET_CPPFLAGS)"

ifeq ($(BR2_PACKAGE_LIBKCAPI_ASYM),y)
LIBKCAPI_CONF_OPTS += --enable-lib-asym
else
LIBKCAPI_CONF_OPTS += --disable-lib-asym
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_DGSTAPP),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-dgstapp
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-dgstapp
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_ENCAPP),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-encapp
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-encapp
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_HASHER),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-hasher
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-hasher
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_KPP),y)
LIBKCAPI_CONF_OPTS += --enable-lib-kpp
else
LIBKCAPI_CONF_OPTS += --disable-lib-kpp
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_RNGAPP),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-rngapp
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-rngapp
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_SPEED),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-speed
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-speed
endif

ifeq ($(BR2_PACKAGE_LIBKCAPI_TEST),y)
LIBKCAPI_CONF_OPTS += --enable-kcapi-test
else
LIBKCAPI_CONF_OPTS += --disable-kcapi-test
endif

$(eval $(autotools-package))
