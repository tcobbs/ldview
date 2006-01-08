/*
Handle quotes in symbolic searchdirs
< > check in sym dirs
*/

/* LDrawIni.c */
/*
If this code works, it was written by Lars C. Hassing.
If not, I don't know who wrote it.
040228 lch First version of LDrawIni handling routines
040307 lch Added LgeoDir
040319 lch Added LDrawInP.h (Really LDrawIniP.h but 8+3 name...)
040513 lch Added LDrawIniParseSymbolicSearchDir
041202 lch Changed parameter ModelDir to ModelPath in LDrawIniComputeRealDirs
*/

#include <TCFoundation/TCDefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
#include "direct.h"
#include <sys/types.h>
*/
#include <sys/stat.h>
#include "LDrawIni.h"
#include "LDrawInP.h"

/*
External references:
malloc
calloc
realloc
free
strdup
strlen
strcpy
strcat
strcmp
strncmp
strncpy
strchr
fopen
fclose
getc
ungetc
ferror
memcpy
sprintf
getenv
stat
*/

/* Preprocessor flags:
 _WIN32      VC++
 __TURBOC__  Borland TurboC
 __linux__   RedHat7.3 gcc -E -dM reveals: __ELF__ __i386 __i386__ i386
                 __i586 __i586__ i586 __linux __linux__ linux
                 __pentium __pentium__ pentium __unix __unix__ unix
 __APPLE__   Mac OSX: gcc -E -dM reveals: __APPLE__=1 __APPLE_API_EVOLVING
                 __APPLE_API_OBSOLETE __APPLE_API_PRIVATE __APPLE_API_STABLE
                 __APPLE_API_STANDARD __APPLE_API_UNSTABLE __APPLE_CC__=1151
                 __GNUC__=3 __MACH__=1 __STDC__=1
*/
/* Naming refers to Windows platform */
#if defined(_WIN32) || defined(__TURBOC__)
#define BACKSLASH_CHAR '\\'
#define BACKSLASH_STRING "\\"
#define SLASH_CHAR '/'
#define MAX_PATH_LENGTH 260
#define HOME_ENV_VAR "USERPROFILE"
#else
#define BACKSLASH_CHAR '/'
#define BACKSLASH_STRING "/"
#define SLASH_CHAR '\\'
#define MAX_PATH_LENGTH 1024
#define HOME_ENV_VAR "HOME"
#endif

#ifdef __TURBOC__
#define  malloc   farmalloc
#define  realloc  farrealloc
#define  free     farfree
#define  strdup   Strdup
static char *Strdup(const char *Str);
#endif

static void FreeSearchDirs(struct LDrawIniS * LDrawIni);
static void FreeSymbolicDirs(struct LDrawIniS * LDrawIni);
static const char *GetDefaultLDrawSearch(void);
static int SplitLDrawSearch(const char *LDrawSearchString,
                            int *nDirs, char ***Dirs);
static int ReadLDrawIniFiles(struct LDrawIniS * LDrawIni,
                             const char *Section, const char *Key,
                             char *Str, int sizeofStr, char *IniFile);
static void FixSlashes(register char *Path);
static int IsDir(char *Path, LDrawFileCaseCallback FileCaseCallback);
static char *L3fgets(char *Str, int n, FILE *fp);
static void TrimRight(char *Str, int BlanksToo);


/*
Initialize and read all settings.
If the argument LDrawDir is not NULL
it will override the normal initialization of the LDraw directory.
If ErrorCode is not NULL it will return a code telling why
LDrawIniGet returned NULL.
If all is OK then a pointer to struct LDrawIniS is returned.
You should then call LDrawIniComputeRealDirs to obtain the search dirs.
Remember to free the struct by calling LDrawIniFree.
*/
struct LDrawIniS *LDrawIniGet(const char *LDrawDir, int *ErrorCode)
{
   struct LDrawIniS *LDrawIni;
   const char    *e;
   char           Str[MAX_PATH_LENGTH];
   int            i;
   char           Key[40];
   struct LDrawIniPrivateDataS *pd;
   char           IniFile[MAX_PATH_LENGTH];

