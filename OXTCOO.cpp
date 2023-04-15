#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <vector>
using namespace std;
int P;//任务数量
int N, M, T = 80, D; //节点数量、管道数量、信道数量、衰减距离
int E = 0; //总边数
int I = 0; //岛数量

class pipe;
class edge;
class task;
class node;
class island;
class treeNode;
int anotherIsland(edge& e, int islandId);
class pipe {
   public:
    int id;
    int cost;
    int channel[80];
    pipe() {}
    void initpipe(int id, int cost) {
        this->id = id;
        this->cost = cost;
        for (int i = 0; i < 80; i++) {
            channel[i] = -1;
        }
    }
};
class edge {
   public:
    int nodeNo1;
    int nodeNo2;
    vector<int> p;
    set<int> PossibleChannel;
    edge() {}
    void initedge(int nodeNo1, int nodeNo2) {
        this->nodeNo1 = nodeNo1;
        this->nodeNo2 = nodeNo2;
        PossibleChannel.clear();
        for (int i = 0; i < T; i++) {
            PossibleChannel.insert(i);
        }
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
    int channel;
	vector<bool> possibleChannel;
    task() {}
    void inittask(int id, int from, int to, int power) {
        possibleChannel.resize(T);
        this->id = id;
        this->from = from;
        this->to = to;
        this->power = power;
        channel = -1;
        for (int i = 0; i < T; i++) possibleChannel[i] = true;
    }
};
task tasks[10005];
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
    int id;
    vector<int> pointSet;
    vector<int> bridges;
    island(int id = -1) { this->id = id; }
    bool isInIsland(int node) {
        for (int i = 0; i < pointSet.size(); i++) {
            if (node == pointSet[i])
                return true;
        }
        return false;
    }
    int getBridge(int goal) {
        for (int i = 0; i < bridges.size(); i++) {
            int edgeId = bridges[i];
            if (anotherIsland(edges[edgeId], id) == goal) {
                return i;
            }
        }
        return -1;
    }
};
class treeNode {
   public:
    treeNode() { parent = -1; }
    int parent;
    int deepth;  // used as the record of the edge in bfs_find_road
};

// task tasks[10005];
vector<pipe> pipes;
node nodes[5005];

vector<island> islands;
vector<int> bridges;

treeNode tree[5000];
treeNode islandTree[5000];
bool nodeFlag[5000];
bool edgeFlag[5000];
bool notBridge[5000];

int anotherIsland(edge& e, int islandId) {
    return nodes[e.nodeNo1].islandId == islandId ? nodes[e.nodeNo2].islandId
                                                 : nodes[e.nodeNo1].islandId;
}
void creatTree() {
    // init
    for (int i = 0; i < 5000; i++) {
        nodeFlag[i] = false;
        edgeFlag[i] = false;
        notBridge[i] = false;
    }
    // creat tree by bfs
    // cout << "\n******creat tree start:******" << endl;
    queue<int> q;
    for (int i = 0; i < N; i++) {
        if (nodeFlag[i] == false) {
            q.push(i);
            nodeFlag[i] = true;
            while (!q.empty()) {
                int parent = q.front();
                // cout << "now node id is :" << parent << endl;
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
                        // cout << "child: " << child
                        //  << " deepth: " << tree[child].deepth << endl;
                    }
                }
            }
        }
    }
    // cout << "******end tree creat******" << endl;
}
void findBridge() {
    // judge bridge by find circle
    // add a edge to tree for a circle
    // cout << "\n******start find bridge:******" << endl;
    for (int i = 0; i < E; i++) {
        if (!edgeFlag[i]) {
            // cout << "edge id:" << i;
            int nodea = edges[i].nodeNo1;
            int nodeb = edges[i].nodeNo2;
            // cout << "  node:" << nodea << "  " << nodeb << endl;
            notBridge[nodes[nodea].getEdge(nodeb)] = true;
            if (tree[nodea].deepth > tree[nodeb].deepth) {
                swap(nodea, nodeb);
            }
            while (tree[nodea].deepth != tree[nodeb].deepth) {
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
                // cout << "new nodeb: " << nodeb << endl;
            }
            while (nodea != nodeb) {
                notBridge[nodes[nodea].getEdge(tree[nodea].parent)] = true;
                nodea = tree[nodea].parent;
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
                // cout << "new nodea&nodeb: " << nodea << " " << nodeb << endl;
            }
            // cout << "LCA: " << nodea << endl;
        }
    }
    for (int i = 0; i < E; i++) {
        if (notBridge[i] == false) {
            bridges.push_back(i);
            // cout << "bridge id :" << i<<endl;
            // cout << "bridge:" << edges[i].nodeNo1 << " " << edges[i].nodeNo2
            //<< endl;
        }
    }
    // cout << "******end of find bridge******" << endl;
}
void divideNode() {
    // cout << "\n******strat divide node******" << endl;
    //  init flags
    for (int i = 0; i < N; i++) {
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
        // cout << "bridge id: " << i << endl;
        // cout << "bridge node: " << node[0] << node[1] << endl;
        for (int k = 0; k < 2; k++) {
            // cout << "begining node: " << node[k] << endl;
            if (nodes[node[k]].islandId != -1) {
                // the node has been in a island
                // cout << "node has been in island:" <<
                // nodes[node[k]].islandId;
                islands[nodes[node[k]].islandId].bridges.push_back(bridges[i]);
                continue;
            }
            int root = node[k];
            nodeFlag[root] = true;
            nodes[root].islandId = I;
            island tem(I);
            islands.push_back(tem);
            I++;
            // cout << "find the new island, id:" << endl;
            //  search point by bfs
            queue<int> q;
            islands[I - 1].pointSet.push_back(root);
            // cout << root << "  ";
            islands[I - 1].bridges.push_back(bridges[i]);
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
                        islands[I - 1].pointSet.push_back(child);
                        nodes[child].islandId = I - 1;
                        // cout << child << "  ";
                        nodeFlag[child] = true;
                    }
                }
            }
            // cout << endl;
        }
    }
    // cout << "******end of dividing node******" << endl;
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

