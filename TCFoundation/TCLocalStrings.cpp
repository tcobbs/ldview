#include "TCLocalStrings.h"
#include "TCDictionary.h"
#include "TCSortedStringArray.h"
#include "mystring.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// The following map came from here:
// http://www.microsoft.com/globaldev/reference/sbcs/1250.mspx
// The <UNMAPPED> entries were added by hand based on the holes in the chart.
static wchar_t g_cp1250[256] =
{
	/*00 = U+*/0x0000, //NULL
	/*01 = U+*/0x0001, //START OF HEADING
	/*02 = U+*/0x0002, //START OF TEXT
	/*03 = U+*/0x0003, //END OF TEXT
	/*04 = U+*/0x0004, //END OF TRANSMISSION
	/*05 = U+*/0x0005, //ENQUIRY
	/*06 = U+*/0x0006, //ACKNOWLEDGE
	/*07 = U+*/0x0007, //BELL
	/*08 = U+*/0x0008, //BACKSPACE
	/*09 = U+*/0x0009, //HORIZONTAL TABULATION
	/*0A = U+*/0x000A, //LINE FEED
	/*0B = U+*/0x000B, //VERTICAL TABULATION
	/*0C = U+*/0x000C, //FORM FEED
	/*0D = U+*/0x000D, //CARRIAGE RETURN
	/*0E = U+*/0x000E, //SHIFT OUT
	/*0F = U+*/0x000F, //SHIFT IN
	/*10 = U+*/0x0010, //DATA LINK ESCAPE
	/*11 = U+*/0x0011, //DEVICE CONTROL ONE
	/*12 = U+*/0x0012, //DEVICE CONTROL TWO
	/*13 = U+*/0x0013, //DEVICE CONTROL THREE
	/*14 = U+*/0x0014, //DEVICE CONTROL FOUR
	/*15 = U+*/0x0015, //NEGATIVE ACKNOWLEDGE
	/*16 = U+*/0x0016, //SYNCHRONOUS IDLE
	/*17 = U+*/0x0017, //END OF TRANSMISSION BLOCK
	/*18 = U+*/0x0018, //CANCEL
	/*19 = U+*/0x0019, //END OF MEDIUM
	/*1A = U+*/0x001A, //SUBSTITUTE
	/*1B = U+*/0x001B, //ESCAPE
	/*1C = U+*/0x001C, //FILE SEPARATOR
	/*1D = U+*/0x001D, //GROUP SEPARATOR
	/*1E = U+*/0x001E, //RECORD SEPARATOR
	/*1F = U+*/0x001F, //UNIT SEPARATOR
	/*20 = U+*/0x0020, //SPACE
	/*21 = U+*/0x0021, //EXCLAMATION MARK
	/*22 = U+*/0x0022, //QUOTATION MARK
	/*23 = U+*/0x0023, //NUMBER SIGN
	/*24 = U+*/0x0024, //DOLLAR SIGN
	/*25 = U+*/0x0025, //PERCENT SIGN
	/*26 = U+*/0x0026, //AMPERSAND
	/*27 = U+*/0x0027, //APOSTROPHE
	/*28 = U+*/0x0028, //LEFT PARENTHESIS
	/*29 = U+*/0x0029, //RIGHT PARENTHESIS
	/*2A = U+*/0x002A, //ASTERISK
	/*2B = U+*/0x002B, //PLUS SIGN
	/*2C = U+*/0x002C, //COMMA
	/*2D = U+*/0x002D, //HYPHEN-MINUS
	/*2E = U+*/0x002E, //FULL STOP
	/*2F = U+*/0x002F, //SOLIDUS
	/*30 = U+*/0x0030, //DIGIT ZERO
	/*31 = U+*/0x0031, //DIGIT ONE
	/*32 = U+*/0x0032, //DIGIT TWO
	/*33 = U+*/0x0033, //DIGIT THREE
	/*34 = U+*/0x0034, //DIGIT FOUR
	/*35 = U+*/0x0035, //DIGIT FIVE
	/*36 = U+*/0x0036, //DIGIT SIX
	/*37 = U+*/0x0037, //DIGIT SEVEN
	/*38 = U+*/0x0038, //DIGIT EIGHT
	/*39 = U+*/0x0039, //DIGIT NINE
	/*3A = U+*/0x003A, //COLON
	/*3B = U+*/0x003B, //SEMICOLON
	/*3C = U+*/0x003C, //LESS-THAN SIGN
	/*3D = U+*/0x003D, //EQUALS SIGN
	/*3E = U+*/0x003E, //GREATER-THAN SIGN
	/*3F = U+*/0x003F, //QUESTION MARK
	/*40 = U+*/0x0040, //COMMERCIAL AT
	/*41 = U+*/0x0041, //LATIN CAPITAL LETTER A
	/*42 = U+*/0x0042, //LATIN CAPITAL LETTER B
	/*43 = U+*/0x0043, //LATIN CAPITAL LETTER C
	/*44 = U+*/0x0044, //LATIN CAPITAL LETTER D
	/*45 = U+*/0x0045, //LATIN CAPITAL LETTER E
	/*46 = U+*/0x0046, //LATIN CAPITAL LETTER F
	/*47 = U+*/0x0047, //LATIN CAPITAL LETTER G
	/*48 = U+*/0x0048, //LATIN CAPITAL LETTER H
	/*49 = U+*/0x0049, //LATIN CAPITAL LETTER I
	/*4A = U+*/0x004A, //LATIN CAPITAL LETTER J
	/*4B = U+*/0x004B, //LATIN CAPITAL LETTER K
	/*4C = U+*/0x004C, //LATIN CAPITAL LETTER L
	/*4D = U+*/0x004D, //LATIN CAPITAL LETTER M
	/*4E = U+*/0x004E, //LATIN CAPITAL LETTER N
	/*4F = U+*/0x004F, //LATIN CAPITAL LETTER O
	/*50 = U+*/0x0050, //LATIN CAPITAL LETTER P
	/*51 = U+*/0x0051, //LATIN CAPITAL LETTER Q
	/*52 = U+*/0x0052, //LATIN CAPITAL LETTER R
	/*53 = U+*/0x0053, //LATIN CAPITAL LETTER S
	/*54 = U+*/0x0054, //LATIN CAPITAL LETTER T
	/*55 = U+*/0x0055, //LATIN CAPITAL LETTER U
	/*56 = U+*/0x0056, //LATIN CAPITAL LETTER V
	/*57 = U+*/0x0057, //LATIN CAPITAL LETTER W
	/*58 = U+*/0x0058, //LATIN CAPITAL LETTER X
	/*59 = U+*/0x0059, //LATIN CAPITAL LETTER Y
	/*5A = U+*/0x005A, //LATIN CAPITAL LETTER Z
	/*5B = U+*/0x005B, //LEFT SQUARE BRACKET
	/*5C = U+*/0x005C, //REVERSE SOLIDUS
	/*5D = U+*/0x005D, //RIGHT SQUARE BRACKET
	/*5E = U+*/0x005E, //CIRCUMFLEX ACCENT
	/*5F = U+*/0x005F, //LOW LINE
	/*60 = U+*/0x0060, //GRAVE ACCENT
	/*61 = U+*/0x0061, //LATIN SMALL LETTER A
	/*62 = U+*/0x0062, //LATIN SMALL LETTER B
	/*63 = U+*/0x0063, //LATIN SMALL LETTER C
	/*64 = U+*/0x0064, //LATIN SMALL LETTER D
	/*65 = U+*/0x0065, //LATIN SMALL LETTER E
	/*66 = U+*/0x0066, //LATIN SMALL LETTER F
	/*67 = U+*/0x0067, //LATIN SMALL LETTER G
	/*68 = U+*/0x0068, //LATIN SMALL LETTER H
	/*69 = U+*/0x0069, //LATIN SMALL LETTER I
	/*6A = U+*/0x006A, //LATIN SMALL LETTER J
	/*6B = U+*/0x006B, //LATIN SMALL LETTER K
	/*6C = U+*/0x006C, //LATIN SMALL LETTER L
	/*6D = U+*/0x006D, //LATIN SMALL LETTER M
	/*6E = U+*/0x006E, //LATIN SMALL LETTER N
	/*6F = U+*/0x006F, //LATIN SMALL LETTER O
	/*70 = U+*/0x0070, //LATIN SMALL LETTER P
	/*71 = U+*/0x0071, //LATIN SMALL LETTER Q
	/*72 = U+*/0x0072, //LATIN SMALL LETTER R
	/*73 = U+*/0x0073, //LATIN SMALL LETTER S
	/*74 = U+*/0x0074, //LATIN SMALL LETTER T
	/*75 = U+*/0x0075, //LATIN SMALL LETTER U
	/*76 = U+*/0x0076, //LATIN SMALL LETTER V
	/*77 = U+*/0x0077, //LATIN SMALL LETTER W
	/*78 = U+*/0x0078, //LATIN SMALL LETTER X
	/*79 = U+*/0x0079, //LATIN SMALL LETTER Y
	/*7A = U+*/0x007A, //LATIN SMALL LETTER Z
	/*7B = U+*/0x007B, //LEFT CURLY BRACKET
	/*7C = U+*/0x007C, //VERTICAL LINE
	/*7D = U+*/0x007D, //RIGHT CURLY BRACKET
	/*7E = U+*/0x007E, //TILDE
	/*7F = U+*/0x007F, //DELETE
	/*80 = U+*/0x20AC, //EURO SIGN
	/*81 = U+*/0xFFFF, //<UNMAPPED>
	/*82 = U+*/0x201A, //SINGLE LOW-9 QUOTATION MARK
	/*83 = U+*/0xFFFF, //<UNMAPPED>
	/*84 = U+*/0x201E, //DOUBLE LOW-9 QUOTATION MARK
	/*85 = U+*/0x2026, //HORIZONTAL ELLIPSIS
	/*86 = U+*/0x2020, //DAGGER
	/*87 = U+*/0x2021, //DOUBLE DAGGER
	/*88 = U+*/0xFFFF, //<UNMAPPED>
	/*89 = U+*/0x2030, //PER MILLE SIGN
	/*8A = U+*/0x0160, //LATIN CAPITAL LETTER S WITH CARON
	/*8B = U+*/0x2039, //SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	/*8C = U+*/0x015A, //LATIN CAPITAL LETTER S WITH ACUTE
	/*8D = U+*/0x0164, //LATIN CAPITAL LETTER T WITH CARON
	/*8E = U+*/0x017D, //LATIN CAPITAL LETTER Z WITH CARON
	/*8F = U+*/0x0179, //LATIN CAPITAL LETTER Z WITH ACUTE
	/*90 = U+*/0xFFFF, //<UNMAPPED>
	/*91 = U+*/0x2018, //LEFT SINGLE QUOTATION MARK
	/*92 = U+*/0x2019, //RIGHT SINGLE QUOTATION MARK
	/*93 = U+*/0x201C, //LEFT DOUBLE QUOTATION MARK
	/*94 = U+*/0x201D, //RIGHT DOUBLE QUOTATION MARK
	/*95 = U+*/0x2022, //BULLET
	/*96 = U+*/0x2013, //EN DASH
	/*97 = U+*/0x2014, //EM DASH
	/*98 = U+*/0xFFFF, //<UNMAPPED>
	/*99 = U+*/0x2122, //TRADE MARK SIGN
	/*9A = U+*/0x0161, //LATIN SMALL LETTER S WITH CARON
	/*9B = U+*/0x203A, //SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	/*9C = U+*/0x015B, //LATIN SMALL LETTER S WITH ACUTE
	/*9D = U+*/0x0165, //LATIN SMALL LETTER T WITH CARON
	/*9E = U+*/0x017E, //LATIN SMALL LETTER Z WITH CARON
	/*9F = U+*/0x017A, //LATIN SMALL LETTER Z WITH ACUTE
	/*A0 = U+*/0x00A0, //NO-BREAK SPACE
	/*A1 = U+*/0x02C7, //CARON
	/*A2 = U+*/0x02D8, //BREVE
	/*A3 = U+*/0x0141, //LATIN CAPITAL LETTER L WITH STROKE
	/*A4 = U+*/0x00A4, //CURRENCY SIGN
	/*A5 = U+*/0x0104, //LATIN CAPITAL LETTER A WITH OGONEK
	/*A6 = U+*/0x00A6, //BROKEN BAR
	/*A7 = U+*/0x00A7, //SECTION SIGN
	/*A8 = U+*/0x00A8, //DIAERESIS
	/*A9 = U+*/0x00A9, //COPYRIGHT SIGN
	/*AA = U+*/0x015E, //LATIN CAPITAL LETTER S WITH CEDILLA
	/*AB = U+*/0x00AB, //LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	/*AC = U+*/0x00AC, //NOT SIGN
	/*AD = U+*/0x00AD, //SOFT HYPHEN
	/*AE = U+*/0x00AE, //REGISTERED SIGN
	/*AF = U+*/0x017B, //LATIN CAPITAL LETTER Z WITH DOT ABOVE
	/*B0 = U+*/0x00B0, //DEGREE SIGN
	/*B1 = U+*/0x00B1, //PLUS-MINUS SIGN
	/*B2 = U+*/0x02DB, //OGONEK
	/*B3 = U+*/0x0142, //LATIN SMALL LETTER L WITH STROKE
	/*B4 = U+*/0x00B4, //ACUTE ACCENT
	/*B5 = U+*/0x00B5, //MICRO SIGN
	/*B6 = U+*/0x00B6, //PILCROW SIGN
	/*B7 = U+*/0x00B7, //MIDDLE DOT
	/*B8 = U+*/0x00B8, //CEDILLA
	/*B9 = U+*/0x0105, //LATIN SMALL LETTER A WITH OGONEK
	/*BA = U+*/0x015F, //LATIN SMALL LETTER S WITH CEDILLA
	/*BB = U+*/0x00BB, //RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	/*BC = U+*/0x013D, //LATIN CAPITAL LETTER L WITH CARON
	/*BD = U+*/0x02DD, //DOUBLE ACUTE ACCENT
	/*BE = U+*/0x013E, //LATIN SMALL LETTER L WITH CARON
	/*BF = U+*/0x017C, //LATIN SMALL LETTER Z WITH DOT ABOVE
	/*C0 = U+*/0x0154, //LATIN CAPITAL LETTER R WITH ACUTE
	/*C1 = U+*/0x00C1, //LATIN CAPITAL LETTER A WITH ACUTE
	/*C2 = U+*/0x00C2, //LATIN CAPITAL LETTER A WITH CIRCUMFLEX
	/*C3 = U+*/0x0102, //LATIN CAPITAL LETTER A WITH BREVE
	/*C4 = U+*/0x00C4, //LATIN CAPITAL LETTER A WITH DIAERESIS
	/*C5 = U+*/0x0139, //LATIN CAPITAL LETTER L WITH ACUTE
	/*C6 = U+*/0x0106, //LATIN CAPITAL LETTER C WITH ACUTE
	/*C7 = U+*/0x00C7, //LATIN CAPITAL LETTER C WITH CEDILLA
	/*C8 = U+*/0x010C, //LATIN CAPITAL LETTER C WITH CARON
	/*C9 = U+*/0x00C9, //LATIN CAPITAL LETTER E WITH ACUTE
	/*CA = U+*/0x0118, //LATIN CAPITAL LETTER E WITH OGONEK
	/*CB = U+*/0x00CB, //LATIN CAPITAL LETTER E WITH DIAERESIS
	/*CC = U+*/0x011A, //LATIN CAPITAL LETTER E WITH CARON
	/*CD = U+*/0x00CD, //LATIN CAPITAL LETTER I WITH ACUTE
	/*CE = U+*/0x00CE, //LATIN CAPITAL LETTER I WITH CIRCUMFLEX
	/*CF = U+*/0x010E, //LATIN CAPITAL LETTER D WITH CARON
	/*D0 = U+*/0x0110, //LATIN CAPITAL LETTER D WITH STROKE
	/*D1 = U+*/0x0143, //LATIN CAPITAL LETTER N WITH ACUTE
	/*D2 = U+*/0x0147, //LATIN CAPITAL LETTER N WITH CARON
	/*D3 = U+*/0x00D3, //LATIN CAPITAL LETTER O WITH ACUTE
	/*D4 = U+*/0x00D4, //LATIN CAPITAL LETTER O WITH CIRCUMFLEX
	/*D5 = U+*/0x0150, //LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
	/*D6 = U+*/0x00D6, //LATIN CAPITAL LETTER O WITH DIAERESIS
	/*D7 = U+*/0x00D7, //MULTIPLICATION SIGN
	/*D8 = U+*/0x0158, //LATIN CAPITAL LETTER R WITH CARON
	/*D9 = U+*/0x016E, //LATIN CAPITAL LETTER U WITH RING ABOVE
	/*DA = U+*/0x00DA, //LATIN CAPITAL LETTER U WITH ACUTE
	/*DB = U+*/0x0170, //LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
	/*DC = U+*/0x00DC, //LATIN CAPITAL LETTER U WITH DIAERESIS
	/*DD = U+*/0x00DD, //LATIN CAPITAL LETTER Y WITH ACUTE
	/*DE = U+*/0x0162, //LATIN CAPITAL LETTER T WITH CEDILLA
	/*DF = U+*/0x00DF, //LATIN SMALL LETTER SHARP S
	/*E0 = U+*/0x0155, //LATIN SMALL LETTER R WITH ACUTE
	/*E1 = U+*/0x00E1, //LATIN SMALL LETTER A WITH ACUTE
	/*E2 = U+*/0x00E2, //LATIN SMALL LETTER A WITH CIRCUMFLEX
	/*E3 = U+*/0x0103, //LATIN SMALL LETTER A WITH BREVE
	/*E4 = U+*/0x00E4, //LATIN SMALL LETTER A WITH DIAERESIS
	/*E5 = U+*/0x013A, //LATIN SMALL LETTER L WITH ACUTE
	/*E6 = U+*/0x0107, //LATIN SMALL LETTER C WITH ACUTE
	/*E7 = U+*/0x00E7, //LATIN SMALL LETTER C WITH CEDILLA
	/*E8 = U+*/0x010D, //LATIN SMALL LETTER C WITH CARON
	/*E9 = U+*/0x00E9, //LATIN SMALL LETTER E WITH ACUTE
	/*EA = U+*/0x0119, //LATIN SMALL LETTER E WITH OGONEK
	/*EB = U+*/0x00EB, //LATIN SMALL LETTER E WITH DIAERESIS
	/*EC = U+*/0x011B, //LATIN SMALL LETTER E WITH CARON
	/*ED = U+*/0x00ED, //LATIN SMALL LETTER I WITH ACUTE
	/*EE = U+*/0x00EE, //LATIN SMALL LETTER I WITH CIRCUMFLEX
	/*EF = U+*/0x010F, //LATIN SMALL LETTER D WITH CARON
	/*F0 = U+*/0x0111, //LATIN SMALL LETTER D WITH STROKE
	/*F1 = U+*/0x0144, //LATIN SMALL LETTER N WITH ACUTE
	/*F2 = U+*/0x0148, //LATIN SMALL LETTER N WITH CARON
	/*F3 = U+*/0x00F3, //LATIN SMALL LETTER O WITH ACUTE
	/*F4 = U+*/0x00F4, //LATIN SMALL LETTER O WITH CIRCUMFLEX
	/*F5 = U+*/0x0151, //LATIN SMALL LETTER O WITH DOUBLE ACUTE
	/*F6 = U+*/0x00F6, //LATIN SMALL LETTER O WITH DIAERESIS
	/*F7 = U+*/0x00F7, //DIVISION SIGN
	/*F8 = U+*/0x0159, //LATIN SMALL LETTER R WITH CARON
	/*F9 = U+*/0x016F, //LATIN SMALL LETTER U WITH RING ABOVE
	/*FA = U+*/0x00FA, //LATIN SMALL LETTER U WITH ACUTE
	/*FB = U+*/0x0171, //LATIN SMALL LETTER U WITH DOUBLE ACUTE
	/*FC = U+*/0x00FC, //LATIN SMALL LETTER U WITH DIAERESIS
	/*FD = U+*/0x00FD, //LATIN SMALL LETTER Y WITH ACUTE
	/*FE = U+*/0x0163, //LATIN SMALL LETTER T WITH CEDILLA
	/*FF = U+*/0x02D9  //DOT ABOVE
};

