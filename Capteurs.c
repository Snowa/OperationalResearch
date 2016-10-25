#include "glpk.h"
#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
//#include <algorithm> // pour la fonction swap

//using namespace std;

const char *intlpfilename="capteurs.lp";
const int Nbcapteurs=5;
const int Zone=5;
//gcc -pthread Capteurs.c -o Capteurs -Wall
// glpsol -cpxlp capteurs.lp - o solution_capteurs

/*
	Structure contenant un tableau et un pointeur sur une autre strecture?
*/
typedef struct config_el config_el;  
struct config_el
{
    int val[Zone];
    struct config_el *nxt;
};
 
typedef config_el* llist;
/*
	Parcours de la liste des configurations élémentaires
*/
void afficherListe(llist liste)
{
	int i=0;
	fprintf(stderr, "Liste des configurations élémentaires :\n" );
    config_el *tmp = liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        /* On affiche */
        for (i = 0; i < Zone; ++i)
        {
        	printf("%i", tmp->val[i]);
        }
        /* On avance d'une case */
        tmp = tmp->nxt;
        if(i>1)
        {
	        printf("\n");
	   		fflush(stdout);
	   	}
    }
    fprintf(stderr, "\n");
}

llist ajouterEnFin(llist liste, int valeur[5])
{
    /* On crée un nouvel élément */
    config_el* nouvelElement =(config_el*) malloc(sizeof(config_el));
    /* On assigne la valeur au nouvel élément */
    int i;
    for (i = 0; i < Zone; i++)
    {
    	nouvelElement->val[i] = valeur[i];
    }
    /* On ajoute en fin, donc aucun élément ne va suivre */
    nouvelElement->nxt = NULL;
    if(liste == NULL)
    {
        /* Si la liste est videé il suffit de renvoyer l'élément créé */
        return nouvelElement;
    }
    else
    {
        /* Sinon, on parcourt la liste à l'aide d'un pointeur temporaire et on
        indique que le dernier élément de la liste est relié au nouvel élément */
        config_el* temp=liste;
        while(temp->nxt != NULL)
        {
            temp = temp->nxt;
        }
        temp->nxt = nouvelElement;
        return liste;
    }
}

int Random (int _iMin, int _iMax) 
{ 
	return (_iMin + (rand () % (_iMax-_iMin+1))); // Tire un random entre _iMin et _iMax
} 

//Test si toutes les zones sont couvertes
bool test_toute_zone_couverte(int tableau[Zone])
{
	int i;
	for ( i = 0; i < Zone; i++)
	{
		if (tableau[i]==0)
		{
			return false;
		}
	}
	return true;
}

//Test si un capteur surveille plusieurs zones.
bool test_occurence_capteurs(int num_capteurs,int Tab_config_el[Zone])
{
	int z;
	//fprintf(stderr, "num du capteurs %d\n",num_capteurs );
	for (z = 0; z < Zone; z++)
	{
		//fprintf(stderr, "%d\n",Tab_config_el[z] );
		if (num_capteurs==Tab_config_el[z])
			return true;
	}
	return false;
}

/*
test si il existe déjà une combinaison élémentaire (Tab_config) 
dans la liste des configurations élémentaires(liste)
revoie false quand ce n'est pas les même élement dans le tableau
renvoie true quand ce sont les même élément dans le tableau
*/
bool comparaison_liste_tab_config(llist liste, int Tab_config_el[Zone])
{
	int i;
	int t;
	bool actuel=false;
	bool precedent=false;
    config_el *tmp = liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        /* On affiche */
        i=0;
        for (i = 0; i < Zone; ++i)
        {
        	t=0;
        	for ( t = 0; t < Zone; t++)
        	{
        		actuel=false;
        		if(tmp->val[i]==Tab_config_el[t])
        		{
        			//printf("true %i %i - %i - %i \n",i,t,tmp->val[i],Tab_config_el[t]);
        			actuel=true;
        			//t=Zone+1;
        		}
        	}
        	if (actuel==false)
        		return false;
        }
        return true;
        /* On avance d'une case */
        tmp = tmp->nxt;
    }
}

