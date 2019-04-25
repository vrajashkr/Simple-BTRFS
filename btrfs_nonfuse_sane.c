#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Use a hash table to store all inodes and files indexed by inode

struct bnode;
struct file;
struct directory;
struct inner_node;

struct file{
    int file_id;
    char file_name[100];
    unsigned char* file_contents;
    long long int file_size;
};
typedef struct file XFILE;

struct directory{
    int dir_id;
    char dir_name[100];
    struct bnode* dir_tree;
    long long int dir_file_count;
};typedef struct directory DIR;

struct inner_node{ // this is for the hash table
    int object_type; // 0 is file 1 is dir
    char name[100];
    struct file* myfile;
    struct directory* mydir;
}; typedef struct inner_node INNERNODE;

struct bnode{
        struct bnode * parent;
        int controller; // 0 is leaf 1 is internal
        struct bnode** children;
        char ** names; // actual index
        long int* ids;
        int length;
}; typedef struct bnode BNODE;

struct sq{
        struct directory* stackqueue[100];
        int top; // top of stack and tail
        int head; //head of the queue
};typedef struct sq STACKQUEUE;

void push_stack(STACKQUEUE* s,DIR* item){
        if (s->head == -1){
                //empty
                s->head = s->top = 0;
                s->stackqueue[s->top] = item;
        }else{
                s->stackqueue[++(s->top)] = item;
        }
}
int pop_stack(STACKQUEUE* s){
        if (s->head == -1){
                //empty
                //nothing
                return 1;
        }else{
                s->top--;
                if (s->top == -1) s->head = -1;
                return 0;
        }
}

DIR* stack_top(STACKQUEUE* s){
        if (s->top != -1){
              return s->stackqueue[s->top];  
        }
        return NULL; 
}

//INODE* get_leaf(BNODE* active_root,char* target,BNODE* current);
int insert_internal(STACKQUEUE*s,BNODE* current_root,char* target,long id,BNODE* current_child,BNODE* new_child);
void insert_leaf(STACKQUEUE *s,BNODE* cr,char* target,long id);
void fx_ls(BNODE* current_dir);
int fx_touch(STACKQUEUE*s ,DIR* current_root,char* name,char* content);
int fx_mkdir(STACKQUEUE*s, DIR* current_root,char* name);
void fx_pwd(STACKQUEUE* s);
// void fx_cd(STACKQUEUE* s,BNODE* active_bnode,DIR* active_dir,char * target_dir);
// void fx_cat(BNODE* active_root,char* target);
INNERNODE** MASSIVE_HASH;
int main(){
        printf("[START] BTRFS initialising....\n");
        MASSIVE_HASH = (INNERNODE**)malloc(sizeof(INNERNODE*)*100000);

        printf("[WARNING] BTRFS is a Non-Persistent System....\n");
        printf("[INFO] BTRFS mounting....\n");
        printf("[INFO] BTRFS mounted and ready for operation....\n");
        DIR* root = (DIR*)malloc(sizeof(DIR));
        strcpy(root->dir_name,"/");
        root->dir_id = 0;
        
        root->dir_file_count = 0;
        root->dir_tree = (BNODE*)malloc(sizeof(BNODE));
        root->dir_tree->children = (BNODE**)malloc(sizeof(BNODE*)*4);
        root->dir_tree->ids = (long int*)malloc(sizeof(long int)*3);
        root->dir_tree->names = (char**)malloc(sizeof(char)*3);
        root->dir_tree->length = -1;
        root->dir_tree->controller = 0;
        root->dir_tree->parent = NULL;
       
        STACKQUEUE dirstack;dirstack.head = -1;dirstack.top = -1;
        push_stack(&dirstack,root);

        char buffer[1024];
        char* words[10];
        char* supported_comms[] = {"ls","cd","mkdir","rmdir","cat","pwd","touch","rm","exit"};
        int i,p;
        int rogue = 1;

        while(rogue){
                printf("\n<X>>> ");
                scanf(" %[^\n]s",buffer);
                i = 0;
                words[i] = strtok(buffer," ");
                while ((words[++i] = strtok(NULL," ")) != NULL);
                for ( p = 0;p<9;p++){
                        if (strcmp(supported_comms[p],words[0]) == 0){
                                break;
                        }
                }
                switch (p)
                {
                        case 0:{fx_ls(stack_top(&dirstack)->dir_tree);}break;
                        //case 1:{fx_cd(&dirstack,active_dir->dir_tree,active_dir,words[1]);}break;
                        case 2:{fx_mkdir(&dirstack,stack_top(&dirstack),words[1]);}break;
                        case 3:{}break;
                        //case 4:{fx_cat(active_dir->dir_tree,words[1]);}break;
                        case 5:{fx_pwd(&dirstack);}break;
                        case 6:{fx_touch(&dirstack,stack_top(&dirstack),words[1],words[2]);}break;
                        case 7:{}break;
                        case 8:{rogue = 0;}break;
                        default:{printf("[ERROR] Is it that hard to follow instructions? That command doesn't exist\n");}
                                break;
                }
        }
        printf("TERMINATED\n");
}

