[Log2Log Chat Log Converter]

╔═══════════════════════════════════════════╤═╤═╤═╗
║Information                                │-│■│X║
╟───────────────────────────────────────────┴─┴─┴─╢
║Version: 1.0.0 (v1.0.0)                          ║
║Website: http://www.deltik.org/                  ║
║Information: http://log2log.sf.net/              ║
║E-Mail: deltik@gmx.com                           ║
║Release Date: 2011/10/01                         ║
╚═════════════════════════════════════════════════╝

▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜
▌ CONTENTS OF THIS FILE ▐
▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟
 * About Log2Log
 * Requirements and Notes
 * Installation
 * Configuration and Features
 * Known Problems
 * More Information
 * Version History


┏━━━━━━━━━━━━━━━┓
┃ ABOUT LOG2LOG ┃
┗━━━━━━━━━━━━━━━┛
Log2Log Chat Log Converter, often simply called "Log2Log", is a desktop and
console client that converts chat logs to another format.

Legal information about Log2Log:
 * Know your rights when using Log2Log:
   See LICENSE.txt in the same directory as this document.


┏━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ REQUIREMENTS AND NOTES ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━┛
Log2Log requires:
  - Any of the following operating systems:
    - Microsoft Windows
    - Mac OS X
    - Linux

Log2Log recommends for the best functionality:
  - A processor with multiple cores or multi-threading.  For single-core
    processors, Log2Log may decrease the performance of your computer for the
    entire duration of format conversion.


┏━━━━━━━━━━━━━━┓
┃ INSTALLATION ┃
┗━━━━━━━━━━━━━━┛
Install Log2Log in three easy steps:
  ┌─────────┐
  │ WINDOWS │
  └─────────┘
  1. Open "Setup.exe".
  2. Follow the installer instructions.
  3. There is no step 3.

  ┌──────────┐
  │ MAC OS X │
  └──────────┘
  1. Open the Log2Log DMG file.
  2. Drag the Log2Log application into your Applications directory.
  3. There is no step 3.
  
  ┌────────────┐
  │ UBUNTU PPA │
  └────────────┘
  1. sudo add-apt-repository ppa:deltik/loguntu
  2. sudo apt-get install log2log
  3. There is no step 3.
  
  ┌───────────────────┐
  │ DEBIAN/UBUNTU DEB │
  └───────────────────┘
  1. Open the Log2Log DEB file.
  2. Confirm the installation.
  3. There is no step 3.
  
  ┌────────────────────┐
  │ FEDORA/RED HAT RPM │
  └────────────────────┘
  1. Open the Log2Log RPM file.
  2. Confirm the installation.
  3. There is no step 3.
  
  ┌────────────┐
  │ LINUX/UNIX │
  └────────────┘
  If you downloaded the Log2Log executable, you have it.  To make "log2log" a
  command, move the executable to /usr/bin .


┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ CONFIGURATION AND FEATURES ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
Log2Log v1.0.0 does not have any configurable options.  Is there something you
want to be configurable?  Suggest it to the project manager (email address in the
"More Information" section)!

┌───────────────────┐
│ FORMAT CONVERTERS │
└───────────────────┘
Here is an informational table about Log2Log's format converters:

NAME			FROM?	TO?	SUPPORT		INTRODUCED
Adium			no	no	planning	
AIM			yes	no	stable		v1.0.0
Digsby			no	no	considered
Empathy			no	no	planning	
Facebook (dl)		no	never	planning	
Gmail Chats (dl)	no	never	planning	
Google Voice (dl)	no	never	planning	
imo.im (dl)		no	never	planning	
Kopete			no	no	considered	
Log2Log JSON		yes	yes	stable		v0.0.1a1
Meebo (dl)		no	never	pre-alpha	v0.0.1a1
Meebo			yes	yes	stable		v0.0.1a1
Minecraft Server	no	no	dev		
Omegle			yes	yes	stable		v1.0.0
Pidgin HTML		yes	no	alpha		v0.0.1a1
Skype			no	never	planning	
Trillian		yes	yes	beta		v1.0.0
WLM			yes	no	stable		v1.0.0

  ┌──────┐
  │ BUGS │
  └──────┘
  - Omegle : Might create extraneous empty logs
  - Pidgin HTML : Poor timestamp support; don't use this format converter yet.
  - Trillian : Group chats not supported yet

  ┌─────────┐
  │ REMARKS │
  └─────────┘
  - AIM : You don't want to switch to AIM ...
  - Log2Log JSON : This dumps the converted chat logs into one Log2Log standard
      chat log.
  - Meebo (dl) : Downloads chat logs from https://www.meebo.com/
  - Meebo : Filenames made in this format are structured to contain metadata,
      but the contents are exactly like raw Meebo chat logs.


┏━━━━━━━━━━━━━━━━┓
┃ KNOWN PROBLEMS ┃
┗━━━━━━━━━━━━━━━━┛
There are no known problems in the Log2Log client, but in general, format
converters may have issues.  For specific information about format converter
compatibility, see the "Configuration and Features" section.

1. Timezone compatibility is very poor, and fixes are intended to be made.

2. Converting chat log formats is no easy task. Note that Log2Log converters
   might not be 100% successful at converting chat logs.

3. Time zone compatibility is rough as of v1.0.0, but fixes are underway.

4. Languages other than English have not been figured out for format
   converters as of v1.0.0.  The Log2Log team would greatly appreciate it if
   you could design a translations system for us. :)

5. This project would like help.  If you would like to contribute to Log2Log in
   any way, we would greatly appreciate it!  Chat log donors, C++ programmers,
   graphics designers ...


┏━━━━━━━━━━━━━━━━━━┓
┃ MORE INFORMATION ┃
┗━━━━━━━━━━━━━━━━━━┛
For more information, updates, and just... more..., visit Deltik's Web site
at <http://www.deltik.org/>.

Also, for Log2Log-specific information, you can visit Log2Log's Web site at
<http://log2log.sourceforge.net/>.

You can contact the project manager, Nick Liu, too: <deltik@gmx.com>


┏━━━━━━━━━━━━━━━━━┓
┃ VERSION HISTORY ┃
┗━━━━━━━━━━━━━━━━━┛
1.0.0 (2011/10/01)
  - NEW: Initial Release

1.0.0dev (2011/06/23)
  - Log2Log as a desktop application: Development planning

0.0.1a4 (2011/06/23)
  - INF: The Log2Log PHP project is being discontinued.
  - FIX: Fixed form security issue of the MeeboConnect interface by switching to POST
  - MOD: MeeboConnect interface allows for capture threshold setting
  - MOD: MeeboConnect interface form submits relative to its location
  - DEV: Added debug information for importing chat log structure
  - PRE: Google Talk is now under consideration.

0.0.1a3 (2011/03/28)
  - NEW: Option to include conversion debug logs in the generated archive
  - FIX: Pidgin.php: Improved support for conversation events
  - FIX: For chat log developers, the template file had a function correction.
  - MOD: Log2Log Standard Chat Log Format now stores timezone definitions.

0.0.1a2 (2011/03/27)
  - NEW: JSON.php: Direct export of the Log2Log Standard Chat Log Format in JSON
  - NEW: Debug error handler
  - FIX: Pidgin.php: Prevented processing chat log entry if corrupt
  - MOD: core.php: Incompatible archive types now pass warnings instead of errors.

0.0.1a1 (2011/03/19)
  - Initial Release

0.0.1dev (2011/01/07)
  - Development Layout

0.0.1pre (2011/01/01)
  - Log2Log Project Founded
