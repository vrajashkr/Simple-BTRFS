#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
struct node;
struct node_tree;
struct file;
struct directory;

struct file{
    int file_id;
    char* file_name;
    unsigned char* file_contents;
    long long int file_size;
    
};
typedef struct file XFILE;

struct directory{
    int dir_id;
    char* dir_name;
    struct node_tree* dir_tree;
    long long int dir_file_count;
};typedef struct directory DIR;

struct node{
    int discriminator;
    struct file* stored_file;
    struct directory* stored_dir;
} ; typedef struct node NODE;

 struct node_tree{
    int tree_length;
    struct node** file_tree_base; // array implementation of BST for now
};typedef struct node_tree NODE_TREE;


typedef struct fs_index_tree{
    DIR* root;
} FS_INDEX_TREE;

static int do_getattr(const char*path,struct stat *st){
        // path is file path
        //stat is the structure to be filled with the attributes
        // returns 0 on success and -1 otherwise
if(strcmp(path, "/") == 0) { 
        st->st_mode = S_IFDIR | 0755; 
        st->st_nlink = 2; return 0;
    } 
    else if(strcmp(path, "/x/") == 0) { 
        st->st_mode = S_IFREG | 0444; 
        st->st_nlink = 1; 
        st->st_size = strlen("hello"); return 0;
    } 
        printf("%s\n",path);
       return -ENOENT;
}
typedef int (*fuse_fill_dir_t) (void *buf, const char *name, const struct stat *stbuf, off_t off);

static int do_readdir(const char * path, void *buffer, fuse_fill_dir_t filler,off_t offset,struct fuse_file_info* fi){
        //path is path
        //buffer is to be filled with names of files and subdirs
        //returns 0 on success
        filler(buffer,".",NULL,0);
        filler(buffer,"..",NULL,0);
        if (strcmp(path,"/") == 0){
                filler(buffer,"file54",NULL,0);
                filler(buffer,"file349",NULL,0);
        }
        return 0;

}

static int do_read(const char *path, char*buffer, size_t size,off_t offset, struct fuse_file_info * fi){
        //path is path
        //buffer is the chunk
        //size is the size
        //offset is the offset to start reading the file
        //returns number of bytes read successfully
        char file54Text[] = "FIle 54";
        char file349Text[] = "FIle 349";
        char* selectedText = NULL;
        return 0;
        if (strcmp(path, "/file54") == 0){
                selectedText = file54Text;
        }else if(strcmp(path,"/file349") == 0){
                selectedText = file349Text;
        }else 
                return -1;

        memcpy(buffer, selectedText+offset,size);
        return strlen(selectedText) - offset;
}
static int do_mkdir(const char* path,mode_t mode){//make a directory
    //write code to scan the path properly
    //strtoken code here
        printf("%s\b",path);return 0;
    DIR * newdir = (DIR*)malloc(sizeof(DIR));
    newdir->dir_id = rand();
    newdir->dir_file_count = 0;
    newdir->dir_name = ""; //last part of path
    newdir->dir_tree = (NODE_TREE*)malloc(sizeof(NODE_TREE));
    newdir->dir_tree->tree_length = 0;
    newdir->dir_tree->file_tree_base = NULL;
}

static int do_rmdir(const char* path){
        return 0;
}

static struct fuse_operations operations = {
        .getattr = do_getattr,
        .readdir = do_readdir,
        .read = do_read,
        .mkdir = do_mkdir,
};

static int do_write(const char *path, char*buffer, size_t size,off_t offset, struct fuse_file_info * fi){
        return 0;
}

// static int do_opendir(const char* path,struct fuse_file_info* fi){

// }
int main(int argc, char* argv[]){
        printf("[START] BTRFUSE initialising....\n");
        printf("[WARNING] BTRFUSE is a Non-Persistent System....\n");
        printf("[INFO] BTRFUSE mounting....\n");
        printf("[INFO] BTRFUSE mounted and ready for operation....\n");
        return fuse_main(argc,argv,&operations,NULL);
}