   if (ErrorCode)
      *ErrorCode = LDRAWINI_ERROR_OUT_OF_MEMORY;   /* Default                */
   LDrawIni = (struct LDrawIniS *) calloc(1, sizeof(struct LDrawIniS));
   if (!LDrawIni)
      return NULL;
   LDrawIni->PrivateData = (struct LDrawIniPrivateDataS *)
      calloc(1, sizeof(struct LDrawIniPrivateDataS));
   if (!LDrawIni->PrivateData)
   {
      free(LDrawIni);
      return NULL;
   }

   /* Now initialize the struct by reading env var or ini files */

   /* LDrawDir */

   e = LDrawDir;                /* Use argument if specified                 */
   if (!e)
      e = getenv("LDRAWDIR");   /* Try environment variable                  */
   if (!e)
   {
      /* Then try LDraw.ini files */
      if (!ReadLDrawIniFiles(LDrawIni, "LDraw", "BaseDirectory", Str, sizeof(Str), NULL))
      {
         if (ErrorCode)
            *ErrorCode = LDRAWINI_ERROR_LDRAWDIR_NOT_SET;
         free(LDrawIni);
         return NULL;
      }
      e = Str;
   }
   LDrawIni->LDrawDir = strdup(e);
   FixSlashes(LDrawIni->LDrawDir);

   /* LDrawSearch, read symbolic dirs */
   pd = LDrawIni->PrivateData;

   /* First try environment variable */
   e = getenv("LDRAWSEARCH");
   if (e)
   {
      i = SplitLDrawSearch(e, &pd->nSymbolicSearchDirs, &pd->SymbolicSearchDirs);
      if (!i)
         return NULL;           /* No more memory, just give up              */
   }
   else
   {
      e = getenv("LDRAWSEARCH01");
      if (e)
      {
         /* LDRAWSEARCH01 set, alloc room for 99 dirs */
         pd->SymbolicSearchDirs = (char **) malloc(99 * sizeof(char *));
         if (!pd->SymbolicSearchDirs)
            return NULL;        /* No more memory, just give up              */
         while (pd->nSymbolicSearchDirs <= 99)
         {
            sprintf(Key, "LDRAWSEARCH%02d", pd->nSymbolicSearchDirs + 1);
            e = getenv(Key);
            if (!e)
               break;
            pd->SymbolicSearchDirs[pd->nSymbolicSearchDirs] = strdup(e);
            if (!pd->SymbolicSearchDirs[pd->nSymbolicSearchDirs++])
               return NULL;     /* No more memory, just give up              */
         }
         /* Reduce memory to those found */
         pd->SymbolicSearchDirs =
            (char **) realloc(pd->SymbolicSearchDirs,
                              pd->nSymbolicSearchDirs * sizeof(char *));
         if (!pd->SymbolicSearchDirs)
            return NULL;        /* No more memory, just give up              */
      }
      else
      {
         /* Then try LDraw.ini files (be sure to read all from same ini
            file)                                                            */
         IniFile[0] = '\0';
         if (ReadLDrawIniFiles(LDrawIni, "LDrawSearch", "1", Str, sizeof(Str), IniFile))
         {
            /* Key "1" found, alloc room for 99 dirs */
            pd->SymbolicSearchDirs = (char **) malloc(99 * sizeof(char *));
            if (!pd->SymbolicSearchDirs)
               return NULL;     /* No more memory, just give up              */
            while (pd->nSymbolicSearchDirs < 99)
            {
               pd->SymbolicSearchDirs[pd->nSymbolicSearchDirs] = strdup(Str);
               if (!pd->SymbolicSearchDirs[pd->nSymbolicSearchDirs++])
                  return NULL;  /* No more memory, just give up              */
               sprintf(Key, "%d", pd->nSymbolicSearchDirs + 1);
               if (!ReadLDrawIniFiles(LDrawIni, "LDrawSearch", Key, Str, sizeof(Str), IniFile))
                  break;
            }
            /* Reduce memory to those found */
            pd->SymbolicSearchDirs =
               (char **) realloc(pd->SymbolicSearchDirs,
                                 pd->nSymbolicSearchDirs * sizeof(char *));
            if (!pd->SymbolicSearchDirs)
               return NULL;     /* No more memory, just give up              */
         }
         else
         {
            /* Not in env, not in ini file, use default */
            i = SplitLDrawSearch(GetDefaultLDrawSearch(),
                                 &pd->nSymbolicSearchDirs,
                                 &pd->SymbolicSearchDirs);
            if (!i)
               return NULL;     /* No more memory, just give up              */
         }
      }
   }

   /* LgeoDir */