// The following map came from here:
// http://www.microsoft.com/globaldev/reference/sbcs/1252.mspx
// The <UNMAPPED> entries were added by hand based on the holes in the chart.
static wchar_t g_cp1252[256] =
{
	/*00 = u+*/0x0000, //NULL
	/*01 = U+*/0x0001, //START OF HEADING
	/*02 = U+*/0x0002, //START OF TEXT
	/*03 = U+*/0x0003, //END OF TEXT
	/*04 = U+*/0x0004, //END OF TRANSMISSION
	/*05 = U+*/0x0005, //ENQUIRY
	/*06 = U+*/0x0006, //ACKNOWLEDGE
	/*07 = U+*/0x0007, //BELL
	/*08 = U+*/0x0008, //BACKSPACE
	/*09 = U+*/0x0009, //HORIZONTAL TABULATION
	/*0A = U+*/0x000A, //LINE FEED
	/*0B = U+*/0x000B, //VERTICAL TABULATION
	/*0C = U+*/0x000C, //FORM FEED
	/*0D = U+*/0x000D, //CARRIAGE RETURN
	/*0E = U+*/0x000E, //SHIFT OUT
	/*0F = U+*/0x000F, //SHIFT IN
	/*10 = U+*/0x0010, //DATA LINK ESCAPE
	/*11 = U+*/0x0011, //DEVICE CONTROL ONE
	/*12 = U+*/0x0012, //DEVICE CONTROL TWO
	/*13 = U+*/0x0013, //DEVICE CONTROL THREE
	/*14 = U+*/0x0014, //DEVICE CONTROL FOUR
	/*15 = U+*/0x0015, //NEGATIVE ACKNOWLEDGE
	/*16 = U+*/0x0016, //SYNCHRONOUS IDLE
	/*17 = U+*/0x0017, //END OF TRANSMISSION BLOCK
	/*18 = U+*/0x0018, //CANCEL
	/*19 = U+*/0x0019, //END OF MEDIUM
	/*1A = U+*/0x001A, //SUBSTITUTE
	/*1B = U+*/0x001B, //ESCAPE
	/*1C = U+*/0x001C, //FILE SEPARATOR
	/*1D = U+*/0x001D, //GROUP SEPARATOR
	/*1E = U+*/0x001E, //RECORD SEPARATOR
	/*1F = U+*/0x001F, //UNIT SEPARATOR
	/*20 = U+*/0x0020, //SPACE
	/*21 = U+*/0x0021, //EXCLAMATION MARK
	/*22 = U+*/0x0022, //QUOTATION MARK
	/*23 = U+*/0x0023, //NUMBER SIGN
	/*24 = U+*/0x0024, //DOLLAR SIGN
	/*25 = U+*/0x0025, //PERCENT SIGN
	/*26 = U+*/0x0026, //AMPERSAND
	/*27 = U+*/0x0027, //APOSTROPHE
	/*28 = U+*/0x0028, //LEFT PARENTHESIS
	/*29 = U+*/0x0029, //RIGHT PARENTHESIS
	/*2A = U+*/0x002A, //ASTERISK
	/*2B = U+*/0x002B, //PLUS SIGN
	/*2C = U+*/0x002C, //COMMA
	/*2D = U+*/0x002D, //HYPHEN-MINUS
	/*2E = U+*/0x002E, //FULL STOP
	/*2F = U+*/0x002F, //SOLIDUS
	/*30 = U+*/0x0030, //DIGIT ZERO
	/*31 = U+*/0x0031, //DIGIT ONE
	/*32 = U+*/0x0032, //DIGIT TWO
	/*33 = U+*/0x0033, //DIGIT THREE
	/*34 = U+*/0x0034, //DIGIT FOUR
	/*35 = U+*/0x0035, //DIGIT FIVE
	/*36 = U+*/0x0036, //DIGIT SIX
	/*37 = U+*/0x0037, //DIGIT SEVEN
	/*38 = U+*/0x0038, //DIGIT EIGHT
	/*39 = U+*/0x0039, //DIGIT NINE
	/*3A = U+*/0x003A, //COLON
	/*3B = U+*/0x003B, //SEMICOLON
	/*3C = U+*/0x003C, //LESS-THAN SIGN
	/*3D = U+*/0x003D, //EQUALS SIGN
	/*3E = U+*/0x003E, //GREATER-THAN SIGN
	/*3F = U+*/0x003F, //QUESTION MARK
	/*40 = U+*/0x0040, //COMMERCIAL AT
	/*41 = U+*/0x0041, //LATIN CAPITAL LETTER A
	/*42 = U+*/0x0042, //LATIN CAPITAL LETTER B
	/*43 = U+*/0x0043, //LATIN CAPITAL LETTER C
	/*44 = U+*/0x0044, //LATIN CAPITAL LETTER D
	/*45 = U+*/0x0045, //LATIN CAPITAL LETTER E
	/*46 = U+*/0x0046, //LATIN CAPITAL LETTER F
	/*47 = U+*/0x0047, //LATIN CAPITAL LETTER G
	/*48 = U+*/0x0048, //LATIN CAPITAL LETTER H
	/*49 = U+*/0x0049, //LATIN CAPITAL LETTER I
	/*4A = U+*/0x004A, //LATIN CAPITAL LETTER J
	/*4B = U+*/0x004B, //LATIN CAPITAL LETTER K
	/*4C = U+*/0x004C, //LATIN CAPITAL LETTER L
	/*4D = U+*/0x004D, //LATIN CAPITAL LETTER M
	/*4E = U+*/0x004E, //LATIN CAPITAL LETTER N
	/*4F = U+*/0x004F, //LATIN CAPITAL LETTER O
	/*50 = U+*/0x0050, //LATIN CAPITAL LETTER P
	/*51 = U+*/0x0051, //LATIN CAPITAL LETTER Q
	/*52 = U+*/0x0052, //LATIN CAPITAL LETTER R
	/*53 = U+*/0x0053, //LATIN CAPITAL LETTER S
	/*54 = U+*/0x0054, //LATIN CAPITAL LETTER T
	/*55 = U+*/0x0055, //LATIN CAPITAL LETTER U
	/*56 = U+*/0x0056, //LATIN CAPITAL LETTER V
	/*57 = U+*/0x0057, //LATIN CAPITAL LETTER W
	/*58 = U+*/0x0058, //LATIN CAPITAL LETTER X
	/*59 = U+*/0x0059, //LATIN CAPITAL LETTER Y
	/*5A = U+*/0x005A, //LATIN CAPITAL LETTER Z
	/*5B = U+*/0x005B, //LEFT SQUARE BRACKET
	/*5C = U+*/0x005C, //REVERSE SOLIDUS
	/*5D = U+*/0x005D, //RIGHT SQUARE BRACKET
	/*5E = U+*/0x005E, //CIRCUMFLEX ACCENT
	/*5F = U+*/0x005F, //LOW LINE
	/*60 = U+*/0x0060, //GRAVE ACCENT
	/*61 = U+*/0x0061, //LATIN SMALL LETTER A
	/*62 = U+*/0x0062, //LATIN SMALL LETTER B
	/*63 = U+*/0x0063, //LATIN SMALL LETTER C
	/*64 = U+*/0x0064, //LATIN SMALL LETTER D
	/*65 = U+*/0x0065, //LATIN SMALL LETTER E
	/*66 = U+*/0x0066, //LATIN SMALL LETTER F
	/*67 = U+*/0x0067, //LATIN SMALL LETTER G
	/*68 = U+*/0x0068, //LATIN SMALL LETTER H
	/*69 = U+*/0x0069, //LATIN SMALL LETTER I
	/*6A = U+*/0x006A, //LATIN SMALL LETTER J
	/*6B = U+*/0x006B, //LATIN SMALL LETTER K
	/*6C = U+*/0x006C, //LATIN SMALL LETTER L
	/*6D = U+*/0x006D, //LATIN SMALL LETTER M
	/*6E = U+*/0x006E, //LATIN SMALL LETTER N
	/*6F = U+*/0x006F, //LATIN SMALL LETTER O
	/*70 = U+*/0x0070, //LATIN SMALL LETTER P
	/*71 = U+*/0x0071, //LATIN SMALL LETTER Q
	/*72 = U+*/0x0072, //LATIN SMALL LETTER R
	/*73 = U+*/0x0073, //LATIN SMALL LETTER S
	/*74 = U+*/0x0074, //LATIN SMALL LETTER T
	/*75 = U+*/0x0075, //LATIN SMALL LETTER U
	/*76 = U+*/0x0076, //LATIN SMALL LETTER V
	/*77 = U+*/0x0077, //LATIN SMALL LETTER W
	/*78 = U+*/0x0078, //LATIN SMALL LETTER X
	/*79 = U+*/0x0079, //LATIN SMALL LETTER Y
	/*7A = U+*/0x007A, //LATIN SMALL LETTER Z
	/*7B = U+*/0x007B, //LEFT CURLY BRACKET
	/*7C = U+*/0x007C, //VERTICAL LINE
	/*7D = U+*/0x007D, //RIGHT CURLY BRACKET
	/*7E = U+*/0x007E, //TILDE
	/*7F = U+*/0x007F, //DELETE
	/*80 = U+*/0x20AC, //EURO SIGN
	/*81 = U+*/0xFFFF, //<UNMAPPED>
	/*82 = U+*/0x201A, //SINGLE LOW-9 QUOTATION MARK
	/*83 = U+*/0x0192, //LATIN SMALL LETTER F WITH HOOK
	/*84 = U+*/0x201E, //DOUBLE LOW-9 QUOTATION MARK
	/*85 = U+*/0x2026, //HORIZONTAL ELLIPSIS
	/*86 = U+*/0x2020, //DAGGER
	/*87 = U+*/0x2021, //DOUBLE DAGGER
	/*88 = U+*/0x02C6, //MODIFIER LETTER CIRCUMFLEX ACCENT
	/*89 = U+*/0x2030, //PER MILLE SIGN
	/*8A = U+*/0x0160, //LATIN CAPITAL LETTER S WITH CARON
	/*8B = U+*/0x2039, //SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	/*8C = U+*/0x0152, //LATIN CAPITAL LIGATURE OE
	/*8D = U+*/0xFFFF, //<UNMAPPED>
	/*8E = U+*/0x017D, //LATIN CAPITAL LETTER Z WITH CARON
	/*8F = U+*/0xFFFF, //<UNMAPPED>
	/*90 = U+*/0xFFFF, //<UNMAPPED>
	/*91 = U+*/0x2018, //LEFT SINGLE QUOTATION MARK
	/*92 = U+*/0x2019, //RIGHT SINGLE QUOTATION MARK
	/*93 = U+*/0x201C, //LEFT DOUBLE QUOTATION MARK
	/*94 = U+*/0x201D, //RIGHT DOUBLE QUOTATION MARK
	/*95 = U+*/0x2022, //BULLET
	/*96 = U+*/0x2013, //EN DASH
	/*97 = U+*/0x2014, //EM DASH
	/*98 = U+*/0x02DC, //SMALL TILDE
	/*99 = U+*/0x2122, //TRADE MARK SIGN
	/*9A = U+*/0x0161, //LATIN SMALL LETTER S WITH CARON
	/*9B = U+*/0x203A, //SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	/*9C = U+*/0x0153, //LATIN SMALL LIGATURE OE
	/*9D = U+*/0xFFFF, //<UNMAPPED>
	/*9E = U+*/0x017E, //LATIN SMALL LETTER Z WITH CARON
	/*9F = U+*/0x0178, //LATIN CAPITAL LETTER Y WITH DIAERESIS
	/*A0 = U+*/0x00A0, //NO-BREAK SPACE
	/*A1 = U+*/0x00A1, //INVERTED EXCLAMATION MARK
	/*A2 = U+*/0x00A2, //CENT SIGN
	/*A3 = U+*/0x00A3, //POUND SIGN
	/*A4 = U+*/0x00A4, //CURRENCY SIGN
	/*A5 = U+*/0x00A5, //YEN SIGN
	/*A6 = U+*/0x00A6, //BROKEN BAR
	/*A7 = U+*/0x00A7, //SECTION SIGN
	/*A8 = U+*/0x00A8, //DIAERESIS
	/*A9 = U+*/0x00A9, //COPYRIGHT SIGN
	/*AA = U+*/0x00AA, //FEMININE ORDINAL INDICATOR
	/*AB = U+*/0x00AB, //LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	/*AC = U+*/0x00AC, //NOT SIGN
	/*AD = U+*/0x00AD, //SOFT HYPHEN
	/*AE = U+*/0x00AE, //REGISTERED SIGN
	/*AF = U+*/0x00AF, //MACRON
	/*B0 = U+*/0x00B0, //DEGREE SIGN
	/*B1 = U+*/0x00B1, //PLUS-MINUS SIGN
	/*B2 = U+*/0x00B2, //SUPERSCRIPT TWO
	/*B3 = U+*/0x00B3, //SUPERSCRIPT THREE
	/*B4 = U+*/0x00B4, //ACUTE ACCENT
	/*B5 = U+*/0x00B5, //MICRO SIGN
	/*B6 = U+*/0x00B6, //PILCROW SIGN
	/*B7 = U+*/0x00B7, //MIDDLE DOT
	/*B8 = U+*/0x00B8, //CEDILLA
	/*B9 = U+*/0x00B9, //SUPERSCRIPT ONE
	/*BA = U+*/0x00BA, //MASCULINE ORDINAL INDICATOR
	/*BB = U+*/0x00BB, //RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	/*BC = U+*/0x00BC, //VULGAR FRACTION ONE QUARTER
	/*BD = U+*/0x00BD, //VULGAR FRACTION ONE HALF
	/*BE = U+*/0x00BE, //VULGAR FRACTION THREE QUARTERS
	/*BF = U+*/0x00BF, //INVERTED QUESTION MARK
	/*C0 = U+*/0x00C0, //LATIN CAPITAL LETTER A WITH GRAVE
	/*C1 = U+*/0x00C1, //LATIN CAPITAL LETTER A WITH ACUTE
	/*C2 = U+*/0x00C2, //LATIN CAPITAL LETTER A WITH CIRCUMFLEX
	/*C3 = U+*/0x00C3, //LATIN CAPITAL LETTER A WITH TILDE
	/*C4 = U+*/0x00C4, //LATIN CAPITAL LETTER A WITH DIAERESIS
	/*C5 = U+*/0x00C5, //LATIN CAPITAL LETTER A WITH RING ABOVE
	/*C6 = U+*/0x00C6, //LATIN CAPITAL LETTER AE
	/*C7 = U+*/0x00C7, //LATIN CAPITAL LETTER C WITH CEDILLA
	/*C8 = U+*/0x00C8, //LATIN CAPITAL LETTER E WITH GRAVE
	/*C9 = U+*/0x00C9, //LATIN CAPITAL LETTER E WITH ACUTE
	/*CA = U+*/0x00CA, //LATIN CAPITAL LETTER E WITH CIRCUMFLEX
	/*CB = U+*/0x00CB, //LATIN CAPITAL LETTER E WITH DIAERESIS
	/*CC = U+*/0x00CC, //LATIN CAPITAL LETTER I WITH GRAVE
	/*CD = U+*/0x00CD, //LATIN CAPITAL LETTER I WITH ACUTE
	/*CE = U+*/0x00CE, //LATIN CAPITAL LETTER I WITH CIRCUMFLEX
	/*CF = U+*/0x00CF, //LATIN CAPITAL LETTER I WITH DIAERESIS
	/*D0 = U+*/0x00D0, //LATIN CAPITAL LETTER ETH
	/*D1 = U+*/0x00D1, //LATIN CAPITAL LETTER N WITH TILDE
	/*D2 = U+*/0x00D2, //LATIN CAPITAL LETTER O WITH GRAVE
	/*D3 = U+*/0x00D3, //LATIN CAPITAL LETTER O WITH ACUTE
	/*D4 = U+*/0x00D4, //LATIN CAPITAL LETTER O WITH CIRCUMFLEX
	/*D5 = U+*/0x00D5, //LATIN CAPITAL LETTER O WITH TILDE
	/*D6 = U+*/0x00D6, //LATIN CAPITAL LETTER O WITH DIAERESIS
	/*D7 = U+*/0x00D7, //MULTIPLICATION SIGN
	/*D8 = U+*/0x00D8, //LATIN CAPITAL LETTER O WITH STROKE
	/*D9 = U+*/0x00D9, //LATIN CAPITAL LETTER U WITH GRAVE
	/*DA = U+*/0x00DA, //LATIN CAPITAL LETTER U WITH ACUTE
	/*DB = U+*/0x00DB, //LATIN CAPITAL LETTER U WITH CIRCUMFLEX
	/*DC = U+*/0x00DC, //LATIN CAPITAL LETTER U WITH DIAERESIS
	/*DD = U+*/0x00DD, //LATIN CAPITAL LETTER Y WITH ACUTE
	/*DE = U+*/0x00DE, //LATIN CAPITAL LETTER THORN
	/*DF = U+*/0x00DF, //LATIN SMALL LETTER SHARP S
	/*E0 = U+*/0x00E0, //LATIN SMALL LETTER A WITH GRAVE
	/*E1 = U+*/0x00E1, //LATIN SMALL LETTER A WITH ACUTE
	/*E2 = U+*/0x00E2, //LATIN SMALL LETTER A WITH CIRCUMFLEX
	/*E3 = U+*/0x00E3, //LATIN SMALL LETTER A WITH TILDE
	/*E4 = U+*/0x00E4, //LATIN SMALL LETTER A WITH DIAERESIS
	/*E5 = U+*/0x00E5, //LATIN SMALL LETTER A WITH RING ABOVE
	/*E6 = U+*/0x00E6, //LATIN SMALL LETTER AE
	/*E7 = U+*/0x00E7, //LATIN SMALL LETTER C WITH CEDILLA
	/*E8 = U+*/0x00E8, //LATIN SMALL LETTER E WITH GRAVE
	/*E9 = U+*/0x00E9, //LATIN SMALL LETTER E WITH ACUTE
	/*EA = U+*/0x00EA, //LATIN SMALL LETTER E WITH CIRCUMFLEX
	/*EB = U+*/0x00EB, //LATIN SMALL LETTER E WITH DIAERESIS
	/*EC = U+*/0x00EC, //LATIN SMALL LETTER I WITH GRAVE
	/*ED = U+*/0x00ED, //LATIN SMALL LETTER I WITH ACUTE
	/*EE = U+*/0x00EE, //LATIN SMALL LETTER I WITH CIRCUMFLEX
	/*EF = U+*/0x00EF, //LATIN SMALL LETTER I WITH DIAERESIS
	/*F0 = U+*/0x00F0, //LATIN SMALL LETTER ETH
	/*F1 = U+*/0x00F1, //LATIN SMALL LETTER N WITH TILDE
	/*F2 = U+*/0x00F2, //LATIN SMALL LETTER O WITH GRAVE
	/*F3 = U+*/0x00F3, //LATIN SMALL LETTER O WITH ACUTE
	/*F4 = U+*/0x00F4, //LATIN SMALL LETTER O WITH CIRCUMFLEX
	/*F5 = U+*/0x00F5, //LATIN SMALL LETTER O WITH TILDE
	/*F6 = U+*/0x00F6, //LATIN SMALL LETTER O WITH DIAERESIS
	/*F7 = U+*/0x00F7, //DIVISION SIGN
	/*F8 = U+*/0x00F8, //LATIN SMALL LETTER O WITH STROKE
	/*F9 = U+*/0x00F9, //LATIN SMALL LETTER U WITH GRAVE
	/*FA = U+*/0x00FA, //LATIN SMALL LETTER U WITH ACUTE
	/*FB = U+*/0x00FB, //LATIN SMALL LETTER U WITH CIRCUMFLEX
	/*FC = U+*/0x00FC, //LATIN SMALL LETTER U WITH DIAERESIS
	/*FD = U+*/0x00FD, //LATIN SMALL LETTER Y WITH ACUTE
	/*FE = U+*/0x00FE, //LATIN SMALL LETTER THORN
	/*FF = U+*/0x00FF  //LATIN SMALL LETTER Y WITH DIAERESIS
};

