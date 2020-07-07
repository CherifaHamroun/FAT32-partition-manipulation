#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RED(str) "\e[0;91;49m"str"\e[0m"
#define GREEN(str) "\e[0;92;49m"str"\e[0m"
FILE *fichier;    
unsigned int TailleSecteur;
unsigned int secteurParCluster;
unsigned int SecteursReserves;
unsigned int AdrDebutFAT;
unsigned int NombreTablesFAT;
unsigned int TailleFAT;
unsigned int ClusterRacine;
unsigned int AdrRacine;

typedef struct Entree {                      //L'entree a une taille de 32 byte
	unsigned char nomf[8];                   //unsigned char sur 1 byte *8
	unsigned char extension[3];              //unsigned char sur 1 byte *3 
	int8_t attrib;                           // int8_t sur 1 byte
	int8_t reserve;
	int8_t millisecondes_creation;
	int16_t heure_creation;                  // int16_t sur 2 byte
	int16_t date_creation;
	int16_t date_dernier_acces;
	int16_t high_numero_premier_cluster_fat;
	int16_t heure_derniere_modification;
	int16_t date_derniere_modification;
	int16_t low_numero_premier_cluster_fat;     // int32_t sur 4 byte
	int32_t taillef;
} Entree;



void lire_secteur(char disque_physique[] , int numSecteur,unsigned char  *buffer){
int k,s,j,taille;
taille=512;
long int depl=taille*numSecteur;  /* secteur */ 
FILE *disk=NULL;
char chemain[10]="/dev/";
disk = fopen(strcat(chemain,disque_physique),"rb");                                 /* Linux  */
if(disk == NULL) printf("\n Erreur, le disque physique n'est pas ouvert\n" );
else{ 
	s=fseek(disk, depl, SEEK_SET); 
   	if(s!=0) printf("\n Erreur de fseek : s= %d",s); 
    else{ k=fread(buffer,taille, 1, disk);  
        if(k<=0) printf("\n Erreur de fread = %d ",k);         
        else {printf("\n [Lire_secteur] Lecture du secteur %d , Nombre d’éléments lus =%d\n",numSecteur,k); 
  		}
	}
   	fclose(disk);
    }
}


void afficher_secteur(char disque_physique[] , int numSecteur){

unsigned char buffer[512];
lire_secteur(disque_physique,numSecteur,buffer);
for(int j=0; j<512; j++)
 if(j % 16 != 15){  printf("%02x ",buffer[j]);}
 else{
	 printf("%02x ",buffer[j]);
	 printf("\n");
 }
}


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
    //InitParametres();
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
            printf("Cluster Libre\n");
            return 0;
        }
        else if (buffer1 >= 0x0FFFFFF0 && buffer1 <= 0x0FFFFFF6)
        {
           printf("Cluster reserve\n");
           return -2;
        }
        else if (buffer1 == 0x0FFFFFF7)
        {
           printf("Cluster Defectueux\n");
           return -2;
        }
        else if (buffer1 >= 0x0FFFFFF8 && buffer1 <= 0x0FFFFFFF)
        {
           printf("Dernier cluster\n");
           return -1;
        }
        else
        {
           return buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;
        }
        
        
        printf("\n");
    }
}


void lire_cluster(char disque_physique[],int cluster)
{
    printf("[LireCluster] cluster = %d\n", cluster);
    unsigned char buffer[TailleSecteur];
    for (int i = 0; i < secteurParCluster; i++)
	{
		lire_secteur(disque_physique, AdrRacine/TailleSecteur + (cluster - ClusterRacine) * secteurParCluster+i, buffer);
        printf("[LireCluster] secteur = %d, adr: %d\n", i,AdrRacine/TailleSecteur + (cluster - ClusterRacine) * secteurParCluster+i);
		Entree* entree = (Entree*) buffer;
		for (int j = 0; j < TailleSecteur / sizeof(Entree) && entree[j].nomf[0] != '\0'; j++)
        {
            //printf("[?] entree = %d\n", j);
            int8_t mask=0x0010;
            int8_t test = mask&(int8_t) entree[j].attrib;
            char type[7];
            if (test!=0){char dossier[]="Dossier";strcpy(type,dossier);} else{char fich[]="Fichier";strcpy(type,fich);}
			if (entree[j].nomf[0] != 0xE5 && entree[j].attrib != 0xf)
            printf(GREEN("\nElementtrouve!\n\tnom+ext: %s\n\textension: %s\n\ttype: %s\n\ttaille: %u\n\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
			else if (entree[j].nomf[0] == 0xE5 && entree[j].attrib != 0xf)
			printf(RED("\nElement supprime!\n\tnom+ext: %s\n\textension: %s\n\ttype: %s\n\ttaille: %u\n\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
        }
	}      
} 

void afficher_Fdel(char disque_physique[])
{
    //InitParametres();
    int numcluster = ClusterRacine;
    printf("[AfficherFdel]cluster racine %d\n",ClusterRacine);
    while(numcluster != -1){
        printf("[AfficherFdel]cluster numero %d\n",numcluster);
        lire_cluster(disque_physique,numcluster);
        numcluster=Cluster_Suivant(disque_physique,numcluster);
    }
}


void main() 
{ 
			
/*printf("Veuillez entrer le disque_physique: ");
char *disque;
scanf("%s",disque);
printf("Veuillez entrer le numéro de secteur: ");
int numsect;
scanf("%d",&numsect);
afficher_secteur(disque,numsect);*/
fichier= fopen("/dev/sdb1", "rb");
        
    if (fichier == NULL){
        printf("Erreur lors de l`ouverture du fichier !\n");
    }
	else{
InitParametres();
//printf("init parametres");
//afficher_secteur("sdb1",0);
afficher_Fdel("sdb1");
//lire_cluster("sdb1",2);
}

} 