   e = getenv("LGEODIR");       /* Try environment variable                  */
   if (!e)
   {
      /* Then try LDraw.ini files */
      if (ReadLDrawIniFiles(LDrawIni, "LDraw", "LgeoDirectory", Str, sizeof(Str), NULL))
         e = Str;               /* It was defined in an ini file             */
   }
   if (e)
   {
      LDrawIni->LgeoDir = strdup(e);
      FixSlashes(LDrawIni->LgeoDir);
   }

   if (ErrorCode)
      *ErrorCode = 0;
   return LDrawIni;
}                               /* LDrawIniGet                               */

/*
Set file case callback
*/
int LDrawIniSetFileCaseCallback(struct LDrawIniS *LDrawIni,
                                LDrawFileCaseCallback FileCaseCallback)
{
   if (LDrawIni->PrivateData)
   {
      LDrawIni->PrivateData->FileCaseCallback = FileCaseCallback;
      return 1;
   }
   else
   {
      return 0;
   }
}

/*
Free the LDrawIni data
*/
void LDrawIniFree(struct LDrawIniS * LDrawIni)
{
   if (!LDrawIni)
      return;
   if (LDrawIni->LDrawDir)
      free(LDrawIni->LDrawDir);
   FreeSearchDirs(LDrawIni);
   if (LDrawIni->ModelDir)
      free(LDrawIni->ModelDir);
   if (LDrawIni->LgeoDir)
      free(LDrawIni->LgeoDir);

   /* Free private data */
   FreeSymbolicDirs(LDrawIni);
   free(LDrawIni->PrivateData);

   free(LDrawIni);
}


/*
Reset search dirs to default if LDrawSearch is NULL
or to the dirs specified in LDrawSearch delimited by |.
Returns 1 if OK, 0 on error
*/
int LDrawIniResetSearchDirs(struct LDrawIniS * LDrawIni,
                            const char *LDrawSearch)
{
   struct LDrawIniPrivateDataS *pd;

   if (!LDrawIni)
      return 0;
   FreeSymbolicDirs(LDrawIni);
   if (!LDrawSearch)
      LDrawSearch = GetDefaultLDrawSearch();
   pd = LDrawIni->PrivateData;
   return SplitLDrawSearch(LDrawSearch,
                           &pd->nSymbolicSearchDirs,
                           &pd->SymbolicSearchDirs);
}


static void FreeSearchDirs(struct LDrawIniS * LDrawIni)
{
   int            i;

   if (LDrawIni->nSearchDirs)
   {
      for (i = 0; i < LDrawIni->nSearchDirs; i++)
      {
         if (LDrawIni->SearchDirs[i].UnknownFlags)
            free(LDrawIni->SearchDirs[i].UnknownFlags);
         free(LDrawIni->SearchDirs[i].Dir);
      }
      free(LDrawIni->SearchDirs);
   }
   LDrawIni->nSearchDirs = 0;
   LDrawIni->SearchDirs = NULL;
}

static void FreeSymbolicDirs(struct LDrawIniS * LDrawIni)
{
   int            i;
   struct LDrawIniPrivateDataS *pd;

   pd = LDrawIni->PrivateData;
   if (pd->nSymbolicSearchDirs)
   {
      for (i = 0; i < pd->nSymbolicSearchDirs; i++)
         free(pd->SymbolicSearchDirs[i]);
      free(pd->SymbolicSearchDirs);
   }
   pd->nSymbolicSearchDirs = 0;
   pd->SymbolicSearchDirs = NULL;
}

static const char *GetDefaultLDrawSearch(void)
{
   return "<MODELDIR>"
   "|<HIDE><LDRAWDIR>\\P"
   "|<LDRAWDIR>\\PARTS"
   "|<LDRAWDIR>\\MODELS";
}

/* Returns 1 if OK, 0 on error */
static int SplitLDrawSearch(const char *LDrawSearchString, int *nDirs, char ***Dirs)
{
   const char    *s;
   const char    *t;
   char          *Dir;
   int            n;
   int            Len;

   /* Count number of dir separators '|' */
   for (n = 1, s = strchr(LDrawSearchString, '|'); s; s = strchr(s + 1, '|'))
      ++n;
   *Dirs = (char **) malloc(n * sizeof(char *));
   if (!*Dirs)
      return 0;
   for (n = 0, s = LDrawSearchString; *s;)
   {
      t = s;
      while (*t && *t != '|')
         ++t;
      Len = t - s;
      Dir = (char *) malloc(Len + 1);
      if (!Dir)
         return 0;
      memcpy(Dir, s, Len);
      Dir[Len] = '\0';
      (*Dirs)[n] = Dir;
      if (!(*Dirs)[n++])
         return 0;
      s = *t ? t + 1 : t;
   }
   *nDirs = n;
   return 1;
}

