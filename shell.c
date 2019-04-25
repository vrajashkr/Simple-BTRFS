#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
        char buffer[1024];
        char* words[10];
        char* supported_comms[] = {"ls","mkdir","rmdir","cat","pwd","touch","rm","exit"};
        int i,p;
        while(1){
                printf("\n<X>>> ");
                scanf(" %[^\n]s",buffer);
                i = 0;
                words[i] = strtok(buffer," ");
                while ((words[++i] = strtok(NULL," ")) != NULL);
                for ( p = 0;p<8;p++){
                        if (strcmp(supported_comms[p],words[0]) == 0){
                                break;
                        }
                }
                switch (p)
                {
                        case 0:{}break;
                        case 1:{}break;
                        case 2:{}break;
                        case 3:{}break;
                        case 4:{}break;
                        case 5:{}break;
                        case 6:{}break;
                        case 7:{}break;
                        default:{printf("[ERROR] Is it that hard to follow instructions? That command doesn't exist\n");}
                                break;
                }
        }
}