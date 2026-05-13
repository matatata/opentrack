#!/bin/bash

if [ -z "$WINE_HOME" ]
then
    echo "You must set WINE_HOME"
    exit 1
fi

docker run --rm -v $(pwd)/..:/otr -v "$WINE_HOME:/wine" -v $(pwd)/opt/opentrack:/opt/opentrack otrubuntu /build.sh

