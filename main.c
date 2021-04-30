#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "userInterface.h"
#include "packTypes.h"

int main(int argc, char *argv[]){

    int bytes;
    double t1, t2, ticspersec;
    struct tms tb1, tb2;
    double sec;
    pack pkg;
    arg_pack *apkg;

    //handle command line arguments
    apkg = argHandler(argc, argv);

    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);
    
    //copy from source dir to destination
    makeDir(argv[2], 0);
    pkg = dirCopy(apkg->source, apkg->dest, 0, apkg->v, apkg->l);

    //delete files from destination tha are not
    //..in the source dir
    //if specified by user
    if(apkg->d)
        dirDelete(apkg->source, apkg->dest);

    t2 = (double) times(&tb2);
    sec = (t2-t1)/ticspersec;

    
    printf("\nthere are %d files/directories in the hierarchy\n", pkg.files);
    printf("number of entities copied is %d\n", pkg.copies);
    printf("copied %d bytes in %lfsec at %lf bytes/sec\n", pkg.bytes, sec, bytes/sec);
    

    return 0;
}