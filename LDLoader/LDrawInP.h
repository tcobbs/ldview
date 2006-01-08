/* LDrawInP.h */
/*
040319 lch Added LDrawInP.h (Really LDrawIniP.h but 8+3 name...)
040513 lch Added LDrawIniParseSymbolicSearchDir
*/

#ifndef LDRAWINP_INCLUDED
#define LDRAWINP_INCLUDED

struct LDrawIniPrivateDataS
{
   /* The LDrawSearch directories as read */
   int            nSymbolicSearchDirs;
   char         **SymbolicSearchDirs;
   LDrawFileCaseCallback FileCaseCallback;
};

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 1 if OK, 0 if Section/Key not found or error */
int LDrawIniReadIniFile(const char *IniFile,
                        const char *Section, const char *Key,
                        char *Str, int sizeofStr);

/*
Parse SymbolicSearchDir into Result
Returns 1 if OK, 0 on error.
*/
int LDrawIniParseSymbolicSearchDir(struct LDrawSearchDirS *Result,
                                   const char *SymbolicSearchDir, 
                                   const char *LDrawDir,
                                   const char *ModelDir,
                                   const char *HomeDir);

#ifdef __cplusplus
}
#endif

#endif
