#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker build -t kitware/kwstyle-testing $script_dir
