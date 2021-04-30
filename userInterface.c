#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "userInterface.h"
#include "packTypes.h"


int bytes = 0;
int files = 0;
int copies = 0;
char rootDir[512];

char *srcdir[256], *dstdir[256];

pack dirCopy(char* source, char* dest, int depth, int verb, int lf){

        struct dirent *de;
        char spath[256], dpath[256], fdpath[256], fspath[256], linkbuf[256], linkpath[512];
        DIR *dr = opendir(source);
        int s_fd, d_fd;
        size_t n;
        char buffer[256];
        pack pkg;
        struct stat ss;

        if (dr == NULL){
                perror("openDir");
                printf("Could not open current directory with name %s %d\n", source, errno);
                exit(-1);
        }

        if(depth == 0)
            strcpy(rootDir, source);

        while ((de = readdir(dr)) != NULL){
                if(strcmp(de->d_name, "..")==0 || strcmp(de->d_name, ".")==0)
                    continue;
                if(de->d_type == DT_DIR){  //DT_LNK, D_REG

                    //relative path of source, and dest sub-folders
                    strcpy(spath, makePath(source, de->d_name));
                    strcpy(dpath, makePath(dest, de->d_name));

                    //print folder
                    if(verb)
                        printf("%s\n", spath);

                    //create sub-folder if it doesnt exist
                    makeDir(dpath, -1);

                    //copy sub-files
                    dirCopy(spath, dpath, depth+1, verb, lf);
                    copies++;
                    files++;
                }
                else if(de->d_type == DT_REG){
                    
                    //open (and create) source file
                    strcpy(fspath, makePath(source, de->d_name));
                    stat(fspath, &ss);

                    if((s_fd = open(fspath, O_RDONLY)) == -1){  
                        perror("open");
                        exit(3);
                    }
                    
                    files++;
                    //open (and create) destination file
                    strcpy(fdpath, makePath(dest, de->d_name));
                    if(fileExists(fdpath)){
                        if(filesIdentical(fspath, fdpath))
                            continue;
                        else{
                            copyFile(s_fd, d_fd);
                            copies++;
                            chmod(fdpath, ss.st_mode);
                            if(verb)
                                printf("%s\n", fdpath);
                        }
                    }
                    else{
                        if((d_fd = open(fdpath, O_WRONLY | O_CREAT, ss.st_mode)) == -1){
                            perror("open");
                            exit(2);
                        }
                        copyFile(s_fd, d_fd);
                        copies++;
                        if(verb)    
                            printf("%s\n", fdpath);
                    }
                }
                else if(de->d_type == DT_LNK){
                    if(lf){
                        strcpy(fspath, makePath(source, de->d_name));
                        strcpy(fdpath, makePath(dest, de->d_name));
                        char *path = realpath(fspath, linkbuf);
                        if(path)
                            symlink(linkbuf, fdpath);
                    }
                    else
                            continue;
                    
                }
        }

        closedir(dr);

        pkg.bytes = bytes;
        pkg.copies = copies;
        pkg.files = files;
        

        return pkg;
}

char* makePath(char *path, char *file){

        char *newPath;
        
        newPath = (char*)malloc(strlen(path) + strlen(file) + 2);

        strcpy(newPath, path);
        strcat(newPath, "/");
        strcat(newPath, file);

        return newPath;
}

int makeDir(char *dir, int depth){
    
    DIR *dr;
    
    if((dr = opendir(dir)) == NULL){
        if(errno == ENOENT){
            if(mkdir(dir, 0700) == -1){
                perror("mkdir");
                exit(2);
            }
            if(depth == 0)
                printf("created directory %s\n", dir);
        }
    }

    return 0;
}


int copyFile(int source, int dest){

    char buffer[256];
    size_t n;

    while((n = read(source, buffer, 256)) > 0){
        write(dest, buffer, n);
        bytes += n;
    }

    close(source);
    close(dest);

    return -1;    
}

int filesIdentical(char *source, char *dest){

    struct stat src, dst;

    char buffer1[256], buffer2[256];

    int dfd, sfd, val;
    size_t n;

    stat(source, &src);
    stat(dest, &dst);
    
    if(dst.st_size == src.st_size){  //if file is same size
        if((dst.st_mtim.tv_sec == src.st_mtim.tv_sec) && (dst.st_mtim.tv_nsec == src.st_mtim.tv_nsec))       //and lastly mofidied
            return 1; //identical;
        
        else
            val = checkSimilarity(source, dest);

        close(sfd);close(dfd);
        return val;
    }
    
    
    return 0;
}