void fx_ls(BNODE* current_dir){
        //traversal of the file_tree starting at current_dir
        if (current_dir == NULL)return;
        int flag = 0;
        
        for (int j = 0; j < current_dir->length+1; j++){
                if (current_dir->children[j]!=NULL){
                        fx_ls(current_dir->children[j]);
                        flag = 1;
                }
        }
        if (flag == 0){
                for (int i = 0; i < current_dir->length+1;i++){ 
                                //printf("i %d %d\n",i,current_dir->ids[i]);fflush(stdout);
                                INNERNODE*n = MASSIVE_HASH[current_dir->ids[i]];
                                int discriminator = n->object_type;
                                
                                if (discriminator == 0){
                                        XFILE* f = n->myfile;
                                        printf("%s %d %s %lld\n","FILE",f->file_id,f->file_name,f->file_size);
                                }else{
                                        DIR* d = n->mydir;
                                        printf("%s %d %s\n","DIR",d->dir_id,d->dir_name);
                                }
                        }
        }
}
int insert_internal(STACKQUEUE*s,BNODE* current_root,char* target,long id,BNODE* current_child,BNODE* new_child){
        if (current_root != NULL){ //non dead
                if (current_root->length < 2){
                        //there is some space
                        int i = 0;
                        for (i = 0; i < current_root->length+1; i++){
                                if (strcmp(target,current_root->names[i]) == 1){
                                        //create space and insert
                                        current_root->names[i+1] = (char*)malloc(sizeof(char)*100);
                                        strcpy(current_root->names[i+1],current_root->names[i]);
                                        current_root->ids[i+1] = current_root->ids[i];
                                        current_root->ids[i] = id;
                                        strcpy(current_root->names[i],target);
                                        current_root->length++;
                                        current_root->children[current_root->length] = new_child;
                                        new_child->parent = current_root;
                                        break;
                                }
                         }
                         if (i == current_root->length+1){
                                current_root->names[i] = (char*)malloc(sizeof(char)*100);
                                strcpy(current_root->names[i],target);
                                current_root->length++;
                        }
                }else{
                        //danger: no space in the damned internal
                        //split node into 2
                        //create new bnode
                        BNODE* bnode = (BNODE*)malloc(sizeof(BNODE));
                        bnode->children = (BNODE**)malloc(sizeof(BNODE*)*4);
                        bnode->ids = (long int*)malloc(sizeof(long int)*3);
                        bnode->names = (char**)malloc(sizeof(char)*3);
                        bnode->length = -1;
                        bnode->controller = 1; bnode->parent = NULL;

                       //redistribute
                        bnode->ids[0] = current_root->ids[1];
                        bnode->ids[1] = current_root->ids[2];
                        bnode->length = 1;
                        bnode->names[0] = (char*)malloc(sizeof(char)*100);
                        bnode->names[1] = (char*)malloc(sizeof(char)*100);
                        strcpy(bnode->names[0],current_root->names[1]);
                        strcpy(bnode->names[1],current_root->names[2]);
                        current_root->length -= 2;
                        insert_internal(s,current_root->parent,bnode->names[0],bnode->ids[0],current_root,bnode);
                }
        }else{
                //root insert,new root to be created
                BNODE* bnode = (BNODE*)malloc(sizeof(BNODE));
                bnode->children = (BNODE**)malloc(sizeof(BNODE*)*4);
                bnode->ids = (long int*)malloc(sizeof(long int)*3);
                bnode->names = (char**)malloc(sizeof(char)*3);
                bnode->length = -1;
                bnode->controller = 1; bnode->parent = NULL;
                
                bnode->children[0] = current_child;
                bnode->children[1] = new_child;
                current_child->parent = bnode;
                new_child->parent = bnode;
                bnode->length = 0;
                bnode->ids[0] = id;
                bnode->names[0] = (char*)malloc(sizeof(char)*100);
                strcpy(bnode->names[0],target);

                stack_top(s)->dir_tree = bnode;
        }     
}
void insert_leaf(STACKQUEUE *s,BNODE* cr,char* target,long id){
        BNODE* current_root = cr;
        int i = 0;
        while (current_root->controller != 0){
                i = 0;
                                if (strcmp(target,current_root->names[i]) == 1){
                                        break;
                                }
                }
                current_root = current_root->children[i];
        }
        if (current_root->controller == 0){ //leaf
                if (current_root->length < 2){
                        //there is some space
                        int i = 0;
                        for ( i = 0; i < current_root->length+1; i++){
                                if (strcmp(target,current_root->names[i]) == 1){
                                        //create space and insert
                                        current_root->names[i+1] = (char*)malloc(sizeof(char)*100);
                                        strcpy(current_root->names[i+1],current_root->names[i]);
                                        current_root->ids[i+1] = current_root->ids[i];
                                        current_root->ids[i] = id;
                                        strcpy(current_root->names[i],target);
                                        current_root->length++;break;
                                }
                                if (strcmp(target,current_root->names[i]) == 0){printf("[WARNING] File exists\n");return;}
                        }
                        if (i == current_root->length+1){
                                current_root->names[i] = (char*)malloc(sizeof(char)*100);
                                strcpy(current_root->names[i],target);
                                current_root->ids[i] = id;
                                current_root->length++;
                        }
                }else{
                        //danger: no space in the damned leaf
                        //split node into 2
                        //create new bnode
                        BNODE* bnode = (BNODE*)malloc(sizeof(BNODE));
                        bnode->children = (BNODE**)malloc(sizeof(BNODE*)*4);
                        bnode->ids = (long int*)malloc(sizeof(long int)*3);
                        bnode->names = (char**)malloc(sizeof(char)*3);
                        bnode->length = -1;
                        bnode->controller = 0; 

                       //redistribute
                        bnode->ids[0] = current_root->ids[1];
                        bnode->ids[1] = current_root->ids[2];
                        bnode->length = 1;
                        bnode->names[0] = (char*)malloc(sizeof(char)*100);
                        bnode->names[1] = (char*)malloc(sizeof(char)*100);
                        strcpy(bnode->names[0],current_root->names[1]);
                        strcpy(bnode->names[1],current_root->names[2]);
                        current_root->length -= 2;

                        insert_internal(s,current_root->parent,bnode->names[0],bnode->ids[0],current_root,bnode);
                }
        }        
}

