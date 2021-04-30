#include "packTypes.h"
pack dirCopy(char*, char*, int, int, int);
int searchInode(ino_t, char*, char*);
char * makePath(char*, char*);
int makeDir(char*, int);
int copyFile(int, int);
int filesIdentical(char *, char *);
int fileExists(char *);
int checkSimilarity(char*, char*);
int dirDelete(char*, char*);
arg_pack* argHandler(int, char**);
char* fixPath(char*, char*);
