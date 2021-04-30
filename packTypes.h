#ifndef PT_H
#define PT_H

typedef struct{
    int files;
    int copies;
    int bytes;
}pack;

typedef struct{
    int v;
    int d;
    int l;
    char source[256];
    char dest[256];
}arg_pack;

#endif