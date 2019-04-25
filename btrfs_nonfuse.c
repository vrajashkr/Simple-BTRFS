#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Working with a B+ tree of order 3 for simplicity
// TODO : stackqueue for path cd and fx_pwd
//TODO : insert code for insert
//THIS IS NOT WORKING.. please try again with a little less nonsense

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

struct inner_node{
    int discriminator;
    struct file* stored_file;
    struct directory* stored_dir;
}; typedef struct inner_node INODE;

struct bnode{
        int controller; //internal node 0 or leaf 1
        struct bnode** children;
        struct inner_node** contents;
        char** ids;
        int length;
}; typedef struct bnode BNODE;

struct sq{
        BNODE** stackqueue[100];
        DIR** dirstackqueue[100];
        int top; // top of stack and tail
        int head; //head of the queue
};typedef struct sq STACKQUEUE;

void push_stack(STACKQUEUE* s,BNODE* item, DIR* directory){
        if (s->head == -1){
                //empty
                s->head = s->top = 0;
                s->stackqueue[s->top] = item;
                s->dirstackqueue[s->top] = directory;
        }else{
                s->stackqueue[++s->top] = item;
                s->dirstackqueue[++s->top] = directory;
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

BNODE* stack_top_bnode(STACKQUEUE* s){
        return s->stackqueue[s->top];
}

DIR* stack_top_dir(STACKQUEUE* s){
        return s->dirstackqueue[s->top];
}
INODE* get_leaf(BNODE* active_root,char* target,BNODE* current);
void fx_ls(BNODE* current_dir);
int fx_touch(BNODE* current_dir,char* name,char* content);
int fx_mkdir(BNODE* current_dir,char* name);
void fx_pwd(STACKQUEUE* s);
void fx_cd(STACKQUEUE* s,BNODE* active_bnode,DIR* active_dir,char * target_dir);
void fx_cat(BNODE* active_root,char* target);

int main(){
        printf("[START] BTRFS initialising....\n");
        printf("[WARNING] BTRFS is a Non-Persistent System....\n");
        printf("[INFO] BTRFS mounting....\n");
        printf("[INFO] BTRFS mounted and ready for operation....\n");
        BNODE* active_bnode = NULL;
        DIR* active_dir = NULL;
        STACKQUEUE dirstack;dirstack.head = -1;dirstack.top = -1;

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
                        case 0:{fx_ls(active_dir->dir_tree);}break;
                        case 1:{fx_cd(&dirstack,active_dir->dir_tree,active_dir,words[1]);}break;
                        case 2:{}break;
                        case 3:{}break;
                        case 4:{fx_cat(active_dir->dir_tree,words[1]);}break;
                        case 5:{fx_pwd(&dirstack);}break;
                        case 6:{}break;
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
        int controller = current_dir->controller;
        if (controller == 0){
                        //internal
                        for (int j = 0; j < current_dir->length+1; j++){
                                fx_ls(current_dir->children[j]);
                        }
        }else{
                        //leafnode
                for (int i = 0; i < current_dir->length+1;i++){
                         int discriminator = current_dir->contents[i]->discriminator;
                                if (discriminator == 0){
                                        XFILE* f = current_dir->contents[i]->stored_file;
                                        printf("%s %d %s %lld\n","FILE",f->file_id,f->file_name,f->file_size);
                                }else{
                                        DIR* d = current_dir->contents[i]->stored_dir;
                                        printf("%s %d %s\n","DIR",d->dir_id,d->dir_name);
                                }
                }
        }
}

BNODE* get_insert_leaf(BNODE* current_root,char* target){
        for (int i = 0; i < current_root->length;i++){
                        if (strcmp(current_root->ids[i],target) == 0){
                                printf("File Exists\n");
                                return NULL;
                        }
                        if (strcmp(current_root->ids[i],target) == -1){
                                if(current_root->controller != 1){
                                        return get_insert_leaf(current_root->children[i-1],target);
                                }
                                if (current_root->controller == 1){
                                        return current_root;
                                }
                        }
        }
        return current_root;
}

int fx_touch(BNODE* current_dir,char* name,char* content){
        XFILE* newfile = (XFILE*)malloc(sizeof(XFILE));
        strcpy(newfile->file_name,name);
        newfile->file_id = rand();
        newfile->file_contents = (unsigned char*)malloc(sizeof(unsigned char));
        strcpy(newfile->file_contents,content);
        newfile->file_size = strlen(content);
        INODE* newi = (INODE*)malloc(sizeof(INODE));
        newi->stored_file = newfile;
        newi->discriminator = 0;

        BNODE* insert_into = get_insert_leaf(current_dir,name);
        BNODE * parent;
        if (insert_into->controller == 0 ){
                //leaf node //use stack for parent of the leaf
                if (insert_into->length < 3){
                      for (int i = 0; i < insert_into->length; i++){
                        if (strcmp(insert_into->contents[i],name)==-1){
                                insert_into[i+1] = insert_into->contents[i];
                                insert_into[i] = newi;
                        }
                }  

                }
                if (insert_into->length == 3){
                        //check parent
                        
                }
                
        }
        return 0;
}

int fx_mkdir(BNODE* current_dir,char* name){
      

        int controller = current_dir->controller;
        if (controller == 0){
                //internal node
                int count = current_dir->length;
                if (count < 2){
                        //no problem with insert
                        
                }
        }else{
              //do nothing  
        }
        return 0;
}

void fx_pwd(STACKQUEUE* s){
        int t = s->head;
        printf("[PATH] /");
        DIR* a;
        while (t <= s->top) {
                a = s->dirstackqueue[t++];
                printf("%s/",a->dir_name);
        }
        printf("\n");
}
void fx_cd(STACKQUEUE* s,BNODE* active_bnode,DIR* active_dir,char * target_dir){
        if (strcmp(target_dir,".")==0){
                //move to parent
                int k = pop_stack(s);
                if (k == 1){
                        printf("[WARN] Can't go higher than root\n");
                }else{
                        active_bnode = stack_top_bnode(s); 
                        active_dir = stack_top_dir(s);
                }
        }else{
                //move downwind,search for the directory first
                BNODE* newnode; INODE* newinode;
                newinode = get_leaf(active_bnode,target_dir,newnode);

                if (newinode!= NULL){
                        //change dir
                        push_stack(s,newnode,newinode->stored_dir);
                        active_bnode = newnode;
                        active_dir = newinode->stored_dir;
                }
        }
}
void fx_cat(BNODE* active_root,char* target){
        //find target and then print contents
        BNODE* current_root = active_root;
        INODE* i = get_leaf(current_root,target,current_root);
        if (i == NULL) return;
        if (i->discriminator == 0){
                printf("[OUTPUT] File Contents: %s | %lld bytes\n",i->stored_file->file_contents,i->stored_file->file_size);
        }else{
                printf("[OUTPUT] This is a directory\n");
        }
}

INODE* get_leaf(BNODE* active_root,char* target,BNODE*current){
        BNODE* current_root = active_root;
        int found = 0;
                for (int i = 0; i < current_root->length;i++){
                        if (strcmp(current_root->ids[i],target) == 0){
                                found = 1;
                                if (current_root->controller != 1){
                                        return get_leaf(current_root->children[i],target,current_root);
                                }
                                if (current_root->controller == 1){
                                        //return the INODE
                                        for (int j = 0; j < current_root->length;j++){
                                                if (current_root->contents[j]->discriminator == 0){
                                                        if (strcmp(current_root->contents[j]->stored_file->file_name,target)==0){
                                                                return current_root->contents[j];
                                                        }
                                                }
                                                if (current_root->contents[j]->discriminator == 1){
                                                        if (strcmp(current_root->contents[j]->stored_dir->dir_name,target)==0){
                                                                return current_root->contents[j];
                                                        }
                                                }
                                        }
                                }
                        }
                        if (strcmp(current_root->ids[i],target) == -1){
                                        return get_leaf(current_root->children[i],target,current_root);
                        }
                }
                
        if (found == 0){
                printf("[OUTPUT] OBJECT Not Found\n"); return NULL;
        }
}