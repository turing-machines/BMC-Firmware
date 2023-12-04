###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= f8d5d6e53a6820f52f10da50320fcf8f612a046b
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
