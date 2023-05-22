################################################################################
#
# sudo
#
################################################################################

SUDO_VERSION_MAJOR = 1.9.13
SUDO_VERSION_MINOR = p3
SUDO_VERSION = $(SUDO_VERSION_MAJOR)$(SUDO_VERSION_MINOR)
SUDO_SITE = https://www.sudo.ws/sudo/dist
SUDO_LICENSE = ISC, BSD-3-Clause
SUDO_LICENSE_FILES = LICENSE.md
SUDO_CPE_ID_VERSION = $(SUDO_VERSION_MAJOR)
SUDO_CPE_ID_UPDATE = $(SUDO_VERSION_MINOR)
SUDO_SELINUX_MODULES = sudo
# We're patching m4/openssl.m4
SUDO_AUTORECONF = YES
# This is to avoid sudo's make install from chown()ing files which fails
SUDO_INSTALL_TARGET_OPTS = INSTALL_OWNER="" DESTDIR="$(TARGET_DIR)" install
SUDO_CONF_OPTS = \
	--with-tzdir=$(if $(BR2_PACKAGE_TZDATA),/usr/share/zoneinfo,no) \
	--enable-tmpfiles.d=$(if $(BR2_PACKAGE_SYSTEMD),/usr/lib/tmpfiles.d,no) \
	--without-lecture \
	--without-sendmail \
	--without-umask \
	--with-logging=syslog \
	--without-interfaces \
	--with-env-editor

ifeq ($(BR2_PACKAGE_LINUX_PAM),y)
define SUDO_INSTALL_PAM_CONF
	$(INSTALL) -D -m 0644 package/sudo/sudo.pam $(TARGET_DIR)/etc/pam.d/sudo
endef

SUDO_DEPENDENCIES += linux-pam
SUDO_CONF_OPTS += --with-pam
SUDO_POST_INSTALL_TARGET_HOOKS += SUDO_INSTALL_PAM_CONF
else
SUDO_CONF_OPTS += --without-pam
endif

ifeq ($(BR2_PACKAGE_ZLIB),y)
SUDO_CONF_OPTS += --enable-zlib
SUDO_DEPENDENCIES += zlib
else
SUDO_CONF_OPTS += --disable-zlib
endif

ifeq ($(BR2_PACKAGE_OPENLDAP),y)
SUDO_DEPENDENCIES += openldap
SUDO_CONF_OPTS += --with-ldap
else
SUDO_CONF_OPTS += --without-ldap
endif

ifeq ($(BR2_PACKAGE_OPENSSL),y)
SUDO_DEPENDENCIES += host-pkgconf openssl
SUDO_CONF_OPTS += --enable-openssl
else
SUDO_CONF_OPTS += --disable-openssl
endif

# mksigname/mksiglist needs to run on build host to generate source files
define SUDO_BUILD_MKSIGNAME_MKSIGLIST_HOST
	$(MAKE) $(HOST_CONFIGURE_OPTS) \
		CPPFLAGS="$(HOST_CPPFLAGS) -I../../include -I../.." \
		-C $(@D)/lib/util mksigname mksiglist
endef

SUDO_POST_CONFIGURE_HOOKS += SUDO_BUILD_MKSIGNAME_MKSIGLIST_HOST

define SUDO_PERMISSIONS
	/usr/bin/sudo f 4755 0 0 - - - - -
endef

define SUDO_REMOVE_DIST_EXAMPLES
	$(RM) $(TARGET_DIR)/etc/sudoers.dist
	rmdir --ignore-fail-on-non-empty $(TARGET_DIR)/etc/sudoers.d
endef
SUDO_POST_INSTALL_TARGET_HOOKS += SUDO_REMOVE_DIST_EXAMPLES

define SUDO_USERS
	- - sudo -1 - - - -
endef

define SUDO_ENABLE_SUDO_GROUP_RULE
	$(SED) '/^# \%sudo\tALL=(ALL:ALL) ALL/s/^# //' $(TARGET_DIR)/etc/sudoers
endef
SUDO_POST_INSTALL_TARGET_HOOKS += SUDO_ENABLE_SUDO_GROUP_RULE

$(eval $(autotools-package))
