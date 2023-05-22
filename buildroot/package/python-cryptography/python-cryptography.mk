################################################################################
#
# python-cryptography
#
################################################################################

PYTHON_CRYPTOGRAPHY_VERSION = 39.0.1
PYTHON_CRYPTOGRAPHY_SOURCE = cryptography-$(PYTHON_CRYPTOGRAPHY_VERSION).tar.gz
PYTHON_CRYPTOGRAPHY_SITE = https://files.pythonhosted.org/packages/6a/f5/a729774d087e50fffd1438b3877a91e9281294f985bda0fd15bf99016c78
PYTHON_CRYPTOGRAPHY_SETUP_TYPE = setuptools
PYTHON_CRYPTOGRAPHY_LICENSE = Apache-2.0 or BSD-3-Clause
PYTHON_CRYPTOGRAPHY_LICENSE_FILES = LICENSE LICENSE.APACHE LICENSE.BSD
PYTHON_CRYPTOGRAPHY_CPE_ID_VENDOR = cryptography_project
PYTHON_CRYPTOGRAPHY_CPE_ID_PRODUCT = cryptography
PYTHON_CRYPTOGRAPHY_DEPENDENCIES = \
	host-python-setuptools-rust \
	host-python-cffi \
	host-rustc \
	openssl
HOST_PYTHON_CRYPTOGRAPHY_DEPENDENCIES = \
	host-python-setuptools-rust \
	host-python-cffi \
	host-rustc \
	host-openssl
PYTHON_CRYPTOGRAPHY_ENV = \
	$(PKG_CARGO_ENV) \
	PYO3_CROSS_LIB_DIR="$(STAGING_DIR)/usr/lib/python$(PYTHON3_VERSION_MAJOR)"
HOST_PYTHON_CRYPTOGRAPHY_ENV = \
	$(HOST_PKG_CARGO_ENV) \
	PYO3_CROSS_LIB_DIR="$(HOST_DIR)/lib/python$(PYTHON3_VERSION_MAJOR)"
# We need to vendor the Cargo crates at download time
PYTHON_CRYPTOGRAPHY_DOWNLOAD_POST_PROCESS = cargo
PYTHON_CRYPTOGRAPHY_DOWNLOAD_DEPENDENCIES = host-rustc
PYTHON_CRYPTOGRAPHY_DL_ENV = \
	$(PKG_CARGO_ENV) \
	BR_CARGO_MANIFEST_PATH=src/rust/Cargo.toml
HOST_PYTHON_CRYPTOGRAPHY_DL_ENV = \
	$(HOST_PKG_CARGO_ENV) \
	BR_CARGO_MANIFEST_PATH=src/rust/Cargo.toml

$(eval $(python-package))
$(eval $(host-python-package))
