#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* table max index */
#define M 10
/* finger table max index */
#define N 6
/* minimum entre a et b */
#define min(a,b) ((a > b) ? b : a)
/* distance entre x et y */
#define dist(x,y) (min((x-y)%N,(y-x)%N))
/* renvoie si k E [a;b[ */
// #define inf(x,y) (abs((y - x)%N) <= N / 2)
#define inf(x,y) (0 < ((y - x)%N) && ((y - x)%N) <= N / 2)

int app(int k, int a, int b)
{
	/*if (a < b){*/
		/*[>printf("%d<%d<%d\n", a, k, b);<]*/
		/*printf("%d<%d=%d %d<%d=%d\n", a, k, inf(a, k), k, b, inf(k, b));*/
		/*return (inf(a, k) && inf(k, b));*/
	/*} else {*/
		/*printf("%d<%d<%d\n", b, k, a);*/
		/*return (inf(k, a) || inf(b, k));*/
	/*}*/

	if (k >= a && k < b){
		printf("%d<%d=%d %d<%d=%d\n", a, k, a<k, k, b, b>k);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	int k = atoi(argv[1]);
	int tab[M] = {27, 29, 31, 47, 56, 65, 74, 83, 92, 120};

	if (k < tab[0]){
		printf("Peculiar case of 0 %d<%d<%d\n", tab[M-1], k, tab[0]);
		printf("%d Belongs to %d\n", k, tab[0]);
		return 0;
	}
	if (k == tab[0]){
		printf("%d Belongs to %d\n", k, tab[1]);
		return 1;
	}

	/*for (i=0; i<M-2 && !(app(k, tab[i], tab[i+1])); i++);*/
	for (i=0; i<M-1 && !(app(k, tab[i], tab[i+1])); i++);

	if (k >= tab[M-1]){
		printf("%d Belongs to %d\n", k, tab[0]);
		return 0;
	}
	
	/* Dans le cas ou on est inferieur au premier */
	/*if (i == 0 && !app(k, tab[i], tab[i+1])){*/
	/*if (i == 0 && k < tab[0]){*/
		/*printf("Peculiar case of 0 %d<%d<%d\n", tab[M-1], k, tab[0]);*/
		/*printf("%d Belongs to %d\n", tab[M-1]);*/
		/*return 0;*/
	/*}*/

	/* Dans le cas ou a > b on depasse a, arrive a 0, va jusqu'a b */
	/*if (i == M-2 && app(k, tab[i+1], tab[0])){*/
	if (i == M-2 && k >= tab[M-1]){
		printf("Peculiar case Last %d<%d<%d\n", tab[M-1], k, tab[0]);
		printf("%d Belongs to %d\n", k, tab[0]);
		return 0;
	}

	printf("bite %d<%d<%d\n", tab[i], k, tab[i+1]);
	printf("%d Belongs to %d\n", k, tab[i+1]);
	return 0;
}
