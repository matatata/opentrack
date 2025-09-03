# Opentrack Wine Bridge

The otrwinebridge package is an alternative to opentrack's existing wine integration mechanisms. It's an accompanying application for matatata's MacOpentrack.

Usually you would use the Wine output module and configure the WINEPREFIX and the path to the Wine-Executable in the proto-wine module's settings dialog so that when tracking is started the windows-game picks up the dll by looking at the registry. The opentrack-wrapper-wine.exe.so WineLib writes the registry entries and copies headpose data from native shared memory to the wine shared memory structures, so that the pure windows freetrackclient.dll and npclient.dll get the data.

However on Unix and in particular on macOS it does not work well with some wine based tools such as CrossOver or Sikarugir (formerly known as Kegworks). What's even worse is that building WinLibs on UNIX with clang is not supported anymore and on macOS clang is the only option. See https://bugs.winehq.org/show_bug.cgi?id=58580.

This is why and where otrwinebridge now comes into play:

## What's different?
I've modified the freetrack and npclient dll so that they now are socalled *builtin* wine dlls that will now call into native code which resides in the accompanying *.so libraries. This way they can directly access the headpose data from the native shared memory without the need to have another process copy it in a loop like opentrack-wrapper-wine.exe.so does. Also opentrack no longer needs to be able to start a process in a given WINEPREFIX. Instead the only thing that needs to be done is managing the registry keys. Basically that's what otrwinebridge.exe's main job is. It does not do anything special. The modified freetrackclient.dll and NPClient.dll libraries do the main job.

## How To Install and Use

### Installation
Copy this whole folder to C: or the C:\Program Files\ directory inside your wine-prefix or Bottle. This is optional, but recommended.

You __must__ copy the provided freeclient and NPCLient libraries (.dll and .so files) from `x86_64-windows`, `x86_64-unix` (if present`i386-unix`) and `i386-window` into the respective directories of your wine installation. They'll usually reside in `<wine-install>/lib/wine`:
* For homebrew this is something like
`/Applications/Wine Stable.app/Contents/Resources/wine/lib/wine`
* For macports it's `/opt/local/lib/wine/`.
* On linux this varies - you need to find it yourself.

This can be done by holding Option while dragging the wine folder into the `<wine-install>/lib/`. Finder will then ask to cancel, overwrite or merge . choose the latter.

Instead of copying the files into the wine-installation you could also work with the `WINEDLLPATH` environment variable, but it's currently being ignored at least by CrossOver and you'd have to somehow make that persistent so that it is effective whenever you start your Windows-Game. That's why I do not recommend it.


**Example Usage**
* Open the otrwinebridge.exe
    - On stock wine navigate to the otrwinebridge folder and run `wine otrwinebridge.exe`.
    - In CrossOver select the Bottle and then use the Menu-Item "Bottle/Run Command" (or Key-Shortcut "Command-R" on a Mac). Navigate to otrwinebridge.exe and run it. The program will write the value
   "Path" in `Software\NaturalPoint\NATURALPOINT\NPClient Location` and/or
    `Software\Freetrack\FreeTrackClient` to the registry depending on the protocol you chosse under "Options". When in doubt choose "Both".
  Save it as launcher if you like, so that you'll have an icon in your dashboard for future use.

* Open opentrack. As unintuitive as it may seem do NOT configure your profile to actually use its wine integration features. Choose the "Opentrack Shared Memory" output module.

You can now close otrwinebridge and will never have to open it again unless you selected the "Remove from registry on exit"  option. See *Options* below.

**Options**
* "Protocol" lets you choose the protocol (recommended: "Both"). You probably have to restart your game.
* "Remove from registry on exit" lets you choose whether you want the registry entries to be removed when the program quits. This makes sense when you usually don't want to use headtracking. In this scenario open otrwinebridge.exe before you start your game and close otrwinebridge.exe when you're done playing.