class TCStringObject : public TCObject
{
public:
	TCStringObject(void) : string(NULL) {}
	TCStringObject(const char *string) : string(copyString(string)) {}
	void setString(const char *value)
	{
		if (string != value)
		{
			delete string;
			string = copyString(value);
		}
	}
	const char *getString(void) { return string; }
protected:
	virtual ~TCStringObject(void);
	virtual void dealloc(void)
	{
		delete string;
		TCObject::dealloc();
	}

	char *string;
};

// I got a compiler warning about this not being inlined when it was in the
// class definition, so I pulled it out.
TCStringObject::~TCStringObject(void)
{
}

TCLocalStrings *TCLocalStrings::currentLocalStrings = NULL;
TCLocalStrings::TCLocalStringsCleanup TCLocalStrings::localStringsCleanup;
IntWCharMap TCLocalStrings::sm_codePages;

TCLocalStrings::TCLocalStringsCleanup::TCLocalStringsCleanup(void)
{
	TCLocalStrings::initCodePages();
}

TCLocalStrings::TCLocalStringsCleanup::~TCLocalStringsCleanup(void)
{
	if (currentLocalStrings)
	{
		currentLocalStrings->release();
	}
}

// Note: Code Page 1252 is Windows Latin I, which is the default.
TCLocalStrings::TCLocalStrings(void):
#ifndef WIN32
	m_textCodec(NULL),
