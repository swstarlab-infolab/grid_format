#include <stdio.h>
#include <stdlib.h>
#include <functional>

void print_line(int *line, int dim){
	for(int i = 0; i < dim; i++){
		printf("%3d ", line[i]);
	}
	printf("\n");
}
void print_matrix(int **matrix, int dim){
	for(int i = 0; i < dim; i++){
		print_line(matrix[i], dim);
	}
	printf("\n");
}
int* read_line(int dim, FILE *f){
	int *line = (int*)calloc(dim, sizeof(int));

	for(int i = 0; i < dim; i++){
		fscanf_s(f, "%d", line + i);
	}
	return line;
}
int** read_matrix(int dim, FILE *f){
	int **matrix = (int**)calloc(dim, sizeof(int*));

	for(int i = 0; i < dim; i++){
		matrix[i] = read_line(dim, f);
	}
	return matrix;
}
/*void sort_line(int *line, int dim, std::function<bool(int, int)> &comparer){
	int i, j, temp;
	int dim1 = dim - 1;

	for(i = dim / 2; i > 0; --i){
		for(j = 0; j < dim1; j += 2){
			if(comparer(line[j], line[j + 1])){
				temp = line[j];
				line[j] = line[j + 1];
				line[j + 1] = temp;
			}
		}
		for(j = 1; j < dim1; j += 2){
			if(comparer(line[j], line[j + 1])){
				temp = line[j];
				line[j] = line[j + 1];
				line[j + 1] = temp;
			}
		}
	}
}*/
void sort_line_pointer(int **line, int dim, std::function<bool(int, int)> &comparer){
	int i, j, temp;
	int dim1 = dim - 1;

	for(i = dim / 2; i > 0; --i){
		for(j = 0; j < dim1; j += 2){
			if(comparer(*line[j], *line[j + 1])){
				temp = *line[j];
				*line[j] = *line[j + 1];
				*line[j + 1] = temp;
			}
		}
		for(j = 1; j < dim1; j += 2){
			if(comparer(*line[j], *line[j + 1])){
				temp = *line[j];
				*line[j] = *line[j + 1];
				*line[j + 1] = temp;
			}
		}
	}
}
void sort_matrix(int **matrix, int dim){
	std::function<bool(int, int)> comparers[2] = {
		[](int a, int b){ return a > b; },
		[](int a, int b){ return a < b; }
	};
	int **vert = (int**)calloc(dim, sizeof(int*));
	int i, j, k;

	for(i = dim; i >= 2; i /= 2){
		for(j = 0; j < dim; j++){
			for(k = 0; k < dim; k++) vert[k] = &matrix[j][k];
			sort_line_pointer(vert, dim, comparers[j % 2]);
		}
		for(j = 0; j < dim; j++){
			for(k = 0; k < dim; k++) vert[k] = matrix[k] + j;
			sort_line_pointer(vert, dim, comparers[0]);
		}
		print_matrix(matrix, dim);
	}
	for(i = 0; i < dim; i++){
		for(k = 0; k < dim; k++) vert[k] = &matrix[i][k];
		sort_line_pointer(vert, dim, comparers[0]);
	}
	free(vert);
}
int main(void){
	FILE *f_in;
	int dim;
	int **matrix;

	fopen_s(&f_in, "input.txt", "r");
	
	fscanf_s(f_in, "%d", &dim);
	matrix = read_matrix(dim, f_in);
	sort_matrix(matrix, dim);
	print_matrix(matrix, dim);
	
	for(int i = 0; i < dim; i++){
		free(matrix[i]);
	}
	free(matrix);
	fclose(f_in);
	system("pause");
	return 0;
}