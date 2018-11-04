[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FDeltik%2FLog2Log.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2FDeltik%2FLog2Log?ref=badge_shield)

[Log2Log Chat Log Converter]

###############################################################################
# !! IMPORTANT INFORMATION ! IMPORTANT INFORMATION ! IMPORTANT INFORMATION !! #
###############################################################################
# The Log2Log v1.x branch has been deprecated.  This is the final release of  #
# Log2Log v1.x (v1.0.7) as of 23 March 2015.  You may ask for a customized    #
# release; Deltik will help you support new format converters if you ask.     #
#                                                                             #
# Development is paused due to a lack of interest in the Log2Log project and  #
# many kinds of technical difficulties and impracticalities.  It's            #
#   - ugly (poor coding practices),                                           #
#   - inflexible (no foreign language support),                               #
#   - inextensible (no plugin support),                                       #
#   - bulky (unoptimized),                                                    #
#   - and leaky (no memory management whatsoever!).                           #
#                                                                             #
# A Log2Log v2.x branch is planned, but there are not enough human resources  #
# to make the project possible.                                               #
#                                                                             #
# Version 2 is planned to                                                     #
#   - improve memory management drastically,                                  #
#   - take advantage of multiple threads (GUI, reading source data,           #
#     converting to destination format, saving converted data),               #
#   - have multilingual support,                                              #
#   - and have format converters in a modular plug-in design.                 #
#                                                                             #
# The Loguntu project might also be started along with Log2Log v2.x.  Loguntu #
# is planned to be the ultimate digital life interface for all personal data. #
# It looks really awesome on paper, but this readme isn't the place to        #
# describe it.                                                                #
#                                                                             #
# If you're interested in the Loguntu project or seeing the Log2Log project   #
# go further, just let us know at <http://loguntu.com/>.                      #
# (The link works as of 23 March 2015; I'll try to keep it up for a while.)   #
#                                                                             #
# And there's always <http://www.deltik.org/>. :)                             #
###############################################################################

╔═══════════════════════════════════════════╤═╤═╤═╗
║Information                                │-│■│X║
╟───────────────────────────────────────────┴─┴─┴─╢
║Version: 1.0.7 (v1.0.7)                          ║
║Website: http://www.deltik.org/                  ║
║Information: http://log2log.sf.net/              ║
║E-Mail: deltik@gmx.com                           ║
║Release Date: 2015/03/23                         ║
╚═════════════════════════════════════════════════╝

@@@@@@@@@@@@@@@@@@@@@@@@@
@ CONTENTS OF THIS FILE @
@@@@@@@@@@@@@@@@@@@@@@@@@
 * About Log2Log
 * Requirements and Notes
 * Installation
 * Configuration and Features
 * Known Problems
 * More Information
 * Version History


+++++++++++++++++
+ ABOUT LOG2LOG +
+++++++++++++++++
Log2Log Chat Log Converter, often simply called "Log2Log", is a desktop and
console client that converts chat logs to another format.

Legal information about Log2Log:
 * Know your rights when using Log2Log:
   See LICENSE.txt in the same directory as this document.


++++++++++++++++++++++++++
+ REQUIREMENTS AND NOTES +
++++++++++++++++++++++++++
Log2Log requires:
  - Any of the following operating systems:
    - Microsoft Windows
    - Mac OS X
    - Linux

