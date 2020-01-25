/************************************************************/
/*  Projet de Programmation Partie II                         */
/*  Réalisé par : Mohamed-Amine Bousahih                      */
/*  Numéro etd  : 21500267                                    */
/*  Université Paris Diderot                                  */
/*  Master 1 Mathématiques et Applications                    */
/*  Subject : Algorithme de Dijkstra - Plus court chemin.     */
/**************************************************************/

/**************************** Librairies ****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/**************************** Structures de données ****************************/

typedef struct node // structure représentent un noeud.
{
	int value; // valeur/étiquette du noeud.
	int poids; // son poids vers son noeud de destination.
	int dest; // son noeud de destination.
}node;

struct ELT // structure représentant les éléments de notre liste d'adjacence.
{
	node noeud; // un noeud.
	struct ELT* next; // pointeur de type ELT qui pointe vers ELT suivant;
};
typedef struct ELT ELT;

typedef struct // structure représentant la liste d'adjacence.
{
	ELT* tete; // un pointeur tete de type ELT afin d'accèder à la liste d'adjacence.
}liste_adjacence;

typedef struct // structure représentant notre graphe.
{
	int n; // n représentant le nombre d'éléments de notre tableau, i.e le nombre de noeuds.
	liste_adjacence* tab_adj; // tableau de liste d'adjacence.
}Graphe;

typedef struct Heap // structure rassemblant toutes les informations liées au tas.
{
	int capacity; // capacité du tableau, sa taille réelle comprenant les cases non utilisées.
	int size; // nombre d'éléments valides , utilisés dans le tas.
	int *F_posi; // un pointeur sur le tableau des positions du tas.
	int *F; // un pointeur sur le tableau de données. Le tas.
}Heap;


/****************************  Remplissage du Graphe / ajout des ELT en tête de liste ****************************/

void ajout(Graphe *G, node new) // ajout en tête de liste.
{
	ELT *link = malloc(sizeof(ELT));
	if(link==NULL)
	{
		printf("Erreur allocation.\n");
		exit(3); // allocation mal déroulée.
	}
	link->noeud = new;
	link->next = G->tab_adj[new.value].tete;

	G->tab_adj[new.value].tete = link;
}


/****************************  Affichage des tableaux des distances, des predecesseurs , positions du Tas et du Tas.  ****************************/

void affiche(int dist[], int pred[], Heap heap) 
{
	int n = heap.capacity; //nombre de noeuds

	for (int i=0; i<n; i++)
	{
		printf("%d | ", i);
		if (dist[i]==INT_MAX)	printf("+inf");
		else					printf("%4d", dist[i]);
		printf(" .. %2d .. %2d ....... %2d \n", pred[i],heap.F_posi[i],heap.F[i]);
	}
	printf("%d noeuds dans le tas\n", heap.size);
}


/****************************  Primitives sur la gestion de tas  *******************************/

Heap createHeap(int n) // Création et initialisation des éléments de la structure Heap.
{
	Heap heap;
	heap.capacity = n;
	heap.size = n; // la taille du tas coincident à celle de la capacité maximale d'après l'énoncé.

	heap.F_posi = malloc(n*sizeof(int)); // allocation en mémoire du tableau des positions des éléments du tas.
	heap.F = malloc(n*sizeof(int)); // allocation en mémoire du tas.

	if(heap.F_posi==NULL || heap.F==NULL)
	{
		printf("Erreur allocation.\n");
		exit(3);
	}
	return heap;
}

// Remarque : L'indice i de mon tas commence à 0.
int leftchild(int i) // renvoie l'indice du fils gauche d'un noeud en position i.
{
	return 2*i+1;
}

int rightchild(int i) // renvoie l'indice du fils droit d'un noeud en position i.
{
	return 2*(i+1);
}

