#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

/* 
 * tab[0] : inde dans la table des fingers 
 * tab[1] : rank pair
 * tab[2] : id pair correspodant
 * */

#define TAGINIT  0
#define NB_SITES 10
#define ANNONCE  -1
#define M 6

#define min(a,b) ((a > b) ? b : a)
#define min_dist(a,b) (min(abs(a-b), abs(b-a)))

void simulateur(void);
int search(int* array, int length, int valueToFind);
int cmpfunc (const void * a, const void * b);
int app(int k, int a, int b);
int lookup(int* tab, int tmp);
void dht(int rang);

int main(int argc, char** argv)
{
	int nb_proc, rang;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	srand(time(NULL));   // should only be called once

	if (nb_proc != NB_SITES+1) {
		printf("Nombre de processus incorrect !\n");
		MPI_Finalize();
		exit(2);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	if (rang == 0 ){
		simulateur();
	} else {
		dht(rang);
	}

	MPI_Finalize();
	return 0;
}

int search(int* array, int length, int valueToFind)
{
	int pos=0;
	int limit=min(length,1);
	int testpos;

	while(limit < length && array[limit] < valueToFind)
	{
		pos=limit+1;
		limit = min(length, limit*2+1);
	}

	while(pos<limit)
	{
		testpos = pos+((limit-pos)>>1);

		if (array[testpos]<valueToFind)
			pos=testpos+1;
		else
			limit=testpos;
	}
	return (pos < length && array[pos]==valueToFind ? pos : -1);
}

int cmpfunc (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

int app(int k, int a, int b)
{
	if (k >= a && k < b){
		return 1;
	}
	return 0;
}


int lookup(int *tab, int lookfor)
{
	int i=0;
	int k = lookfor;

	if (k < tab[0]){
		/*printf("1) %d Belongs to %d\n", k, tab[0]);*/
		return 0;
	}

	if (k == tab[0]){
		/*printf("2) %d Belongs to %d\n", k, tab[1]);*/
		return 1;
	}
<<<<<<< HEAD

=======
	
>>>>>>> 7c7259229280f7e079df756fd3935556cc817f98
	for (; i<NB_SITES-1 && !(app(k, tab[i], tab[i+1])); i++);

	if (k >= tab[NB_SITES-1]){
		/*printf("3) %d Belongs to %d\n", k, tab[0]);*/
		return 0;
	}

	/*printf("4) %d Belongs to %d\n", k, tab[i+1]);*/
	return i+1;
}

void simulateur(void)
{
	int i, j, k;
<<<<<<< HEAD
	int value, id, res;
=======
>>>>>>> 7c7259229280f7e079df756fd3935556cc817f98
	MPI_Status status;
	int* tab;
	int tmp;
	int* tabtmp;

	tab = malloc(NB_SITES*sizeof(int));

	for (i=0; i<NB_SITES; i++){
		tab[i] = 0;
	}
	/* Trie du tableau et verification sur doublons */
	for (i=0; i<NB_SITES; i++){
		/* Et bien oui si tu depasse 2^M apres les valeur vont faire
		 * n'importe quoi et n'appartiendront pas a notre jolie cercle
		 * ce qui va creer un bug impossible a comprendre
		 * */
		tmp = (rand()) %(1<<M)+ 1;
		qsort(tab, i, sizeof(int), cmpfunc);
		if (search(tab, i, tmp) == -1)
			tab[i] = tmp;
		else 
			i--;
	}
	qsort(tab, NB_SITES, sizeof(int), cmpfunc);

	/* Envoie des Id pour chaque site */
	for (i=0; i<NB_SITES; i++){
		printf("%s : tab[%d] = %d\n", __func__, i, tab[i]);
		MPI_Send(&tab[i], 1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);    
	}

	/* Calcule des DHT et envoie */
	tabtmp = malloc(3*sizeof(int));
	for (i=0; i<NB_SITES; i++){
		for (j=0; j<M; j++){
			tmp = (tab[i] + (1 << j)) % (1 << M);
			/*printf("%d seek for %d\n", tab[i], tmp);*/
			tabtmp[0] = j;
			tabtmp[1] = lookup(tab, tmp);
			tabtmp[2] = tab[tabtmp[1]];
			MPI_Send(tabtmp, 3, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);    
		}
	}
<<<<<<< HEAD


	id = (rand()) % 10+ 1;
	/*id = tab[tmp];*/

	value = (rand()) %(1<<M)+ 1;
	printf("first id to receive %d:%d will search :%d\n", id, tab[id], value);
	/*printf("lookfor %d\n", value);*/

	/* send request */
	MPI_Send(&value, 1, MPI_INT, id, TAGINIT, MPI_COMM_WORLD);    

	/* accept result */
	MPI_Recv(&res, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	printf("%d ---> %d\n", value, res);

	/* finish */
	/* broadcast the finish*/
	value = 0;
	for (i=0; i<NB_SITES; i++){
		MPI_Send(&value, 1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);    
	}

=======
>>>>>>> 7c7259229280f7e079df756fd3935556cc817f98
	free(tabtmp);
}

void dht(int rang)
{
	int i, k;
<<<<<<< HEAD
	int tmp, dad;
	int id, tmpid;
=======
	int id;
>>>>>>> 7c7259229280f7e079df756fd3935556cc817f98
	MPI_Status status;
	int **finger_table;

	finger_table = malloc(M*sizeof(int*));
	for (i=0; i<M; i++){
		finger_table[i] = malloc(3*sizeof(int));
	}

	MPI_Recv(&id, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

<<<<<<< HEAD
	for (i=0; i<M; i++){
		MPI_Recv(finger_table[i], 3, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
		/*printf("me:(%d|%d)		id=%d mpirank=%d pair=%d\n",*/
				/*rang, id,*/
				/*finger_table[i][0],*/
				/*finger_table[i][1],*/
				/*finger_table[i][2]);*/
	}

	/* Traitement requete */
	MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	dad = status.MPI_SOURCE;

	/* Recherche ? ou jump finish?*/
	if (dad == 0){
		/* finish? */
		if (tmp == 0){
			printf("daddy killed me\n");
			return;
		}
		/* Recherche init */ 
		printf("%d start search\n", id);
		if (id >= tmp && finger_table[0][2] <= tmp){
			//found
			MPI_Send(0, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD);    
		}
		for (i=1; i<M-1; i++){
			printf("looking in table %d>%d? \n", tmp, finger_table[M-i][2]);
			if (finger_table[M-i][2] < tmp){
				printf("%d:sending %d at %d\n",i, tmp, finger_table[M-i][2]);
				MPI_Send(&tmp, 1, MPI_INT, finger_table[M-i][1], TAGINIT, MPI_COMM_WORLD);    
				break;
			}
		}
	} else {
		if (id >= tmp && finger_table[0][2] >= tmp){
			//found
			MPI_Send(&finger_table[0][2], 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD);    
		}
		
		for (i=1; i<M-1; i++){
			printf("looking in table %d>%d? \n", tmp, finger_table[M-i][2]);
			if (finger_table[M-i][2] < tmp){
				printf("sending %d at %d\n",tmp, finger_table[M-i][2]);
				MPI_Send(&tmp, 1, MPI_INT, finger_table[M-i][1], TAGINIT, MPI_COMM_WORLD);    
				break;
			}
		}
	}

	/* finish */
	MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	if (tmp == 0){
		printf("daddy killed me\n");
		return;
	}
=======
	for (i=0; i<3; i++){
		MPI_Recv(finger_table[i], 3, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
		printf("me:(%d|%d)		id=%d mpirank=%d pair=%d\n",
				rang, id,
				finger_table[i][0],
				finger_table[i][1],
				finger_table[i][2]);
	}

	/* Traitement */

	return;
>>>>>>> 7c7259229280f7e079df756fd3935556cc817f98
}
