FROM mcr.microsoft.com/devcontainers/base:ubuntu24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    subversion \
    git-core \
    libncurses5-dev \
    zlib1g-dev \
    gawk \
    bc \
    cpio \
    file \
    flex \
    quilt \
    libssl-dev \
    xsltproc \
    libxml-parser-perl \
    mercurial \
    bzr \
    ecj \
    cvs \
    unzip \
    zlib1g-dev \
    libncurses-dev \
    u-boot-tools \
    mkbootimg \
    xxd \
    shellcheck \
    && rm -rf /var/lib/apt/ \
    && rm -rf /var/cache/apt/ \
    && wget -q -O /usr/bin/hadolint https://github.com/hadolint/hadolint/releases/download/v2.12.0/hadolint-Linux-x86_64 \
    && chmod 755 /usr/bin/hadolint
