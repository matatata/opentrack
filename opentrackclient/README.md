# opentrackclient

This library allows macOS or Linux programs to read opentrack's headpose by loading a shared library and using a simple API. - similar to how it's done under Windows.

See [otrclient-tester.c](otrclient-tester.c) as an example. Opentrack's X-Plane plugin uses the opentrack client library as well (linked statically). As a proof-of-concept I was able to easily integrate it into [my fork of the FreeSpace2-Engine](https://github.com/matatata/fs2open.github.com/tree/native_macopentrack) so that I could play FreeSpace and other mods on macOS with native headtracking. I'll provide a merge-request so they might integrate it in their repo.

It still uses shared memory so it probably won't work if you create a sanboxed macOS. Having that abstraction now allows to use a different inter-process-communication mechanism in future.

# macOS
You can get my prebuilt and code-signed opentrack binaries from https://matatata.gumroad.com/l/macopentrack . You'll find the code-signed opentrackclient.framework on the .dmg Diskimage.

## Compiling yourself

It should be enough to do the following:

``` 
git clone https://github.com/matatata/opentrack.git
cd opentrack
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
cd opentrackclient
make
```

Optionally you can execute `sudo make install` which would install the following files:

```
/usr/local/
|-- include
|   `-- opentrackclient
|       `-- opentrackclient.h
|-- lib
|   |-- libopentrackclient-loader.a (not on macOS)
|   `-- libopentrackclient.so
|-- share
|   `-- opentrackclient
|       |-- README.md
|       `-- otrclient-tester.c
```

Note that on macOS `opentrackclient.framework` is also installed into `/Library/Frameworks`



