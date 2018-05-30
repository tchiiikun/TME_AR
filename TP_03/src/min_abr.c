/* Inspiration Ilyas Toumlilt */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#define TAGINIT  0
#define NB_SITES 6
#define ANNONCE  -1

#define min(a,b) ((a > b) ? b : a)

void simulateur(void);
void calcul_min(int rang);

int main(int argc, char** argv)
{
	int nb_proc, rang;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	if (nb_proc != NB_SITES+1) {
		printf("Nombre de processus incorrect !\n");
		MPI_Finalize();
		exit(2);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	if (rang == 0 ){
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

	/* nb_voisins[i] est le nombre de voisins du site i */
	int nb_voisins[NB_SITES+1] = {-1, 2, 3, 2, 1, 1, 1};

	/* Valeur local a chaque node */
	int min_local[NB_SITES+1] = {-1, 3, 11, 8, 14, 5, 17};

	/* liste des voisins */
	/* pere, filsG, filsD */
	int voisins[NB_SITES+1][3] = {{-1, -1, -1},
		{2, 3, -1}, {1, 4, 5}, 
		{1, 6, -1}, {2, -1, -1},
		{2, -1, -1}, {3, -1, -1}};

	for (i=1; i <= NB_SITES; i++){

		/* envoie du nombre de voisins */
		MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		/* envoie des rangs des voisins */
		MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
		/* envoie du min_local */
		MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
	}

	/* attente de décision et affichage du résultat */
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

	/* reception du nombre de voisins */
	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	voisins = (int*)(malloc(nb_voisins*sizeof(int)));
	received = (int*)(malloc(nb_voisins*sizeof(int)));

	/* reception des rangs des voisins */
	MPI_Recv(voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	/* reception du min_local */
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	/* tableau des flags de reception */
	for (i=0; i<nb_voisins; i++)
		received[i] = 0;

	/*** Partie calcul ***/
	/* Les feuilles sont initiatrices (1 seul voisin) */
	if (nb_voisins == 1 ){
		MPI_Send(&min_local, 1, MPI_INT, voisins[0], TAGINIT, MPI_COMM_WORLD);
	} else {
		/* attente de nb_voisins-1 messages */
		for (i=0; i<nb_voisins-1; i++){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
			for (j=0; j<nb_voisins; j++){
				/* acces au rank de la derniere reception pour
				 * verifier si c'est le voisin[j], alors on le
				 * marque comme recu */
				if (voisins[j] == status.MPI_SOURCE){
					received[j] = 1;
					break;
				}
			}
			min_local = min(min_local, buf);
		}
		/* Envoie du min_local au voisin restant */
		for (i=0; i<nb_voisins; i++)
			if (!received[i]){
				MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGINIT, MPI_COMM_WORLD);
			}
	}

	/* Merci Ilyas */
	/* attente du dernier message, ce message peut être : 
	   un min_local, ce qui voudrait dire que je suis décideur.
	   une annonce ( -1 ), sinon */
	MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
	if (buf != ANNONCE){
		/* prise de décision */
		min_local = min(min_local, buf);
		MPI_Send(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD);
	}
	buf = ANNONCE;
	for (i=0; i<nb_voisins; i++)
		if (received[i]){
			MPI_Send(&buf, 1, MPI_INT, voisins[i], TAGINIT, MPI_COMM_WORLD);
		}
	free(received);
	free(voisins);
}
