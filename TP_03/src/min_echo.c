/* Tribute to Ilyas */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>
#include <time.h>

#define NB_SITE 6
#define TAGINIT 0

#define min(a,b) ((a > b) ? b : a)

void simulateur(void);
void calcul_min(int rang);

int main (int argc, char* argv[])
{
	int nb_proc, rang;

	srand(time(NULL));
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	if (nb_proc != NB_SITE+1) {
		printf("Nombre de processus incorrect !\n");
		MPI_Finalize();
		exit(2);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	if (rang == 0) {
		simulateur();
	} else {
		calcul_min(rang);
	}

	MPI_Finalize();
	return 0;
}

void simulateur(void)
{
	int i;
	MPI_Status status;
	int max_int = INT_MAX;

	/* nb_voisins[i] est le nombre de voisins du site i */
	int nb_voisins[NB_SITE+1] = {-1, 3, 3, 2, 3, 5, 2};
	int min_local[NB_SITE+1] = {-1, 12, 11, 8, 14, 5, 17};

	/* liste des voisins */
	int voisins[NB_SITE+1][5] = {{-1, -1, -1, -1, -1},
		{2, 5, 3, -1, -1}, {4, 1, 5, -1, -1}, 
		{1, 5, -1, -1, -1}, {6, 2, 5, -1, -1},
		{1, 2, 6, 4, 3}, {4, 5, -1, -1, -1}};

	for (i=1; i<=NB_SITE; i++){
		MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
	}

	/* l'initiateur sera choisi aléatoirement : */
	/* On lui passe max int */
	i = (rand() % NB_SITE) + 1;
	MPI_Send(&max_int, 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);

	/* attente et affichage du résultat final */
	MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
	printf("%s : min = %d\n",__func__, i);
}

void calcul_min(int rang)
{
	int nb_voisins;
	int *voisins; 	// [nb_voisins];
	int min_local;
	int *received; 	// [nb_voisins];
	int i, j;
	int buf; /* Buffer qui transporte la valeur du min */
	MPI_Status status;
	int dad = -1; /* rang du père */
	int rec = 1;

	/* reception du nombre de voisins */
	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	voisins = (int*)(malloc(nb_voisins*sizeof(int)));
	received = (int*)(malloc(nb_voisins*sizeof(int)));

	/* reception des rangs des voisins */
	MPI_Recv(voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	/* reception du min_local */
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	/* attente du message du père */
	MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
	dad = status.MPI_SOURCE;
	printf("%s : daddy = %d\n",__func__,dad);
	printf("local_min=%d, father_min=%d\n", min_local, buf);
	min_local = min(min_local, buf);


	// God's message
	if (dad == 0){
		/* Chosen one */
		/* echo aux autre voisins */
		for (i=0; i<nb_voisins; i++){
			MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGINIT, MPI_COMM_WORLD);
		}

		/* attente des retours */
		for (i=0; i<nb_voisins; i++){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
			received[i] = 1;
			min_local = min(min_local, buf);
		}

		for (i=0; i<nb_voisins; i++){
			if (received[i] == 1){
				rec &= 1;
			}
		}

		if (rec)
			MPI_Send(&min_local, 1, MPI_INT, dad, TAGINIT, MPI_COMM_WORLD);
	} else { 
		/* Not the chosen one */
		received[i] = 1;
		for (i=0; i<nb_voisins; i++){
			if ( voisins[i] != dad ){
				MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGINIT, MPI_COMM_WORLD);
			}
		}

		for (i=0; i<nb_voisins-1; i++){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
			received[i] = 1;
			min_local = min(min_local, buf);
		}

		printf("%s : min=%d\n", __func__, min_local);
		MPI_Send(&min_local, 1, MPI_INT, dad, TAGINIT, MPI_COMM_WORLD);
	}

	free(received);
	free(voisins);

}
