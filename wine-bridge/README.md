# Opentrack Wine Bridge

This is an alternative approach to get Opentrack with Windows-Games running via Wine. There are known issues  particularly on macOS that made me create all this. It consist of a set of custom so-called Wine builtin DLLs, accompanying unix libraries, and tools to facilitate the installation and setup.

The linux build happens to work fine on Linux as well. I successfully tested it with Steam's Proton and a few games. I intent to support Linux as well, but for now macOS and CrossOver is my primary focus.

You can also find this README at `/Library/Application Support/opentrack-wine-bridge/README.html` and the latest version [online](https://github.com/matatata/opentrack/blob/develop/wine-bridge/README.md).


## Disclaimer
It may or it may not work with your game or Wine installation! I cannot know! That's why I you should try the demo version first. If it works for you and with the games you play, please consider to buy the unrestricted version, or uninstall/disable it otherwise (see chapter 6). I decided to charge a small amount for the prebuilt binaries, since it's been an incredible amount of work and developing/distributing software for macOS really does cost money.

## Credits
This alternative wine integration has been created and brought to you by [matatata](https://matatata.github.io)

All of it would not be possible without

- Stanislaw Halik (opentrack)
- Wim Vriend, Ron Hendriks (FreeTrackNoIR)
- uglyDwarf (linuxtrack)


# Instructions for macOS

## Getting started - I don't want to read the whole README just yet

1. Install the .pkg file  - allow it to install DLLs into your Wine environment(s) (CrossOver / homebrew's Wine).

    - Optionally or if necessary use the __Opentrack DLL Manager.app__ to prepare your Wine environments yourself.
1. Enable head-tracking for individual Wine-prefixes/Bottles by running __Opentrack Protocol Config.exe__ via Wine in the context of that particular Wine-prefix/Bottle. Enable both protocols.

2. Run the mac Opentrack.app and use the WINE/X-Plane output-module, but don't bother configuring the Wine-settings, but set it to "X-Plane"-mode. That will become less counterintuitive in a future release of MacOpentrack. Start tracking.

3. Start you game. It'll likely pick up the headpose.

__Opentrack DLL Manager.app__ and __Opentrack Protocol Config.exe__ are located in the Applications-folder.

## 1 Installation

1. Install the .pkg package you find on the disk image
2. If you have CrossOver or Homebrew's Wine already installed (at the default locations), then the installer will ask you, if it may prepare those Wine environments right away. You can perform the steps yourself later (see chapter 2), but it's convenient to let it do that for you.

## 2 Preparation of Wine-environment(s)

For Opentrack to be able to directly pass the headpose to windows-games running via Wine, the Wine environment must use the provided builtin DLLs instead of the windows-native DLLs of the head-tracking protocol's libraries. For some Wine-environments using the environment Variable WINEDLLPATH is enough, but the most reliable way is to create symbolik links at the right places. We'll simply refer to this process as _installing DLLs_, althought we're actually creating symbolic links to various library-files located at `/Library/Application Support/opentrack-wine-bridge/lib/wine`.

**Note that when your Wine environment is updated, you most likely will need to reinstall Opentrack's DLLs as described in this chapter - or by installing the .pkg if it was sufficient at the first time.**

*If you only use CrossOver or Homebrew and you did not skip step 2 of chapter 1 then you can continue with chapter 3*

### 2.1 App bundles such as CrossOver.app or Homebrew's Wine*.app
1. Open _Opentrack DLL Manager.app_ that you should find in the `/Applications` folder.
2. If it asks navigate to your Wine environment, or use File->Open in the Menu.
	- _CrossOver.app_ will be in `/Applications` or `/Users/<yourname>Applications`
	- Homebrew's _Wine Devel.app_, _Wine Stable.app_ or _Wine Staging.app_ are in `/Applications`
3. The app will show you whether the DLLs are alreay installed or not. Click the Install-Button to install them if necessary

### 2.2 MacPorts's Wine or other environments

In a Terminal execute `sudo /usr/local/bin/otrwine install /opt/local/lib/wine`.

## 3 Preparation of a Wineprefix / CrossOver Bottle

The installation of the DLLs is not enough. We need add some values to the Windows-registry, so that games know that you want to do head-tracking and by usgin what protocol(s). Opentrack's classic WINE-Module tries to do that for you, but it does not work with CrossOver and will not be supported in futuere. Therefore you'll learn how to deal with this youself:

### 3.1 CrossOver

- Open CrossOver and select the Bottle you want to use head-tracking with. Use the "Run Command" button in the top right corner or the Command-R Key-Command.
- Browse to /Applications/Opentrack-Configure-Protocol.exe
- Press "Save command as Launcher" so it's easier to run it the next time you need it. It even shows up in Spotlight, since CrossOver creates a little macOS application bundle for it.
- Run it and refer to chapter 3.4

You could also Drag `/Applications/Opentrack-Configure-Protocol.exe` from the Finder into the CrossOver-Window while a specific Bottle is selected.

### 3.2 Homebrew
If you only use the default Wine-prefix at `~/.wine` then simply open `/Applications/Opentrack Protocol Config.exe` via Finder, or in a Terminal execute `wine /Applications/Opentrack\ Protocol\ Config.exe` and refer to chapter 3.4.

### 3.3 Other Wine environments

With Homebrew or MacPorts it's easy: open the Terminal and execute:	`WINEPREFIX=<your-wine-prefix> wine /Applications/Opentrack\ Protocol\ Config.exe` and refer to chapter 3.4.

For everything else you need to figure out yourself how to execute an EXE, that does not happen to be the primary executable (the game). I might provide more information in future.

### 3.4 Using _Opentrack Protocol Config.exe_

This is Windows program allows you to enable/disable head-tracking for the Wine-prefix/Bottle it's being run in.

#### 3.4.1 Enabling head-tracking protocols
It's recommended that you enable all protocols by selecting "Both (recommended)" in the protocol dropdown, hit Save and then Close. Once configured, your game should pick up MacOpentrack's headpose and you do not have to use the utilty again, unless you don't always want to use head-tracking. If you only want to temporarily use head-tracking, then I recommend you confiugure the protocols, and check the "Disable all protocols on close" options. You leave it open, until you're done playing.

#### 3.4.2 Disabling head-tracking protocols
Select "None" in the protcol dropdown, hit Save and then Close.

## 4 Configure the Octopus for usage with Wine
- Open the latest release of the Opentrack application (currently version `2024.1.1-matatata.1`) and select or create a suitable profile.
- Select the WINE/X-Plane output-protocol. But we'll actually configure it to NOT use Wine at all, but instead set it up as if we wnated to use X-Plane: Use the wrench-icon to open the module's option-dialog and select the Radio-Button "X-Plane (disabled)". In a future version of MacOpentrack this will be changed and less counterintuitive.


## 5 Installing the full version
If you had installed the demo version and now you've purchased the unrestristed version, then all you need to do is to install its .pkg file. No need to prepare your Wine environment(s) again. They'll see the new files now, since they're only referenced via symbolic links.

## 6 Uninstalling

### 6.1 Uninstall from CrossOver, Homebrew or  other app bundles

1. Use _Opentrack DLL Manager.app_ to uninstall the DLLs from the Wine environment.
2. Follow chapter 3.4.2 to remove the registry keys from all wine-prefixes / CrossOver-Bottles.

###  6.2 Uninstall from MacPorts
In a Terminal execute `sudo /usr/local/bin/otrwine uninstall /opt/local/lib/wine`.

### 6.3 Uninstall all the tools and stuff

If you want to also completely remove the whole Opentrack Wine Support software package, then delete the following directories and files AFTER having performed the above steps.

Remove The following files/folders

- /Applications/Opentrack DLL Manager.app
- /Applications/Opentrack Protocol Config.exe
- /usr/local/bin/otrwine
- /Library/Application Support/opentrack-wine-bridge



