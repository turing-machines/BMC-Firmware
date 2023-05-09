## Install the necessary dependent packages

```makefile
sudo apt-get install build-essential subversion git-core libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc libxml-parser-perl mercurial bzr ecj cvs unzip lib32z1 lib32z1-dev lib32stdc++6 libstdc++6 libncurses-dev u-boot-tools mkbootimg -y
```

## build

```shell
# Build main image:
cd buildroot
make   BR2_EXTERNAL=../tp2bmc  tp2bmc_defconfig
make


# Package up swu (for OTA update via webui):
cd output/images/
cp -ra ../../../tp2bmc/swupdate/* .
./genSWU.sh 1.0.0
```
