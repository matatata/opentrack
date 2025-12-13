# Opentrack Wine Bridge

This is an alternative approach to get native Opentrack working with Windows-Games running via Wine. There are [known issues](https://trac.macports.org/ticket/72792) particularly on macOS that made me create all this. It consist of a set of custom so-called Wine builtin DLLs, accompanying unix libraries, and tools to facilitate the installation and setup.

The linux build happens to work fine on Linux as well. I successfully tested it with Steam's Proton and a few games. I intend to support Linux as well, but for now macOS and CrossOver is my primary focus.

You can also find this README at `/Library/Application Support/opentrack-wine-bridge/README.html` and the latest version [online](https://github.com/matatata/opentrack/blob/master/wine-bridge/README.md).


## Disclaimer
It may or it may not work with your game or Wine installation! I cannot know! That's why you should try the demo version first. If it works for you and with the games you play, please consider purchasing the unrestricted version, or uninstall/disable it otherwise (see chapter 6). I decided to charge a small amount for the prebuilt binaries, since it's been an incredible amount of work, and developing/distributing software for macOS really does cost money.

## Credits
This alternative wine integration has been created and brought to you by [matatata](https://matatata.github.io).

All of it would not be possible without software created by:

- Stanislaw Halik (opentrack)
- Wim Vriend, Ron Hendriks (FreeTrackNoIR)
- uglyDwarf (linuxtrack)

Many thanks to [Bharat Iyer](https://twitter.com/bvsveera) for testing and contributing to this documentation.

# Instructions for macOS

## Getting started - I don't want to read the whole README just yet

1. Install the .pkg file  - allow it to install DLLs into your Wine environment(s) (CrossOver / homebrew's Wine).
    - Optionally, or if necessary, use the __Opentrack DLL Manager.app__ to prepare your Wine environments yourself. You only need to do this once, or when your Wine environment has been updated or reinstalled.
2. Run __opentrack.app__. If you don't have it already, get MacOpentrack [here](https://matatata.gumroad.com/l/macopentrack). Configure your profile to use the Wine/X-Plane output module, then open the module's settings pane and select the "Disabled (X-Plane)" option. This will become less counterintuitive in a future release of MacOpentrack. Start tracking.
3. Run __Opentrack Wine Bridge.exe__ via Wine in the context of the Wine-prefix/Bottle in which you installed your game. Don't close this program.
4. Start your game. It should pick up the headpose data.
5. Close  __Opentrack Wine Bridge.exe__ once you're done playing.

__Opentrack DLL Manager.app__ and __Opentrack Wine Bridge.exe__ are located at `/Applications`.

## 1 Installation

1. Install the .pkg package located on the disk image
2. If you have CrossOver or Homebrew's Wine already installed (at the default locations), then the installer will ask if it can prepare those Wine environments automatically, for your convenience. Should you choose not to accept this, or if you have Wine applications installed in other locations (e.g. CrossOver Preview.app), you can perform these steps yourself later (see chapter 2).

## 2 Preparation of Wine-environment(s)

For __Opentrack Wine Bridge.exe__ to be able to pass opentrack's headpose to Windows games, it needs to load the provided otrclient.dll and its accompanying unix library. For some Wine environments, using the environment variable WINEDLLPATH is enough, but the most reliable way is to create symbolic links at the right places. We'll simply refer to this process as _installing DLLs_, although we're actually creating symbolic links to various library files located at `/Library/Application Support/opentrack-wine-bridge/lib/wine`.

**Please note that whenever you update your Wine environment(s), you will likely need to reinstall Opentrack's DLLs again, as described below - or by installing the .pkg, if it was sufficient the first time.**

*If you only use CrossOver or Homebrew and you did not skip step 2 of chapter 1 then you can proceed to chapter 3*

### 2.1 App bundles such as CrossOver.app or Homebrew's Wine*.app
1. Open _Opentrack DLL Manager.app_ located within the `/Applications` folder.
2. If it asks, navigate to your Wine environment, or use File->Open in the Menu.
	- _CrossOver.app_ will be in `/Applications` or `/Users/<yourname>/Applications`
	- Homebrew's _Wine Devel.app_, _Wine Stable.app_ or _Wine Staging.app_ are in `/Applications`
3. The app will show you whether the DLLs are already installed or not. Click the Install button to install them if necessary.

### 2.2 MacPorts's Wine or other environments

In a Terminal window, execute `sudo /usr/local/bin/otrwine install /opt/local/lib/wine` for For MacPorts's Wine. For other Wine environments you need to figure out the destination path yourself.

## 3 Configure the Octopus for usage with Wine
- Open the latest release of the Opentrack application (currently version `2024.1.1-matatata.1`), and select or create a suitable profile.
- Select the Wine/X-Plane output protocol. To configure this for use with Wine, click the hammer icon to open the module's settings pane, then select the "Disabled (X-Plane)" radio button, followed by the OK button. This step will be updated in a future version of MacOpentrack to be less counterintuitive.


## 4 Before starting your Game

The installation of the DLLs is not enough. We need add some values to the Wine bottle's Windows Registry, so that the games know that you want to use head tracking, and through which protocol(s) you will be accessing it. Furthermore, opentrack's headpose data needs to be copied to where the Windows game expects it. Opentrack's classic WINE module tries to do exactly that, but it does not work with CrossOver, among other issues. This is why I decided to create __Opentrack Wine Bridge.exe__ as an alternative, and YOU have to make sure this program is open each time you want to use head tracking. It may not be as convenient as before, but in order to support as many Wine environments as possible, this is currently the most straightforward solution. I'll explain how to launch __Opentrack Wine Bridge.exe__ in some popular Wine environments:

### 4.1 CrossOver

- Open CrossOver and select the Bottle you want to use head tracking with. Click the "Run Command" button in the top right corner, or use the Command-R keyboard shortcut.
- Browse to `/Applications/Opentrack Wine Bridge.exe`
- Click "Save command as Launcher" to create a shortcut to this program. It even shows up in Spotlight, since CrossOver creates a little macOS application bundle for it.
- Launch it, then proceed to chapter 4.4

Note: you can also drag `/Applications/Opentrack Wine Bridge.exe` from the Finder into the CrossOver window while a specific Bottle is selected.

### 4.2 Homebrew
If you only use the default Wine-prefix at `~/.wine`, then simply open `/Applications/Opentrack Wine Bridge.exe` via Finder. Alternatively, if you prefer Terminal, execute `wine /Applications/Opentrack\ Wine\ Bridge.exe`, then proceed to chapter 4.4.

### 4.3 Other Wine environments

With Homebrew or MacPorts, it's easy: open the Terminal and execute:	`WINEPREFIX=<your-wine-prefix> wine /Applications/Opentrack\ Wine\ Bridge.exe`, then proceed to chapter 4.4.

For other environments, you'll need to figure out for yourself how to launch an EXE that does not happen to be the primary executable (the game). I may be able to provide more information in the future.


### 4.4 Using _Opentrack Wine Bridge.exe_

This is a Windows program that allows you to enable/disable head tracking for the Wine-prefix/Bottle it's installed within. While running, it will make the headpose data available to Windows games also running in the same Wine-prefix/Bottle. For most users, all you need to do is to launch the program before launching your game - no further configuration required. If you need to choose specific head tracking protocols, or want to adjust when head tracking is active, read below.

#### 4.4.1 Settings

Open the Settings dialog by clicking the "Settings" button.

- **To enable head tracking**, it is advised to activate all protocols. Open the Settings dialog, select "Both (recommended)" in the protocol dropdown, then click Save.

- **To disable head tracking**, open the Settings dialog, select "None", then click Save. This will permanently clear the registry keys, reverting the Windows game to its pre-head tracking state.

If you only want to temporarily use head tracking, you can configure the protocol(s) of your choice (again, "Both" is recommended), then check the "Clear on exit" option. This way, the registry keys will be cleared when the program is closed. Once you launch it again, the keys will be re-established. In this case, it's crucial that you do it before you start your game.

#### 4.4.2 FPS
You can configure the rate at which the headpose data is being copied to the Windows game. The default value is set to 25, but you can adjust this to match either the rate at which opentrack is operating at, or to the FPS at which your game is running.

## 5 Replacing the demo with the full version
If you installed the demo version, and then purchased the unrestricted version, all you need to do is to install the new .pkg file. No need to prepare your Wine environment(s) again. They'll pick up the new DLL files via the symbolic links created earlier.

## 6 Uninstalling

### 6.1 Uninstall from CrossOver, Homebrew, or  other app bundles

1. Use _Opentrack DLL Manager.app_ to uninstall the DLLs from the Wine environment(s).
2. Use _Opentrack Wine Bridge.exe_ to remove the registry keys from all Wine-prefixes/Bottles (see chapter 4.4.1).

###  6.2 Uninstall from MacPorts
In a Terminal window, execute `sudo /usr/local/bin/otrwine uninstall /opt/local/lib/wine`.

### 6.3 Uninstall the remaining tools and software

If you want to completely remove the Opentrack Wine Bridge software package, perform the above steps first, then delete the following files and directories:

- `/Applications/Opentrack DLL Manager.app`
- `/Applications/Opentrack Wine Bridge.exe`
- `/usr/local/bin/otrwine`
- `/Library/Application Support/opentrack-wine-bridge`



