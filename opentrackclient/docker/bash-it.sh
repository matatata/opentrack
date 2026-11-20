#!/bin/bash


docker run --rm -v $(pwd)/../..:/otr \
                -v $(pwd)/dist:/otr/build/linux/otrclient/dist \
                -v $(pwd)/assets:/assets \
                -it otrclient_build bash

