FROM debian:8
MAINTAINER Insight Software Consortium <community@itk.org>

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  ninja-build \
  git 

RUN mkdir -p /usr/src/KWStyle-build
WORKDIR /usr/src