int fileExists(char *file){
    return (access(file, F_OK) == 0);
}

int checkSimilarity(char *source, char* dest){

    int fd1, fd2;
    size_t n;
    char buffer1[256], buffer2[256];

    fd1 = open(source, O_RDONLY);
    fd2 = open(dest, O_RDONLY);

    while((n = read(fd1, buffer1, 256)) > 0){
        n = read(fd2, buffer2, 256);
        if(n < 0){
            perror("read");
            exit(4);
        }

        if(strncmp(buffer1, buffer2, 256) != 0)
            return 0;
    }
    
    return 1;
}


int dirDelete(char* source, char* dest){

        struct dirent *de;
        char spath[256], dpath[256], fdpath[256], fspath[256];
        DIR *dr;
        DIR *dir;
        int s_fd, d_fd;
        size_t n;
        char buffer[256];

        if ((dr = opendir(dest)) == NULL){
                perror("openDir");
                printf("Could not open current directory with name %s %d\n", source, errno);
                exit(-1);
        }

        
    
        while ((de = readdir(dr)) != NULL){
                if(strcmp(de->d_name, "..")==0 || strcmp(de->d_name, ".")==0)
                    continue;
                if(de->d_type == DT_DIR){  //DT_LNK, D_REG

                    //relative path of source, and dest sub-folders
                    strcpy(spath, makePath(source, de->d_name));
                    strcpy(dpath, makePath(dest, de->d_name));
                    dirDelete(spath, dpath);
                }
                else if(de->d_type == DT_REG){
                    
                    //open dest file
                    strcpy(fdpath, makePath(dest, de->d_name));
                    
                    //open sourcefile
                    strcpy(fspath, makePath(source, de->d_name));

                    if(!fileExists(fspath)){
                        unlink(fdpath);
                    }
                }
        }

        closedir(dr);
        return 0;
}

arg_pack* argHandler(int argc, char** argv){

    arg_pack *apk = malloc(sizeof(arg_pack));


    if(argc < 3){
        printf("Wrong execution parameters.\nUsage ./quic [srcFolder] [destFolder]\n");
        printf("[-v to print statistics]\n");
        printf("[-d to delete existing files from destination folder who are not on source]\n");
        printf("[-l to handle soft/hard links]\n");
        exit(88);
    }
    else if(argc >= 3){
        strcpy(apk->source, argv[1]);
        strcpy(apk->dest, argv[2]);

        apk->l = 0; apk->d = 0; apk->v = 0;
        for(int i = 3; i<argc; i++){
            if(strcmp(argv[i], "-v") == 0)
                apk->v = 1;
            if(strcmp(argv[i], "-d") == 0)
                apk->d = 1;
            if(strcmp(argv[i], "-l") == 0)
                apk->l = 1;
        }
    }

    return apk;
}

char* fixPath(char *path1, char *path2){

    int i, count=0, j=0;
    char copy[100];


    while(path2[i]){
        if(path2[i] != '.' && path2[i] != '/')
            break;
        i++;
    }
    while(path2[i]){
        copy[j++] = path2[i];
        
        if(j == 99){
            copy[j] = 0;
            break;
        }

        i++;
    }

    copy[j] = 0;
    printf("copy: %s\n", copy);
}

int searchInode(ino_t inode, char* path, char* root){
        
        struct dirent *de;
        char spath[256], dpath[256], fdpath[256], fspath[256];
        DIR *dr;
        DIR *dir;
        int s_fd, d_fd;
        size_t n;
        char buffer[256];
        struct  stat ss;
        

        if ((dr = opendir(root)) == NULL){
                perror("openDir");
                printf("Could not open current directory with name %s %d\n", root, errno);
                exit(-1);
        }
    
        while ((de = readdir(dr)) != NULL){
                if(strcmp(de->d_name, "..")==0 || strcmp(de->d_name, ".")==0)
                    continue;
                if(de->d_type == DT_DIR){  //DT_LNK, D_REG

                    //relative path of source, and dest sub-folders
                    strcpy(dpath, makePath(root, de->d_name));
                    return searchInode(inode, path, dpath);
                }
                else if(de->d_type == DT_REG){
                    
                    strcpy(fdpath, makePath(root, de->d_name));
                    stat(fdpath, &ss);
                    if(ss.st_ino == inode)
                        if(strcmp(path, fdpath)){
                            return 1;
                        }
                }
        }

        closedir(dr);
        return 0;
}