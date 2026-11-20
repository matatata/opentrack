#!/bin/bash

docker run --rm -v $(pwd)/../..:/otr \
                -v $(pwd)/dist:/otr/build/linux/otrclient/dist \
                -v $(pwd)/assets:/assets \
                otrclient_build bash /assets/build.sh

