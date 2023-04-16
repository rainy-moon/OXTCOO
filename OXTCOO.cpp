#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>
using namespace std;

int N, M, T = 80, P, D;  // 节点数量、管道数量、任务数量、信道数量、衰减距离
int E = 0;  // 总边数
int I = 0;  // 岛数量

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
    int edgeId;
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
        for (int i = 0; i < P; i++) {
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
    vector<int> path;
    vector<bool> possibleChannel;
    vector<int> amplifier;
    task() {}
    void inittask(int id, int from, int to, int power) {
        possibleChannel.resize(P);
        this->id = id;
        this->from = from;
        this->to = to;
        this->power = power;
        channel = -1;
        for (int i = 0; i < P; i++)
            possibleChannel[i] = true;
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
// find bridge
void creatTree() {
    // init
    for (int i = 0; i < 5000; i++) {
        nodeFlag[i] = false;
        edgeFlag[i] = false;
        notBridge[i] = false;
    }
    // creat tree by bfs
    queue<int> q;
    for (int i = 0; i < N; i++) {
        if (nodeFlag[i] == false) {
            q.push(i);
            nodeFlag[i] = true;
            while (!q.empty()) {
                int parent = q.front();
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
                    }
                }
            }
        }
    }
}
void findBridge() {
    // judge bridge by find circle
    // add a edge to tree for a circle
    for (int i = 0; i < E; i++) {
        if (!edgeFlag[i]) {
            // cout << "edge id:" << i;
            int nodea = edges[i].nodeNo1;
            int nodeb = edges[i].nodeNo2;
            notBridge[nodes[nodea].getEdge(nodeb)] = true;
            if (tree[nodea].deepth > tree[nodeb].deepth) {
                swap(nodea, nodeb);
            }
            while (tree[nodea].deepth != tree[nodeb].deepth) {
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
            }
            while (nodea != nodeb) {
                notBridge[nodes[nodea].getEdge(tree[nodea].parent)] = true;
                nodea = tree[nodea].parent;
                notBridge[nodes[nodeb].getEdge(tree[nodeb].parent)] = true;
                nodeb = tree[nodeb].parent;
            }
        }
    }
    for (int i = 0; i < E; i++) {
        if (notBridge[i] == false) {
            bridges.push_back(i);
        }
    }
}
void divideNode() {
    //  init flags
    memset(nodeFlag, false, N * sizeof(bool));
    memset(edgeFlag, false, E * sizeof(bool));
    for (int i = 0; i < bridges.size(); i++) {
        edgeFlag[bridges[i]] = true;
    }
    // divide node by bfs
    for (int i = 0; i < bridges.size(); i++) {
        int node[2] = {edges[bridges[i]].nodeNo1, edges[bridges[i]].nodeNo2};
        for (int k = 0; k < 2; k++) {
            if (nodes[node[k]].islandId != -1) {
                // the node has been in a island
                islands[nodes[node[k]].islandId].bridges.push_back(bridges[i]);
                continue;
            }
            int root = node[k];
            nodeFlag[root] = true;
            nodes[root].islandId = I;
            island tem(I);
            islands.push_back(tem);
            I++;
            //  search point by bfs
            queue<int> q;
            islands[I - 1].pointSet.push_back(root);
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
                        nodeFlag[child] = true;
                    }
                }
            }
        }
    }
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
// search road between islands
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
list<int> findPathIsland(task& t, list<int>& first) {
    int start = nodes[t.from].islandId;
    int end = nodes[t.to].islandId;
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
// input
void initData() {
    scanf("%d %d %d %d %d", &N, &M, &T, &P, &D);
    for (int i = 0; i < N; i++) {
        nodes[i].initnode(i);
    }
    pipes.resize(M);
    for (int i = 0; i < M; i++) {
        int si, ti, di;
        scanf("%d %d %d", &si, &ti, &di);
        if (di > D)
            continue;
        pipes[i].initpipe(i, di);
        int get_edge = nodes[si].getEdge(ti);
        if (get_edge == -1) {
            edges[E].initedge(si, ti);
            pipes[i].edgeId = E;
            edges[E].p.push_back(i);
            nodes[si].e.push_back(E);
            nodes[ti].e.push_back(E++);
        } else {
            edges[get_edge].p.push_back(i);
        }
    }
    for (int i = 0; i < T; i++) {
        int sj, tj;
        scanf("%d %d", &sj, &tj);
        tasks[i].inittask(i, sj, tj, D);
    }
}

void broadcastEdge(edge& e, int &newc){
    bool flag = false;
    for (int j = 0; j < e.p.size(); j++) {
        if (pipes[e.p[j]].channel[newc] == -1) {
            flag = true;
            break;
        }
    }
    if(!flag){
        e.PossibleChannel.erase(newc);
        for (vector<int>::iterator i = e.p.begin(); i != e.p.end(); i++){
            for (int j = 0; j < P;j++){
                if (pipes[*i].channel[j] == -1){
                    continue;
                }
                tasks[pipes[*i].channel[j]].possibleChannel[newc] = false;
            }
        }
    } 
}

void Update_channel(task& t, int& channel) {
    // 协商后更新信道 
    int oldChannel = t.channel;
    t.channel = channel;
    for (int i = 0; i < t.path.size(); i++) {
        pipes[i].channel[oldChannel] = -1;
        pipes[i].channel[channel] = t.id;
        edge &temEdge = edges[pipes[i].edgeId];
        broadcastEdge(temEdge, channel);
    }
}

void Finded_Path(task& t, vector<bool> possible_channel) {
    // 更新可能信道
    t.possibleChannel = possible_channel;
    // 更新路径
    stack<int> s;
    int nowStep = t.to;
    while (nowStep != t.from) {
        s.push(tree[nowStep].deepth);
        nowStep = tree[nowStep].parent;
    }
    while (!s.empty()) {
        t.path.push_back(s.top());
        s.pop();
    }
}

int Channel_Communicate(vector<bool> possible_channel, int edgeno, int turns) {
    for (int i = 0; i < P; i++) {
        if (possible_channel[i] == true) {
            edge tempe = edges[edgeno];
            for (int j = 0; j < tempe.p.size(); j++) {
                task& tempt = tasks[pipes[tempe.p[j]].channel[i]];  // 肯定被占用了，因此肯定有这个任务
                for (int t = 0; t < P; t++) {
                    if (tempt.possibleChannel[t] && t != i) {
                        Update_channel(tempt, t);
                        return tempt.channel;
                    }
                }
                /*
                if (!tempt.possibleChannel.empty()) {
                    tempt.channel = tempt.possibleChannel[0];
                    Update_channel(tempt, tempt.channel);
                    return tempt.channel;
                }*/
                /*else if(turns>1){
                    Channel_Communicate(, edgeno, turns - 1);
                }*/
            }
        }
    }
    return -1;
}

int Add_Pipe(int& choosed_blocknode,
             vector<int> blocknode,
             vector<vector<bool> > block_possible_channel) {
    //? 排除nodeflag标记过的节点
    //? 连通度是以edge算的（没有排除已经被走过的节点？）

    //&修改新加边状态
    int bestNode = edges[nodes[blocknode[0]].e[0]].another(blocknode[0]);
    int thisNode = blocknode[0];
    int edgeId;
    for (int i = 0; i < blocknode.size(); i++) {
        for (vector<int>::iterator j = nodes[blocknode[i]].e.begin(); j != nodes[blocknode[i]].e.end(); j++) {
            int nextNode = edges[*j].another(blocknode[i]);
            if (!nodeFlag[nextNode] &&
                nodes[nextNode].e.size() > nodes[bestNode].e.size()) {
                bestNode = nextNode;
                thisNode = blocknode[i];
                edgeId = *j;
            }
        }
    }
    pipe newpipe;
    edgeId = nodes[thisNode].getEdge(bestNode);
    edges[edgeId].p.push_back(M);
    for (int i = 0; i < P; i++) {
        edges[edgeId].PossibleChannel.insert(i);
    }
    pipes.push_back(newpipe);
    int best_cost = D;
    for (int i = 0; i < edges[edgeId].p.size(); i++) {
        if (best_cost > pipes[edges[edgeId].p[i]].cost) {
            best_cost = pipes[edges[edgeId].p[i]].cost;
        }
    }
    newpipe.initpipe(M, best_cost);
    M++;
    tree[bestNode].parent = thisNode;
    tree[bestNode].deepth = edgeId;
    choosed_blocknode = thisNode;
    return bestNode;
}

void bfs_Find_Path(task& t) {
    cout << "task id: " << t.id << endl;
    if(t.from == t.to){
        return;
    }
    memset(nodeFlag, false, N * sizeof(bool));
    queue<int> q;           // BFS节点队列
    vector<int> blocknode;  // 阻塞节点记录
    vector<vector<bool> >
        blocknode_possible_channel;  // 到阻塞节点处可用信道记录
    q.push(t.from);
    queue<vector<bool> > qpossible;  // BFS节点可用信道队列
    qpossible.push(t.possibleChannel);
    tree[t.from].parent = -1;
    nodeFlag[t.from] = true;
    // 加死循环直到找到路径？
    while (true) {
        while (!q.empty()) {
            vector<int> tempe = nodes[q.front()].e;
            vector<bool> tempp =
                qpossible.front();  // 记录走到当前位置还可用的信道
            int have_edge = 1;
            for (int i = 0; i < tempe.size(); i++) {
                qpossible.front() = tempp;
                if (nodeFlag[edges[tempe[i]].another(q.front())]) {
                    continue;
                }
                else {
                    bool hava_channel = false;
                    for (int j = 0; j < P; j++) {
                        qpossible.front()[j] = qpossible.front()[j] & edges[tempe[i]].PossibleChannel.count(j);
                        if (qpossible.front()[j])
                            hava_channel = true;
                    }
                    if (hava_channel) {
                        int next = edges[tempe[i]].another(q.front());
                        tree[next].parent = q.front();
                        tree[next].deepth = tempe[i];
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
                    } else {
                        int communicate_channel =
                            Channel_Communicate(tempp, tempe[i], 1);
                        if (communicate_channel <
                            0) {  // 当前可用信道、边、协商伦次（深度）
                            // 如果协商失败
                            if (have_edge == 1)
                                have_edge = 0;
                        } else {
                            // 协商成功
                            qpossible.front()[communicate_channel] = true;
                            int next = edges[tempe[i]].another(q.front());
                            tree[next].parent = q.front();
                            tree[next].deepth = tempe[i];
                            if (next == t.to) {
                                Finded_Path(t, qpossible.front());
                                return;
                            } else {
                                have_edge = 2;
                                q.push(next);
                                vector<bool> temp = qpossible.front();
                                qpossible.push(temp);
                                nodeFlag[next] = true;
                            }
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
        // 当队列为空但是没有退出本函数说明没有找到目的地的路径，执行加边
        int choosed_blocknodeId;
        int nextNode = Add_Pipe(
            choosed_blocknodeId, blocknode,
            blocknode_possible_channel);  // 返回加边阻塞节点在vector中的下标
        // 加边后从加了边的堵塞节点恢复搜索}
        q.push(nextNode);
        qpossible.push(blocknode_possible_channel[choosed_blocknodeId]);
    }
}

void divideTask(task& t) {
    list<int> b;
    findPathIsland(t, b);
    int nowStep = t.from;
    int begin = nowStep;
    int goal = t.to;
    for (list<int>::iterator i = b.begin(); i != b.end(); i++) {
        int edgeId = bridges[*i];
        t.from = nowStep;
        t.to = nodes[edges[edgeId].nodeNo1].islandId == nodes[nowStep].islandId
                   ? edges[edgeId].nodeNo1
                   : edges[edgeId].nodeNo2;
        bfs_Find_Path(t);
        // 过桥
        t.from = t.to;
        t.to = edges[edgeId].another(t.to);
        bfs_Find_Path(t);
        nowStep = t.to;
    }
    t.from = nowStep;
    t.to = goal;
    bfs_Find_Path(t);
    // 更新
    for (int i = 0; i < P; i++) {
        if (t.possibleChannel[i]) {
            t.channel = i;
            break;
        }
    }
    // 加放大器并广播
    int nowNode = begin;
    for (int i = 0; i < t.path.size(); i++) {
        int edgeId = t.path[i];
        int pipeId;
        // 广播并选pipe
        for (int j = 0; j < edges[edgeId].p.size(); j++) {
            if (pipes[edges[edgeId].p[j]].channel[t.channel] == -1) {
                pipeId = edges[edgeId].p[j];
            }
        }
        pipes[pipeId].channel[t.channel] = t.id;
        broadcastEdge(edges[pipes[pipeId].edgeId], t.channel);
        t.path[i] = pipeId;
        // 放大器
        if (t.power - pipes[pipeId].cost < 0) {
            t.amplifier.push_back(nowNode);
            t.power = D - pipes[pipeId].cost;
        } else {
            t.power = t.power - pipes[pipeId].cost;
        }
        nowNode = edges[edgeId].another(nowNode);
    }
    cout << endl;
}

int main() {
    initData();
    creatTree();
    findBridge();
    divideNode();
    creatBridgeTree();
    /*
    for (int i = 0; i < T; i++) {
        cout << "task" << i << endl;
        list<int> t;
        findPathIsland(tasks[i],t);
    }*/

    /* task& t = tasks[0];
     t.from = 5;
     t.to = 4;*/
    cout << "\ntest bfs_find_path********" << endl;
    for (int i = 0; i < T; i++){
        divideTask(tasks[i]);
    }
    for (int i = 0; i < T; i++) {
        cout << "task id: " << i << endl;
        for (int j = 0; j < tasks[i].path.size(); j++){
            cout << tasks[i].path[j] << " ";
        }
        cout << endl;
    }
    /* for (int i = 0; i < T; i++)
             bfs_Find_Path(tasks[i]);*/
}