FROM mcr.microsoft.com/devcontainers/base:ubuntu22.04

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
    && rm -rf /var/lib/apt/lists/*