#ifdef __APPLE__
/* XML/plist format:
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="1.0">
<dict>
    <key>Author</key>
    <string>William Shakespeare</string>
    <key>Lines</key>
    <array>
        <string>It is a tale told by an idiot,</string>
        <string>Full of sound and fury, signifying nothing.</string>
    </array>
    <key>Birthdate</key>
    <integer>1564</integer>
</dict>
</plist>
*/

/* Returns 1 if OK, 0 if Section/Key not found or error */
int LDrawIniReadPlistFile(const char *PlistFile,
                          const char *Section, const char *Key,
                          char *Str, int sizeofStr)
{
   return 0;                    /* Key not found                             */
}                               /* LDrawPlistReadIniFile                     */
#endif

/* Returns 1 if OK, 0 if Section/Key not found or error */
#ifdef __APPLE__
/* IniFile may be in either Windows Ini format or XML/plist format */
#endif
int LDrawIniReadIniFile(const char *IniFile,
                        const char *Section, const char *Key,
                        char *Str, int sizeofStr)
{
   char           Buf[400];
   FILE          *fp;
   int            InSection;
   int            SectionLen;
   int            KeyLen;
#ifdef __APPLE__
   int            FirstLine = 1;
#endif

   fp = fopen(IniFile, "rt");
   if (!fp)
      return 0;

   InSection = 0;
   SectionLen = strlen(Section);
   KeyLen = strlen(Key);
   while (L3fgets(Buf, sizeof(Buf), fp))
   {
#ifdef __APPLE__
      if (FirstLine)
      {
         FirstLine = 0;
         if (Buf[0] == '<' && Buf[1] == '?')
         {
            /* Switch to parsing XML/plist format: <?xml version="1.0" ... */
            fclose(fp);
            return LDrawIniReadPlistFile(IniFile, Section, Key, Str, sizeofStr);
         }
      }
#endif
      if (!InSection)
      {
         if (Buf[0] == '[' &&
             strncmp(Buf + 1, Section, SectionLen) == 0 &&
             Buf[1 + SectionLen] == ']')
         {
            InSection = 1;
         }
      }
      else
      {
         if (Buf[0] == '[')
            break;              /* End of our section                        */
         if (strncmp(Buf, Key, KeyLen) == 0 && Buf[KeyLen] == '=')
         {
            fclose(fp);
            strncpy(Str, Buf + KeyLen + 1, sizeofStr - KeyLen - 1);
            Str[sizeofStr - 1] = '\0'; /* Be sure to terminate               */
            TrimRight(Str, 1);
            return 1;
         }
      }
   }
   fclose(fp);
   return 0;                    /* Key not found                             */
}                               /* LDrawIniReadIniFile                       */

/* Returns 1 if OK, 0 if Section/Key not found or error */
/* If IniFile is specified then that files is read. If IniFile is an empty
   string it is assumed to be a buffer of size MAX_PATH_LENGTH which will receive
   the path of the inifile actually read.                                    */
static int ReadLDrawIniFiles(struct LDrawIniS * LDrawIni,
                             const char *Section, const char *Key,
                             char *Str, int sizeofStr, char *IniFile)
{
   char           TmpPath[MAX_PATH_LENGTH];
   const char    *e;
   int            Res;

   /* First see if IniFile is specified */
   if (IniFile && *IniFile)
      return LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);

   /* It wasn't, find out which ini file to use */
   if (!IniFile)
      IniFile = TmpPath;        /* Caller doesn't care, use our own buffer   */
   e = getenv("LDRAWINI");
   if (e)
   {
      strcpy(IniFile, e);
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
   }
