/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING CODE
WRITTEN BY OTHER STUDENTS.
Tiantian Li */
#include <sys/stat.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include "inodemap.h"
#include <sys/time.h>
#include <utime.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef struct
{   int op;
    int given_f;
    char* dirName;
    char* archName;
} options;

options processing (int argc, char *argv[]);
void createFile(char *dirName, char *archName);
void create_sub_dir(char *dirName, FILE *archFile);
void extractFile(char *archName);
void printFile(char *archName);


options processing (int argc, char *argv[]){
    int o;
    int numMode=0;
    options opt;
    opt.given_f=0;
    opt.op=0;
    while ((o = getopt(argc, argv, "cxtf:")) != -1) {     
        switch(o){
            case 'c':
                if (numMode>0){
                    fprintf(stderr, "Error: Multiple modes specified.\n");
                    exit(-1);
                }
                numMode+=1;
                opt.op=1;
                break;
            case 'x':
                if (numMode>0){
                    fprintf(stderr, "Error: Multiple modes specified.\n");
                    exit(-1);
                }
                numMode+=1;
                opt.op=2;
                break;
            case 't':
                if (numMode>0){
                    fprintf(stderr, "Error:Multiple modes specified.\n");
                    exit(-1);
                }
                numMode+=1;
                opt.op=3;
                break;
            case 'f':
                opt.given_f=1;
                numMode+=1;
                
                opt.archName=optarg;
                break;
        }
    }
    //check if mode is given
    if (numMode == 0){                                  
        fprintf(stderr, "Error: No mode specified.\n");
        exit(-1);
    }
    //check if mode -c and no target directory
    if(opt.op==1 && argc == optind){
        fprintf(stderr,"Error: No directory target specified.\n");
        exit(-1);
    }
    else if (opt.op==1){
        opt.dirName=argv[optind];
    }
    //check if -f is specified
    if(opt.given_f==0){
        fprintf(stderr, "Error: No tarfile specified.\n");
        exit(-1);
    }
    //check validity of given directory
    struct stat s;
    int err = stat(opt.dirName, &s);
    if(opt.op==1){
    if(err==-1) {
        fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opt.dirName);
        exit(-1);
    }   
    else {
        if(!S_ISDIR(s.st_mode)){
            fprintf(stderr, "Error: Specified target (\"%s\") is not a directory.\n", opt.dirName);
            exit(-1);
        }
    }}
    return opt;
}
void createFile(char *dirName, char *archName){
    //create a new file for writing and reading
    FILE *archFile=fopen(archName, "w+");
    if(archFile==NULL){
        perror("fopen");
        exit(-1);
    }
    //append magic number
    int size =0;
    u_int32_t magic=0x7261746D;
    size=fwrite(&magic, 4, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }

    struct stat s;
    if (stat(dirName, &s) != 0) {
            perror("lstat");
            exit(-1);
        }
    //write inode
    u_int64_t inode=s.st_ino;
    set_inode(inode, dirName);
    size=fwrite(&inode, 8, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }
    //write nameLength
    u_int32_t nameLength=strlen(dirName);
    size=fwrite(&nameLength, 4, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }
    //write name
    size=fwrite(dirName, nameLength, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }
    
    //write mode
    size=fwrite(&s.st_mode, 4, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }
    //write modification time
    u_int64_t intTime;
    intTime=(u_int64_t)s.st_mtime;
    size=fwrite(&intTime, 8, 1, archFile);
    if(size!=1){
        perror("fwrite");
        exit(-1);
    }
    
    create_sub_dir(dirName, archFile);
    
    if (fclose(archFile) != 0){                                
        perror("fclose");
        exit(-1);
    }
}
void create_sub_dir(char *dirName, FILE *archFile){
    char *fullname;
    struct dirent *de;
    struct stat s;
    DIR *curDir=opendir(dirName);

    if(curDir==NULL){
        perror("opendir");
        exit(-1);
    }

    fullname = (char *) malloc(sizeof(char)*(strlen(dirName)+258));
   
    for (de = readdir(curDir); de != NULL; de = readdir(curDir)) {
        
        if (sprintf(fullname, "%s/%s", dirName, de->d_name)<0){
            perror("sprintf");
            exit(-1);
        }
        
        if (lstat(fullname, &s) != 0){                                           
                perror("lstat");
                exit(-1);
        }
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && !S_ISLNK(s.st_mode) && strcmp(de->d_name, ".DS_Store") != 0){
            //write inode number
            u_int64_t inode=s.st_ino;
            
            int size;
            size=fwrite(&inode, 8, 1, archFile);
            if(size!=1){
                perror("fwrite");
                exit(-1);
            }
            //write nameLength
            u_int32_t nameLength=strlen(fullname);
            size=fwrite(&nameLength, 4, 1, archFile);
            if(size!=1){
                perror("fwrite");
                exit(-1);
            }
            //write name
            size=fwrite(fullname, nameLength, 1, archFile);
            
            if(size!=1){
                perror("fwrite");
                exit(-1);
            }
            //if not hard link
            if(get_inode(inode)==NULL){
                set_inode(inode, fullname);
                //write mode
                uint32_t mode=s.st_mode;
                size=fwrite(&mode, 4, 1, archFile);
                if(size!=1){
                    perror("fwrite");
                    exit(-1);
                }
                //write modification time
                u_int64_t intTime;
                intTime=(u_int64_t)s.st_mtime;
                size=fwrite(&intTime, 8, 1, archFile);
                if(size!=1){
                    perror("fwrite");
                    exit(-1);
                }
                //if not hard link or directory
                if(!S_ISDIR(mode)){
                    //write file size
                    uint64_t file_size=s.st_size;
                    size=fwrite(&file_size, 8, 1, archFile);
                    if(size!=1){
                        perror("fwrite");
                        exit(-1);
                    }
                    
                    if(file_size!=0){
                        FILE *fp;
                        char *buffer=malloc(file_size*sizeof(char));
                        fp=fopen(fullname, "r");
                        if (fp==NULL){
                            perror("fopen");
                            exit(-1);
                        }
                        if(fread(buffer, file_size, 1, fp)!=1){
                            perror("fread");
                            exit(-1);
                        }
                        if(fwrite(buffer, file_size, 1, archFile)!=1){
                            perror("fwrite");
                            exit(-1);
                        }
                        if(fclose(fp)!=0){
                            perror("fclose");
                            exit(-1);
                        }
                    }
                }
            }
        }

        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && S_ISDIR(s.st_mode)){
            create_sub_dir(fullname, archFile);
            
        }
    }
    
}
void extractFile(char *archName){

FILE *archFile;
archFile=fopen(archName, "r");
if (archFile==NULL){
    perror("fopen");
    exit(-1);
}

uint32_t magic;
if(fread(&magic, 4, 1, archFile)!=1){
    perror("fread");
    exit(-1);
}
//check magic number
if(magic!=0x7261746D){
    fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic, 0x7261746D);
    exit(-1);
}
uint64_t inode;
while(fread(&inode, 8, 1, archFile)==1){

//read name length
uint32_t nameLength;
if(fread(&nameLength, 4, 1, archFile)!=1){
    perror("fread");
    exit(-1);
}
//read relative path
char *fullname =  malloc(sizeof(char) * (nameLength)+1); 
if(fread(fullname, nameLength, 1, archFile)!=1){
    perror("fread");
    exit(-1);
}
fullname[nameLength]='\0';
//if is hard link
if(get_inode(inode)!=NULL){
    char *source=malloc(sizeof(char)*(strlen(get_inode(inode))+1));
    strcpy(source, get_inode(inode));
    if(link(source, fullname)!=0){
        perror("link");
        exit(-1);
    }
    //free(source);
}
else{
    set_inode(inode, fullname);
    //read file mode
    uint32_t mode;
    if(fread(&mode, 4, 1, archFile)!=1){
        perror("fread");
        exit(-1);
    }
    //read modification time
    uint64_t mtime;
    if(fread(&mtime, 8, 1, archFile)!=1){
        perror("fread");
        exit(-1);
    }
    //if is a directory
    if(S_ISDIR((mode_t) mode)){
        if(mkdir(fullname, (mode_t) mode)==-1){
            perror("mkedir");
            exit(-1);
        }
    }
    else{
        //is a regular file
        //read file size
        uint64_t size;
        if(fread(&size, 8, 1, archFile)!=1){
        perror("fread");
        exit(-1);
        }
        
        FILE *newFile = fopen(fullname, "w+"); 
        if(newFile==NULL){
            perror("fopen");
            exit(-1);
        }
        if(size!=0){
            FILE *fp;
            char *buffer=malloc(size*sizeof(char));
            fp=fopen(fullname, "r");
            if (fp==NULL){
                perror("fopen");
                exit(-1);
            }
            if(fread(buffer, size, 1, archFile)!=1){
                perror("fread");
                exit(-1);
            }
            if(fwrite(buffer, size, 1, newFile)!=1){
                perror("fwrite");
                exit(-1);
            }
            if(fclose(fp)!=0){
                perror("fclose");
                exit(-1);
            }
        }
        if(chmod(fullname, (mode_t) mode)!=0){
            perror("chmod");
            exit(-1);
        }
        if (fclose(newFile) != 0){                                
        perror("fclose");
        exit(-1);
        }
        struct timeval createTime;
        struct timeval modTime;
        if(gettimeofday(&createTime, NULL)!=0){
            perror("gettimeofday");
            exit(-1);
        }
        modTime.tv_sec=mtime;
        modTime.tv_usec=0;
        struct timeval timestruct[2]={createTime, modTime};
        if(utimes(fullname, timestruct)!=0){
            perror("utimes");
            exit(-1);
        }
        
    }
    }
}
}
void printFile(char *archName){
    FILE *archFile;
    archFile=fopen(archName, "r");
    if (archFile==NULL){
        perror("fopen");
        exit(-1);
    }
    uint32_t magic;
    if(fread(&magic, 4, 1, archFile)!=1){
        perror("fread");
        exit(-1);
    }
    //check magic number
    if(magic!=0x7261746D){
        fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic, 0x7261746D);
        exit(-1);
    }
    uint64_t inode;
    while(fread(&inode, 8, 1, archFile)==1){
        inode=(ino_t) inode;
        //read name length
        uint32_t nameLength;
        if(fread(&nameLength, 4, 1, archFile)!=1){
            perror("fread");
            exit(-1);
        }
    
        //read relative path
        char *fullname =  malloc(sizeof(char) * (nameLength)); 
        if(fread(fullname, nameLength, 1, archFile)!=1){
            perror("fread");
            exit(-1);
        }
        
        fullname[nameLength]='\0';
        //if is hard link
        if(get_inode(inode)!=NULL){
            printf("%s -- inode: %lu\n", fullname, inode);
        }
        else{
            set_inode(inode, fullname);
            //read file mode
            uint32_t mode;
            if(fread(&mode, 4, 1, archFile)!=1){
                perror("fread");
                exit(-1);
            }
            //read modification time
            uint64_t mtime;
            if(fread(&mtime, 8, 1, archFile)!=1){
                perror("fread");
                exit(-1);
            }
            mtime=(time_t) mtime;
            //if is a directory
            if(S_ISDIR((mode_t) mode)){
                printf("%s/ -- inode: %lu, mode: %o, mtime: %llu\n", fullname, inode, mode, (unsigned long long) mtime);
            }
            else{
                //is a regular file
                //read file size
                uint64_t size;
                if(fread(&size, 8, 1, archFile)!=1){
                perror("fread");
                exit(-1);
                }
                if(size!=0){
                    char *content = malloc(size*sizeof(char));
                    if (fread(content, size, 1, archFile)!=1){
                        perror("fread");
                        exit(-1);
                    }
                }
                if (mode & S_IXUSR){
                    printf("%s* -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", fullname, inode, mode, (unsigned long long) mtime, size);
                }
                else{
                    printf("%s -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", fullname, inode, mode, (unsigned long long) mtime, size);
                }
            }
    }
    }
}

int main(int argc, char *argv[]) {
    options opt=processing(argc, argv);
    if (opt.op==1){
        createFile(opt.dirName, opt.archName);
    }
    if(opt.op==2){
        extractFile(opt.archName);
    }
    if(opt.op==3){
        printFile(opt.archName);
    }
}