int nombreElements(llist liste)
{
    /* Si la liste est vide, il y a 0 élément */
    if(liste == NULL)
        return 0;
 
    /* Sinon, il y a un élément (celui que l'on est en train de traiter)
    plus le nombre d'éléments contenus dans le reste de la liste */
    return nombreElements(liste->nxt)+1;
}
llist liste_config_el(int tableau[Zone+1][Nbcapteurs],llist ma_liste)
{
	int i;
	int m;
	int Nbconfig = 0;

	int Zone_couverte[Zone];
	int Tab_config_el[Zone];
	for ( i = 0; i < Zone; i++)
	{
		Zone_couverte[i]=0;
		Tab_config_el[i]=0;
	}
	int It=0;
	int l;
	srand(time(NULL)); 
	while(nombreElements(ma_liste)!=4)
	{
		int It=0;
	while (true)
	{
		l=Random(0, Nbcapteurs-1);
		//fprintf(stderr, "%d\n",l );
		m=0
		for ( m = 0; m < Zone ; m++)
		{
			fprintf(stderr, "%d",tableau[m][l] );
			if (tableau[m][l]==1 and Zone_couverte[m]!=1)
			{
				if (!test_occurence_capteurs(l+1,Tab_config_el))
				{
					Tab_config_el[It]=l+1;
					Zone_couverte[m]=1;
					It++;
				}
				Zone_couverte[m]=1;
			}
		}
		fprintf(stderr, "- Capteurs N°%d\n Zone couverte : ",l+1);
		i=0;
		for ( i = 0; i < Zone; ++i)
		{
			fprintf(stderr, "%d",Zone_couverte[i] );
		}
		fprintf(stderr, "\n");

		if (test_toute_zone_couverte(Zone_couverte)==true)
		{
			if (!comparaison_liste_tab_config(ma_liste, Tab_config_el))
			{
				ma_liste = ajouterEnFin(ma_liste, Tab_config_el);
				fprintf(stderr, "\tajouté\n" );
			}
			//else fprintf(stderr, "\nNON");
			i=0;
			for (i = 0; i < Zone; i++)
			{
				Zone_couverte[i]=0;
				Tab_config_el[i]=0;
			}
			break;
		}
	}
	}		
	fprintf(stderr, "Il y a %d configuration élémentaire.\n",nombreElements(ma_liste));
	return ma_liste;
}
int nombre_membre_equation_config_el(int num_capteurs, llist liste)
{
	int nb=0;
	int i=0;
    config_el *tmp = liste;
    while(tmp != NULL)
    {
    	i=0;
        for ( i = 0; i < Zone; ++i)
        {
        	if(tmp->val[i]==num_capteurs)
        		nb++;
       	}
        tmp = tmp->nxt;
    }
    fprintf(stderr, "%d\n",nb );
	return nb;
}

void construction_equation(FILE *ofile, int num_capteurs, llist liste,int tableau[Zone+1][Nbcapteurs],int Nb_total)
{
	int nb_membre_total=nombre_membre_equation_config_el(num_capteurs,liste);
	int nb_membre_courant=0;
	int n_config_el_courante=0;
    config_el *tmp = liste;
    int i;
    while(tmp != NULL)
    {
    	n_config_el_courante++;
        /* On affiche */
        for (i = 0; i < Zone; ++i)
        {
        	if(tmp->val[i]==num_capteurs)
        	{
        		fprintf(ofile, "t_%i", n_config_el_courante);
        		nb_membre_courant++;
        		if(nb_membre_total!=nb_membre_courant)
        		fprintf(ofile,"+");
        	}

       	}
        /* On avance d'une case */
        tmp = tmp->nxt;
	}
	//system("bash -c 'head -n -1 capteurs.lp > capteurs.lp'")
	fprintf(ofile, "<=%d\n",tableau[Zone][num_capteurs-1] );
}
void resoudre(int Nbconfig)
{
	/*
	LPX *lp;
	lp=lpx_create_prob();
	lpx_set_prob_name(lp,"PL");
	lp=lpx_read_cpxlp(intlpfilename);
	lp=lpx_simplex(lp);
	for (int i = 0; i < Nbconfig; ++i)
	{
		Valprimal[i]=lpx_get_col_prim(lp,i+1);
	}
	lpx_delete_prob(lp);
	*/
}
int main(int argc, char const *argv[])
{
	llist ma_liste = NULL;
	int Nbconfig;
	int tableau[Zone+1][Nbcapteurs]={
		{1,0,0,1,0}, //Zone 1
		{0,1,0,0,1}, //Zone 2
		{0,0,1,0,0}, //Zone 3
		{1,1,1,0,0}, //Zone 4
		{0,0,0,0,1}, //Zone 5
		{4,6,3,1,7}  //Temps 
	};
	int tab[Zone]={1,2,3,4,5};
	//test_occurence_capteurs(1,tab);
	ma_liste=liste_config_el(tableau,ma_liste);
	afficherListe(ma_liste);

	Nbconfig=nombreElements(ma_liste);
	FILE *ofile;
	if ((ofile=fopen(intlpfilename,"w"))==NULL)
	{
		cerr<<"erreur! nous ne nous ne pouvons pas creer"<<intlpfilename<<endl;
		return false;
	}
	fprintf(ofile, "Maximize ");
	for (int i = 0; i < Nbconfig; i++)
	{
		fprintf(ofile, "t_%i",i+1);
		if (i < Nbconfig-1)
			fprintf(ofile,"+");
	}
	fprintf(ofile, "\nSubject To\n\n");
	int i;
	for ( i = 1; i <= Nbcapteurs; i++)
	{
		 construction_equation(ofile,i,ma_liste,tableau,Nbconfig);
	}
	fprintf(ofile, "\nEND\n");
	fclose(ofile);

	//resoudre(Nbconfig);

	return 0;
}