#if defined(_WIN32) || defined(__TURBOC__)
   e = getenv("USERPROFILE");
   if (e)
   {
      strcpy(IniFile, e);
      strcat(IniFile, "\\LDraw.ini");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
   }
   e = getenv("ALLUSERSPROFILE");
   if (e)
   {
      strcpy(IniFile, e);
      strcat(IniFile, "\\LDraw.ini");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
   }
   if (LDrawIni->LDrawDir)
   {
      strcpy(IniFile, LDrawIni->LDrawDir);
      strcat(IniFile, "\\LDraw.ini");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
   }
   e = getenv("windir");
   if (!e)
      e = "C:\\Windows";
   strcpy(IniFile, e);
   strcat(IniFile, "\\LDraw.ini");
   Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
   if (Res)
      return Res;
#else
   /* Linux + MacOSX */
   e = getenv("HOME");
   if (e)
   {
      strcpy(IniFile, e);
      strcat(IniFile, "/.ldrawrc");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
      strcpy(IniFile, e);
      strcat(IniFile, "/ldraw.ini");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
#ifdef __APPLE__
      /* No need to read ~/.MacOSX/environment.plist, getenv() does that! */
      /* ~/Library/Preferences/org.ldraw.plist */
      strcpy(IniFile, e);
      strcat(IniFile, "/Library/Preferences/org.ldraw.plist");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
#endif
   }
#ifdef __APPLE__
   /* /Library/Preferences/org.ldraw.plist */
   strcpy(IniFile, "/Library/Preferences/org.ldraw.plist");
   Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
   if (Res)
      return Res;
#endif
   if (LDrawIni->LDrawDir)
   {
      strcpy(IniFile, LDrawIni->LDrawDir);
      strcat(IniFile, "/ldraw.ini");
      Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
      if (Res)
         return Res;
   }
   strcpy(IniFile, "/etc/ldraw.ini");
   Res = LDrawIniReadIniFile(IniFile, Section, Key, Str, sizeofStr);
   if (Res)
      return Res;
#endif
   IniFile[0] = '\0';
   return 0;                    /* Key not found                             */
}                               /* ReadLDrawIniFiles                         */

/*
Compute Real Dirs by substituting <LDRAWDIR> and <MODELDIR> in
the Symbolic Dirs read from the env vars or ini files.
If OnlyValidDirs is true then non-existing search dirs is skipped
If AddTrailingSlash is true then the search dirs will have a slash/backslash appended.
If ModelPath is NULL then search dir <MODELDIR> is skipped.
Otherwise LDrawIni->ModelDir is updated.
Returns 1 if OK, 0 on error
*/
int LDrawIniComputeRealDirs(struct LDrawIniS * LDrawIni,
                            int OnlyValidDirs,
                            int AddTrailingSlash,
                            const char *ModelPath)
{
   struct LDrawIniPrivateDataS *pd;
   const char    *HomeDir;
   int            i;
   int            Res;
   struct LDrawSearchDirS SearchDir;


   if (!LDrawIni)
      return 0;
   if (!LDrawIni->LDrawDir)
      return 0;
   pd = LDrawIni->PrivateData;
   if (!pd->nSymbolicSearchDirs)
      return 0;
   FreeSearchDirs(LDrawIni);

   /* We may allocate too much here because some dirs may be skipped... */
   LDrawIni->SearchDirs =
      (struct LDrawSearchDirS *) calloc(pd->nSymbolicSearchDirs,
                                        sizeof(struct LDrawSearchDirS));
   if (!LDrawIni->SearchDirs)
      return 0;
   if (LDrawIni->ModelDir)
      free(LDrawIni->ModelDir);
   if (ModelPath)
   {
/*
ModelPath    ModelDir
dir/car.ldr  dir
dir\car.ldr  dir
car.ldr      .
c:\car.ldr   c:
/car.ldr
*/
      i = strlen(ModelPath);
      while (--i >= 0)
      {
         if (ModelPath[i] == '/' || ModelPath[i] == '\\')
            break;  
      }
      /* i is now reduced to only copy dir part of ModelPath */
      if (i < 0)
      {
         /* ModelPath is only a filename, use "." as directory */
         ModelPath = ".";
         i = 1;
      }
      LDrawIni->ModelDir = (char *) malloc(i + 1);
      if (!LDrawIni->ModelDir)
         return 0;
      memcpy(LDrawIni->ModelDir, ModelPath, i);
      LDrawIni->ModelDir[i] = '\0';
   }
   else
      LDrawIni->ModelDir = NULL;
   HomeDir = getenv(HOME_ENV_VAR);
   for (i = 0; i < pd->nSymbolicSearchDirs; i++)
   {
      Res = LDrawIniParseSymbolicSearchDir(&SearchDir,
                                           pd->SymbolicSearchDirs[i],
                                           LDrawIni->LDrawDir,
                                           LDrawIni->ModelDir,
                                           HomeDir);

      if (Res < 0)
      {
         FreeSearchDirs(LDrawIni);
         return 0;
      }
      if (Res == 0)
         continue;              /* ModelDir/HomeDir not applicable           */
      /* ModelDir may be "" for current dir */
      /* NOTE: IsDir might change the case of the value in SearchDir.Dir to */
      /* match the case of the actual directory on the filesystem. */
      if ((OnlyValidDirs && SearchDir.Dir[0] && !IsDir(SearchDir.Dir,
          LDrawIni->PrivateData->FileCaseCallback)) ||
          (SearchDir.Flags & LDSDF_SKIP))
      {
         if (SearchDir.UnknownFlags)
            free(SearchDir.UnknownFlags);
         free(SearchDir.Dir);
         continue;
      }
      if (SearchDir.Dir[0] && AddTrailingSlash)
         strcat(SearchDir.Dir, BACKSLASH_STRING);  /* Dir has room for this  */
      LDrawIni->SearchDirs[LDrawIni->nSearchDirs++] = SearchDir;
   }
   return 1;
}                               /* LDrawIniComputeRealDirs                   */

