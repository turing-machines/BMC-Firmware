## Disclaimer

This repository is currently in the development state. For the stable version of the firmware, visit [here](https://github.com/wenyi0421/turing-pi).

## Install the necessary dependent packages

```shell
sudo apt-get install build-essential subversion git-core libncurses5-dev zlib1g-dev gawk flex \
        quilt libssl-dev xsltproc libxml-parser-perl mercurial bzr ecj cvs unzip lib32z1 lib32z1-dev \
        lib32stdc++6 libstdc++6 libncurses-dev u-boot-tools mkbootimg -y
```

## Build

```shell
# Build main image:
cd buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig
make

# Prepare SWUpdate image (for OTA update via webui):
cd output/images/
cp -a ../../../tp2bmc/swupdate/* .
./genSWU.sh 1.0.0
```