int fx_touch(STACKQUEUE*s,DIR* current_root,char* name,char* content){
        BNODE* current_dir = current_root->dir_tree;
        XFILE* newfile = (XFILE*)malloc(sizeof(XFILE));
        strcpy(newfile->file_name,name);
        long int id = rand() % 10000;
        printf("%ld",id);
        newfile->file_id = id;
        newfile->file_contents = (unsigned char*)malloc(sizeof(unsigned char)*strlen(content));
        strcpy(newfile->file_contents,content);
        newfile->file_size = strlen(content);
        INNERNODE* newi = (INNERNODE*)malloc(sizeof(INNERNODE));
        newi->myfile = newfile;
        newi->object_type = 0;
        MASSIVE_HASH[id] = newi;
        current_root->dir_file_count++;
        insert_leaf(s,current_dir,name,id);
        return 0;
}

int fx_mkdir(STACKQUEUE*s, DIR* current_root,char* name){
        BNODE* current_dir = current_root->dir_tree;
        DIR* newdir = (DIR*)malloc(sizeof(DIR));
        strcpy(newdir->dir_name,name);
        long int id = rand() % 10000;
        printf("%ld",id);
        newdir->dir_id = id;
        newdir->dir_file_count= 0;
        INNERNODE* newi = (INNERNODE*)malloc(sizeof(INNERNODE));
        newi->mydir = newdir;
        newi->object_type = 1;
        MASSIVE_HASH[id] = newi;
        current_root->dir_file_count++;
        insert_leaf(s,current_dir,name,id);
        return 0;
}

