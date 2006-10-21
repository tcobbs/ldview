========================================================================
       DYNAMIC LINK LIBRARY : LDViewLang
========================================================================


////////////////////////////////////////////////////////////////////////////////

Usage:

This project contains all the resources necessary to create a language module
for LDView.  Please read this whole section before you begin, since some of the
instructions at the end make the beginning more clear.  Note that this project
requires Visual Studio 6 to create the translation DLL.  If you have a more
recent Visual Studio, it will probably still work, but I can't make any absolute
guarantees.  If you do have a later Visual Studio version, please follow the
numbered instructions below prior to having Visual Studio convert the project to
the newer project version.

When starting a new translation, it is recommended that you do the following:

1. Copy the EnglishUS directory, and rename it to the name of your language.
   For this example, I'll use Latin as the name of the language.
2. In the newly renamed Latin directory, rename EnglishUS.* to be Latin.*.  Also
   rename LDView-EnglishUS.reg to LDView-Latin.reg.
3. Edit Latin.cpp, and do a global search and replace from EnglishUS to Latin.
4. Edit Latin.dsp in a text editor, and do a global search and replace from
   "English (United States)" to "Latin" (without the quotes).
5. Still in Latin.dsp, do a case-sensitive global search and replace from
   EnglishUS to Latin.
6. Still in Latin.dsp, do a case-sensitive global search and replace from
   ENGLISHUS to LATIN.  You can save and close Latin.dsp now.
7. Edit Latin.dsw in a text editor, and do a global search and replace from
   EnglishUS to Latin.
8. Edit LDView-Latin.reg in a text editor, and replace LDView-EnglishUS with
   LDView-Latin.
9. If you're going to translate the HTML help file also, rename Help.html to
   Help-Latin.html.  Please note that if you do this, you must also update the
   HelpHtml entry in LDViewMessages.ini to match your new filename.
10.If you're going to translate the "What's this?" help, edit Help\LDView.hpj
   in a text editor, and change the HLP=LDView.hlp line to be
   HLP=LDView-Latin.hlp.  Then update the LDView.hlp entry in LDViewMessages.ini
   to match the filename you chose.

Once you've done the above, you're ready to start your translation.  To create
your language module, modify all of the dialog, menu, string, and accelerator
resources to translate them to the desired language.  (Note that the
accelerators must be updated to match any changes you make in the menus.  For
example, if you change Ctrl+O to Ctrl+A, you need to update the ID_FILE_OPEN
accelerator to be Ctrl+A.)

If you want to translate the "What's this?" help, you will need Microsoft Word
to open the RTF files.  It might work in other RTF-capable editors, but it also
might not.  The Microsoft help compiler is fairly picky about what it will
accept.  Since Visual Studio 6 doesn't seem to register the .hpj file type to
Microsoft Help Workshop, you need to do this yourself.  Double click on the
LDView.hpj file, and when prompted to pick a program, pick the following
program, and click the check box to always open with that program:

C:\Program Files\Microsoft Visual Studio\Common\Tools\HCW.EXE

(The above assumes you installed Visual Studio 6 in the default location.  If
you installed it in some other location, the path will differ.)  Once you've
translated all the RTF files, open LDView.hpj in Microsoft Help Workshop and
click the "Save and Compile" button in the bottom right.  This will create the
help file.

You have two choices for output DLL name, the long name and the short name.  The
instructions above were for the short name.  If you want to use the long name
instead, you can do so, but that makes the language module specific to a smaller
region.  For example, the following are all valid names:

LDView-English (United States).dll
LDView-English (United Kingdom).dll
LDView-English.dll

The first would only be loaded in the United States.  The second would only
be loaded in the United Kingdom.  (This example is somewhat contrived, since
LDView itself is written using US English.  However, it would be perfectly
legitimate for someone to create a UK translation.)

Note that if a full name match isn't found, the short version is used instead.
Also note that the full name should be in that native language.  The short name
is always the name of the language in English.  In general, it's recommended to
use the short name, since you have to create a whole new DLL for each language.

You can use the program LangCheck.exe to tell you both the full and short names
of the current language that you have set in Windows.

Once you have translated all the resource, you need to create a translation of
LDViewMessages.ini.  This file contains messages that get shown to the user, but
aren't in the application resources.  Please read the instructions at the top of
that file for instructions on translating it.

Finally, translate install.txt, which contains installation instructions for the
language DLL.  Note that this file references LDView-EnglishUS.  Make sure to
replace all occurences of EnglishUS in this file with Latin (continuing with the
example language name from above).  Also, replace English (United States) with
your language name.  (Don't use the English translation of your language name
here; use the language name that your users will see in Windows.)  Also, unless
you're doing a specific dialect, just use the short language name.  Rename
install.txt to an appropriate filename for your language.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Updating:

Each time a new version of LDView is released, the language modules have to be
updated in order to work.  LDView checks the version resource in the language
module, and won't load the module if it doesn't match the LDView version.  Since
you won't want to start from scratch each time there is a new release, it's best
to copy all the changes from LDView's updated sample language module, then
translate them and update the version by hand.  You do want to be extremely
careful not to miss anything, though, and a good file comparison program will
help out tremendously.

////////////////////////////////////////////////////////////////////////////////
