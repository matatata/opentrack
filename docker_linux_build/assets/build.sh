#!/bin/bash

cmake -S /otr -B /otr/build/linux --toolchain cmake/linux.cmake \
&& cmake --build /otr/build/linux  --target install -j6

