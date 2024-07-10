###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= f9a5d58f42428f861693bdeac5acc0171872d807
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