#endif // WIN32
	m_codePage(1252)
{
	stringDict = new TCDictionary;
	sm_codePages[1250] = g_cp1250;
	sm_codePages[1252] = g_cp1252;
}

TCLocalStrings::~TCLocalStrings(void)
{
}

void TCLocalStrings::dealloc(void)
{
	TCObject::release(stringDict);
	TCObject::dealloc();
}

void TCLocalStrings::initCodePages(void)
{
}

bool TCLocalStrings::setStringTable(const char *stringTable, bool replace)
{
	return getCurrentLocalStrings()->instSetStringTable(stringTable, replace);
}

bool TCLocalStrings::setStringTable(const wchar_t *stringTable, bool replace)
{
	return getCurrentLocalStrings()->instSetStringTable(stringTable, replace);
}

#ifndef WIN32
const QString &TCLocalStrings::get(const char *key)
#else // WIN32
const char *TCLocalStrings::get(const char *key)
#endif // WIN32
{
	return getCurrentLocalStrings()->instGetLocalString(key);
}

const char *TCLocalStrings::getUtf8(const char *key)
{
	return getCurrentLocalStrings()->instGetUtf8LocalString(key);
}

const wchar_t *TCLocalStrings::get(const wchar_t *key)
{
	return getCurrentLocalStrings()->instGetLocalString(key);
}

