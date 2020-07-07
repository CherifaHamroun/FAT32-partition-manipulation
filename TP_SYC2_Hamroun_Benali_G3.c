//{1} Bibliotheques C utilisees
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//{2} Affichage
#define Yellow(str) "\e[0;33m"str"\e[0m"
#define Blue(str) "\e[0;34m"str"\e[0m"
#define Magenta(str) "\e[0;36m"str"\e[0m"

//{3} Variables declarees globales
FILE *fichier;   //[1] Fichier Disque a etudier (Demande en entree a l`utilisateur)

//[2] Parametres extraits depuis la section BOOT (l`utilite de chacun est representee par son nom)
unsigned int TailleSecteur; 
unsigned int secteurParCluster;
unsigned int SecteursReserves;
unsigned int AdrDebutFAT;
unsigned int NombreTablesFAT;
unsigned int TailleFAT;
unsigned int ClusterRacine;
unsigned int AdrRacine;

//[3] Entree est une structure definie le contenu d`une entree format court d`un repertoire 
//sa taille est de 32octets 
//l`utilite de chaque champs de la structure est representee par son nom
typedef struct Entree {                     
	unsigned char nomf[8];                   
	unsigned char extension[3];             
	int8_t attrib;                           // int8_t sur 1 byte
	int8_t reserve;
	int8_t millisecondes_creation;
	int16_t heure_creation;                  // int16_t sur 2 byte
	int16_t date_creation;
	int16_t date_dernier_acces;
	int16_t high_numero_premier_cluster_fat;
	int16_t heure_derniere_modification;
	int16_t date_derniere_modification;
	int16_t low_numero_premier_cluster_fat;     
	int32_t taillef;                        // int32_t sur 4 byte
} Entree;

//{4} Fonctions Demandees
//Cette fonction permet d`afficher tous les disques physique connectes sur le PC
//Principe de fonctionnement :
//
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
        printf("\t\t\t\t\t\t[%d] %s\n",i,dev);
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
        printf("\t\t\t\t\t\t[%d] %s\n",i,dev);
        fclose(disk);
    }
 }
 printf("\n");
}

void lire_secteur(char disque_physique[] , int numSecteur,unsigned char  *buffer){
int n,s,j,taille;
taille=512;
long int depl=taille*numSecteur;  /* secteur */ 
FILE *disk=NULL;
char chemain[10]="/dev/";
disk = fopen(strcat(chemain,disque_physique),"rb");                                 /* Linux  */
if(disk == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
else{ 
	s=fseek(disk, depl, SEEK_SET); 
   	if(s!=0) printf("\n Erreur de fseek : s= %d",s); 
    else{ n=fread(buffer,taille, 1, disk);  
        if(n<=0) printf("\n Erreur de fread = %d ",n);         
        else { 
  		}
	}
   	fclose(disk);
    }
}


void afficher_secteur(char disque_physique[] , int numSecteur){

unsigned char buffer[512];
lire_secteur(disque_physique,numSecteur,buffer);
    printf("\n");
	printf("\t\t\t\t[%d]\t ",0);
for(int j=0; j<512; j++)
 if(j % 16 != 15){  printf("%02x ",buffer[j]);}
 else{
    printf("%02x ",buffer[j]);
	printf("\n");
    if (j+1!=512) printf("\t\t\t\t[%d]\t ",j+1);
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
    printf("\n");
    printf(Yellow("\t\t\tParametres du secteur Boot FAT32 :\n"));
    printf("\t\t\t\tTaille dun Secteur :%d\n\t\t\t\tSecteurs Par Cluster :%d\n\t\t\t\tSecteurs Reserves :%d\n",TailleSecteur,secteurParCluster,SecteursReserves);
    printf("\t\t\t\tNomre de tables FAT :%d\n\t\t\t\tTaille FAT :%d\n\t\t\t\tCluster Racine :%d\n\t\t\t\tAdr du repertoire Racine:%d\n",NombreTablesFAT,TailleFAT,ClusterRacine,AdrRacine);
    printf("\t\t\t\tAdr debut FAT :%d\n",AdrDebutFAT);
    printf("\n");
}

int Cluster_Suivant(int numCluster){
    int ns,ss,taille;
    taille=4;
    unsigned char buffer[4];
    long int depl=taille*numCluster;
    if(fichier == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
    else{ 
        ss=fseek(fichier, AdrDebutFAT+depl, SEEK_SET); 
        ns=fread(buffer,taille, 1, fichier);  
        if(ns<=0) printf("\n Erreur de fread = %d ",ns);          
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
    unsigned char buffer[TailleSecteur];
    for (int i = 0; i < secteurParCluster; i++)
	{
		lire_secteur(disque_physique, AdrRacine/TailleSecteur + (cluster - ClusterRacine) * secteurParCluster+i, buffer);
		Entree* entree = (Entree*) buffer;
		for (int j = 0; j < TailleSecteur / sizeof(Entree) && entree[j].nomf[0] != '\0'; j++)
        {
            int8_t mask=0x0010;
            int8_t test = mask&(int8_t) entree[j].attrib;
            char type[7];
            if (test!=0){char dossier[]="Dossier";strcpy(type,dossier);} else{char fich[]="Fichier";strcpy(type,fich);}
			if (entree[j].nomf[0] != 0xE5 && entree[j].attrib != 0xf){
            printf(Blue("\n\t\t\tFichier trouve!\n\t\t\t\tnom+ext: %s\n\t\t\t\textension: %s\n\t\t\t\ttype: %s\n\t\t\t\ttaille: %u\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
            printf(Blue("\t\t\t\tPremier Cluster du fichier: %u %u\n"), entree[j].high_numero_premier_cluster_fat,entree[j].low_numero_premier_cluster_fat);
            printf(Blue("\t\t\t\tPere : / \n"));

            }
			else if (entree[j].nomf[0] == 0xE5 && entree[j].attrib != 0xf)
			printf(Yellow("\n\t\t\tElement supprime!\n\t\t\t\tnom+ext: %s\n\t\t\t\textension: %s\n\t\t\t\ttype: %s\n\t\t\t\ttaille: %u\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
        }
	}      
} 

void afficher_Fdel(char disque_physique[])
{
    int numcluster = ClusterRacine;
    while(numcluster != -1){
        lire_cluster(disque_physique,numcluster);
        numcluster=Cluster_Suivant(numcluster);
    }
}


int main(int argc, char const *argv[])
{
    printf(Magenta("[1] Fonction Liste_Disques : "));
    printf("Les disques physiques connectes sur votre PC sont :\n");
    printf("\n");
    Liste_Disques();
    printf(Magenta("[2] Fonction Lire_secteur : "));
    printf("Veuillez entrer le disque_physique: ");
    char disque[10];
    scanf("%s",disque);
    printf("\t\t\t    Veuillez entrer le numéro de secteur: ");
    int numsect;
    scanf("%d",&numsect);
    printf("\n");
    printf(Magenta("[3] Fonction Afficher_secteur : "));
    afficher_secteur(disque,numsect);
    printf("\n");
    printf(Magenta("[4] Fonction Afficher_Fdel: "));
    printf("Veuillez entrer la partition FAT32: ");
	char disk[10];
	scanf("%s",disk);
	char chemain[10]="/dev/";
    fichier= fopen(strcat(chemain,disk), "rb");
    if (fichier == NULL){
        printf("Erreur lors de l`ouverture du fichier !\n");
    }
	else{
    InitParametres();
    afficher_Fdel(disk);
    fclose(fichier);
    }
    return 0;
}