Log2Log recommends for the best functionality:
  - A processor with multiple cores or multi-threading.  For single-core
    processors, Log2Log may decrease the performance of your computer for the
    entire duration of format conversion.
  - At least 4GB of RAM.  This is due to a terrible code practice that causes
    Log2Log to eat up all the RAM it can allocate.  This probably will NOT be
    fixed in Log2Log v1.0.x due to a lack of support of Log2Log.  Sorry. :(


++++++++++++++++
+ INSTALLATION +
++++++++++++++++
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
  │ LINUX/UNIX │
  └────────────┘
  If you downloaded the Log2Log executable, you can run the program.  To make
  "log2log" a command, move the executable to "/usr/bin".  Make sure to
  `chmod +x log2log`.
  Dependencies can be matched with the command:
    ldd log2log
  Unfortunately, there are no packages (RPM, DEB, etc.) at this time.


++++++++++++++++++++++++++++++
+ CONFIGURATION AND FEATURES +
++++++++++++++++++++++++++++++
Log2Log v1.0.7 does not have any configurable options.  Is there something you
want to be configurable?  Suggest it to the project manager (email address in the
"More Information" section)!

┌───────────────────┐
│ FORMAT CONVERTERS │
└───────────────────┘
Here is an informational table about Log2Log's format converters:

NAME                    FROM?   TO?     SUPPORT         INTRODUCED      RETIRED
Adium                   no      no      planning
AIM                     yes     no      stable          v1.0.0
Digsby                  yes     no      beta            v1.0.3
Empathy                 no      no      planning
Facebook (dl)           no      never   planning
Google+ Hangouts (dl)   no      never   planning
Google Voice (dl)       no      never   planning
imo.im                  no      never   planning
Kopete                  no      no      considered
Log2Log JSON            yes     yes     stable          v0.0.1a1
Meebo (dl)              no      never   pre-alpha       v0.0.1a1        v1.0.1
Meebo (farewell)        yes     no      stable          v1.0.1
Meebo                   yes     yes     stable          v0.0.1a1
Minecraft Server        no      no      planning
Omegle HTML             yes     yes     stable          v1.0.0
Pidgin                  yes     yes     alpha           v0.0.1a1
Skype                   no      no      planning
Trillian                yes     yes     beta            v1.0.0
WLM                     yes     yes     stable          v1.0.0

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
  - Digsby : Returns all timestamps in UTC
  - Log2Log JSON : This dumps the converted chat logs into one Log2Log standard
      chat log.
  - Meebo (dl) : Downloads chat logs from https://www.meebo.com/
  - Meebo : Filenames made in this format are structured to contain metadata,
      but the contents are exactly like raw Meebo chat logs.


++++++++++++++++++
+ KNOWN PROBLEMS +
++++++++++++++++++
Log2Log v1.x is very problematic in its implementation.  (Scroll to the
beginning of this document for clarification.)  The biggest issue with the
client is that it is manages memory extremely poorly. Unfortunately, this takes
more effort to fix than the manpower we have available can offer.

The Log2Log client will crash if there are too many chat logs or if too many
conversions are made in one session of the program.  To work around this, break
up the chat logs to convert into chunks and close and reopen Log2Log until all
of your chat logs are converted.  We apologize for the inconvenience that this
may cause.

Format converters may also have issues.  For specific information about format
converter compatibility, see the "Configuration and Features" section.

1. Timezone compatibility is very poor, and fixes are intended to be made.

2. Converting chat log formats is no easy task. Note that Log2Log converters
   might not be 100% successful at converting chat logs.

3. Time zone compatibility is rough as of v1.0.7.

4. Languages other than English have not been figured out for format
   converters as of v1.0.7.  The Log2Log team would greatly appreciate it if
   you could develop a translation system for us.

5. This project would like help.  If you would like to contribute to Log2Log in
   any way, we would greatly appreciate it!  Chat log donors, C++ programmers,
   graphics designers, etc.


++++++++++++++++++++
+ MORE INFORMATION +
++++++++++++++++++++
For more information, updates, and just... more..., visit Deltik's Web site
at <http://www.deltik.org/>.

Also, for Log2Log-specific information, you can visit Log2Log's Web site at
<http://log2log.sourceforge.net/>.

You can contact the project manager, Nick Liu, too: <deltik@gmx.com>


+++++++++++++++++++
+ VERSION HISTORY +
+++++++++++++++++++
1.0.7 (2015/03/23)
  - FIX: HTML tag stripping in Windows Live Messenger `To` format converter

1.0.6 (2015/03/22)
  - NEW: Experimental Pidgin plaintext `From` format converter
  - FIX: Program crash due to the way StdFormat tried to access timezone information
  - FIX: Possible Pidgin HTML `From` format converter infinite loop
  - FIX: Improved sender handling in Windows Live Messenger `To` format converter
  - FIX: XML integrity with "<" and ">" symbols in Windows Live Messenger `To` format converter

1.0.5 (2015/03/20)
  - FIX: Significant improvements in Pidgin `From` format converter robustness
  - FIX: Made Pidgin `From` format converter datetimestamps less buggy

1.0.4 (2015/03/17)
  - NEW: WLM `To` format converter, made specially for the Messenger Revivers
  - FIX: Pidgin `From` timestamps were always zeroed due to Qt changes.
  - FIX: Corrected the default path to the Windows Live Messenger format converter
  - MOD: Updated timezone maps
  - MOD: Improved readability of the compatibility check section in the main window
  - MOD: Removed disabled MeeboConnect code

1.0.3 (2014/10/02)
  - NEW: Digsby `From` format converter
  - FIX: Update checker works again now that it sends a user agent.

1.0.2 (2012/07/03)
  - INF: The Log2Log v1.x branch is deprecated.
  - FIX: Improved Trillian `To` format converter protocol conversions, especially for Facebook and Google Talk

1.0.1 (2012/06/23)
  - INF: The Log2Log v1.x branch is being discontinued.
  - NEW: Meebo (farewell) format converter
  - NEW: You can now use the [Enter] key in a text field to start the conversion.
  - FIX: Default paths work now.
  - MOD: Cleaned up the user interface

1.0.0 (2011/10/01)
  - NEW: Initial Release
  - NEW: Omegle format converter
  - NEW: Trillian format converter
  - NEW: WLM `From` format converter
  - NEW: AIM `From` format converter

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


[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FDeltik%2FLog2Log.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2FDeltik%2FLog2Log?ref=badge_large)