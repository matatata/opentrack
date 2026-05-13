# opentrackclient

This library allows POSIX software on macOS or Linux to read opentrack's headpose by loading a shared library and using a simple API. - similar to how it's done under Windows.

See [otrclient-tester.c](./otrclient-tester.c) as an example. Opentrack's [X-Plane plugin](../x-plane-plugin/plugin.c) now uses it as well.
