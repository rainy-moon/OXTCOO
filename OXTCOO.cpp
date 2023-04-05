#include <cstdio>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
int P;//任务数量
int N, M, T, D; //节点数量、管道数量、信道数量、衰减距离
int E = 0; //总边数
int I = 0; //岛数量

class pipe;
class edge;
class task;
class node;
class island;
class treeNode;

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
class node {
   public:
    int id;
    int islandId;
    vector<int> e;
    node() {}
	void initnode(int id) {
		this->id = id;
        islandId = -1;
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
class island {
   public:
    vector<int> pointSet;
    vector<int> bridges;
    bool isInIsland(int node) {
        for (int i = 0; i < pointSet.size(); i++) {
            if (node == pointSet[i])
                return true;
        }
        return false;
    }
};
class treeNode {
   public:
    treeNode() { parent = -1; }
    int parent;
    int deepth;
};


task tasks[10005];
vector<pipe> pipes;
node nodes[5005];

vector<island> islands;
vector<int> bridges;

treeNode tree[5000];
bool nodeFlag[5000];
bool edgeFlag[5000];
bool notBridge[5000];


void creatTree() {
    // init
    for (int i = 0; i < 5000; i++) {
        nodeFlag[i] = false;
        edgeFlag[i] = false;
        notBridge[i] = false;
    }
    // creat tree by bfs
    cout << "\n******creat tree start:******" << endl;
    queue<int> q;
    for (int i = 0; i < N; i++) {
        if (nodeFlag[i] == false) {
            q.push(i);
            nodeFlag[i] = true;
            while (!q.empty()) {
                int parent = q.front();
                cout << "now node id is :" << parent << endl;
                q.pop();
                for (int j = 0; j < nodes[parent].e.size(); j++) {
                    int idEdge = nodes[parent].e[j];
                    int child = edges[idEdge].another(parent);
                    if (!nodeFlag[child]) {
                        q.push(child);
                        tree[child].parent = parent;
                        tree[child].deepth = tree[parent].deepth + 1;
                        nodeFlag[child] = true;
                        edgeFlag[idEdge] = true;
                        cout << "child: " << child
                             << " deepth: " << tree[child].deepth << endl;
                    }
                }
            }
        }
    }
    cout << "******end tree creat******" << endl;
}
void findBridge() {
    //judge bridge by find circle
    //add a edge to tree for a circle
    cout << "\n******start find bridge:******" << endl;
    for (int i = 0; i < E; i++) {
        if (!edgeFlag[i]) {
            cout << "edge id:" << i;
            int nodea = edges[i].nodeNo1;
            int nodeb = edges[i].nodeNo2;
            cout << "  node:" << nodea << "  " << nodeb << endl;
            notBridge[nodes[nodea].getEdge(nodeb)] = true;
            if (tree[nodea].deepth > tree[nodeb].deepth) {
                swap(nodea, nodeb);
            }
            while (tree[nodea].deepth != tree[nodeb].deepth) {
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
                cout << "new nodeb: " << nodeb << endl;
            }
            while (nodea != nodeb) {
                notBridge[nodes[nodea].getEdge(tree[nodea].parent)] = true;
                nodea = tree[nodea].parent;
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
                cout << "new nodea&nodeb: " << nodea << " " << nodeb << endl;
            }
            cout << "LCA: " << nodea << endl;
        }
    }
    for (int i = 0; i < E; i++) {
        if (notBridge[i] == false) {
            bridges.push_back(i);
            cout << "bridge id :" << i<<endl;
            cout << "bridge:" << edges[i].nodeNo1 << " " << edges[i].nodeNo2
                 << endl;
        }
    }
    cout << "******end of find bridge******" << endl;
}
void divideNode() {
    cout << "\n******strat divide node******" << endl;
    // init flags
    for (int i = 0; i < N; i++){
        nodeFlag[i] = false;
    }
    for (int i = 0; i < E; i++) {
        edgeFlag[i] = false;
    }
    for (int i = 0; i < bridges.size(); i++) {
        edgeFlag[bridges[i]] = true;
    }
    // divide node by bfs
    for (int i = 0; i < bridges.size(); i++) {
        int node[2] = {edges[bridges[i]].nodeNo1, edges[bridges[i]].nodeNo2};
        cout << "bridge id: " << i << endl;
        cout << "bridge node: " << node[0] << node[1] << endl;
        for (int k = 0; k < 2; k++) {
            cout << "begining node: " << node[k] << endl;
            if (nodes[node[k]].islandId != -1) {
                //the node has been in a island
                cout << "node has been in island:" << nodes[node[k]].islandId;
                islands[nodes[node[k]].islandId].bridges.push_back(bridges[i]);
                continue;
            }
            int root = node[k];
            nodeFlag[root] = true;
            nodes[root].islandId = i;
            islands.push_back(island());
            I++;
            cout << "find the new island, id:" << endl;
            // search point by bfs
            queue<int> q;
            islands[I-1].pointSet.push_back(root);
            cout << root << "  ";
            islands[I-1].bridges.push_back(bridges[i]);
            q.push(root);
            while (!q.empty()) {
                int parent = q.front();
                q.pop();
                for (int j = 0; j < nodes[parent].e.size(); j++) {
                    int idEdge = nodes[parent].e[j];
                    if (edgeFlag[idEdge])
                        continue;  // delete bridge
                    int child = edges[idEdge].another(parent);
                    if (!nodeFlag[child]) {
                        q.push(child);
                        islands[I-1].pointSet.push_back(child);
                        cout << child << "  ";
                        nodeFlag[child] = true;
                    }
                }
            }
            cout << endl;
        }
    }
    cout << "******end of dividing node******" << endl;
}
void testPrint(island i) {
    cout << "pointSet: " << endl;
    for (int j = 0; j < i.pointSet.size(); j++) {
        cout << i.pointSet[j] << "  ";
    }
    cout << "\nbridges:" << endl;
    for (int j = 0; j < i.bridges.size(); j++) {
        cout << "id:" << i.bridges[j] << "  nodes:";
        cout << edges[i.bridges[j]].nodeNo1 << "  "
             << edges[i.bridges[j]].nodeNo2 << endl;
    }
}

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
        tasks[i].inittask(i, sj, tj, D);
    }
}

int main() {
    initData();
    creatTree();
    findBridge();
    divideNode();
    cout << "\ntest********" << endl;
    for (int i = 0; i < islands.size(); i++){
        cout << "island id: " << i << endl;
        testPrint(islands[i]);
        cout << endl;
    }
}
