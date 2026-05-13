# MacOpentrack

June 2026

This is my macOS binary distribution of opentrack build from the sources of [my opentrack fork](https://github.com/matatata/opentrack) where I have made several changes and additions focussing macOS (and coincidently Linux). The binaries are signed by me and notarized by Apple.

Copy the opentrack.app bundle to your /Applications-folder or whereever you want. When you start it for the first time it will ask you to access the Documents folder, because it wants to create a Folder to store its profiles. If you use a camera then it will ask for permission as well. You might need to restart the app.

As far as I know there are not many programs that work with MacOpentrack because headtracking has never been a thing on macOS and commercial manufacturers seemed to have never even bothered. Windows is where it all shines.

However MacOpentrack most notably does work well with
- [X-Plane](https://www.x-plane.com), because opentrack provides a plugin for that (see the [X-Plane-Plugin folder](X-Plane-Plugin/) nearby).
- Most Windows games being run via a [Wine](https://www.winehq.org)-based solution such as [Crossover](https://www.codeweavers.com/crossover) using [Opentrack Wine Bridge](https://matatata.gumroad.com/l/opentrackwinebridge). Yes there's been a Wine integration before, but it no longer works with recent Wine versions on macOS and is therefore no longer included in my builds since version 26.1.1.

## Fetures and current state:

Much of the macOS integration has been created by contributers a long time ago and some fetures, trackers and output modules might not work anymore. These modules are known to work:

- point-racker for IR based solutions (recommended) such as [DelanClip's](https://delanclip.com/?a=tomatec).
- neuralet-tracker for face tracking using a regular webcam
- udp network for tracking external sources and output
- opentrack 2.0 Enhanced for X-Plane, Wine Bridge and potentially other native games that are willing to integrate with opentrack using my opentrackclient-library (see [sdk Folder](sdk) nearby)
- Open Sound Control (OSC) to control musical instruments

Please refer to [https://matatata.github.io/MacOpentrack.html](https://matatata.github.io/MacOpentrack.html) for more information.


Brought to you by matatata [at] me.com

## Credits

I'm not the original author of Opentrack. It has a long history and many contributors. Here's the original repository: https://github.com/opentrack/opentrack