bool TCLocalStrings::loadStringTable(const char *filename, bool replace)
{
	FILE *tableFile = fopen(filename, "rb");
	bool retValue = false;

	if (tableFile)
	{
		long fileSize;
		TCByte *fileData;

		fseek(tableFile, 0, SEEK_END);
		fileSize = ftell(tableFile);
		fseek(tableFile, 0, SEEK_SET);
		fileData = new TCByte[fileSize + 1];
		if (fread(fileData, 1, fileSize, tableFile) == (unsigned)fileSize)
		{
			bool bUnicode16 = false;
			bool bBigEndian = true;

			// Todo? 32-bit Unicode?
			if (fileData[0] == 0xFF && fileData[1] == 0xFE)
			{
				// Little Endian Unicode
				bUnicode16 = true;
				bBigEndian = false;
			}
			else if (fileData[0] == 0xFE && fileData[1] == 0xFF)
			{
				// Big Endian Unicode
				bUnicode16 = true;
			}
			if (bUnicode16)
			{
				std::wstring wstringTable;
				int i;
				int count = fileSize / 2;

				wstringTable.reserve(count + 1);
				// Note: skip first 2 bytes, which are the Byte Order Mark.
				for (i = 2; i < fileSize; i += 2)
				{
					int uByte;
					int lByte;

					if (bBigEndian)
					{
						uByte = fileData[i];
						lByte = fileData[i + 1];
					}
					else
					{
						uByte = fileData[i + 1];
						lByte = fileData[i];
					}
					wchar_t wc = (wchar_t)((uByte << 8) | lByte);
					wstringTable.append(&wc, 1);
				}
				// wstringTable now contains the string table.
				retValue = setStringTable(wstringTable.c_str(), replace);
			}
			else
			{
				char *stringTable = (char *)fileData;

				// Null terminate the string table
				stringTable[fileSize] = 0;
				retValue = setStringTable(stringTable, replace);
			}
		}
		delete fileData;
		fclose(tableFile);
	}
	return retValue;
}

