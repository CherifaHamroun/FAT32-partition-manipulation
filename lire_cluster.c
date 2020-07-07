#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RED(str) "\e[0;91;49m"str"\e[0m"
#define GREEN(str) "\e[0;92;49m"str"\e[0m"
FILE *fichier;    
unsigned int TailleSecteur;
unsigned int secteurParCluster;
unsigned int SecteursReserves;
unsigned int NombreTablesFAT;
unsigned int TailleFAT;
unsigned int ClusterRacine;
unsigned int AdrRacine;

typedef struct Entree {
	unsigned char filename[8];
	unsigned char extension[3];
	int8_t attrib;
	int8_t reserved;
	int8_t millisecondes_creation;
	int16_t heure_creation;
	int16_t date_creation;
	int16_t date_dernier_acces;
	int16_t high_numero_premier_cluster_fat;
	int16_t heure_derniere_modification;
	int16_t date_derniere_modification;
	int16_t low_numero_premier_cluster_fat;
	int32_t filesize;
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
        else {printf("\n Lecture du secteur %d , Nombre d’éléments lus =%d\n",numSecteur,k); 
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


void InitParametres(char disque_physique[]){
    unsigned char buffer[512];
    lire_secteur(disque_physique,0,buffer);
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

void lire_entree(unsigned char buffer[32])
{
    //printf(buffer);
    unsigned char nom[10]="";
    for(int i=0;i<8;i++){
        nom[i]=buffer[i];
    }
    if (nom[0]==0x00){
        printf("entree libre");
    }else if (nom[0]==0xE5){
        printf("entree supprimee");
    }else{
    //nom= buffer[0] ;//| buffer[1]<<8;
    for(int i=0;i<8;i++){
        printf("%s",nom);
    }
    //printf("carract: %02x %02x %02x %02x %02x %02x %02x %02x",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
    }
}

void lire_cluster(char disque_physique[],int cluster)
{
    printf("[?] cluster = %d\n", cluster);
    unsigned char buffer[32];
    int k,s,j,taille=32;
    long int depl=(SecteursReserves+NombreTablesFAT*TailleFAT/TailleSecteur+(cluster-2)*secteurParCluster)*TailleSecteur;  //adr du cluster  
    FILE *disk=NULL;
    char chemain[10]="/dev/";
    disk = fopen(strcat(chemain,disque_physique),"rb");                                 /* Linux  */
    if(disk == NULL) printf("\n Erreur, le disque physique n'est pas ouvert\n" );
    else{ 
        s=fseek(disk, depl, SEEK_SET); 
        if(s!=0) printf("\n Erreur de fseek : s= %d",s); 
        else{ 
            for (int i=0; i<TailleSecteur*secteurParCluster/taille; i++){
                k=fread(buffer,taille, 1, disk);  
                if(k<=0) printf("\n Erreur de fread = %d ",k);         
                else {
                    printf("\n Lecture de l entree %d \n",i); 
                    lire_entree(buffer);
                    //s=fseek(disk, depl+32, SEEK_SET); 
                }
            }
        }
    }
} 

void afficher_Fdel(char disque_physique[])
{
	
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
InitParametres("sdb1");
//afficher_secteur("sdb1",0);
afficher_Fdel("sdb1");
lire_cluster("sdb1",2);

} 
