------------------------------------------------------------------------------
OpenCity Neo
------------------------------------------------------------------------------

<p align="center">
    <img src="screenshots/screen01.png" width="600px">
</p>

OpenCity Neo is a fork from OpenCity (http://www.opencity.info).
Is a city simulator game. You must build a city with three zones, 
Residential, Commercial and Industrial.
Each one depends of the other for its developer.
Try to give them what they need and your city grow up.

We like that OpenCity Neo have graphic quality, new features and
it be an online game.


------------------------------------------------------------------------------
Software requirement:
------------------------------------------------------------------------------
Operative system:
	Linux (recommended)
	Mac OS
	Microsoft Windows XP/2000

Library:
	GL (OpenGL)
	GLU
	SDL
	SDL_image
	SDL_net
	SDL_mixer (optional)


------------------------------------------------------------------------------
Hotkeys
------------------------------------------------------------------------------
		n: Clear the current tool
		r: Residential zone
		c: Commercial zone
		i: Industrial zone
		p: Build paths
		l: Electric line
		x: Destroy buildings
		q: Object query
		e: Power plant
		ctrl: Clear zones
	
		u: Ground level up

Navigation:
		up / down / left / right: Move the map
		w / s / a / d: Move the map
		pageup / pagedown: Map rotate
		insert / sup: Zoom in/out
		alt: Multiply the effect of buttons for 10
	
		h: Move the map to home
		escape: Clear the current tool. Open/Close the main menu

Options:
		b: Show/Hide buildings
		g: Show/Hide the grid
		k: Show/hide the main state bar
		f: Show wired graphics
		o: Projection Orthogonal/Perspective
		t: Show/Hide ground

Sound and music:
		< / >: play previus / play next
		m: On/Off music
		s: On/Off sound

Experimental:
		F2: Save
		F6: Load

Features without documents, it can be risky:
		v: probe MAS of the menu
		z: Connect to local server ZeN


------------------------------------------------------------------------------
Sound & music
------------------------------------------------------------------------------
	OpenCity uses SDL_mixer for playing sound & music. Thus, it supports
all file types that SDL_mixer supports.

	If you don't hear any sound, try to modify "sound.m3u" and
set the path accordingly to the "opencity" executable file. Please note
that OpenCity automatically appends a prefix to all the sound filenames.
The prefix is where the installation procedure has put all OpenCity's data
files.

	However, you can put a file called "music.m3u" in the "music" directory
and OpenCity will load and play your favorite musics. If you don't find
any "music" directory just create it then put your list "music.m3u" in it.

	I'd like to warn you that SDL_mixer is not that stable. It can hang
OpenCity while playing few kinds of ".mp3" song. Blame the SDL_mixer's 
guys not me ;) If you experience such a problem, just turn the music 
off (key 'm').


------------------------------------------------------------------------------
Mouse action
------------------------------------------------------------------------------
	In OpenCity, whenever you click the RMB (Right Mouse Button), you will
open the toolcircle. Click RMB again to hide the toolcircle. I try to make
everything simple and handy. But sometimes you still need your keyboard.

	OpenCity also understands the mouse's wheel ! Push the wheel forward
does a zoom in (as 'Insert' key) and roll the wheel backward does a
zoom out (as 'Delete' key).

	When you hold 'Shift' and play with your mouse's wheel you'll notice
that it translates the map in some direction. Whereas the 'Ctrl' button
translates the map along another axis.


------------------------------------------------------------------------------
Main config file "opencity.xml"
------------------------------------------------------------------------------
	See the in-line comments for more details.


------------------------------------------------------------------------------
Command-line options
------------------------------------------------------------------------------
	In OpenCity, you can overwrite the config file's options with the
command-line options. Few options are available in both short and long format.

	-fs | --full-screen
	Enable fullscreen mode. The video resolution is read from the config.xml
file. If the resolution width and height are both 0, OpenCity will try to
autodetect the best available fullscreen resolution.

	-glv | --gl-version
	Show the OpenGL driver information (vendor, renderer, version and extension)

	-gld | --gl-driver openGLDriverName
	Try to load the given driver name. Under Windows, it should be something
