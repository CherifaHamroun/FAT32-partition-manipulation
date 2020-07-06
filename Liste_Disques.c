#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void Liste_Disques(){
 FILE *disk = NULL;
 int i = 0;
char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
 char sd[4] = "sd";
 char hd[4] = "hd";

 while (i<26)
 {
    char dev[10] = "/dev/";
    sd[2]=alphabet[i];
    sd[3]='\0';
    strcat(dev,sd);
    disk = fopen(dev,"rb");
    i++;
    if(disk != NULL){
        printf("%s\t",dev);
        fclose(disk);
    }
 }
 i = 0;
 while (i<26)
 {
    char dev[10] = "/dev/";
    hd[2]=alphabet[i];
    hd[3]='\0';
    strcat(dev,sd);
    disk = fopen(dev,"rb");
    i++;
    if(disk != NULL){
        printf("%s\t",dev);
        fclose(disk);
    }
 }
 printf("\n");
}
int main(int argc, char const *argv[])
{
    Liste_Disques();
    return 0;
}
