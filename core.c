#include <stdio.h>
#include <stdlib.h>

union type{ //dynamic types
        int integer;
        char* string;
        double dbl;
}

typedef struct dtype{
        union type data;
        int discriminator;
} TYPE;

struct llistnode{
        struct llistnode* left;
        struct a* core;
        struct llistnode* right;
}

typedef struct llistnode LISTNODE;

struct a{
        //Struct for the data storage, single unit
        long long int number_of_bytes;
        TYPE* contents;
        long long int inode;
}
typedef struct a SingleTNode;

struct t {
        SingleTNode* keys;
        int number_of_keys;
}
typedef struct t TreeNode;

struct x{
        TreeNode* root;
        long long int number_of_nodes;
}
typedef struct x Btree;

int insert(Btree* tree, SingleTNode* newitem){

}

int delete(Btree* tree, int item){


}

int search(Btree* tree, int* key){
        
}