TCLocalStrings *TCLocalStrings::getCurrentLocalStrings(void)
{
	if (!currentLocalStrings)
	{
		currentLocalStrings = new TCLocalStrings;
	}
	return currentLocalStrings;
}

void TCLocalStrings::dumpTable(const char *filename, const char *header)
{
	getCurrentLocalStrings()->instDumpTable(filename, header);
}

void TCLocalStrings::instDumpTable(const char *filename, const char *header)
{
	FILE *file = fopen(filename, "w");

	if (file)
	{
		TCSortedStringArray *keys = stringDict->allKeys();
		int i;
		int count = keys->getCount();

		if (header)
		{
			fprintf(file, "%s\n", header);
		}
		for (i = 0; i < count; i++)
		{
			const char *key = keys->stringAtIndex(i);
			const char *value = ((TCStringObject *)stringDict->objectForKey(key))->getString();

			fprintf(file, "%s = %s\n", key, value);
		}
		for (WStringWStringMap::iterator it = m_strings.begin(); it != m_strings.end(); it++)
		{
			fprintf(file, "%S = %S\n", it->first.c_str(), it->second.c_str());
		}
		fclose(file);
	}
}

bool TCLocalStrings::instSetStringTable(const char *stringTable, bool replace)
{
	bool sectionFound = false;
	int lastKeyIndex = -1;
	std::string lastKey;

	if (replace)
	{
		stringDict->removeAll();
	}
	while (1)
	{
		const char *eol = strchr(stringTable, '\n');

		if (!eol && strlen(stringTable) > 0)
		{
			eol = stringTable + strlen(stringTable);
		}
		if (eol)
		{
			int len = eol - stringTable;
			char *line = new char[len + 1];

			strncpy(line, stringTable, len);
			line[len] = 0;
			stripCRLF(line);
			stripLeadingWhitespace(line);
			if (!sectionFound)
			{
				// We haven't found the [StringTable] section yet
				stripTrailingWhitespace(line);
				if (stringHasCaseInsensitivePrefix(line, "[StringTable") &&
					stringHasSuffix(line, "]"))
				{
					char *codePageString = strcasestr(line, "CP=");

					sectionFound = true;
					if (codePageString)
					{
						int codePage;

						if (sscanf(&codePageString[3], "%d", &codePage) == 1)
						{
							instSetCodePage(codePage);
						}
					}
				}
				// Note that we are ignoring all lines until we find the section
			}
			else
			{
				// We're in the [StringTable] section
				if (line[0] == '[' && strchr(line, ']'))
				{
					// We found another section header, which means we are at
					// the end of the [StringTable] section, so we're done
					break;
				}
				else if (line[0] != ';')
				{
					// Comment lines begin with ;
					char *equalSpot = strchr(line, '=');

					if (equalSpot)
					{
						char *value;
						char *key = line;
						TCStringObject *stringObject;
						int keyLen;

						*equalSpot = 0;
						stripTrailingWhitespace(key);
						keyLen = strlen(key);
						if (keyLen)
						{
							bool appended = false;
							std::wstring wkey;
							std::wstring wvalue;

							mbstowstring(wkey, key, keyLen);
							value = copyString(equalSpot + 1);
							processEscapedString(value);
							mbstowstring(wvalue, value);
//							value = stringByReplacingSubstring(equalSpot + 1,
//								"\\n", "\n");
							if (isdigit(key[keyLen - 1]))
							{
								int keyIndex;

								// If the last character of the key is a digit,
								// then it must be a multi-line key.  So strip
								// off all trailing digits, and append to any
								// existing value.  Note that keys aren't
								// allowed to end in a digit, so even if there
								// is only one line, the key still gets the
								// number stripped off the end.
								while (isdigit(key[keyLen - 1]) && keyLen > 0)
								{
									keyLen--;
								}
								keyIndex = atoi(&key[keyLen]);
								key[keyLen] = 0;
								if (lastKey != key)
								{
									lastKeyIndex = 0;
								}
								if (lastKey == key &&
									lastKeyIndex + 1 != keyIndex)
								{
									debugPrintf(
										"Key index out of sequence: %s%d\n",
										key, keyIndex);
								}
								lastKeyIndex = keyIndex;
								lastKey = key;
								mbstowstring(wkey, key, keyLen);
								stringObject = (TCStringObject*)stringDict->
									objectForKey(key);
								if (stringObject)
								{
									// If we've already got data for this key,
									// we need to append to it and note that we
									// did so.
									char *newValue = new char[strlen(value) +
										strlen(stringObject->getString()) + 1];

									strcpy(newValue, stringObject->getString());
									strcat(newValue, value);
									// Note that we don't have to update the
									// dict; we're simply updating the text in
									// the string object already there.
									stringObject->setString(newValue);
									delete newValue;
									appended = true;
									// wstring copy constructor broken in VC++
									// 2005?!?!?  The below doesn't work without
									// the .c_str() calls.
									m_strings[wkey.c_str()] += wvalue.c_str();
								}
							}
							if (!appended)
							{
								if (stringDict->objectForKey(line))
								{
									debugPrintf("Local String key \"%s\" "
										"defined multiple times.\n", line);
								}
								stringObject = new TCStringObject(value);
								stringDict->setObjectForKey(stringObject, line);
								stringObject->release();
								// wstring copy constructor broken in VC++
								// 2005?!?!?  The below doesn't work without the
								// .c_str() calls.
								m_strings[wkey.c_str()] = wvalue.c_str();
							}
							delete value;
						}
					}
				}
			}
			delete line;
			if (!eol[0])
			{
				// If there isn't an EOL at the end of the file, we're done now.
				break;
			}
			stringTable += len + 1;
			while (stringTable[0] == '\r' || stringTable[0] == '\n')
			{
				stringTable++;
			}
		}
		else
		{
			break;
		}
	}
#ifndef WIN32
	buildQStringMap();
#endif // WIN32
	// Note that the load is considered a success if the [StringTable] section
	// is found in the data.
	return sectionFound;
}

