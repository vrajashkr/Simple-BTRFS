#include <stdio.h>
#include <stdlib.h>
/*
        This is a basic implementation of a B+ Tree, en route to becoming a basic BTRFS based Ramdisk filesystem
*/
int inode_alpha = 0;

int main(){
        printf("-----Loading File System-------\n");
        printf("File system operations:\n
        0. Insert a data item\n
        1. Delete an item\n
        2. Traverse File System\n
        3. Search for a file\n
        4. Erase and Quit\n");
        int n;
        Btree* btrfs = (Btree*)malloc(sizeof(Btree));
        btrfs->root = NULL; btrfs->number_of_nodes = 0;
        scanf("%d",&n);
        switch (n)
        {
                case 0:
                        int u = accept_input_insert(btrfs);
                        if (u == 0){
                                printf("File System Failure\n");
                        }else{
                                printf("Insert Successful\n");
                        }break;

                case 1:
                         int u = delete_item(btrfs);
                        if (u == 0){
                                printf("File System Failure\n");
                        }else{
                                printf("Delete Successful\n");
                        }break;
                case 3:
                         int u = search_item(btrfs);
                        if (u == -1){
                                printf("Search turned up no results\n");
                        }else{
                                printf("Search Successful, file found\n");
                                printf("Details\n");
                        }break;
                case 2:
                        break;
                case 4:
                        //free everything if you want
                        printf("----Terminating File System-----\n");
                        return;
                
        }
}
int accept_input_insert(Btree* tree){
        SingleTNode* newnode = (SingleTNode*)malloc(sizeof(SingleTNode));
        newnode->number_of_bytes = 0;
        newnode->contents = NULL;
        newnode->inode = inode_alpha++;
        printf("Enter list item count\n");
        int m = 0
        scanf("%d",&m);
        newnode->contents = (TYPE*)malloc(sizeof(TYPE)*m);

        int d; char t3[200];
        for (int u = 0; u < m;u++){
                //take input for the data
                printf("Object %d: Enter Type followed by value: ",u);
                scanf("%d",&d);
                switch (d)
                {
                        case 0:
                                //discrim represents an integer
                                scanf("%d",&newnode->contents[u]->data.integer);
                                newnode->contents[u]->discriminator = 0;
                                break;
                        case 1:
                                //discrim represents a string
                                scanf("%[^\n]s",t3);
                                newnode->contents[u]->data = (char*)malloc(sizeof(char)*strlen(t3));
                                strcpy(newnode->contents[u]->data,t3);
                                newnode->contents[u]->discriminator = 1;
                                getchar();
                                break;
                        case 2:
                                //data represents a double
                                scanf("%ld",&newnode->contents[u]->data.dbl);
                                newnode->contents[u]->discriminator =2;
                                break;
                        
                }
        }

        int v = insert(btree, newnode);
        return v;

}

int delete_item(Btree* tree){
        long long int target;
        printf("Enter the inode of the target\n");
        scanf("%ld",&target);
        return delete(tree, target);
}

int search_item(Btree* tree){
        long long int target;
        printf("Enter the inode of the search target\n");
        scanf("%ld",&target);
        return search(tree,item);
}

int traverse_system(Btree* tree){

}