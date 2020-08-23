#include <stdio.h>
#include <stdlib.h>
#include <vector>

class Job {
public:
	int id;
	int deadline;
	int profit;
};
int find_collapsing(std::vector<int> &p, int i) {
	int r;

	for (r = i; p[r] >= 0; r = p[r]);
	while (i != r) {
		int s = p[i];

		p[i] = r;
		i = s;
	}
	return r;
}
void union_weighting(std::vector<int> &p, std::vector<int> &q, int i, int j) {
	int temp = p[i] + p[j];

	if (p[i] > p[j]) {
		p[i] = j;
		p[j] = temp;
	}
	else {
		p[j] = i;
		p[i] = temp;
		q[i] = q[j];
	}
}
int main(void) {
	FILE *f_in;
	std::vector<Job> list;
	std::vector<int> p, q, s;
	int len, i;

	fopen_s(&f_in, "input.txt", "r");
	fscanf_s(f_in, "%d", &len);
	for (i = 0; i <= len; i++) {
		p.push_back(-1);
		q.push_back(i);
		s.push_back(-1);
	}
	p[0] = len;
	q[0] = len;
	for (i = 0; i < len; i++) {
		Job job;
		int r, r2;

		fscanf_s(f_in, "%d %d %d", &job.id, &job.deadline, &job.profit);
		r = find_collapsing(p, job.deadline);
		s.at(q[r]) = job.id;
		union_weighting(p, q, r, find_collapsing(p, q[r] - 1));
	}
	for (i = 1; i <= len; i++) {
		printf("%d ", s.at(i));
	}
	printf("\n");

	fclose(f_in);
	system("pause");
	return 0;
}