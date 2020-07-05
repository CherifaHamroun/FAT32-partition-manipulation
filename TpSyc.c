#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 


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
        else {printf("\n Lecture du secteur %d , Nombre d’éléments lus =%d\n",numSecteur,n); 
  		}
	}
   	fclose(disk);
    }
}


void afficher_secteur(char disque_physique[] , int numSecteur){

unsigned char buffer[512];
lire_secteur(disque_physique,numSecteur,&buffer);
for(int j=0; j<512; j++)
 if(j % 16 != 15){  printf("%02x ",buffer[j]);}
 else{
	 printf("%02x ",buffer[j]);
	 printf("\n");
 }
}


void main() 
{ 
			
printf("Veuillez entrer le disque_physique: ");
char *disque;
scanf("%s",disque);
printf("Veuillez entrer le numéro de secteur: ");
int numsect;
scanf("%d",&numsect);

afficher_secteur(disque,numsect);

} 