#include<stdio.h>
#include<vector>
using namespace std;

int P = 20;
int N, M, T, D;


class pipe {
public:
	int id;
	int cost;
	int* channel = new int[P];
	pipe() {}
	pipe(int id, int cost) {
		this->id = id;
		this->cost = cost;
	}
};
class edge {
public:
	int nodeNo1;
	int nodeNo2;
	vector<int> p;
	edge() {}
	edge(int nodeNo1, int nodeNo2) {
		this->nodeNo1 = nodeNo1;
		this->nodeNo2 = nodeNo2;
	};

	int another(int node1) {
		return this->nodeNo1 == node1 ? this->nodeNo2 : this->nodeNo1;
	}
};
class task {
public:
	int id;
	int from;
	int to;
	int power;
	bool* possibleChannel = new bool[P];
	task() {}
	task(int id, int from, int to, int power) {
		this->id = id;
		this->from = from;
		this->to = to;
		this->power = power;
	}
};
class node {
public:
	int id;
	vector<int> e;
	node() {}
	node(int id) {
		this->id = id;
	};

};

edge edges[5005];
vector<pipe> pipes;
task tasks[10005];
node nodes[5005];

int main() {
	printf("hello world");
}