void fx_pwd(STACKQUEUE* s){
        int t = s->head;
        printf("[PATH] /");
        DIR* a;
        while (t <= s->top) {
                a = s->stackqueue[t++];
                printf("%s/",a->dir_name);
        }
        printf("\n");
}

// void fx_cd(STACKQUEUE* s,BNODE* active_bnode,DIR* active_dir,char * target_dir){
//         if (strcmp(target_dir,".")==0){
//                 //move to parent
//                 int k = pop_stack(s);
//                 if (k == 1){
//                         printf("[WARN] Can't go higher than root\n");
//                 }else{
//                         active_bnode = stack_top_bnode(s); 
//                         active_dir = stack_top_dir(s);
//                 }
//         }else{
//                 //move downwind,search for the directory first
//                 BNODE* newnode; INODE* newinode;
//                 newinode = get_leaf(active_bnode,target_dir,newnode);

//                 if (newinode!= NULL){
//                         //change dir
//                         push_stack(s,newnode,newinode->stored_dir);
//                         active_bnode = newnode;
//                         active_dir = newinode->stored_dir;
//                 }
//         }
// }
// void fx_cat(BNODE* active_root,char* target){
//         //find target and then print contents
//         BNODE* current_root = active_root;
//         INODE* i = get_leaf(current_root,target,current_root);
//         if (i == NULL) return;
//         if (i->discriminator == 0){
//                 printf("[OUTPUT] File Contents: %s | %lld bytes\n",i->stored_file->file_contents,i->stored_file->file_size);
//         }else{
//                 printf("[OUTPUT] This is a directory\n");
//         }
// }

// INODE* get_leaf(BNODE* active_root,char* target,BNODE*current){
//         BNODE* current_root = active_root;
//         int found = 0;
//                 for (int i = 0; i < current_root->length;i++){
//                         if (strcmp(current_root->ids[i],target) == 0){
//                                 found = 1;
//                                 if (current_root->controller != 1){
//                                         return get_leaf(current_root->children[i],target,current_root);
//                                 }
//                                 if (current_root->controller == 1){
//                                         //return the INODE
//                                         for (int j = 0; j < current_root->length;j++){
//                                                 if (current_root->contents[j]->discriminator == 0){
//                                                         if (strcmp(current_root->contents[j]->stored_file->file_name,target)==0){
//                                                                 return current_root->contents[j];
//                                                         }
//                                                 }
//                                                 if (current_root->contents[j]->discriminator == 1){
//                                                         if (strcmp(current_root->contents[j]->stored_dir->dir_name,target)==0){
//                                                                 return current_root->contents[j];
//                                                         }
//                                                 }
//                                         }
//                                 }
//                         }
//                         if (strcmp(current_root->ids[i],target) == -1){
//                                         return get_leaf(current_root->children[i],target,current_root);
//                         }
//                 }
                
//         if (found == 0){
//                 printf("[OUTPUT] OBJECT Not Found\n"); return NULL;
//         }
// }