like opengl32.dll	

	-dd | --data-dir newDataPath
	Specify to new path to OpenCity's data. For example:
		Under linux: --data-dir /home/foo/bar/share/opencity/
		Under win32: --data-dir "C:/Program Files/OpenCity/share"

	-cd | --conf-dir newConfigPath
	Specify to new path to OpenCity's config. For example:
		Under linux: --conf-dir /home/foo/bar/etc/opencity/
		Under win32: --conf-dir "C:/Program Files/OpenCity/etc"

	-na | --no-audio:
	Disable the audio system

	--generator-seed seed
	The seed is the random integer seed used by the map generator

	--generator-map MAP-TYPE
		MAP-TYPE         : 0=plain (default), 1=hill, 2=mountain

	--generator-water WATER-TYPE
		WATER-TYPE       : 0=dry, 1=lake (default), 2=coast

	--generator-map-shape MAP-SHAPE-TYPE
		MAP-SHAPE-TYPE   : 0=none (default), 1=island, 2=volcano, 3=crater

	--generator-tree-density TREE-DENSITY-TYPE
		TREE-DENSITY-TYPE: 0=sparse (default), 1=normal, 2=dense

	--generator-height-map heightMapPicture
	Load a gray-level picture as height map (PNG). The water level is gray
level 128, each map level is one gray level. That's why the whole picture seems
to be in a medium gray, because there's only a minor difference between the
pixels. You can draw yourself the height-map or generate from any map sources.
The interesting data for this work are the DEM satellite data. The map
generator in OpenCity will crop the picture if it's too big.


------------------------------------------------------------------------------
Heightmap package
------------------------------------------------------------------------------
	OpenCity is packaged with few heightmaps which are generated from
satellite's data. Here is the full list:

	Athenes-1024x1024.png
	Athenes-51x51.png
	Dakar-51x51.png
	Ibiza-101x101.png
	Istanbul-1025x1025.png
	Mallorca-93x61.png
	Palermo-51x51.png
	Sao_Tome-101x101.png
	Strasbourg-1025x1025.png
	Woodstock-51x51.png

	In order to load those heightmaps, you have to give a command-line option
to OpenCity. For example:

	opencity --generator-height-map map/heightmap/Athenes-51x51.png

	Currently, heightmap loading still is an experimental feature, try 
at your own risks.


------------------------------------------------------------------------------
Save/Load issues
------------------------------------------------------------------------------
	The following directory is used for save/load

		* GNU/Linux:
			$HOME/.OpenCity/

		* Windows:
			C:\Documents and Settings\username\Application Data\OpenCity\
			If you are running a localized version of Windows, the above
		path may be different

		* Windows Vista:
			C:\Users\username\AppData\Roaming\OpenCity\


------------------------------------------------------------------------------
Known issues
------------------------------------------------------------------------------
	In the ExtentensionManager, GCC complains about the ISO C++ casting
with the following message:

	"warning: ISO C++ forbids casting between pointer-to-function and
pointer-to-object"

	Currently, there's nothing that we can do about this because it's a
limitation of the C++ standards.


------------------------------------------------------------------------------
Troubleshootings
------------------------------------------------------------------------------
	1) I have a ATI Radeon 9000 M video card on my machine.
When I tested the game with "EnablePageFlip" ( see man radeon )
the game works weirdly. If you encounter the same problem,
try to disable this feature of the XFree server ( see man XF86Config )

	2) With ATI Radeon 9000M video card, the SmoothVision(tm)
must be deactivated in order to run OpenCity correctly.

	3) Here is the version of my X server

 - XFree86 Version 4.3.0
 - Release Date: 9 May 2003
 - X Protocol Version 11, Revision 0, Release 6.6
 - Build Operating System: Linux 2.4.18-23mdksmp i686 [ELF]
 - Build Date: 10 September 2003

	It seems that the driver for my ATI Radeon 9000 M is
very buggy. If you experience any trouble, try to type this
in the shell (bash) before launching OpenCity.
		"export R200_NO_VTXFMT="

	Or you should try a newer XFree.


------------------------------------------------------------------------------
Duong Khang (neoneurone) NGUYEN