/*
Parse SymbolicSearchDir into Result
Returns 1 if OK, 0 if ModelDir/HomeDir not applicable, -1 on memory error.
*/
int LDrawIniParseSymbolicSearchDir(struct LDrawSearchDirS * Result,
                                   const char *SymbolicSearchDir,
                                   const char *LDrawDir,
                                   const char *ModelDir,
                                   const char *HomeDir)
{
   const char    *s;
   const char    *t;
   char          *Dir;
   int            Flags;
   int            OldLen;
   int            Len;
   char          *UnknownFlags;
   const char    *PrefixDir;


   if (!Result)
      return -1;
   if (!SymbolicSearchDir)
      return -1;
   if (!LDrawDir)
      return -1;

   s = SymbolicSearchDir;
   Flags = 0;
   UnknownFlags = NULL;
   /* First parse any flags on the form <FLAG> */
   while (*s == '<')
   {
      if (strncmp(s, "<SKIP>", 6) == 0)
      {
         Flags |= LDSDF_SKIP;
         s += 6;
         continue;
      }
      if (strncmp(s, "<HIDE>", 6) == 0)
      {
         Flags |= LDSDF_HIDE;
         s += 6;
         continue;
      }
      if (strncmp(s, "<SHOW>", 6) == 0)
      {
         /* Just skip this flag, it is the default */
         s += 6;
         continue;
      }
      if (strncmp(s, "<DEFPART>", 9) == 0)
      {
         Flags |= LDSDF_DEFPART;
         s += 9;
         continue;
      }
      if (strncmp(s, "<DEFPRIM>", 9) == 0)
      {
         Flags |= LDSDF_DEFPRIM;
         s += 9;
         continue;
      }
      if (strncmp(s, "<LDRAWDIR>", 10) == 0 ||
          strncmp(s, "<MODELDIR>", 10) == 0 ||
          strncmp(s, "<HOMEDIR>", 9) == 0)
      {
         /* These are not flags, but indicate start of Dir */
         break;
      }
      /* Not a known flag, save it */
      t = strchr(s + 1, '>');
      if (!t)
         break;                 /* Hm, no matching >, use rest as Dir        */
      Len = t - s + 1;
      OldLen = UnknownFlags ? strlen(UnknownFlags) : 0;
      UnknownFlags = (char *) realloc(UnknownFlags, OldLen + Len + 1);
      if (!UnknownFlags)
         return 0;
      memcpy(UnknownFlags + OldLen, s, Len);
      UnknownFlags[OldLen + Len] = '\0';
      s += Len;
   }                            /* while (*s == '<')                         */

   /* s now points to Dir. Handle double-quotes */
   t = strchr(s, '"');
   if (t)
   {
      s = t + 1;                /* s points to after begin quote             */
      t = strchr(s, '"');       /* Find end quote                            */
   }
   PrefixDir = "";              /* Empty (NULL is error)                     */
   if (strncmp(s, "<LDRAWDIR>", 10) == 0)
   {
      s += 10;
      PrefixDir = LDrawDir;
      if (*s)
      {
         if (strcmp(s + 1, "P") == 0)
            Flags |= LDSDF_DEFPRIM;
         if (strcmp(s + 1, "PARTS") == 0)
            Flags |= LDSDF_DEFPART;
      }
   }
   else if (strncmp(s, "<MODELDIR>", 10) == 0)
   {
      s += 10;
      PrefixDir = ModelDir;
      Flags |= LDSDF_MODELDIR;
   }
   else if (strncmp(s, "<HOMEDIR>", 9) == 0)
   {
      s += 9;
      PrefixDir = HomeDir;
   }
   if (!PrefixDir)
   {
      if (UnknownFlags)
         free(UnknownFlags);
      return 0;                 /* ModelDir/HomeDir not applicable           */
   }
   OldLen = strlen(PrefixDir);
   Len = t ? t - s : strlen(s);
   Dir = (char *) malloc(OldLen + Len + 1 + 1); /* See AddTrailingSlash      */
   if (!Dir)
      return 0;
   strcpy(Dir, PrefixDir);
   memcpy(Dir + OldLen, s, Len);
   Dir[OldLen + Len] = '\0';
   FixSlashes(Dir);
   Result->Flags = Flags;
   Result->UnknownFlags = UnknownFlags;
   Result->Dir = Dir;

   return 1;
}                               /* LDrawIniParseSymbolicSearchDir            */

