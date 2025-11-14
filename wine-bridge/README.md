# Opentrack Wine Bridge

This is an alternative approach to get Opentrack with Windows-Games running via Wine. There are [known issues](https://trac.macports.org/ticket/72792) particularly on macOS that made me create all this. It consist of a set of custom so-called Wine builtin DLLs, accompanying unix libraries, and tools to facilitate the installation and setup.

The linux build happens to work fine on Linux as well. I successfully tested it with Steam's Proton and a few games. I intent to support Linux as well, but for now macOS and CrossOver is my primary focus.

You can also find this README at `/Library/Application Support/opentrack-wine-bridge/README.html` and the latest version [online](https://github.com/matatata/opentrack/blob/develop/wine-bridge/README.md).


## Disclaimer
It may or it may not work with your game or Wine installation! I cannot know! That's why I you should try the demo version first. If it works for you and with the games you play, please consider to buy the unrestricted version, or uninstall/disable it otherwise (see chapter 6). I decided to charge a small amount for the prebuilt binaries, since it's been an incredible amount of work and developing/distributing software for macOS really does cost money.

## Credits
This alternative wine integration has been created and brought to you by [matatata](https://matatata.github.io)

All of it would not be possible without software created by

- Stanislaw Halik (opentrack)
- Wim Vriend, Ron Hendriks (FreeTrackNoIR)
- uglyDwarf (linuxtrack)


# Instructions for macOS

## Getting started - I don't want to read the whole README just yet

1. Install the .pkg file  - allow it to install DLLs into your Wine environment(s) (CrossOver / homebrew's Wine).
    - Optionally or if necessary use the __Opentrack DLL Manager.app__ to prepare your Wine environments yourself. You only need to do this once or when your Wine environment has been updated or reinstalled.
2. Run __opentrack.app__. If you don't have it already get MacOpentrack [here](https://matatata.gumroad.com/l/macopentrack).  Configure your profile to use the WINE/X-Plane output-module, but don't bother configuring actual setting, but check the "Diabled (X-Plane)" option in the module's settings dialog. This will become less counterintuitive in a future release of MacOpentrack. Start tracking.
3. Run __Opentrack Wine Bridge.exe__ via Wine in the context of the Wine-prefix/Bottle you installed your game. Don't close the program.
4. Start you game. It'll likely pick up the headpose.
5. Close  __Opentrack Wine Bridge.exe__ once you're done playing.

__Opentrack DLL Manager.app__ and __Opentrack Wine Bridge.exe__ are both located in the Applications-folder.

## 1 Installation

1. Install the .pkg package you find on the disk image
2. If you have CrossOver or Homebrew's Wine already installed (at the default locations), then the installer will ask you, if it may prepare those Wine environments right away. You can perform the steps yourself later (see chapter 2), but it's convenient to let it do that for you.

## 2 Preparation of Wine-environment(s)

For __Opentrack Wine Bridge.exe__ to be able to pass opentrack's headpose to Windows games, it needs to load the provided otrclient.dll and it's accompanying unix library. For some Wine-environments using the environment Variable WINEDLLPATH is enough, but the most reliable way is to create symbolic links at the right places. We'll simply refer to this process as _installing DLLs_, althought we're actually creating symbolic links to various library-files located at `/Library/Application Support/opentrack-wine-bridge/lib/wine`.

**Note that when your Wine environment is updated, you most likely will need to reinstall Opentrack's DLLs as described in this chapter - or by installing the .pkg if it was sufficient the first time.**

*If you only use CrossOver or Homebrew and you did not skip step 2 of chapter 1 then you can continue with chapter 3*

### 2.1 App bundles such as CrossOver.app or Homebrew's Wine*.app
1. Open _Opentrack DLL Manager.app_ that you should find in the `/Applications` folder.
2. If it asks navigate to your Wine environment, or use File->Open in the Menu.
	- _CrossOver.app_ will be in `/Applications` or `/Users/<yourname>Applications`
	- Homebrew's _Wine Devel.app_, _Wine Stable.app_ or _Wine Staging.app_ are in `/Applications`
3. The app will show you whether the DLLs are alreay installed or not. Click the Install-Button to install them if necessary

### 2.2 MacPorts's Wine or other environments

In a Terminal execute `sudo /usr/local/bin/otrwine install /opt/local/lib/wine`.

## 3 Configure the Octopus for usage with Wine
- Open the latest release of the Opentrack application (currently version `2024.1.1-matatata.1`) and select or create a suitable profile.
- Select the WINE/X-Plane output-protocol. But we'll actually configure it to NOT use Wine at all, but instead set it up as if we wnated to use X-Plane: Use the wrench-icon to open the module's option-dialog and select the Radio-Button "X-Plane (disabled)". In a future version of MacOpentrack this will be changed and less counterintuitive.


## 4 Before starting your Game

The installation of the DLLs is not enough. We need add some values to the Windows-registry, so that games know that you want to do head-tracking and by usgin what protocol(s). Furthermore opentrack's headpose needs to be copied to where the Windows game expects it. Opentrack's classic WINE-Module tries to do exactly that, but it does not work with CrossOver and there are other obstacles why I decided to create __Opentrack Wine Bridge.exe__ as an alternative and YOU have make sure it's running whenever you want to use head-tracking. It may no be as convenient as before, but in order to support as many Wine environments as possible, I realized it's easiest to let You run __Opentrack Wine Bridge.exe__ using the tools the Wine environment of your is offering you. I'll explain it for some popular Wine environments:

### 4.1 CrossOver

- Open CrossOver and select the Bottle you want to use head-tracking with. Use the "Run Command" button in the top right corner or the Command-R Key-Command.
- Browse to /Applications/Opentrack-Configure-Protocol.exe
- Press "Save command as Launcher" so it's easier to run it the next time you need it. It even shows up in Spotlight, since CrossOver creates a little macOS application bundle for it.
- Run it and refer to chapter 4.4

You could also Drag `/Applications/Opentrack-Configure-Protocol.exe` from the Finder into the CrossOver-Window while a specific Bottle is selected.

### 4.2 Homebrew
If you only use the default Wine-prefix at `~/.wine` then simply open `/Applications/Opentrack Wine Bridge.exe` via Finder, or in a Terminal execute `wine /Applications/Opentrack\ Bridge\ Bridge.exe` and refer to chapter 4.4.

### 4.3 Other Wine environments

With Homebrew or MacPorts it's easy: open the Terminal and execute:	`WINEPREFIX=<your-wine-prefix> wine /Applications/Opentrack\ Protocol\ Config.exe` and refer to chapter 4.4.

For everything else you need to figure out yourself how to execute an EXE, that does not happen to be the primary executable (the game). I might provide more information in future.


### 4.4 Using _Opentrack Wine Bridge.exe_

This is Windows program allows you to enable/disable head-tracking for the Wine-prefix/Bottle it's being run in. While running it will furthermore make the headpose available to the Windows game being run in the same Wine-preifx/Bottle. If all you want is to **always** use heead-tracking then running it should suffice - no configuration necessary. If you need to choose specfific head-tracking protocols or you want to NOT use head-tracking from time to time read on...

#### 4.4.1 Settings

Open the Settings-Dialog by clicking the "Settings" button.

- **To use head-tracking** it's recommended that you enable all protocols by opening the Settings-Dialog: Select "Both (recommended)" in the protocol dropdown and hit Save.

- **To NOT use head-tracking** chose "None" and hit Save to permanently clear the registry keys. This way the Windows game will think there's no head-tracking installed.

If you only want to temporarily use head-tracking while playing a particular game, then I recommend you confiugure the protocol(s) of your choice (again "Both" is recommended), but check the "Clear on exit" option. This way the the registry keys are cleared when the program terminates. Once you start it again they're being reestablished. In this case it's crucial that you do it before you start your game.

##### FPS
You can configure the rate at which the headpose is being copied to the Windows game. The default value is set to 25, but you might want to change it so it matches the rate at which opentrack is tracking your movements and/or the FPS at which your game runs.

## 5 Replacing the demo with the full version
If you had installed the demo version and purchased the unrestristed version, then all you need to do is to install the new .pkg file. No need to prepare your Wine environment(s) again. They'll see the new files now, since they're only referenced via symbolic links.

## 6 Uninstalling

### 6.1 Uninstall from CrossOver, Homebrew or  other app bundles

1. Use _Opentrack DLL Manager.app_ to uninstall the DLLs from the Wine environment.
2. Use _Opentrack Wine Bridge.exe_ to remove the registry keys from all wine-prefixes / CrossOver-Bottles. See chapter 4.4.1.

###  6.2 Uninstall from MacPorts
In a Terminal execute `sudo /usr/local/bin/otrwine uninstall /opt/local/lib/wine`.

### 6.3 Uninstall the remaining tools and software

If you want to also completely remove the whole Opentrack Wine Support software package, then delete the following directories and files AFTER having performed the above steps.

Remove The following files/folders

- /Applications/Opentrack DLL Manager.app
- /Applications/Opentrack Wine Bridge.exe
- /usr/local/bin/otrwine
- /Library/Application Support/opentrack-wine-bridge



