#ifndef _FILE_H
#define _FILE_H

#include <gint/bfile.h>

// typedef int FILE;

void ctfc(char *src, uint16_t *dst) {
	int i;
	for (i=0;src[i]!=0;i++) dst[i] = src[i];
	dst[i] = 0;
}

void mkpth(uint16_t *dst,char *root,char *fold,char *fn) {
	char tp[2+strlen(root)+1+strlen(fold)+1+strlen(fn)+1]; // probably off by 1 or 2 bytes
	if(strlen(fold)==0) sprintf(tp,"\\\\%s\\%s",root,fn); //File path without folder
	else if(strlen(fn)==0) sprintf(tp,"\\\\%s\\%s",root,fn); //File path without file
	else sprintf(tp,"\\\\%s\\%s\\%s",root,fold,fn); //File path with file & folder
	ctfc((char*)&tp,dst);
}

int open(const char* filepath, const char* mode) {
    uint16_t path[strlen(filepath)+1];
	mkpth((uint16_t*)&path,"fls0","",(char*)filepath);

    if (strlen(mode) < 3 && strlen(mode) > 0) {
        int bfile_mode = 0;
        switch (mode[0]) {
            case 'r':
                if (mode[1] == 'w' || mode[1] == '+') {
                    bfile_mode = BFile_ReadWrite;
                } else {
                    bfile_mode = BFile_ReadOnly;
                }
                break;
            case 'w':
                bfile_mode = BFile_WriteOnly;
                break;
        }

        int file = BFile_Open(path, bfile_mode);
        return file;
    } else {
        // print("Wrong mode");
        exit(1);
    }
}

#endif
