#include <stdlib.h>
#include <stdio.h>
#include <string.h>
FILE *fichier;

    unsigned int TailleSecteur;
    unsigned int secteurParCluster;
    unsigned int SecteursReserves;
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
    printf("--------Parametres du secteur Boot FAT32----------\n");
    printf("\tTaille dun Secteur :%d\n\tSecteurs Par Cluster :%d\n\tSecteurs Reserves :%d\n",TailleSecteur,secteurParCluster,SecteursReserves);
    printf("\tNomre de tables FAT :%d\n\tTaille FAT :%d\n\tCluster Racine :%d\n\tAdr du repertoire Racine:%d\n",NombreTablesFAT,TailleFAT,ClusterRacine,AdrRacine);
    printf("--------------------------------------------------\n");
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
	InitParametres();
}
