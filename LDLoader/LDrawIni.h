/* LDrawIni.h */
/*
040228 lch First version of LDrawIni handling routines
040307 lch Added LgeoDir
040319 lch Added LDrawInP.h (Really LDrawIniP.h but 8+3 name...)
*/

#ifndef LDRAWINI_INCLUDED
#define LDRAWINI_INCLUDED

struct LDrawSearchDirS
{
   int            Flags;        /* Parsed and known flags LDSDF_XXX          */
   char          *UnknownFlags; /* Any unknown flags <XXX>                   */
   char          *Dir;          /* The full path of a search dir             */
};
struct LDrawIniPrivateDataS;

struct LDrawIniS
{
   /* The LDRAWDIR containing the P, PARTS and MODELS directories */
   char          *LDrawDir;

   /* The LDrawSearch directories ready to use */
   int            nSearchDirs;
   struct LDrawSearchDirS *SearchDirs;

   /* The LGEODIR (named LGEO) containing the LGEO .inc files */
   char          *LgeoDir;

   /* Private date for the LDrawIni routines */
   struct LDrawIniPrivateDataS *PrivateData;
};

/* LDrawSearchDir Flags */
#define LDSDF_HIDE     0x0001   /* Hide directory                            */
#define LDSDF_SKIP     0x0002   /* Skip directory                            */
#define LDSDF_DEFPART  0x0004   /* Default filetype: Part                    */
#define LDSDF_DEFPRIM  0x0008   /* Default filetype: Primitive               */
#define LDSDF_MODELDIR 0x0010   /* <MODELDIR>                                */

#ifdef __cplusplus
extern "C" {
#endif

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
struct LDrawIniS *LDrawIniGet(const char *LDrawDir, int *ErrorCode);
/* 
Compute Real Dirs by substituting <LDRAWDIR> and <MODELDIR> in
the Symbolic Dirs read from the env vars or ini files.
If OnlyValidDirs is true then non-existing search dirs is skipped
If AddTrailingSlash is true then the search dirs will have a slash/backslash appended.
If ModelDir is NULL then search dir <MODELDIR> is skipped.
Returns 1 if OK, 0 on error
*/
int LDrawIniComputeRealDirs(struct LDrawIniS * LDrawIni, 
                            int OnlyValidDirs,
                            int AddTrailingSlash,
                            const char *ModelDir);
/* 
Reset search dirs to default if LDrawSearch is NULL
or to the dirs specified in LDrawSearch delimited by |.
Returns 1 if OK, 0 on error
*/
int LDrawIniResetSearchDirs(struct LDrawIniS * LDrawIni, 
                            const char *LDrawSearch);
/*
Free the LDrawIni data
*/
   void LDrawIniFree(struct LDrawIniS * LDrawIni);

#ifdef __cplusplus
}
#endif

/* Error codes returned by LDrawIniGet */
#define LDRAWINI_ERROR_OUT_OF_MEMORY     1
#define LDRAWINI_ERROR_LDRAWDIR_NOT_SET  2

#endif
