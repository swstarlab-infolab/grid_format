#include <stdio.h>
#define MAX_SIZE 1024

typedef struct _Edge {
	int src;
	int dest;
	int weight;
} Edge;

int find(int map[MAX_SIZE], int i) {
	int r, s;

	for (r = i; map[r] >= 0; r = map[r]);
	while (i != r) {
		s = map[i];
		map[i] = r;
		i = s;
	}
	return r;
}
void merge(int map[MAX_SIZE], int src, int dest) {
	int temp = map[src] + map[dest];

	if (map[src] > map[dest]) {
		map[src] = dest;
		map[dest] = temp;
	}
	else {
		map[dest] = src;
		map[src] = temp;
	}
}
void kruskal(FILE *f_in, int n, int m, Edge ff[MAX_SIZE]) {
	int map[MAX_SIZE];
	int i, j;
	int s = 0, len = 0;
	Edge e;

	for (i = 1; i <= n; i++) {
		map[i] = -1;
	}
	while (len < n - 1) {
		int p, q;

		fscanf_s(f_in, "%d %d %d", &e.src, &e.dest, &e.weight);
		i = e.src;
		j = e.dest;
		p = find(map, i);
		q = find(map, j);
		if (p != q) {
			merge(map, p, q);
			ff[len++] = e;
		}
	}
}
int main() {
	FILE *f_in, *f_out;
	Edge tree_out[MAX_SIZE];
	int n_edges, n_vertices;
	int i;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d %d", &n_vertices, &n_edges);

	kruskal(f_in, n_vertices, n_edges, tree_out);
	fclose(f_in);

	fopen_s(&f_out, "output.txt", "w");
	for (i = 0; i < n_vertices - 1; i++) {
		fprintf(f_out, "%d %d %d\n", tree_out[i].src, tree_out[i].dest, tree_out[i].weight);
	}
	fclose(f_out);

	return 0;
}