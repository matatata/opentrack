*Purpose*

The otrwinebrdige.exe is an alternative to opentrack's existing wine integration mechanisms.
Normally you would configure the WINEPREFIX and the path to the Wine-Executable
so that when tracking is started the windows-game picks up the dll by looking at the registry.

On MacOS however it does not work well with wine based tools such as CrossOver or Kegworks.
Here's where otrwinebridge comes into play:

*How To Install and Use*

*Installation*
Copy this whole folder to C: or the C:\Program Files\ directory inside your wine-prefix or Bottle. This is optional, but recommended.
If macOS complains that the sofware was not scanned for malicious content then you should not use it unless you know the origin is trustworthy.
When I "matatata" provide it, it will be signed and checked by Apple. I you want to still use it take it out of quarantine by
opening a Terminal and execute "sudo xattr -cr OtrWineBridge" on the whole folder.

**Example Usage with CrossOver**
- Open the otrwinebridge.exe in CrossOver by selecting the Bottle and selecting the Menu-Item "Bottle/Run Command" (or Key-Shortcut "Command-R").
  Navigate to otrwinebridge.exe. A small window opens. The program will write the value
   "Path" in "Software\\NaturalPoint\\NATURALPOINT\\NPClient Location" and/or
    "Software\\Freetrack\\FreeTrackClient" to the registry. It'll then start to copy the headpose data from the native macOS shared-memory area to
    where a windows-game can read it when running under wine.
  Save it as launcher if you like, so that you don't have to repeat the above step.

- Configure MacOpenTrack to NOT use the wine-integration: Choose the output module ("Disabled (X-Plane)") or (Native Shared Memory in newer versions of MacOpentrack).

- Start tracking

- Start your game

- Close otrwinebridge's window to sto

The order of the steps is not relevant, unless on first run and if you selected the "Remove from registry on exit"  option. See *Options* below.

**Options**
- "Protocol" lets you choose the protocol (recommended: "Both"). You probably have to restart your game.
- "Remove from registry on exit" lets you choose whether you want the registry entries to be removed when the program quits. For instance becuase you usually don't want to use headtracking
 except when you exploicitly start otrwinebridge.exe. Be aware that in that case you need to start otrwinebridge.exe before starting the game, otherwise the game will not know that you want to use headtracking.






