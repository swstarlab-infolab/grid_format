#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <functional>

enum Flag{
	AVAIL,
	USED
};
template<typename T> class MinHeap{
private:
	std::vector<T> list;
public:
	int heap_size;
	MinHeap(){
		heap_size = 0;
		list.push_back(NULL);
	}

	bool is_empty(){
		return heap_size < 1;
	}
	void push(T value){
		list.push_back(value);
	}
	void make_heap(){
		heap_size = list.size() - 1;
		for(auto i = heap_size / 2; i >= 1; i--){
			sift_down(i);
		}
	}
	void sift_down(int index){
		T key = list[index];
		int p = index;
		bool found = false;
		int c_smaller;
		int c_left = 2 * p, c_right = 2 * p + 1;

		while(c_left <= heap_size && !found){
			if(c_left < heap_size && *list[c_left] > *list[c_right]){
				c_smaller = c_right;
			} else{
				c_smaller = c_left;
			}
			if(*key > *list[c_smaller]){
				list[p] = list[c_smaller];
				p = c_smaller;
				c_left = 2 * p;
				c_right = 2 * p + 1;
			} else{
				found = true;
			}
		}
		list[p] = key;
	}
	void print(FILE *f_target){
		for(auto i = 1; i <= heap_size; i++){
			HeapItem *item = (HeapItem*)list[i];

			fprintf(f_target, "%d ", item->cost);
		}
		fprintf(f_target, "\n");
	}
	void print_sorted(FILE *f_target){
		std::vector<T> temp_list(list);
		int org_heap_size = heap_size;

		for(auto i = 0; i < org_heap_size; i++){
			fprintf(f_target, "%d ", delete_min());
		}
		list = temp_list;
		heap_size = list.size() - 1;
		fprintf(f_target, "\n");
	}
	void insert(T value){
		int index = ++heap_size;

		list.push_back(value);
		while(index > 1 && *list[index / 2] > *value){
			list[index] = list[index / 2];
			index /= 2;
		}
		list[index] = value;
	}
	T delete_min(){
		T ret;

		if(heap_size < 1) return NULL;

		ret = list[1];
		list[1] = list[heap_size--];
		sift_down(1);
		list.pop_back();
		return ret;
	}
};
class HeapItem{
public:
	int index;
	int cost;

	HeapItem(int index, int cost){
		this->index = index;
		this->cost = cost;
	}
	bool operator>(HeapItem &operand){
		return cost > operand.cost;
	}
	bool operator>=(HeapItem &operand){
		return cost >= operand.cost;
	}
};
class ListItem{
public:
	int src;
	int dest;
	int cost;
	ListItem *next;

	ListItem(int src, int dest, int cost){
		this->src = src;
		this->dest = dest;
		this->cost = cost;
	}
};
class ListHead{
public:
	ListItem *start;
	ListItem *end;
	ListHead(){
		start = NULL;
		end = NULL;
	}
	~ListHead(){
		this->for_each([&](ListItem *v){
			delete v;
		});
	}
	void for_each(std::function<void(ListItem*)> const &callback){
		for(ListItem *node = start; node != NULL; ){
			ListItem *temp = node->next;

			callback(node);
			node = temp;
		}
	}
	void push(ListItem *item){
		if(end == NULL){
			start = end = item;
		} else{
			end->next = item;
			end = item;
		}
	}
};

ListHead** init_adj_list(int n, int e, FILE *f_in){
	ListHead **R = (ListHead**)calloc(sizeof(ListHead*), n + 1);
	int src, dest, cost;
	int i;

	for(i = 1; i <= n; i++){
		R[i] = new ListHead();
	}
	for(i = 0; i < e; i++){
		fscanf_s(f_in, "%d %d %d", &src, &dest, &cost);

		R[src]->push(new ListItem(src, dest, cost));
		// R[dest]->push(new ListItem(dest, src, cost));
	}
	return R;
}
int* init_list(int n){
	return (int*)calloc(sizeof(int), n + 1);
}
void free_adj_list(int n, ListHead **W){
	for(int i = 1; i <= n; i++){
		delete W[i];
	}
	free(W);
}
void dijkstra(int n, ListHead **W, int *D, int *P, int *F){
	int R = 1;
	MinHeap<HeapItem*> heap;
	int i;

	for(i = 1; i <= n; i++){
		D[i] = INT_MAX;
		F[i] = AVAIL;
	}
	D[R] = 0;
	P[R] = -1;
	heap.insert(new HeapItem(R, D[R]));
	while(!heap.is_empty()){
		HeapItem *item = heap.delete_min();
		int u = item->index;

		if(F[u] == AVAIL){
			W[u]->for_each([&](ListItem *node){
				int v = node->dest;
				int cost = D[u] + node->cost;

				if(F[v] == AVAIL && cost < D[v]){
					P[v] = u;
					D[v] = cost;
					heap.insert(new HeapItem(v, D[v]));
				}
			});
		}
		if(heap.heap_size > 2) heap.print(stdout);
		F[u] = USED;
		delete item;
	}
}
void print_adj_list(int n, ListHead **W){
	for(int i = 1; i <= n; i++){
		printf("%3d: ", i);
		W[i]->for_each([&](ListItem *node){
			printf("[%3d %3d]", node->dest, node->cost);
		});
		printf("\n");
	}
}
void print_list(int n, int *list){
	for(int i = 1; i <= n; i++){
		printf("%3d", list[i]);
	}
	printf("\n");
}
int sum_list(int n, int *list){
	int R = 0;

	for(int i = 1; i <= n; i++){
		R += list[i];
	}
	return R;
}
int main(){
	FILE *f_in;
	ListHead **W;
	int *D, *P, *F;
	int num_vertices;
	int num_edges;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d %d", &num_vertices, &num_edges);
	W = init_adj_list(num_vertices, num_edges, f_in);
	D = init_list(num_vertices);
	P = init_list(num_vertices);
	F = init_list(num_vertices);

	printf("Adj. List:\n");
	print_adj_list(num_vertices, W);
	printf("Heap:\n");
	dijkstra(num_vertices, W, D, P, F);
	printf("P: ");
	print_list(num_vertices, P);
	printf("D: ");
	print_list(num_vertices, D);

	fclose(f_in);
	free_adj_list(num_vertices, W);
	free(D);
	free(P);
	free(F);
	system("pause");
	return 0;
}