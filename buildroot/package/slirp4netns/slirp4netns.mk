################################################################################
#
# slirp4netns
#
################################################################################

SLIRP4NETNS_VERSION = 1.2.0
SLIRP4NETNS_SITE = $(call github,rootless-containers,slirp4netns,v$(SLIRP4NETNS_VERSION))
SLIRP4NETNS_LICENSE = GPL-2.0
SLIRP4NETNS_LICENSE_FILES = COPYING
SLIRP4NETNS_DEPENDENCIES = libcap libglib2 libseccomp slirp

SLIRP4NETNS_AUTORECONF = YES

$(eval $(autotools-package))
