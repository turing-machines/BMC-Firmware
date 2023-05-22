################################################################################
#
# shapelib
#
################################################################################

SHAPELIB_VERSION = 1.5.0
SHAPELIB_SITE = http://download.osgeo.org/shapelib
SHAPELIB_LICENSE = MIT or LGPL-2.0
SHAPELIB_LICENSE_FILES = web/license.html COPYING
SHAPELIB_CPE_ID_VENDOR = osgeo
SHAPELIB_INSTALL_STAGING = YES

# 0001-Remove-double-free-in-contrib-shpsrt.patch
SHAPELIB_IGNORE_CVES += CVE-2022-0699

$(eval $(autotools-package))
