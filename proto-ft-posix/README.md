# freetrack POSIX

This protocol is primarily intended to replace proto-wine on macOS, since it [can't be used anymore on x86-64 platforms using clang](https://bugs.winehq.org/show_bug.cgi?id=58580). However it is usable on Linux as well.

This protocol should be used with
- Native clients such as the provided X-Plane plugin. Previosly the x-plane plugin needed to open opentrack's shared memory directory whereas now opentrack providevs the [opentrackclient shared library](../opentrackclient/README.md) which hides it all away. My hope is that this facilitates integration of opentrack into macOS or Linux-Softare as there's now a simple API for that - similar to how it's done on Windows.
- Or with windows software run via [WINE](https://www.winehq.org). For that [Wine-Bridge](../wine-bridge/README.md) was created to facilitate exactly that using a different approach than proto-wine does.

