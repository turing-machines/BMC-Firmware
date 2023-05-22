################################################################################
#
# balena-engine
#
################################################################################

BALENA_ENGINE_VERSION = 20.10.26
BALENA_ENGINE_SITE = $(call github,balena-os,balena-engine,v$(BALENA_ENGINE_VERSION))

BALENA_ENGINE_LICENSE = Apache-2.0
BALENA_ENGINE_LICENSE_FILES = LICENSE

BALENA_ENGINE_GOMOD = github.com/docker/docker

BALENA_ENGINE_LDFLAGS = \
	-X $(BALENA_ENGINE_GOMOD)/dockerversion.GitCommit= \
	-X $(BALENA_ENGINE_GOMOD)/dockerversion.Version=$(BALENA_ENGINE_VERSION) \
	-X github.com/containerd/containerd/version.Version=N/A \
	-X github.com/docker/cli/cli/version.BuildTime= \
	-X github.com/docker/cli/cli/version.GitCommit= \
	-X github.com/docker/cli/cli/version.Version=N/A \
	-X github.com/opencontainers/runc.version=N/A

BALENA_ENGINE_TAGS = \
	cgo \
	no_btrfs \
	no_cri \
	no_devmapper \
	no_zfs \
	exclude_disk_quota \
	exclude_graphdriver_btrfs \
	exclude_graphdriver_devicemapper \
	exclude_graphdriver_zfs

BALENA_ENGINE_BUILD_TARGETS = cmd/balena-engine

# create the go.mod file with language version go1.19
# remove the conflicting vendor/modules.txt
# remove the conflicting vendor/archive (not allowed in go1.20)
# https://github.com/moby/moby/issues/44618#issuecomment-1343565705
define BALENA_ENGINE_FIX_VENDORING
	printf "module $(BALENA_ENGINE_GOMOD)\n\ngo 1.19\n" > $(@D)/go.mod
	rm -f $(@D)/vendor/modules.txt
	rm -rf $(@D)/vendor/archive
endef
BALENA_ENGINE_POST_EXTRACT_HOOKS += BALENA_ENGINE_FIX_VENDORING

ifeq ($(BR2_INIT_SYSTEMD),y)
BALENA_ENGINE_DEPENDENCIES += systemd
BALENA_ENGINE_TAGS += journald
endif

define BALENA_ENGINE_INSTALL_INIT_SYSTEMD
	$(INSTALL) -D -m 644 $(@D)/contrib/init/systemd/balena-engine.service \
		$(TARGET_DIR)/usr/lib/systemd/system/balena-engine.service
	$(INSTALL) -D -m 644 $(@D)/contrib/init/systemd/balena-engine.socket \
		$(TARGET_DIR)/usr/lib/systemd/system/balena-engine.socket
endef

define BALENA_ENGINE_USERS
	- - balena-engine -1 * - - - balenaEngine daemon
endef

define BALENA_ENGINE_LINUX_CONFIG_FIXUPS
	$(call KCONFIG_ENABLE_OPT,CONFIG_POSIX_MQUEUE)
	$(call KCONFIG_ENABLE_OPT,CONFIG_CGROUPS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NAMESPACES)
	$(call KCONFIG_ENABLE_OPT,CONFIG_UTS_NS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_IPC_NS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_PID_NS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NET_NS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NETFILTER)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NF_CONNTRACK)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NETFILTER_XT_MATCH_ADDRTYPE)
	$(call KCONFIG_ENABLE_OPT,CONFIG_NETFILTER_XT_MATCH_CONNTRACK)
	$(call KCONFIG_ENABLE_OPT,CONFIG_IP_NF_IPTABLES)
	$(call KCONFIG_ENABLE_OPT,CONFIG_IP_NF_FILTER)
	$(call KCONFIG_ENABLE_OPT,CONFIG_IP_NF_NAT)
	$(call KCONFIG_ENABLE_OPT,CONFIG_BRIDGE)
	$(call KCONFIG_ENABLE_OPT,CONFIG_VETH)
endef

define BALENA_ENGINE_INSTALL_SYMLINK
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-daemon
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-containerd
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-containerd-shim
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-containerd-ctr
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-runc
	ln -f -s balena-engine $(TARGET_DIR)/usr/bin/balena-engine-proxy
	$(if $(BR2_PACKAGE_TINI),ln -f -s tini $(TARGET_DIR)/usr/bin/balena-engine-init)
endef
BALENA_ENGINE_POST_INSTALL_TARGET_HOOKS += BALENA_ENGINE_INSTALL_SYMLINK

$(eval $(golang-package))
