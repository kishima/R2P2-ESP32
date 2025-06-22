#!/bin/bash

set -e

IDF_VER=v5.4.1

docker build \
  --build-arg IDF_VER=$IDF_VER \
  -t custom-idf:$IDF_VER \
  -f Dockerfile .