void minHeapify(Heap *heap, int i, int dist[]) // Heapify en min-tas
{
	int cas=0; // cas est utilisé pour savoir s'il faut permuter.
	int temp;
	int l = leftchild(i);
	int r = rightchild(i);
	while(1){

		// 2 fils et min est le fils droit
		if((r < heap->size) && (dist[heap->F[l]] > dist[heap->F[r]]) && (dist[heap->F[i]] > dist[heap->F[r]]))
		{cas=2;break;}

		// 2 fils et min est le fils gauche
		if((r < heap->size) && (dist[heap->F[l]] < dist[heap->F[r]]) && (dist[heap->F[i]] > dist[heap->F[l]]))
			{cas=1;break;}

		// 1 fils et min est le fils gauche
		if((l==heap->size) && (dist[heap->F[i]] > dist[heap->F[l]]))
			{cas=1;break;}

		break;
	}
	switch(cas)
	{
		case 0 : return;
		case 1 : 
		{
			temp = heap->F[l]; heap->F[l] = heap->F[i]; heap->F[i] = temp;
			temp = heap->F_posi[heap->F[l]]; heap->F_posi[heap->F[l]] = heap->F_posi[heap->F[i]]; heap->F_posi[heap->F[i]] = temp;
			minHeapify(heap,l,dist); return;
		}
		case 2:  
		{
			temp = heap->F[r]; heap->F[r] = heap->F[i]; heap->F[i] = temp; 
			temp = heap->F_posi[heap->F[r]]; heap->F_posi[heap->F[r]] = heap->F_posi[heap->F[i]]; heap->F_posi[heap->F[i]] = temp;
			minHeapify(heap,r,dist); return;
		}
	}
}

void heapExtractMin(Heap *heap, int dist[]) // Extraction du min - racine du tas puis mise en min-tas.
{
	
	int root = heap->F[0]; // stockage du noeud racine de départ.
	heap->F[0] = heap->F[heap->size-1]; // le dernier noeud devient la racine.
	heap->F_posi[heap->F[heap->size-1]] = 0; // mise à jour de la position de la nouvelle racine.
	heap->F_posi[root] = -1; // on indique que notre racine de départ est sortie du tas en mettant à jour sa position à -1.
	heap->F[heap->size-1] = -1; // on indique la sortie de notre racine du tas.

	heap->size = heap->size-1; // on décrémente la taille de notre tas.

	if(heap->size>=2)
		minHeapify(heap,0,dist); // mise en min-tas à partir du nouveau noeud en tête de tas.
} 

int PARENT(int i) // renvoie l'indice du parent d'un noeud en position i.
{
	return (int) i-1/2;
}

void heapDecreaseKey(Heap *heap, int dist[], int i) // Remonte le noeud en position i à la position i-1/2 (position du noeud parent).
{
	//Comparaison de la distance d'un noeud en position i avec son noeud parent en position i-1/2.
	while(i>0 && dist[heap->F[PARENT(i)]]>dist[heap->F[i]])
	{
		// échanger le noeud position i avec son parent.

		int tmp = heap->F[i];
		heap->F[i] = heap->F[PARENT(i)];
		heap->F[PARENT(i)] = tmp;

		//mise à jour des positions.

		tmp = heap->F_posi[heap->F[i]];
		heap->F_posi[heap->F[i]] = heap->F_posi[heap->F[PARENT(i)]];
		heap->F_posi[heap->F[PARENT(i)]] = tmp;

		i = PARENT(i); //mise à jour de l'indice i. 
	}
}

/**************************** Initialisation de nos tableaux & du tas pour l'algorithme de Dijkstra - PCC ****************************/

void initialize_Dijkstra(int s, int dist[], int pred[], Heap* heap) // initialisation des tableaux et du tas.
{
	int n = heap->capacity; // récupération du nombre de noeuds.
	int i = 0;
	for(int j = 0; j<n; j++)
	{
		dist[j] = INT_MAX; // tableau des poids à +inf.
		pred[j] = -1; // tableau des prédecesseurs à -1.

		// initialisation du tas et du tableau des positions du tas en fonction du noeud racine entré par l'utilisateur.
		if(j==0)
		{
			heap->F[j] = s;
			heap->F_posi[s] = j;
		}
		if(j!=0)
		{
			heap->F[j] = i;
			heap->F_posi[i] = j;
			i++;
		}
		if(j==s)
		{
			i++;
		}
	}
	dist[s] = 0; // distance du noeud d'origine à 0.
}


												/******************************** MAIN  ********************************/


	/*******  Importation des fichiers et utilisation des données. *********/

