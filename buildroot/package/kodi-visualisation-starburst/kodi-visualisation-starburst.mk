################################################################################
#
# kodi-visualisation-starburst
#
################################################################################

KODI_VISUALISATION_STARBURST_VERSION = 20.2.0-Nexus
KODI_VISUALISATION_STARBURST_SITE = $(call github,xbmc,visualization.starburst,$(KODI_VISUALISATION_STARBURST_VERSION))
KODI_VISUALISATION_STARBURST_LICENSE = GPL-2.0+
KODI_VISUALISATION_STARBURST_LICENSE_FILES = LICENSE.md
KODI_VISUALISATION_STARBURST_DEPENDENCIES = glm kodi

$(eval $(cmake-package))
