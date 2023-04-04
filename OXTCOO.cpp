#include<stdio.h>
#include<vector>
#include<iostream>
using namespace std;
int P;//任务数量
int N, M, T, D; //节点数量、管道数量、信道数量、衰减距离
int E = 0; //总边数

class pipe {
public:
	int id;
	int cost;
	int channel[80];
	pipe() {}
	void initpipe(int id, int cost) {
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
	void initedge(int nodeNo1, int nodeNo2) {
		this->nodeNo1 = nodeNo1;
		this->nodeNo2 = nodeNo2;
	};

	int another(int node1) {
		return this->nodeNo1 == node1 ? this->nodeNo2 : this->nodeNo1;
	}
};
edge edges[5005];
class task {
public:
	int id;
	int from;
	int to;
	int power;
	bool possibleChannel[80];
	task() {}
	void inittask(int id, int from, int to, int power) {
		this->id = id;
		this->from = from;
		this->to = to;
		this->power = power;
	}
};
task tasks[10005];
class node {
public:
	int id;
	vector<int> e;
	node() {}
	void initnode(int id) {
		this->id = id;
	};
	int getEdge(int dst) {
		for (int i = 0; i < e.size(); i++) {
			int edgeId = e[i];
			if (edges[edgeId].another(id) == dst) {
				return edgeId;
			}
		}
		return -1;
	}
};
node nodes[5005];
vector<pipe> pipes;

void initData() {
	scanf("%d %d %d %d %d", &N, &M, &T, &P, &D);
	for (int i = 0; i < N; i++) {
		nodes[i].initnode(i);
	}
	pipes.resize(M);
	for (int i = 0; i < M; i++) {
		int si, ti, di;
		scanf("%d %d %d", &si, &ti, &di);
		pipes[i].initpipe(i, di);
		if (nodes[si].getEdge(ti) == -1) {
			edges[E].initedge(si, ti);
			nodes[si].e.push_back(E);
			nodes[ti].e.push_back(E++);
		}
	}
	for (int i = 0; i < T; i++) {
		int sj, tj;
		scanf("%d %d", &sj, &tj);
		tasks[i].inittask(i, sj, tj,D);
	}
}

int main() {
	initData();
	cout << ("hello world");
}

