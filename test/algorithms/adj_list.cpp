#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <functional>

int* init_list(int n){
	return (int*)calloc(sizeof(int), n + 1);
}
class ListItem{
public:
	int src;
	int dest;
	int height;
	int area;
	int weight;
	int value;
	ListItem *next;

	ListItem(int src, int dest, int height, int area, int weight){
		this->src = src;
		this->dest = dest;
		this->height = height;
		this->area = area;
		this->weight = weight;
	}
	ListItem(int value){
		this->value = value;
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
	void unshift(ListItem *item){
		if(start == NULL){
			start = end = item;
		} else{
			item->next = start;
			start = item;
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
class AdjList{
#define LIST_START 0
#define LIST_END n + 1
#define DFS_WHITE 0
#define DFS_GRAY 1
#define DFS_BLACK 2
private:
	ListHead **list;
	ListHead *ts_list;
	int n;

	void dfs_visit(int i, int *C, int *P){
		C[i] = DFS_GRAY;
		list[i]->for_each([&](ListItem *v){
			if(C[v->dest] == DFS_WHITE){
				P[v->dest] = i;
				dfs_visit(v->dest, C, P);
			}
		});
		ts_list->unshift(new ListItem(i));
		C[i] = DFS_BLACK;
	}

public:
	AdjList(int n, FILE *f_in){
		int height, area, weight;
		int i, j;

		this->n = n;
		list = (ListHead**)calloc(sizeof(ListHead*), n + 2);
		for(i = LIST_START; i <= LIST_END; i++){
			list[i] = new ListHead();
		}
		for(i = 1; i <= n; i++){
			fscanf_s(f_in, "%d %d %d", &area, &height, &weight);
			list[LIST_START]->push(new ListItem(LIST_START, i, 0, 0, 0));
			list[i]->push(new ListItem(i, LIST_END, height, area, weight));
		}
		for(i = 1; i <= n; i++){
			for(j = 1; j <= n; j++){
				if(i == j) continue;
				// i -> j 가 가능한가?
				if(list[i]->start->weight < list[j]->start->weight) continue;
				if(list[i]->start->area < list[j]->start->area) continue;
				// i -> j
				list[i]->push(new ListItem(i, j, list[i]->start->height, list[i]->start->area, list[i]->start->weight));
			}
		}
	}
	~AdjList(){
		for(int i = 1; i <= n; i++){
			delete list[i];
		}
		free(list);
	}
	void print(){
		for(int i = LIST_START; i <= LIST_END; i++){
			printf("[%3d]", i);
			list[i]->for_each([](ListItem *v){
				printf("→%d", v->dest);
			});
			printf("\n");
		}
	}
	void print_ts(){
		printf("Topological: ");
		ts_list->for_each([](ListItem *v){
			printf("%d ", v->value);
		});
		printf("\n");
	}
	void dfs(){
		int *C = init_list(n + 2);
		int *P = init_list(n + 2);
		int i;

		ts_list = new ListHead();
		for(i = LIST_START; i <= LIST_END; i++){
			C[i] = DFS_WHITE;
			P[i] = -1;
		}
		for(i = LIST_START; i <= LIST_END; i++){
			if(C[i] == DFS_WHITE){
				dfs_visit(i, C, P);
			}
		}
		free(C);
		free(P);
	}
	void dag(){
		int *D = init_list(n + 2);
		int *P = init_list(n + 2);
		int chain = -1; // 시작 제외
		int total_height = 0;
		int i;

		// 초기화
		for(i = LIST_START; i <= LIST_END; i++){
			D[i] = 0;
			P[i] = -1;
		}
		D[LIST_START] = 99999999;
		// 수행
		ts_list->for_each([&](ListItem *v){
			list[v->value]->for_each([&](ListItem *w){
				dag_relax(v->value, w->dest, w->height, D, P);
			});
		});
		// 출력
		printf("Output: \n");
		for(i = P[LIST_END]; i != -1; i = P[i]){
			chain++;
			total_height += list[i]->start->height;
		}
		printf("%d\n", chain);
		for(i = P[LIST_END]; i > LIST_START; i = P[i]){
			printf("%d\n", i);
		}
		printf("Height: %d\n", total_height);
		free(D);
		free(P);
	}
	void dag_relax(int u, int v, int cost, int *D, int *P){
		if(D[v] < D[u] + cost){
			D[v] = D[u] + cost;
			P[v] = u;
		}
	}
};

int main(){
	FILE *f_in;
	AdjList *list;
	int n;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d", &n);
	list = new AdjList(n, f_in);

	list->print();
	list->dfs();
	list->print_ts();
	list->dag();

	delete list;
	fclose(f_in);

	system("pause");
	return 0;
}