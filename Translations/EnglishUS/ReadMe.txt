========================================================================
       DYNAMIC LINK LIBRARY : LDViewLang
========================================================================


////////////////////////////////////////////////////////////////////////////////

Usage:

This project contains all the resources necessary to create a language module
for LDView.  To create the language module, modify all of the dialog, menu, and
string resources to translate them to the desired language, and change the name
of the output DLL to be LDView-<Language>.dll, where <Language> is the name of
the desired language (either full or short).  For example, the following are all
valid names:

LDView-English (United Kingdom).dll
LDView-English.dll

Note that if a full name match isn't found, the short version is used instead.
Also note that the full name should be in that native language.  The short name
is always the name of the language in English.  In general, it's recommended to
use the short name, since you have to create a whole new DLL for each language.

You can use the program LangCheck.exe to tell you both the full and short names
of the current language that you have set in Windows.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Updating:

Each time a new version of LDView is released, the language modules have to be
updated in order to work.  LDView checks the version resource in the language
module, and won't load the module if it doesn't match the LDView version.  Since
you won't want to start from scratch each time there is a new release, it's best
to copy all the changes from LDView's updated sample language module, then
translate them and update the version by hand.

////////////////////////////////////////////////////////////////////////////////
