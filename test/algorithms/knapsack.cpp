#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define INF 2147483647
#define BUF_SIZE 1024
#define MONT_RATE 0.95

void on_error(const char*);
int is_promising(int*, int, int);
void do_n_queen_mont(int*, int);
void do_n_queen_det(int*, int, int);
void print_array(FILE*, int*, int);
int* create_array(int);

int main(){
	FILE *f_in;
	FILE *f_out = fopen("output.txt", "w");
	char name[BUF_SIZE];

	int *arr;
	int dim;

	printf("input file name: ");
	fgets(name, BUF_SIZE, stdin);
	name[strlen(name) - 1] = '\0';
	f_in = fopen(name, "r");

	if(f_in == NULL) on_error("input file");
	if(f_out == NULL) on_error("output file");

	// GO!
	srand(time(NULL));

	fscanf(f_in, "%d", &dim);
	arr = create_array(dim);
	do_n_queen_mont(arr, dim);
	print_array(f_out, arr, dim);

	free(arr);
	fclose(f_in);
	fclose(f_out);
	return 0;
}
void on_error(const char *msg){
	printf("Error: %s\n", msg);
	exit(1);
}
int is_promising(int *out, int c, int v){
	int o, p;
	int i;

	for(i = 0; i < c; i++){
		o = out[i] - v;
		p = c - i;

		if(o == 0 || o + p == 0 || o - p == 0){
			return 0;
		}
	}
	return 1;
}
void do_n_queen_mont(int *out, int dim){
	int *cand = create_array(dim);
	int mont = dim * MONT_RATE;
	int len, lv;
	int i;

	out[dim - 1] = -1;
	while(out[dim - 1] == -1){
		lv = 0;
		while(lv < mont){
			// 후보 목록을 만든다.
			len = 0;
			for(i = 0; i < dim; i++){
				if(!is_promising(out, lv, i)) continue;
				cand[len++] = i;
			}
			// 후보가 있는가?
			if(len > 0){
				out[lv++] = cand[rand() % len];
			}else{
				// 다시 한다.
				lv = 0;
			}
		}
		do_n_queen_det(out, dim, lv);
	}
	free(cand);
}
void do_n_queen_det(int *out, int dim, int c){
	static int end = 0;
	int i;

	// 끝났는가?
	if(c >= dim){
		end = 1;
		return;
	}
	for(i = 0; i < dim; i++){
		if(!is_promising(out, c, i)) continue;
		// i 열에 둔다.
		out[c] = i;
		// 다음 행을 계산한다.
		do_n_queen_det(out, dim, c + 1);
		if(end == 1) return;
	}
	out[c] = -1;
}
void print_array(FILE *file, int *arr, int dim){
	int i;

	for(i = 0; i < dim; i++){
		fprintf(file, "%d", arr[i] + 1);
		if(i + 1 < dim) fprintf(file, "\n");
	}
}
int* create_array(int len){
	int *arr = (int*)calloc(sizeof(int), len);

	if(arr == NULL) on_error("memory allocation");

	return arr;
}