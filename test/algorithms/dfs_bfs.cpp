#include <stdio.h>
#include <stdlib.h>
#include <vector>

struct Node{
	int value;
	Node *next;
};
struct NodeHead{
	Node *head;
	Node *tail;
};
enum Color{
	WHITE, GRAY, BLACK
};

void initialize(std::vector<Color> &C, std::vector<int> &P, std::vector<int> &D){
	for(auto &v : C){
		v = WHITE;
	}
	for(auto &v : P){
		v = -1;
	}
	for(auto &v : D){
		v = INT_MAX;
	}
}
void dfs_visit(NodeHead *head, std::vector<NodeHead*> &list, std::vector<Color> &C, std::vector<int> &P, NodeHead *result){
	int u = head->head->value;

	C[u] = GRAY;
	for(auto v = head->head; v != NULL; v = v->next){
		if(C[v->value] == WHITE){
			P[v->value] = u;
			dfs_visit(list[v->value], list, C, P, result);
		}
	}
	if(result->head == NULL){
		result->head = result->tail = new Node{ u, NULL };
	}else{
		result->head = new Node{ u, result->head };
	}
	C[u] = BLACK;
}
void dfs(std::vector<NodeHead*> &list, std::vector<Color> &C, std::vector<int> &P, std::vector<int> &D, NodeHead *result){
	initialize(C, P, D);
	for(auto &v : list){
		for(auto w = v->head; w != NULL; w = w->next){
			if(C[w->value] == WHITE){
				dfs_visit(list[w->value], list, C, P, result);
			}
		}
	}
}
void bfs(std::vector<NodeHead*> &list, std::vector<Color> &C, std::vector<int> &P, std::vector<int> &D, NodeHead *result){
	int s = 1;
	Node *currentNode = new Node{ s, NULL };
	NodeHead *queue = new NodeHead{ currentNode, currentNode };

	initialize(C, P, D);
	C[s] = GRAY;
	D[s] = 0;
	while(queue->head != NULL){
		Node *temp;
		int u = currentNode->value;

		for(auto v = list[u]->head; v != NULL; v = v->next){
			if(C[v->value] == WHITE){
				C[v->value] = GRAY;
				D[v->value] = D[u] + 1;
				P[v->value] = u;
				temp = new Node{ v->value, NULL };
				queue->tail->next = temp;
				queue->tail = temp;
			}
		}
		queue->head = currentNode->next;
		delete currentNode;
		currentNode = queue->head;
		C[u] = BLACK;
	}
}
int main(void){
	FILE *f_in;
	std::vector<NodeHead*> list;
	std::vector<Color> C;
	std::vector<int> P;
	std::vector<int> D;
	NodeHead *result = new NodeHead{ NULL, NULL };
	int numV, numE;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d %d", &numV, &numE);
	for(auto i = 0; i <= numV; i++){
		Node *baby = new Node{ i, NULL };
		NodeHead *head = new NodeHead{ baby, baby };

		list.push_back(head);
		C.push_back(WHITE);
		P.push_back(-1);
		D.push_back(INT_MAX);
	}
	for(auto i = 0; i < numE; i++){
		int vFrom, vTo;
		NodeHead *head;
		Node *baby;

		fscanf_s(f_in, "%d %d", &vFrom, &vTo);
		head = list[vFrom];
		baby = new Node{ vTo, NULL };
		head->tail->next = baby;
		head->tail = baby;
	}
	dfs(list, C, P, D, result);
	printf("dfs p[i] : ");
	for(auto i = 1; i <= numV; i++){
		printf("%d ", P[i]);
	}
	printf("\n");

	bfs(list, C, P, D, result);
	printf("bfs p[i] : ");
	for(auto i = 1; i <= numV; i++){
		printf("%d ", P[i]);
	}
	printf("\n\n");
	for(auto v = result->head; v->next != NULL; v = v->next){
		printf("%d ", v->value);
	}
	printf("\n");

	fclose(f_in);
	system("pause");
	return 0;
}