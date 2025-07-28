#!/bin/bash

docker run -it --rm --group-add=dialout --privileged --user $(id -u):$(id -g) -v $PWD/..:/project esp32_build_container:v5.4.1 bash