void TCLocalStrings::instSetCodePage(int codePage)
{
	m_codePage = codePage;
	if (m_codePage == 1250)
	{
		return;
	}
#ifndef WIN32
	QString name;

	name.sprintf("CP%d", codePage);
	m_textCodec = QTextCodec::codecForName(name);
#endif // WIN32
}

bool TCLocalStrings::instSetStringTable(const wchar_t *stringTable,
										bool replace)
{
	bool sectionFound = false;
	int lastKeyIndex = -1;
	std::wstring lastKey;

	if (replace)
	{
		stringDict->removeAll();
	}
	while (1)
	{
		const wchar_t *eol = wcschr(stringTable, '\n');

		if (!eol && wcslen(stringTable) > 0)
		{
			eol = stringTable + wcslen(stringTable);
		}
		if (eol)
		{
			int len = eol - stringTable;
			wchar_t *line = new wchar_t[len + 1];

			wcsncpy(line, stringTable, len);
			line[len] = 0;
			stripCRLF(line);
			stripLeadingWhitespace(line);
			if (!sectionFound)
			{
				// We haven't found the [StringTable] section yet
				stripTrailingWhitespace(line);
				if (stringHasCaseInsensitivePrefix(line, L"[StringTable") &&
					stringHasSuffix(line, L"]"))
				{
					sectionFound = true;
					debugPrintf("Code Page setting found in Unicode string "
						"file.  Ignoring.\n");
				}
				// Note that we are ignoring all lines until we find the section
			}
			else
			{
				// We're in the [StringTable] section
				if (line[0] == '[' && wcschr(line, ']'))
				{
					// We found another section header, which means we are at
					// the end of the [StringTable] section, so we're done
					break;
				}
				else if (line[0] != ';')
				{
					// Comment lines begin with ;
					wchar_t *equalSpot = wcschr(line, '=');

					if (equalSpot)
					{
						wchar_t *value;
						wchar_t *key = line;
						TCStringObject *stringObject;
						int keyLen;

						*equalSpot = 0;
						stripTrailingWhitespace(key);
						keyLen = wcslen(key);
						if (keyLen)
						{
							bool appended = false;
							std::string skey;
							std::string svalue;

							wcstostring(skey, key, keyLen);
							value = copyString(equalSpot + 1);
							processEscapedString(value);
							wcstostring(svalue, value);
//							value = stringByReplacingSubstring(equalSpot + 1,
//								"\\n", "\n");
							if (isdigit(key[keyLen - 1]))
							{
								int keyIndex;

								// If the last character of the key is a digit,
								// then it must be a multi-line key.  So strip
								// off all trailing digits, and append to any
								// existing value.  Note that keys aren't
								// allowed to end in a digit, so even if there
								// is only one line, the key still gets the
								// number stripped off the end.
								while (isdigit(key[keyLen - 1]) && keyLen > 0)
								{
									keyLen--;
								}
								keyIndex = wcstoul(&key[keyLen], NULL, 10);
								key[keyLen] = 0;
								if (lastKey != key)
								{
									lastKeyIndex = 0;
								}
								if (lastKey == key &&
									lastKeyIndex + 1 != keyIndex)
								{
									debugPrintf(
										"Key index out of sequence: %s%d\n",
										key, keyIndex);
								}
								lastKeyIndex = keyIndex;
								lastKey = key;
								wcstostring(skey, key, keyLen);
								stringObject = (TCStringObject*)stringDict->
									objectForKey(skey.c_str());
								if (stringObject)
								{
									// If we've already got data for this key,
									// we need to append to it and note that we
									// did so.
									char *newValue = new char[svalue.size() +
										strlen(stringObject->getString()) + 1];

									strcpy(newValue, stringObject->getString());
									strcat(newValue, svalue.c_str());
									// Note that we don't have to update the
									// dict; we're simply updating the text in
									// the string object already there.
									stringObject->setString(newValue);
									delete newValue;
									appended = true;
									m_strings[key] += value;
								}
							}
							if (!appended)
							{
								if (stringDict->objectForKey(skey.c_str()))
								{
									debugPrintf("Local String key \"%s\" "
										"defined multiple times.\n",
										skey.c_str());
								}
								stringObject =
									new TCStringObject(svalue.c_str());
								stringDict->setObjectForKey(stringObject,
									skey.c_str());
								stringObject->release();
								m_strings[key] = value;
							}
							delete value;
						}
					}
				}
			}
			delete line;
			if (!eol[0])
			{
				// If there isn't an EOL at the end of the file, we're done now.
				break;
			}
			stringTable += len + 1;
			while (stringTable[0] == '\r' || stringTable[0] == '\n')
			{
				stringTable++;
			}
		}
		else
		{
			break;
		}
	}
#ifndef WIN32
	buildQStringMap();
#endif // WIN32
	// Note that the load is considered a success if the [StringTable] section
	// is found in the data.
	return sectionFound;
}