int main(int argc, char*argv[])
{

	if(argc<=1) // vérifier qu'un nom de fichier a été donné.
	{
		printf("Nom de fichier manquant.\n");
		exit(1); // sortir du programme le cas échant.
	}
	FILE* pf = fopen(argv[1],"rt");
	if(pf==NULL) // vérification de existence du fichier.
	{
		printf("Erreur dans l'ouverture du fichier. \n");
		exit(2); // forcer la sortie.
	}

	int numberOfNodes, val_node, to, poids;
	fscanf(pf,"%d",&numberOfNodes);
	printf("Il y a %d noeuds.\n",numberOfNodes);


	/*********  Initialisation du graphe. ************/

	Graphe G;
	G.n = numberOfNodes;
	G.tab_adj = (liste_adjacence *)malloc(numberOfNodes*sizeof(liste_adjacence));
	for(int i = 0; i<numberOfNodes; i++)
	{
		G.tab_adj[i].tete = NULL;
	}
	while(!feof(pf))
	{
		node p;
		fscanf(pf, " %d %d %d", &val_node,&to,&poids);
		//printf("%d %d %d\n",val_node,to,poids);
		p.dest = to;
		p.poids = poids;
		p.value = val_node;
		ajout(&G,p);
	}

	/*********** Affichage de la liste d'adjacence ********/

	/* for(int i=0; i<numberOfNodes; i++)
	{
		printf("LISTE %d\n", i);
		for(ELT* curr=G.tab_adj[i].tete; curr!=NULL; curr=curr->next)
		{
			printf("\tnoeud de destination : %d distance a %d : %d\n",curr->noeud.dest,curr->noeud.value,curr->noeud.poids);
		}
	} */

	fclose(pf); // fermeture du fichier


	/***************************		 Algorithme de Dijkstra - PCC 			********************************/


	printf("\n      				*** Algorithme de Dijkstra - PCC  *** \n");
	printf("\n");

	int d[numberOfNodes]; // tableau des poids/distances.
	int p[numberOfNodes]; // tableau des prédecesseurs.

	Heap heap = createHeap(numberOfNodes); // création et initialisation des éléments de la structure Heap.

	int nodeOrigin; // noeud origine entré par utilisateur. 
	printf("Entrez le noeud d'origine: \n");
	scanf("%d",&nodeOrigin);

	// Gestion des cas où la valeur du noeud d'origine inférieur à 0, supérieur au nombre de noeuds.
	// Gestion du cas où le noeud d'origine est un noeud puit. 
	if(nodeOrigin<0 || nodeOrigin>=numberOfNodes ||G.tab_adj[nodeOrigin].tete==NULL)
		return -1;

	printf("\n 	*** Etape 1 : initialisation du tableau des poids, des predecesseurs, position tas et du tas. ***	\n");
	printf("\n");

	initialize_Dijkstra(nodeOrigin,d,p,&heap);
	affiche(d,p,heap);

	printf("\n 	*** Etape 2 : Calculs et mise a jour des tableaux. *** \n");
	printf("\n");

	while (heap.size!=0) // tant que le tas est non vide.
	{
		int u = heap.F[0]; // premier élément du tas, le min.
		heapExtractMin(&heap,d); // extraire le min du tas puis mise en min-tas.

		ELT* tmp = G.tab_adj[u].tete;
		while (tmp!=NULL) // parcourt de la liste d'adjacence.
		{
			int v = tmp->noeud.dest;
			int w = tmp->noeud.poids;
            int temp = d[u] + w; 
            if(d[u] == INT_MAX){temp = INT_MAX;} // cas où d[u] = +inf, en ajoutant le poids w on a d[v] = -oo. On force temp à prend INT_MAX comme valeur dans ce cas. 
			if(d[v] > temp)
			{
				d[v] = temp;
				heapDecreaseKey(&heap,d,v);
				p[v] = u;
			}
			tmp = tmp->next;
		}
		affiche(d,p,heap);
	}

	printf("\n 	*** Etape 3 : Plus courts chemins a partir de %d et leurs distances. *** \n",nodeOrigin);
	printf("\n");

	for (int i=0; i<numberOfNodes; i++)
	{
		printf("Chemin origine %d vers %d. Distance  = %d . ", nodeOrigin,i, d[i]);
		printf("Chemin : %d ", i);
		int pred = p[i]; // parcourt et affichage du tableau des prédecesseurs. 
		while (pred != -1)
		{
			printf(" <-- %d", pred);
			pred = p[pred];

		}
		printf("\n");
	}


	return 0;
}

