#!/bin/bash

cmake -DOTRCLIENT_LINUX_INTEL_MULTILIB=false -S /otr/opentrackclient -B /otr/build/linux/otrclient \
&& cmake --build /otr/build/linux/otrclient  --target install -j6

