//{1} Bibliotheques C utilisees
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//{2} Couleurs utilisees pour l`Affichage
#define Yellow(str) "\e[0;33m"str"\e[0m"
#define Blue(str) "\e[0;34m"str"\e[0m"
#define Magenta(str) "\e[0;36m"str"\e[0m"

//{3} Variables declarees globales

//[1] Fichier Disque a etudier (Demande en entree a l`utilisateur)
FILE *fichier;   

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
// sa taille est de 32octets 
// l`utilite de chaque champs de la structure est representee par son nom
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
//[1 Liste_Disques] Cette fonction permet d`afficher tous les disques physique connectes sur le PC
//Cherche tous les disques connectes dans le repertoire /dev et affiche 
//Les disques ayant une interface ATA (hd) et SATA (sd)
void Liste_Disques(){
 FILE *disk = NULL;
 int i = 0;
 char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";//Les disques sont nommes sdX ou hdX tq X = a,b,c ou d ...
 char sd[4] = "sd";
 char hd[4] = "hd";
 //Boucle Affichage des disques (sd)
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
//Boucle Affichage des disques (hd)
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

//[2 Lire_Secteur--Utilisee dans Afficher_Secteur] Cette fonction permet de lire un secteur numero numSecteur 
//dans buffer a partir du fichier se trouvant dans /dev/disque_physique
void lire_secteur(char disque_physique[] , int numSecteur,unsigned char  *buffer){
int n,s,j,taille; 
taille=512;// taille d`un secteur logique sur ubuntu 20.04
long int depl=taille*numSecteur;  // deplacement vers ladr ou se trouve le secteur numSecteur
FILE *disk=NULL; 
char chemain[10]="/dev/";
disk = fopen(strcat(chemain,disque_physique),"rb");//ouverture du fichier                     
if(disk == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
else{ 
	s=fseek(disk, depl, SEEK_SET); //positionnement de la tete de lecture 
   	if(s!=0) printf("\n Erreur de fseek : s= %d",s); 
    else{ n=fread(buffer,taille, 1, disk);  //lecture du secteur dans le buffer
        if(n<=0) printf("\n Erreur de fread = %d ",n);         
        else { 
  		}
	}
   	fclose(disk);//fermeture du fichier
    }
}

//[3 Afficher_Secteur] Cette fonction permet d`afficher un secteur
// numero numSecteur a partir d`un fichier dans /dev
void afficher_secteur(char disque_physique[] , int numSecteur){
unsigned char buffer[512];
lire_secteur(disque_physique,numSecteur,buffer);
    printf("\n");
	printf("\t\t\t\t[%d]\t ",0);
    //Boucle d`affichage du buffer
for(int j=0; j<512; j++)
 if(j % 16 != 15){  printf("%02x ",buffer[j]);}
 else{
    printf("%02x ",buffer[j]);
	printf("\n");
    if (j+1!=512) printf("\t\t\t\t[%d]\t ",j+1);
 }
}

//[4 InitParametres -- Utilisee dans Afficher_Fdel] Cette fonction permet d`initialiser les variables globales 
// ces variables sont les parametres extraits depuis le secteur boot
//concernant l`organisation des fichiers en clusters et secteurs ainsi que les tables FAT
void InitParametres(){
    unsigned char buffer[512];
    fread(buffer, 1, 512, fichier);
    TailleSecteur = buffer[11] | buffer[12]<<8; //initialisation de la taille des secteurs tenant sur 2 octets en little endian ce qui fait que pour recuperer le nombre en decimal il faut faire un decalage et un ou bit a bit
    secteurParCluster = buffer[13];// initialisation du nombre de secteur par cluster qui se trouve a l`octet 13 du secteur BOOT
    SecteursReserves = buffer[14] | buffer[15]<<8;//initialisation du nombre de secteurs de la zone reservee tenant sur 2 octets en little endian ce qui fait que pour recuperer le nombre en decimal il faut faire un decalage et un ou bit a bit
    NombreTablesFAT = buffer[16];// initialisation du nombre de tables FAT qui se trouve a l`octet 16 du secteur BOOT (Generalement 2)
    TailleFAT = buffer[36] | buffer[37]<<8 | buffer[38]<<16 | buffer[39]<<24;//initialisation de la taille de la table FAT  tenant sur 4 octets (par nombre de secteur) en little endian ce qui fait que pour recuperer le nombre en decimal il faut faire un decalage et un ou bit a bit
    ClusterRacine = buffer[44] | buffer[45]<<8 | buffer[46]<<16 | buffer[47]<<24;//initialisation du nombre du cluster ou se trouve le cluster racine tenant sur 4 octets en little endian ce qui fait que pour recuperer le nombre en decimal il faut faire un decalage et un ou bit a bit
    AdrRacine = (SecteursReserves + NombreTablesFAT * TailleFAT) * TailleSecteur;//initialisation de l`adresse du repertoire racine qui se trouve apres le secteur boot , la zone reservee et les 2 FAT
    AdrDebutFAT =SecteursReserves * TailleSecteur;//initialisation de l`adresse debut de la 1ere FAT qui se trouve apres le secteur boot , la zone reservee
    //Affichage de tous les parametres extraits et calcules plus haut
    printf("\n");
    printf(Yellow("\t\t\tParametres du secteur Boot FAT32 :\n"));
    printf("\t\t\t\tTaille dun Secteur :%d\n\t\t\t\tSecteurs Par Cluster :%d\n\t\t\t\tSecteurs Reserves :%d\n",TailleSecteur,secteurParCluster,SecteursReserves);
    printf("\t\t\t\tNomre de tables FAT :%d\n\t\t\t\tTaille FAT :%d\n\t\t\t\tCluster Racine :%d\n\t\t\t\tAdr du repertoire Racine:%d\n",NombreTablesFAT,TailleFAT,ClusterRacine,AdrRacine);
    printf("\t\t\t\tAdr debut FAT :%d\n",AdrDebutFAT);
    printf("\n");
}

//[5 Cluster_Suivant -- Utilisee dans Afficher_Fdel] Cette fonction retourne le numero du cluster qui suit 
// le cluster numCluster (en decimal)
int Cluster_Suivant(int numCluster){
    int ns,ss,taille;
    taille=4;
    unsigned char buffer[4];
    long int depl=taille*numCluster;//Deplacement vers l`adresse du cluster numero numCluster
    if(fichier == NULL) printf("\n Erreur le disque physique1 n'est pas ouvert\n" );
    else{ 
        ss=fseek(fichier, AdrDebutFAT+depl, SEEK_SET); //Positionnement du curseur 
        ns=fread(buffer,taille, 1, fichier);  
        if(ns<=0) printf("\n Erreur de fread = %d ",ns);          
        unsigned int buffer1;
        buffer1 = buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;// Conversion de la valeur en hexa et littre endian en decimal (Decalage et ou bit a bit)
        if (buffer1==0)//Cas de cluster Libre
        {
            printf("Cluster Libre\n");
            return 0;
        }
        else if (buffer1 >= 0x0FFFFFF0 && buffer1 <= 0x0FFFFFF6)//Cas de cluster reserve
        {
           printf("Cluster reserve\n");
           return -2;
        }
        else if (buffer1 == 0x0FFFFFF7)//Cas de cluster defectueux
        {
           printf("Cluster Defectueux\n");
           return -2;
        }
        else if (buffer1 >= 0x0FFFFFF8 && buffer1 <= 0x0FFFFFFF)//Cas de dernier cluster 
        {
           return -1;
        }
        else// cas du cluster suivant existant
        {
           return buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;//Conversion de la valeur en hexa et littre endian en decimal (Decalage et ou bit a bit)
        }
        
        
        printf("\n");
    }
}

//[6 Lire_Cluster -- Utilisee dans Afficher_Fdel] Cette fonction lit un cluster et affiche
//toutes les infomations se trouvant dans les entrees d`un repertoire se trouvant dans ce meme cluster
void lire_cluster(char disque_physique[],int cluster)
{
    unsigned char buffer[TailleSecteur];
    for (int i = 0; i < secteurParCluster; i++)
	{
		lire_secteur(disque_physique, AdrRacine/TailleSecteur + (cluster - ClusterRacine) * secteurParCluster+i, buffer);//Lectre du secteur i du cluster  
		Entree* entree = (Entree*) buffer;//Conversion du buffer lit en un tableau d`entrees format court de 32 octets 
        //Parcour entree par entree et affichage de nom,extention,type,1er cluster,pere et autres parametres ...
		for (int j = 0; j < TailleSecteur / sizeof(Entree) && entree[j].nomf[0] != '\0'; j++)
        {
            int8_t mask=0x0010;//Masque pour extraire le type (repertoir ou fichier)
            int8_t test = mask&(int8_t) entree[j].attrib;
            char type[7];
            if (test!=0){char dossier[]="Dossier";strcpy(type,dossier);} else{char fich[]="Fichier";strcpy(type,fich);}
			if (entree[j].nomf[0] != 0xE5 && entree[j].attrib != 0xf){//Cas d`une entree disponible
            printf(Yellow("\n\t\t\tFichier trouve!\n\t\t\t\tnom+ext: %s\n\t\t\t\textension: %s\n\t\t\t\ttype: %s\n\t\t\t\ttaille: %u\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
            printf(Yellow("\t\t\t\tPremier Cluster du fichier: %u %u\n"), entree[j].high_numero_premier_cluster_fat,entree[j].low_numero_premier_cluster_fat);
            printf(Yellow("\t\t\t\tPere : / \n"));

            }
            //Cas d`une entree supprimee
			else if (entree[j].nomf[0] == 0xE5 && entree[j].attrib != 0xf)
			printf(Blue("\n\t\t\tElement supprime!\n\t\t\t\tnom+ext: %s\n\t\t\t\textension: %s\n\t\t\t\ttype: %s\n\t\t\t\ttaille: %u\n"), entree[j].nomf, entree[j].extension, type, entree[j].taillef);
        }
	}      
} 
//[7 Afficher_Fdel] Cette fonction affiche
//tous les repertoires et fichiers dans la partions ainsi que les parametres 
void afficher_Fdel(char disque_physique[])
{
    int numcluster = ClusterRacine;
    while(numcluster != -1){
        lire_cluster(disque_physique,numcluster);//Lecture du cluster
        numcluster=Cluster_Suivant(numcluster);//Passage au cluster suivant
    }
}

//{5}Programme principal
int main(int argc, char const *argv[])
{
    printf(Magenta("[1] Fonction Liste_Disques : "));
    printf("Les disques physiques connectes sur votre PC sont :\n");
    printf("\n");
    //Appel de la fonction 1
    Liste_Disques();
    printf(Magenta("[2] Fonction Lire_secteur : "));
    printf("Veuillez entrer le disque_physique: ");
    char disque[10];
    scanf("%s",disque);//Demande du nom du disque physique en entree
    printf("\t\t\t    Veuillez entrer le numéro de secteur: ");
    int numsect;
    scanf("%d",&numsect);//Demande du numero de secteur en entree
    printf("\n");
    printf(Magenta("[3] Fonction Afficher_secteur : "));
    //Appel des fonctions 2 et 3
    afficher_secteur(disque,numsect);
    printf("\n");
    printf(Magenta("[4] Fonction Afficher_Fdel: "));
    printf("Veuillez entrer la partition FAT32: ");
	char disk[10];
	scanf("%s",disk);//Demande du nom de la partition FAT32 en entree
	char chemain[10]="/dev/";
    fichier= fopen(strcat(chemain,disk), "rb");//Ouverture du fichier
    if (fichier == NULL){
        printf("Erreur lors de l`ouverture du fichier !\n");
    }
	else{
    InitParametres();//Initialisation des parametres qu`utilisera Afficher_Fdel
    //Appel de la fonction 4
    afficher_Fdel(disk);
    fclose(fichier);//Fermeture du fichier
    }
    return 0;
}