const wchar_t *TCLocalStrings::instGetLocalString(const wchar_t *key)
{
	WStringWStringMap::const_iterator it = m_strings.find(key);

	if (it != m_strings.end())
	{
		return it->second.c_str();
	}
	else
	{
		std::string temp;

		wstringtostring(temp, key);
		debugPrintf("LocalString %s not found!!!!!!\n", temp.c_str());
		// It should really be NULL, but that means a mistake will likely cause
		// a crash.  At least with an empty string it's less likely to crash.
		return L"";
	}
}

void TCLocalStrings::mbstowstring(std::wstring &dst, const char *src,
								  int length /*= -1*/)
{
	wchar_t *codePageTable = NULL;
	IntWCharMap::const_iterator it = sm_codePages.find(m_codePage);

	if (it != sm_codePages.end())
	{
		codePageTable = it->second;
	}
	if (codePageTable)
	{
		int i;

		if (length == -1)
		{
			length = strlen(src);
		}
		dst.resize(length);
		for (i = 0; i < length; i++)
		{
			dst[i] = codePageTable[(TCByte)src[i]];
		}
	}
#ifndef WIN32
	else if (m_textCodec)
	{
		QString unicodeString = m_textCodec->toUnicode(src);
		dst.clear();
		dst.resize(unicodeString.length());
		for (int i = 0; i < (int)unicodeString.length(); i++)
		{
			QChar qchar = unicodeString.at(i);

			dst[i] = (wchar_t)qchar.unicode();
		}
	}
#endif // WIN32
	else
	{
		::mbstowstring(dst, src, length);
	}
}

#ifndef WIN32
const QString &TCLocalStrings::instGetLocalString(const char *key)
{
	QStringQStringMap::iterator it = m_qStrings.find(key);

	if (it != m_qStrings.end())
	{
		return it->second;
	}
	else
	{
		debugPrintf("LocalString %s not found!!!!!!\n", key);
		return m_emptyQString;
	}
}

void TCLocalStrings::buildQStringMap(void)
{
	m_qStrings.clear();
	for (WStringWStringMap::iterator it = m_strings.begin();
		it != m_strings.end(); it++)
	{
		QString key;
		QString value;

		wstringtoqstring(key, it->first);
		wstringtoqstring(value, it->second);
		m_qStrings[key] = value;
	}
}

#else // WIN32
const char *TCLocalStrings::instGetLocalString(const char *key)
{
	TCStringObject *stringObject =
		(TCStringObject*)stringDict->objectForKey(key);

	if (stringObject)
	{
		return stringObject->getString();
	}
	else
	{
		debugPrintf("LocalString %s not found!!!!!!\n", key);
		// It should really be NULL, but that means a mistake will likely cause
		// a crash.  At least with an empty string it's less likely to crash.
		return "";
	}
}
#endif // WIN32

const char *TCLocalStrings::instGetUtf8LocalString(const char *key)
{
	StringStringMap::const_iterator it = m_utf8Strings.find(key);

	if (it != m_utf8Strings.end())
	{
		return it->second.c_str();
	}
	else
	{
#ifdef TC_NO_UNICODE
		m_utf8Strings[key] = instGetLocalString(key);
#else // TC_NO_UNICODE
		std::wstring wKey;
		const wchar_t *wValue;
		char *utf8Value;

		mbstowstring(wKey, key);
		wValue = instGetLocalString(wKey.c_str());
		utf8Value = ucstringtoutf8(wValue);
		if (utf8Value)
		{
			m_utf8Strings[key] = utf8Value;
			delete utf8Value;
		}
		else
		{
			m_utf8Strings[key] = (const char *)instGetLocalString(key);
		}
#endif // TC_NO_UNICODE
		return m_utf8Strings[key].c_str();
	}
}
