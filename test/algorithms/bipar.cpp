#include <stdio.h>
#include <stdlib.h>
#include <vector>
#define FRONT 0
#define BACK 1

class Edge{
public:
	int src;
	int dest;
	int cap;
	int flow;
	Edge *mirror = NULL;
	Edge *next = NULL;

	Edge(int src, int dest, int cap, int flow) {
		this->src = src;
		this->dest = dest;
		this->cap = cap;
		this->flow = flow;
	}
	~Edge() {
		if(next != NULL) delete next;
	}
};
class IntQueueItem{
public:
	int value;
	IntQueueItem *next = NULL;

	IntQueueItem(int value){
		this->value = value;
	}
};
class IntQueue{
public:
	IntQueueItem *head = NULL;
	IntQueueItem *tail = NULL;

	~IntQueue(){
		for(auto v = head; v != NULL; ){
			IntQueueItem *temp = v->next;

			delete v;
			v = temp;
		}
	}
	void enqueue(IntQueueItem *item){
		if(head == NULL){
			head = tail = item;
		} else{
			tail->next = item;
			tail = item;
		}
	}
	IntQueueItem* dequeue(){
		IntQueueItem *R = head;

		head = head->next;
		return R;
	}
	bool isEmpty(){
		return head == NULL;
	}
};
enum Color{
	WHITE, GRAY, BLACK
};

Edge* find_mirror(Edge **list, int src) {
	Edge *R = list[FRONT];

	while(R != NULL) {
		if(R->dest == src) return R;
		R = R->next;
	}
	return R;
}
void append_edge(Edge **target, Edge **mirror_list, Edge *item) {
	Edge *mirror;

	if(target[BACK] == NULL) {
		target[FRONT] = item;
		target[BACK] = item;
	} else {
		mirror = find_mirror(target, item->dest);
		if(mirror == NULL) {
			target[BACK]->next = item;
			target[BACK] = item;
		} else {
			mirror->cap = item->cap;
		}
	}
	mirror = find_mirror(mirror_list, item->src);
	if(mirror == NULL) {
		item->mirror = new Edge(item->dest, item->src, 0, 0);
		append_edge(mirror_list, target, item->mirror);
	} else {
		item->mirror = mirror;
	}
}
void append_edge_to(std::vector<Edge**> &list, Edge *item){
	append_edge(list.at(item->src), list.at(item->dest), item);
}
void subtract(std::vector<Edge*> &E, int size){
	int max_flow = INT_MAX;
	int f;

	for(auto v = E[size]; v != NULL; v = E[v->src]){
		f = v->cap - v->flow;
		if(f < max_flow) max_flow = f;
	}
	for(auto v = E[size]; v != NULL; v = E[v->src]){
		v->flow += max_flow;
		v->mirror->flow -= max_flow;
	}
}
bool bfs(std::vector<Edge**> &list, int size){
	std::vector<Color> C;
	std::vector<int> D;
	std::vector<int> P;
	std::vector<Edge*> E;
	IntQueue queue;
	int s = 0;

	for(auto i = 0; i <= size; i++){
		C.push_back(WHITE);
		D.push_back(INT_MAX);
		P.push_back(-1);
		E.push_back(NULL);
	}
	C[s] = GRAY;
	D[s] = 0;
	queue.enqueue(new IntQueueItem(s));
	while(!queue.isEmpty()){
		IntQueueItem *item = queue.dequeue();

		for(auto v = list[item->value][FRONT]; v != NULL; v = v->next){
			if(v->cap - v->flow <= 0) continue;
			if(C[v->dest] != WHITE) continue;
			C[v->dest] = GRAY;
			D[v->dest] = D[item->value] + 1;
			P[v->dest] = item->value;
			E[v->dest] = v;
			queue.enqueue(new IntQueueItem(v->dest));
		}
		C[item->value] = BLACK;
		delete item;
	}
	if(P[size] == -1) return true;
	subtract(E, size);
	return false;
}
int get_max_flow(std::vector<Edge**> &list, int size){
	int R = 0;
	int f;

	for(auto v = list[size][FRONT]; v != NULL; v = v->next){
		f = v->cap - v->flow;
		if(f > 0) R += f;
	}
	return R;
}
int get_v_id(int index, int offset){
	return index + offset;
}
int main(void){
	FILE *f_in;
	std::vector<Edge**> list;
	int num_l, num_r, num_edges;
	int size;
	int num_dots = 0, max_flow;
	int offset_l = 0, offset_r;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d %d %d", &num_l, &num_r, &num_edges);
	size = num_l + num_r + 1;
	offset_r = num_l;
	for(auto i = 0; i <= size; i++) {
		Edge **sub_list = (Edge**)calloc(2, sizeof(Edge*));

		list.push_back(sub_list);
	}
	for(auto i = 1; i <= num_l; i++){
		append_edge_to(list, new Edge(0, get_v_id(i, offset_l), 1, 0));
	}
	for(auto i = 1; i <= num_r; i++){
		append_edge_to(list, new Edge(get_v_id(i, offset_r), size, 1, 0));
	}
	for(auto i = 0; i < num_edges; i++){
		int from, to;

		fscanf_s(f_in, "%d %d", &from, &to);
		append_edge_to(list, new Edge(get_v_id(from, offset_l), get_v_id(to, offset_r), 1, 0));
	}

	while(!bfs(list, size));
	max_flow = get_max_flow(list, size);
	printf("Max Flow is %d\n", max_flow);

	for(auto i = size; i >= 0; i--){
		delete list[i][FRONT];
		free(list[i]);
	}
	fclose(f_in);
	system("pause");
	return 0;
}