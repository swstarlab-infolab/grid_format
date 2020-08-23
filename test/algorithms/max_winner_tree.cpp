#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX(a, b) ((a > b) ? a : b)

class MaxWinTree{
	typedef int index;
private:
	int num_bins;
	int num_objects;
	int bin_capacity;
	int offset;
	int low_ext;
	int *players;
	int *index_tree;

	index to_index(index d){
		return d <= low_ext ? d + offset : d - low_ext + num_bins - 1;
	}
	index to_datum(index i){
		return i > offset ? i - offset : i + low_ext - num_bins + 1;
	}
	index get_parent(index d){
		return to_index(d) / 2;
	}
	index find(int obj){
		index R = 1;

		while(R < num_bins){
			if(players[index_tree[R]] >= obj){
				R = R * 2;
			}else{
				R = R * 2 + 1;
			}
		}
		if(R > offset && R <= offset + low_ext) return R - offset;
		return index_tree[R / 2];
	}
	void put(FILE *f){
		int obj;
		index i;

		fscanf_s(f, "%d", &obj);
		i = find(obj);
		// printf("<<%d>>\n", i);
		while(players[i] < obj) i++;
		players[i] -= obj;
		refresh_tree(i);
		printf("[%d] ", obj);
		print_players(true);
	}
	void refresh_tree(index bottom){
		index R = to_index(bottom);
		index R_bro;
		bool skewed, datum;
		int ri;

		while(R > 1){
			R_bro = R % 2 == 0 ? R + 1 : R - 1;
			skewed = MAX(R, R_bro) == num_bins;
			datum = R >= num_bins;
			if(skewed){
				ri = datum ? index_tree[R_bro] : to_datum(R_bro);
			}else{
				ri = datum ? to_datum(R_bro) : index_tree[R_bro];
			}
			if(players[bottom] < players[ri]){
				index_tree[R / 2] = ri;
			}else{
				index_tree[R / 2] = bottom;
			}
			R /= 2;
			bottom = index_tree[R];
		}
	}

public:
	void input(FILE *f){
		fscanf_s(f, "%d %d %d", &num_bins, &num_objects, &bin_capacity);
	}
	void initialize(){
		index i, p;
		int s = 1 << ((int)ceil(log2(num_bins)) - 1);

		offset = 2 * s - 1;
		low_ext = 2 * ((num_bins - 1) - (s - 1));
		players = (int*)calloc(num_bins + 1, sizeof(int));
		index_tree = (int*)calloc(num_bins, sizeof(int));
		if(num_bins % 2 == 0){
			for(i = 1; i <= num_bins; i += 2){
				p = get_parent(i);
				index_tree[p] = i;
			}
		}else{
			for(i = 1; i <= low_ext; i += 2){
				p = get_parent(i);
				index_tree[p] = i;
			}
			for(i = low_ext + 2; i <= num_bins; i += 2){
				p = get_parent(i);
				index_tree[p] = i;
			}
		}
		for(i = (num_bins - 1) / 2; i > 0; i--){
			index_tree[i] = index_tree[i * 2];
		}
		for(i = 1; i <= num_bins; i++){
			players[i] = bin_capacity;
		}
	}
	void start_put(FILE *f){
		for(int i = 0; i < num_objects; i++) put(f);
	}
	void print_players(bool all = false){
		for(int i = 1; i <= num_bins; i++){
			if(all || players[i] < bin_capacity) printf("%d ", players[i]);
		}
		printf("\n");
	}
	void finalize(){
		free(players);
		free(index_tree);
	}
};
int main(void){
	FILE *f_in;
	MaxWinTree tree;
	
	fopen_s(&f_in, "input.txt", "r");
	tree.input(f_in);
	tree.initialize();
	tree.start_put(f_in);
	printf("\n");
	tree.print_players(true);

	tree.finalize();
	fclose(f_in);
	system("pause");
	return EXIT_SUCCESS;
}