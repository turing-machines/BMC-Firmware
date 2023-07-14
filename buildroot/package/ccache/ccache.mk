################################################################################
#
# ccache
#
################################################################################

CCACHE_VERSION = 4.7.4
CCACHE_SITE = https://github.com/ccache/ccache/releases/download/v$(CCACHE_VERSION)
CCACHE_SOURCE = ccache-$(CCACHE_VERSION).tar.xz
CCACHE_LICENSE = GPL-3.0+, others
CCACHE_LICENSE_FILES = LICENSE.adoc GPL-3.0.txt
HOST_CCACHE_DEPENDENCIES = host-hiredis host-zstd

# We are ccache, so we can't use ccache
HOST_CCACHE_CONF_OPTS += \
	-UCMAKE_C_COMPILER_LAUNCHER \
	-UCMAKE_CXX_COMPILER_LAUNCHER \
	-DZSTD_FROM_INTERNET=OFF \
	-DHIREDIS_FROM_INTERNET=OFF

# Patch host-ccache as follows:
#  - Use BR_CACHE_DIR instead of CCACHE_DIR, because CCACHE_DIR
#    is already used by autotargets for the ccache package.
#    BR_CACHE_DIR is exported by Makefile based on config option
#    BR2_CCACHE_DIR.
#  - Change hard-coded last-ditch default to match path in .config, to avoid
#    the need to specify BR_CACHE_DIR when invoking ccache directly.
#    CCache replaces "home_dir" with the home directory of the current user,
#    so rewrite BR_CACHE_DIR to take that into consideration for SDK purpose
HOST_CCACHE_DEFAULT_CCACHE_DIR = $(patsubst $(HOME)/%,%,$(BR_CACHE_DIR))

define HOST_CCACHE_PATCH_CONFIGURATION
	sed -i 's,getenv("CCACHE_DIR"),getenv("BR_CACHE_DIR"),' $(@D)/src/Config.cpp
	sed -i 's,".ccache","$(HOST_CCACHE_DEFAULT_CCACHE_DIR)",' $(@D)/src/Config.cpp
	sed -i 's,"/.cache/ccache","/$(HOST_CCACHE_DEFAULT_CCACHE_DIR)",' $(@D)/src/Config.cpp
	sed -i 's,"/.config/ccache","/$(HOST_CCACHE_DEFAULT_CCACHE_DIR)",' $(@D)/src/Config.cpp
	sed -i 's,getenv("XDG_CACHE_HOME"),nullptr,' $(@D)/src/Config.cpp
	sed -i 's,getenv("XDG_CONFIG_HOME"),nullptr,' $(@D)/src/Config.cpp
endef

HOST_CCACHE_POST_PATCH_HOOKS += HOST_CCACHE_PATCH_CONFIGURATION

define HOST_CCACHE_MAKE_CACHE_DIR
	mkdir -p $(BR_CACHE_DIR)
endef

HOST_CCACHE_POST_INSTALL_HOOKS += HOST_CCACHE_MAKE_CACHE_DIR

# Provide capability to do initial ccache setup (e.g. increase default size)
BR_CCACHE_INITIAL_SETUP = $(call qstrip,$(BR2_CCACHE_INITIAL_SETUP))
ifneq ($(BR_CCACHE_INITIAL_SETUP),)
define HOST_CCACHE_DO_INITIAL_SETUP
	@$(call MESSAGE,"Applying initial settings")
	$(CCACHE) $(BR_CCACHE_INITIAL_SETUP)
	$(CCACHE) -s
endef

HOST_CCACHE_POST_INSTALL_HOOKS += HOST_CCACHE_DO_INITIAL_SETUP
endif

$(eval $(host-cmake-package))

ifeq ($(BR2_CCACHE),y)
ccache-stats: host-ccache
	$(Q)$(CCACHE) -s

ccache-options: host-ccache
ifeq ($(CCACHE_OPTIONS),)
	$(Q)echo "Usage: make ccache-options CCACHE_OPTIONS=\"opts\""
	$(Q)echo "where 'opts' corresponds to one or more valid ccache options" \
	"(see ccache help text below)"
	$(Q)echo
endif
	$(Q)$(CCACHE) $(CCACHE_OPTIONS)
endif
