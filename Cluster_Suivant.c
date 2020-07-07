#include <stdlib.h>
#include <stdio.h>
#include <string.h>
FILE *fichier;

    unsigned int TailleSecteur;
    unsigned int secteurParCluster;
    unsigned int SecteursReserves;
    unsigned int AdrDebutFAT;
    unsigned int NombreTablesFAT;
    unsigned int TailleFAT;
    unsigned int ClusterRacine;
    unsigned int AdrRacine;


void InitParametres(){
    unsigned char buffer[512];
    fread(buffer, 1, 512, fichier);
    TailleSecteur = buffer[11] | buffer[12]<<8;
    secteurParCluster = buffer[13];
    SecteursReserves = buffer[14] | buffer[15]<<8;
    NombreTablesFAT = buffer[16];
    TailleFAT = buffer[36] | buffer[37]<<8 | buffer[38]<<16 | buffer[39]<<24;
    ClusterRacine = buffer[44] | buffer[45]<<8 | buffer[46]<<16 | buffer[47]<<24;
    AdrRacine = (SecteursReserves + NombreTablesFAT * TailleFAT) * TailleSecteur;
    AdrDebutFAT =SecteursReserves * TailleSecteur;
    printf("--------Parametres du secteur Boot FAT32----------\n");
    printf("\tTaille dun Secteur :%d\n\tSecteurs Par Cluster :%d\n\tSecteurs Reserves :%d\n",TailleSecteur,secteurParCluster,SecteursReserves);
    printf("\tNomre de tables FAT :%d\n\tTaille FAT :%d\n\tCluster Racine :%d\n\tAdr du repertoire Racine:%d\n",NombreTablesFAT,TailleFAT,ClusterRacine,AdrRacine);
    printf("\tAdr debut FAT :%d\n",AdrDebutFAT);
    printf("--------------------------------------------------\n");
}
int Cluster_Suivant(char disque_physique[] ,int numCluster){
    FILE *disks=NULL;
    char chemains[10]="/dev/";
    disks = fopen(strcat(chemains,disque_physique),"rb");    
    InitParametres();
    int ns,ss,taille;
    taille=4;
    unsigned char buffer[4];
    long int depl=taille*numCluster;
    if(disks == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
    else{ 
        ss=fseek(disks, AdrDebutFAT+depl, SEEK_SET); 
        ns=fread(buffer,taille, 1, disks);  
        if(ns<=0) printf("\n Erreur de fread = %d ",ns);         
        else printf("\n Lecture du cluster %d , Nombre d’éléments lus =%d\n",numCluster,ns); 
         for(int j=0; j<4; j++)
        if(j % 16 != 15){  printf("%02x ",buffer[j]);}
        else{
        printf("%02x ",buffer[j]);
        printf("\n");
         }
        printf("\n");
        unsigned int buffer1;
        buffer1 = buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;
        if (buffer1==0)
        {
            printf("Cluster Libre");
            return 0;
        }
        else if (buffer1 >= 0x0FFFFFF0 && buffer1 <= 0x0FFFFFF6)
        {
           printf("Cluster reserve");
           return -2;
        }
        else if (buffer1 == 0x0FFFFFF7)
        {
           printf("Cluster Defectueux");
           return -2;
        }
        else if (buffer1 >= 0x0FFFFFF8 && buffer1 <= 0x0FFFFFFF)
        {
           printf("Dernier cluster");
           return -1;
        }
        else
        {
           return buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;
        }
        
        
        printf("\n");
    }
}
int main(){
	printf("Veuillez entrer le disque_physique: ");
	char disque[10];
	scanf("%s",disque);
	char chemain[10]="/dev/";
    fichier= fopen(strcat(chemain,disque), "rb");
        
    if (fichier == NULL){
        printf("Erreur lors de l`ouverture du fichier !\n");
    }
	else{
    printf("Veuillez entrer le numero du Cluster a lire: ");
	int num;
	scanf("%d",&num);
    int pro = Cluster_Suivant(disque,num);
    printf("%d",pro);
    int n,s,taille;
    taille=512;
    unsigned char buffer[512];
    if(fichier == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
    else{ 
        s=fseek(fichier, AdrDebutFAT, SEEK_SET); 
        n=fread(buffer,taille, 1, fichier);  
        if(n<=0) printf("\n Erreur de fread = %d ",n);         
        else printf("\n Lecture, Nombre d’éléments lus =%d\n",n); 
        for(int j=0; j<512; j++)
            if(j % 16 != 15){  printf("%02x ",buffer[j]);}
            else{
                    printf("%02x ",buffer[j]);
                    printf("\n");
                }

    fclose(fichier);
    }


} 
}
