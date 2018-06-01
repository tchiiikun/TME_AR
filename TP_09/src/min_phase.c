/* thx to Ilyas */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>
#include <unistd.h>

#define TAGINIT 0
#define NB_SITE 6

/* TODO change if needed */
#define DIAMETRE 4

#define min(a,b) ((a > b) ? b : a)

void simulateur(void);
void calcul_min(int rang);

int main (int argc, char* argv[]) 
{
	int nb_proc,rang;
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
	int buf = INT_MAX;
	int p_elu = ((rand()%NB_SITE)+1);

	/* nb_voisins_in[i] est le nombre de voisins entrants du site i */
	int nb_voisins_in[NB_SITE+1] = {-1, 2, 1, 1, 2, 1, 1};

	/* nb_voisins_out[i] est le nombre de voisins sortants du site i */
	int nb_voisins_out[NB_SITE+1] = {-1, 2, 1, 1, 1, 2, 1};

	int min_local[NB_SITE+1] = {-1, 4, 7, 1, 6, 2, 9};

	/* liste des voisins entrants */
	int voisins_in[NB_SITE+1][2] = {{-1, -1},
		{4, 5}, {1, -1}, {1, -1},
		{3, 5}, {6, -1}, {2, -1}};

	/* liste des voisins sortants */
	int voisins_out[NB_SITE+1][2] = {{-1, -1},
		{2, 3}, {6, -1}, {4, -1},
		{1, -1}, {1, 4}, {5,-1}};

	for (i=1; i<=NB_SITE; i++){
		MPI_Send(&nb_voisins_in[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(&nb_voisins_out[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(voisins_in[i], nb_voisins_in[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(voisins_out[i], nb_voisins_out[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
	}

	/* choix aléatoire de l'initiateur */
	srand(getpid());
	MPI_Send(&buf, 1, MPI_INT, p_elu, TAGINIT, MPI_COMM_WORLD);
	printf("Process 0 a choisi %d comme initiateur\n", p_elu);

	/* attente des décisions */
	MPI_Status status;
	for (i=0; i<NB_SITE; i++){
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("Process %d returned min=%d\n", status.MPI_SOURCE, buf);
	}
}

void calcul_min(int rang)
{
	int nb_voisins_in;
	int nb_voisins_out;
	MPI_Status status;

	int *voisins_in;
	int *voisins_out;
	int *received;

	int min_local;
	int buf;       /* les communications se feront par int */
	int phase = 0; /* phase counter */
	int i;

	MPI_Recv(&nb_voisins_in, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&nb_voisins_out, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	voisins_in = (int*)(malloc(nb_voisins_in*sizeof(int)));
	voisins_out = (int*)(malloc(nb_voisins_out*sizeof(int)));
	received = (int*)(malloc(nb_voisins_in*sizeof(int)));

	for (i=0; i<nb_voisins_in; i++){
		voisins_in[i] = 0;
		received[i] = 0;
	}
	for (i=0; i<nb_voisins_out; i++){
		voisins_out[i] = 0;
	}

	MPI_Recv(voisins_in, nb_voisins_in, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(voisins_out, nb_voisins_out, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);


	printf("%s : min_local =%d\n", __func__, min_local);
	for (i=0; i<nb_voisins_in; i++)
		received[i]=0;

	while(1){
		/* RECP */
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
		min_local = min(min_local, buf);
		for (i=0; i<nb_voisins_in; i++){
			if (voisins_in[i] == status.MPI_SOURCE){
				received[i]++;
				break;
			}
		}

		/** SEND **/
		i = 0;
		if (phase < DIAMETRE){ 
			while (i < nb_voisins_in && received[i] >= phase){
				i++;
			}
		}
		if (i == nb_voisins_in){
			for (i=0; i<nb_voisins_out; i++){
				MPI_Send(&min_local, 1, MPI_INT, voisins_out[i], TAGINIT, MPI_COMM_WORLD);
			}
			phase++;
		}

		/* DECIDE  */
		i=0;
		if (phase >= DIAMETRE){
			while (i < nb_voisins_in && received[i] >= DIAMETRE){
				i++;
			}
		}
		if ( i == nb_voisins_in )
			break;
	}

	/* tout le monde décide */
	MPI_Send(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD);
	free(received);
	free(voisins_in);
	free(voisins_out);
}