/* Modifies the string: OS-correct slashes or backslashes */
static void FixSlashes(register char *Path)
{
   for (; *Path; Path++)
      if (*Path == SLASH_CHAR)
         *Path = BACKSLASH_CHAR;
}

static int IsDir(char *Path, LDrawFileCaseCallback FileCaseCallback)
{
   struct stat    Stat;

   if (stat(Path, &Stat) == 0)
   {
      return (Stat.st_mode & S_IFDIR);
   }
   else if (FileCaseCallback != NULL && FileCaseCallback(Path))
   {
      if (stat(Path, &Stat) == 0)
      {
         return (Stat.st_mode & S_IFDIR);
      }
   }
   return 0;
}

/* Like fgets, except that any line ending is accepted
   (\n (unix), \r\n (DOS/Windows), \r (Mac (OS9))                            */
static char *L3fgets(char *Str, int n, FILE *fp)
{
   register int   c;
   int            nextc;
   register char *s = Str;

   while (--n > 0)
   {
      if ((c = getc(fp)) == EOF)
         break;
      if (c == '\032')
         continue;              /* Skip CTRL+Z                               */
      if (c == '\r' || c == '\n')
      {
         *s++ = '\n';
         /* We got CR or LF, eat next character if LF or CR respectively */
         if ((nextc = getc(fp)) == EOF)
            break;
         if (nextc == c || (nextc != '\r' && nextc != '\n'))
            ungetc(nextc, fp);  /* CR-CR or LF-LF or ordinary character      */
         break;
      }
      *s++ = (char)c;
   }
   *s = 0;

   if (ferror(fp))
      return NULL;
   if (s == Str)
      return NULL;

   return Str;
}

static void TrimRight(char *Str, int BlanksToo)
{
   register char *s;

   /* 971114/lch. Win95: If a file ends with a line not terminated by CR/LF
      fgets will not append a newline. So Line[Len-1] may not be a newline but
      a valid character.                                                     */
   /* 010930/lch. Linux: Also remove Ctrl+Z, the DOS EOF character */
   /* TurboC huge pointers for pointer comparison? */
   s = Str + strlen(Str) - 1;
   while (s >= Str &&
          (*s == '\n' || *s == '\r' || *s == '\032' ||
           BlanksToo && (*s == '\t' || *s == ' ')))
   {
      *s-- = '\0';              /* Clear newline and trailing tabs and
                                   spaces                                    */
   }
}

#ifdef __TURBOC__
static char *Strdup(const char *Str)
{
   char          *Copy;

   Copy = (char *) malloc(strlen(Str) + 1);  /* farmalloc in TurboC          */
   if (Copy)
      strcpy(Copy, Str);
   return Copy;
}
#endif

#if (0)
void DeleteTrailingBackslash(char *Str)
{
   register char *s;

   s = Str + strlen(Str) - 1;
   while (s >= Str && *s == BACKSLASH_CHAR)
      *s-- = '\0';
}

static char *FirstNonBlank(register char *s)
{
   while (*s == '\t' || *s == ' ')
      s++;
   return s;
}
#endif
