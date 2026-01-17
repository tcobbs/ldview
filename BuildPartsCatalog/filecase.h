#ifndef __FILECASE_H__
#define __FILECASE_H__
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <map>
#include <TCFoundation/mystring.h>

extern bool fileCaseCallback(char *filename);
#endif
