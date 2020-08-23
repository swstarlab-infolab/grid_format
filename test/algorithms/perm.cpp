#include <iostream>
#include <stdlib.h>
#include <string.h>
#define MAX_CHAR 1024
using namespace std;

void sort(char *str, int k, int n) {
	int i;

	for (i = k + 1; i < n; i++) {
		char temp = str[i];
		int aux = i - 1;

		while (aux >= k && str[aux] > temp) {
			str[aux + 1] = str[aux];
			aux--;
		}
		str[aux + 1] = temp;
	}
}
int get_exchangable(char *str, int k, int n) {
	int i;

	for (i = k + 1; i < n; i++) {
		if (str[i] > str[k]) return i;
	}
	return -1;
}
void perm(char *str, int k, int n) {
	static int count = 0;
	int m;

	if (k >= n) {
		cout << "#" << ++count << ": " << str << endl;
		return;
	}	
	sort(str, k, n);
	perm(str, k + 1, n);
	sort(str, k + 1, n);
	while ((m = get_exchangable(str, k, n)) != -1) {
		char temp = str[m];

		str[m] = str[k];
		str[k] = temp;
		perm(str, k + 1, n);
		sort(str, k + 1, n);
	}
}

int main() {
	char str[MAX_CHAR];

	cout << "Input: ";
	cin >> str;

	perm(str, 0, strlen(str));
	
	system("pause");
	return 0;
}