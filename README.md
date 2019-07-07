# PD Loader

**PD Loader** lets you load plugins into **PDAFT** by placing them into a **"plugins"** folder, it includes useful ones such as input emulation and a graphical launcher.

## License and Legal

The code included in this repository is intended for the solely usage of investigative reasons for the **PDAFT** videogame.
Based on your current jurisdiction, it may be illegal to tamper with an arcade cabinet or its hardware and so it would be the usage of this software.
**You must have your own legal dump of the game to use this software.**
Additionally, a MIT License is provided.

## Installation

* Have a **full clean** game version **"7.10"** decompressed without any modifications.
* Download the latest release of **PD Loader**.
* Decompress everything into the **root folder** of the game.

* Install the official **"mdata"**.
**M215** to **M270** are supported, custom ones are also supported.

To install them you have to drop each **"mdata"** update decompressed into the **"mdata\"** folder in the root folder of the game (create it if it doesn't exists).

**e.g.** For all the official **"mdata"** you should have the following folders inside:
  * M215
  * M220
  * M230
  * M240
  * M250
  * M260
  * M270

Files required to make **"mdata"** available in the game are included (**"ram\databank\PvList*"** files).

## Usage

**IMPORTANT:** Do *not* edit any of the *.ini* configuration files with Notepad. Use [Notepad++](https://notepad-plus-plus.org/download) (or WordPad) with UTF-8 encoding instead, otherwise your game may crash.

Use **"plugins\components.ini"** to enable or disable TLAC components.
Use **"plugins\config.ini"** to enable or disable plugins loading along other useful settings.
Use **"plugins\keyconfig.ini"** to change your game keybinds.
Use **"plugins\playerdata.ini"** to change your playername and other player customization.

Place any **.dva** plugin you want to use inside the **"plugins"** folder.

Launch **"diva.exe"**.
Use **"--launch"** argument to bypass the launcher and to be able to use other arguments.

## Modules

Edit **"plugins\playerdata.ini** with [Notepad++](https://notepad-plus-plus.org/download) (or WordPad) and set use_card to true.

## Sound

If you do not hear any audio from the game, please install [DivaSound](https://github.com/somewhatlurker/DivaSound).

## Screenshots

Your **root** game folder should look like this:

![root game folder](https://i.imgur.com/c57lQPH.png)

Your **"mdata"** folder should look like this:

![mdata folder](https://i.imgur.com/2jUjosM.png)

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

MIT (see "LICENSE").

## Third party plugins

[DiscordDiva](https://github.com/bela333/DiscordDiva) - Discord integration by **bela333**.

[DivaSound](https://github.com/somewhatlurker/DivaSound) - Replacement audio output by **somewhatlurker**, this plugin is a must if you have audio issues or no audio at all.

## FAQ

Only **NVIDIA** videocards are supported for now, AMD and Intel GPU support is possible but further research needs to be done.

**Linux** can run the game with **Wine**, for audio you must use [DivaSound](https://github.com/somewhatlurker/DivaSound).
WMV movie videos are not supported and it is recommended to set **No_Movies** to **1** to prevent crashes when the game attempts to load a movie.

**Windows 7** is supported except for WMV movie videos.

Previous versions used **dinput8.dll** in the game root folder, if you still have it you must delete it.

Always use the original game executable.

Special thanks to **Brolijah**, **Skyth**, **korenkonder**, **Samyuu**, **somewhatlurker**, **Cainan**, **crash5band**, **ano**, **lybxlpsv** and everyone else who helped at the [Project DIVA Modding 2nd Community Discord](https://discord.gg/cvBVGDZ), come join us if you want to help us to improve this project.

## Developer FAQ

To enable logs and dumps create a folder called "logs" in the root folder, each dump size is 200MB so watch your disk space.

This is a stripdown of [**ThirteenAG's** Ultimate ASI Loader v4.47](https://github.com/ThirteenAG/Ultimate-ASI-Loader/) for **"dnsapi.dll"** usage only.

Solution for **Visual Studio 2019**.
Requires platform toolset **v142**.
Requires **Windows 10 SDK (10.0.17763.0)**.

Requires **Desktop development with C++**.
Requires **Windows Universal CRT SDK**.

Requires **.NET desktop development** (Launcher plugin).
Requires **.NET Framework 4 targeting pack** (Launcher plugin).
Requires **C++/CLI support for v142 build tools (14.21)** (Launcher plugin)

Requires [**Microsoft Research Detours Package**](https://github.com/microsoft/Detours) (included in the solution).
Requires [**FreeGLUT 3.0.0 PD**](https://github.com/Rayduxz/FreeGLUT) (included in the solution).
