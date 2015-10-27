#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/usr/src/KWStyle \
    kitware/kwstyle-testing \
      /usr/src/KWStyle/Testing/Docker/test.sh
