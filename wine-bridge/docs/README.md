``` {=html}
<style>
body { min-width: 80% !important; }
</style>
```
# Opentrack Wine Bridge (LONG VERSION)
introduced by matatata October 2025

This is an alternative to opentrack's existing Wine output module. I created it, because Wine's hybrid winedlls are no longer supported on macOS (more precisely Unix and the clang compiler). See my tickets at [WineHQ](https://bugs.winehq.org/show_bug.cgi?id=58580) and [macports](https://bugs.winehq.org/show_bug.cgi?id=58580) for details. Besides, it was difficult or impossible to get the Wine module to work with some other Wine based solutions such as CrossOver anyway. The same may be true for Proton/Steam in Linux.

My new approach uses Wine's more modern _Unix Call interface_ which allowed me to create so so-called _Wine Builtin_ versions of freetrackclient.dll and other related libraries. Those Builtin dlls are able to call into accompanying native Unix shared libraries which can access opentrack's headpose data directly. The original Wine module required a seperate program _opentrack-wine-wrapper.dll.exe_ to copy the data from one memory space to another. Starting with Wine 10.x it can no longer be compiled on Unix/clang (including macOS) nor is it supported according to Wine's lead developer. It may still work, but it's a risky. On Linux the hybrid winedlls ARE still supported, because gcc is able to deal with Microsofts ABI. So we're mainly talking about macOS. Yet, I found the new approach advantageous even on Linux as I'm going to describe.

**Pros and Cons of the classic and my new Wine integration:**

Classic:

Pro

- Uses unaltered windows native dll, which are proven to work - even with anti-cheat software.
- If it does works, well then it's quite convenient and the solution is self-contained. You only have to configure the Wine prefix and executable.

Con

- Needs the opentrack-wine-wrapper.dll.exe process, which can be difficult/impossible to compile and/or start - at least on some platforms and environments.
- On macOS it does not work with CrossOver which is a very popular and capable Wine based solution, e.g. I've heard about troubles with Proton/Steam in Linux, but I have almost no experience with that so far.
- Configuring the Wine prefix and executable may be not easy for all users.


Wine-Brige:

Pro

- Does not use the unsupported hybrid winedll but the Unix Wine interface, which is expected to be stable in future.
- As far as I can tell it works pretty well in all Wine environments I've tried so far - including CrossOver on macOS and Proton/Steam on Linux.
- More robust and stable, since opentrack no longer needs to know anything about the particular Wine environment you might use. It does not need to know how to start a process under Proton, stock Wine or other solutions which all have different requirments.
- Without the separate opentrack-wine-wrapper.dll.exe process you could achieve a higher framerate and a smoother experience.
- In my opinion enabling the freetrackclient.dll and related libraries to directly access the headpose data is more straight-forward than the previous approach. Furthermore my design uses an abstraction layer called _opentrackclient_ which facilitates future changes under the hood.
- Just try it! The installation is non-destructive and can be undone very easily. Afterall we're dealing with symbolic links added to specific directories of your Wine installation. For many Wine solutions there's even no installation necessary at all. Setting the _WINEDLLPATH_ environment variable may suffice.

Con

- Usually requires a separate installation step to get the builtin dlls into the right places. I tried to make it as easy, reliable and flexible as possible, but since there are so many Wine based variants (particulary on Linux) the installation might require a little bit of attention.
- Opentrack no longer manages the registry keys for you. Luckily there's a separate tool for that shipping with Opentrack-Wine-Bridge.
- New and not widely tested. I have no idea, whether anti-cheat software may be confused by the currently unknown dlls. However I can imaging that anti-cheat software may struggle in Wine environments and particulary on Apple Silicon processors anyway.

**Credits**

Opentrack-Wine-Bridge has been a lot of work and it sure is my original work, but quite a lot of it is obviously based on or inspired by software/code provided/created at least by

 - Stanislaw Halik and other authros of opentrack
 - Wim Vriend, Ron Hendriks of FreeTrackNoIR
 - uglyDwarf of linuxtrack

**Disclaimer**

I can't possibly test every game and I have no idea which games actually work and if there's an unintended impact.
installation can be undone very easily. and you can define dll overrides via winecfg to effectively disable the Builtin wine dlls for each Wine prefix/bottle in case you want to use the original freetrackclient and npclient dlls provided with the windows version of opentrack.

**Known to work**

I've successfully tested this approach with the following games and environments:

- Homebrew/macports Wine 10.x
  - Falcon BMS 4.37/8
- CrossOver
  - Falcon BMS 4.37/8
  - Euro Truck Simulator 1 and 2 (Steam)
  - Elite Dangerous (Steam)
  - IL-2 Sturmovik (Steam)
  - XWWM (X-Wing community project)
- Steam/Proton Manajro-Linux
  - Exosky (Demo) Steam

## Installation on macOS
Install the pkg you've downloaded. It will install several files into `/Library/Application Support/opentrack-wine-bridge`. During installation a script will look for Wine installations at common locations. Foreach match it will ask whether you'd like the script to enable that Wine installation for use with opentrack. If you confirm it will create symlinks from the aformentioned wine-bridge directory into specific folders at `<wine-installation>/[...]/lib/wine/`. This will allow wine to load those custom dlls whenever a game tries to load a dll named freetrackclient(64).dll or npclient(64).dll. The installer will look for

- `/Applications/CrossOver*.app` (CrossOver Mac)
- `/Applications/Wine*.app` (homebrew)
- `$HOME/Applications/Kegworks/...` (Sikarugir aka Kegworks)
- `/opt/local/lib/wine` (macports)

Other Wine based solutions are not supported yet, but the necessary steps could be easily performed manually and chances are good that it'll work. Follow the instruction under 'Manual installation'.

### Uninstall

To undo what the installer did to your wine installs run this:

    find /opt/local/lib /Applications/CrossOver*.app /Applications/Wine*.app \( -name "freetrackclient*.*" -o -name "npclient*.*"  \) -exec sudo rm {} \;

## Using Wine-Games with Opentrack

OpentrackWineBridge 1.0 is compatible with the Wine module of the current MacOpentrack 2024.1.1-matatata.1, but this will probably change with the next release of opentrack.

## Opentrack

In MacOpentrack version 2024.1.1-matatata use the X-Plane/Wine protocol and select the "Disabled (X-Plane)". In the upcoming MacOpentrack version there that module will be replaced. Start tracking.

### Creating the registry keys

The game needs to know whether you want to use headtracking at all. It will look for specific keys in the registry and if present, will use the configured locations to find and load freetrackclient(64).dll or npclient(64).dll. The old Wine module created those registry keys for you which is why you had to tell it the `WINEPREFIX` and wine executable. This is no longer opentrack's job but YOUR job. But don't worry it's easy and more reliable.

*CrossOver Mac:*

Select the Bottle you wnat to use opentrack with. Chose the Menu-Item "Run Command" (Command-R). Navigate to `/Library/Application Support/opentrack-wine-bridge/bin` (use Command-G and start typing) and select the `ftnoir-registry-tool`.

You'll see something like this:

    === ftnoir-registry-tool.exe ===
    This program manages the registry entries that dictate if and where games will look for the freetrackclient and NPCLient dlls.

    [n] Enable NPClient protocol exclusively
    [f] Enable FreeTrackNoir protocol exclusively
    [b] Enable all protocols (recommended)
    [d] Disable all protocols
    [p] Print current Settings
    [q] Quit

    Enter your choice:

Type 'a' to enable all protocols. You could optionally use 'p' to verify the settings before and afterwards. Then 'q' to exit. Before closing the Runner-Window I recommend to save the command as a launcher for future use.

Start your game. You don't have to perform any of the above steps unless you want to not always use headtracking. In this case open the registry tool when you're done and disable all protocols by hitting 'd'.

## Advanced topics

### Manual installation

- Let's call the location of the Wine installation you want to install opentrack wine-bridge into `<some-wine-installation>`. You'll have to find a folder inside it that has the following structure:

The target directory structure will probably look like this:

    .
    ├── i386-windows
    ├── x86_64-unix
    └── x86_64-windows

It is likely called lib/wine.

Then take a look at the wine-bridge installation. on macOS you'll have to navigate to `/Library/Application Support/opentrack-wine-bridge/lib/wine`. There you'll find a directory structure similar to this:

    .
    ├── i386-windows
    │   ├── freetrackclient.dll
    │   ├── [...]
    ├── x86_64-unix
    │   ├── freetrackclient.so
    │   ├── freetrackclient64.so
    │   ├── [...]
    └── x86_64-windows
        ├── freetrackclient64.dll
        ├── [...]

Those files need to be copied or symlinked into the matching directories inside the target directory. On macOS you couly drag and drop `wine` into the directory containing `wine` target directory while holding the Option-Key for the whole time. Finder will ask whether you want to merge or replace the contents. You'll want to merge not never replace!

Of course one could script all that but beware that sandboxed macOS applications might prevent you from altering the app bundle - even when using sudo. If you instead do it via Finder you're allowed to do so. Use [https://github.com/matatata/opentrack/blob/develop/wine-bridge/manual-install.sh](https://github.com/matatata/opentrack/blob/develop/wine-bridge/manual-install.sh)

and call it like this e.g. (you may need to prepend sudo)

    # for macports
    sh manual-install.sh /opt/local/lib

    # homebrew
    sh manual-install.sh /Applications/Wine\ Devel.app

To undo what the script did to `/some/path/to/wine/install` for instance :

    find /some/path/to/wine/install  \( -name "freetrackclient*.*" -o -name "npclient*.*"  \) -delete


### Disable Opoentrack's Builtin dlls for a wine prefix / bottle

By default Opentrack's Builtin wine dlls are effective in all wine prefixes and bottles if installed in the wine installation running it. To disable them in your prefix use `winecfg`inside your prefix/bottle. Select Tab "Libraries" and add overrides for freetrackclient, freetrackclient64, npclient and npclient64. Set each to "Native then Builtin" or just "Native". This way they won't have an effect in your prefix.

### Troubleshooting

To verify that Builtin winelibs are installed in CrossOver.app e.g. :

    find /Applications/CrossOver.app \( -name "freetrackclient*.*" -o -name "npclient*.*"  \) -exec ls -ls {} \;

You an output similar to this:

    /Applications/CrossOver.app/Contents/SharedSupport/CrossOver/lib/wine/x86_64-unix/freetrackclient64.so -> /Library/Application Support/opentrack-wine-bridge/lib/wine/x86_64-unix/freetrackclient64.so
    /Applications/CrossOver.app/Contents/SharedSupport/CrossOver/lib/wine/x86_64-unix/freetrackclient.so -> /Library/Application Support/opentrack-wine-bridge/lib/wine/x86_64-unix/freetrackclient.so
    /Applications/CrossOver.app/Contents/SharedSupport/CrossOver/lib/wine/i386-windows/freetrackclient.dll -> /Library/Application Support/opentrack-wine-bridge/lib/wine/i386-windows/freetrackclient.dll
    /Applications/CrossOver.app/Contents/SharedSupport/CrossOver/lib/wine/x86_64-windows/freetrackclient64.dll -> /Library/Application Support/opentrack-wine-bridge/lib/wine/x86_64-windows/freetrackclient64.dll
    [...]


