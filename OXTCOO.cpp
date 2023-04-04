#include<stdio.h>
#include<vector>

using namespace std;

class node {
public:
	int no;
	vector<edge> e;

	node(int no) {
		this->no = no;
	};


};
class edge {
public:
	int nodeNo1;
	int nodeNo2;
	vector<pipe> pipes;

	edge(int nodeNo1, int nodeNo2) {
		this->nodeNo1 = nodeNo1;
		this->nodeNo2 = nodeNo2;
	};

	int another(int node1) {
		return this->nodeNo1 == node1 ? this->nodeNo2 : this->nodeNo1;
	}
};
class pipe {
public:
	int cost;
	int channel[8] = { 0,0,0,0,0,0,0,0 };

	pipe(int cost) {
		this->cost = cost;
	}
};
class task {
public:
	int from;
	int to;
	int power;
	bool possibleChannel[8] = { false };

	task(int from, int to, int power) {
		this->from = from;
		this->to = to;
		this->power = power;
	}
};

int main() {
	printf("hello world");
}