void creatBridgeTree() {
    vector<bool> islandFlag(islands.size(), false);
    queue<int> q;
    for (int i = 0; i < islands.size(); i++) {
        if (islandFlag[i] == false) {
            q.push(i);
            islandFlag[i] = true;
            while (!q.empty()) {
                int parent = q.front();
                q.pop();
                for (int j = 0; j < islands[parent].bridges.size(); j++) {
                    int idEdge = islands[parent].bridges[j];
                    int child = anotherIsland(edges[idEdge], parent);
                    if (!islandFlag[child]) {
                        q.push(child);
                        islandTree[child].parent = parent;
                        islandTree[child].deepth =
                            islandTree[parent].deepth + 1;
                        islandFlag[child] = true;
                    }
                }
            }
        }
    }
}

list<int> findPathIsland(task& t) {
    int start = nodes[t.from].islandId;
    int end = nodes[t.to].islandId;
    list<int> first;
    list<int> second;
    if (start == end) {
        return first;
    }
    while (islandTree[start].deepth > islandTree[end].deepth) {
        int next = islandTree[start].parent;
        first.push_back(islands[start].getBridge(next));
        start = next;
    }
    while (islandTree[start].deepth < islandTree[end].deepth) {
        int next = islandTree[end].parent;
        second.push_front(islands[end].getBridge(next));
        end = next;
    }
    while (start != end) {
        int next = islandTree[start].parent;
        first.push_back(islands[start].getBridge(next));
        start = next;
        next = islandTree[end].parent;
        second.push_front(islands[end].getBridge(next));
        end = next;
    }
    first.splice(first.end(), second, second.begin(), second.end());
    cout << "the list of bridge" << endl;
    for (list<int>::iterator i = first.begin(); i != first.end(); i++) {
        cout << "id:" << *i << " from-to " << edges[bridges[*i]].nodeNo1 << "-"
             << edges[bridges[*i]].nodeNo2 << endl;
    }
    return first;
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
        if(di>D) continue;
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

void Finded_Path(task t,vector<bool> possible_channel) {

}

int Channel_Communicate(vector<bool> possible_channel, int edgeno, int turns) { 
    for (int i = 0; i < T; i++) {
        if (possible_channel[i] = true) {
            edge tempe = edges[edgeno];
            for (int j = 0; j < tempe.p.size(); j++) {
                task tempt = tasks[pipes[tempe.p[j]].channel[i]];//肯定被占用了，因此肯定有这个任务
                if (!tempt.possibleChannel.empty()) {
                    tempt.channel = tempt.possibleChannel[0];
                    Update_channel(tempt, tempt.channel);
                    return tempt.channel;
                }
                /*else if(turns>1){
                    Channel_Communicate(, edgeno, turns - 1);
                }*/
            }
        }
    }
    return -1;
}

int Add_Pipe(vector<int> blocknode, vector<vector<bool> >block_possible_channel) {

    return 1;
}

void bfs_Find_Path(task t) {
    memset(nodeFlag, false, T * sizeof(bool));
    queue<int> q;//BFS节点队列
    vector<int> blocknode;//阻塞节点记录
    vector<vector<bool> > blocknode_possible_channel;//到阻塞节点处可用信道记录
    q.push(t.from);
    queue<vector<bool> > qpossible;  // BFS节点可用信道队列
    qpossible.push(t.possibleChannel);
    tree[t.from].parent = -1;
    nodeFlag[t.from] = true;
    //加死循环直到找到路径？
    while (true) {
        while (!q.empty()) {
            vector<int> tempe = nodes[q.front()].e;
            vector<bool> tempp = qpossible.front();//记录走到当前位置还可用的信道
            int have_edge = 1;
            for (int i = 0; i < tempe.size(); i++) {
                if (nodeFlag[edges[tempe[i]].another(q.front())]) {

                    continue;
                }
                //else if (edges[tempe[i]].PossibleChannel.size() == 0) {
                //    if (Channel_Communicate(tempp, i, 1) < 0) {//当前可用信道、边、协商伦次（深度）
                //        //如果协商失败
                //        blocknode.push_back(tempe[i]);
                //        blocknode_possible_channel.push_back(qpossible.front());
                //        q.pop();
                //        qpossible.pop();
                //    }
                //    else {
                //        //协商成功
                //    }
                //}
                else {
                    bool hava_channel = false;

                    for (int j = 0; j < T; j++) {
                        if ((qpossible.front())[i] = (qpossible.front())[i] & edges[tempe[i]].PossibleChannel.count(i))
                            hava_channel = true;
                    }
                    if (hava_channel) {
                        int next = edges[tempe[i]].another(q.front());
                        tree[next].parent = q.front();
                        tree[next].deepth = i;
                        if (next == t.to) {
                            Finded_Path(t, qpossible.front());
                            return;
                        }
                        else {
                            have_edge = 2;
                            q.push(next);
                            vector<bool> temp = qpossible.front();
                            qpossible.push(temp);
                            nodeFlag[next] = true;
                        }
                    }
                    else {
                        if (Channel_Communicate(tempp, tempe[i], 1) < 0) {//当前可用信道、边、协商伦次（深度）
                        //如果协商失败
                            if (have_edge == 1) have_edge = 0;
                        }
                        else {
                            //协商成功
                        }
                    }
                }

            }
            if (!have_edge) {
                blocknode.push_back(q.front());
                blocknode_possible_channel.push_back(tempp);
            }
            q.pop();
            qpossible.pop();
        }
        //当队列为空但是没有退出本函数说明没有找到目的地的路径，执行加边
        int add_pipe_node_index = Add_Pipe(blocknode, blocknode_possible_channel);//返回加边阻塞节点在vector中的下标
        //加边后从加了边的堵塞节点恢复搜索}
        q.push(blocknode[add_pipe_node_index]);
        qpossible.push(blocknode_possible_channel[add_pipe_node_index]);
        
    }

}
int main() {
    initData();
    creatTree();
    findBridge();
    divideNode();
    creatBridgeTree();
    /*
    cout << "\nislandtree\n";
    for (int i = 0; i < islands.size(); i++) {
        cout << islandTree[i].parent << islandTree[i].deepth << endl;
    }
    cout << "\ntest dividenode********" << endl;
    for (int i = 0; i < islands.size(); i++) {
        cout << "island id: " << i << islands[i].id << endl;
        testPrint(islands[i]);
        cout << endl;
    }
    */
    for (int i = 0; i < T; i++) {
        cout << "task" << i << endl;
        findPathIsland(tasks[i]);
    }
    cout << "\ntest bfs_find_path********" << endl;
    for (int i = 0; i < P; i++)
        bfs_Find_Path(tasks[i]);
}