//最小生成树
#include <iostream>
using namespace std;
#define INFINITY 32767                   //表示最大值∞
#define MAX_VERTEX_NUM  20      //最大顶点数
#define MAX_EDGE_NUM  20         //最大边数
#define OK 0
#define ERROR -1


typedef int Status;
typedef enum {DG,DN,UDG,UDN} GraphKind;  //图的枚举类型
typedef int VRType;
typedef char InfoType;  

typedef struct ArcCell { 
        VRType  adj ;          //对无权图用1或0表示；对带权图，则为权值类型。
        InfoType *info;       //该边或弧相关信息的指针（字符串） 
} ArcCell, AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

typedef char VertexType; //顶点数据的类型(字符）

typedef struct{

        VertexType vexs[MAX_VERTEX_NUM];   //顶点数组，存放顶点信息
        AdjMatrix   arcs;                  //邻接矩阵
        int vexnum, arcnum;           //图中顶点总数与弧数
        GraphKind  kind;                 //图的种类标志
} MGraph;      


Status CreateMGraph(GraphKind GKind, MGraph &G, int vexnum, int arcnum, const char *vexs, int arcs[][3]) {
    //创建图的邻接矩阵存储结构
    int i, j, k;
    if (vexnum > MAX_VERTEX_NUM || arcnum > MAX_VERTEX_NUM * (MAX_VERTEX_NUM - 1)) {
        cerr << "Error: Too many vertices or arcs!" << endl;
        return -1;
    }
    G.kind = GKind;
    G.vexnum = vexnum;
    G.arcnum = arcnum;

    for (i = 0; i < G.vexnum; i++) {
        G.vexs[i]=vexs[i]; // 存储顶点信息
    }

    for (i = 0; i < G.vexnum; i++) {  //初始化邻接矩阵,边的权值为∞,边的信息为空
        for (j = 0; j < G.vexnum; j++) {
            G.arcs[i][j].adj = INFINITY;
            G.arcs[i][j].info = NULL;
        }
    }

    for (k = 0; k < G.arcnum; k++) { //存储边的权值
        i = arcs[k][0];
        j = arcs[k][1];
        G.arcs[i][j].adj = arcs[k][2];
        if (G.kind == UDG || G.kind == UDN) {
            G.arcs[j][i].adj = G.arcs[i][j].adj;
        }
    }
    return OK;
}
int LocateVex(MGraph G, VertexType u){
    //返回顶点u在图G中的位置
    for(int i=0;i<G.vexnum;i++){
        if(G.vexs[i]==u){
            return i;
        }
    }
    return -1;
}


Status OutMGraph(MGraph G) { //输出邻接矩阵
    for (int i = 0; i < G.vexnum; i++) {
        cout << G.vexs[i] << " ";
    }
    cout << endl;

    for (int i = 0; i < G.vexnum; i++) { 
        for (int j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j].adj == INFINITY)
                cout << "∞ ";
            else
                cout << G.arcs[i][j].adj << " ";
        }
        cout << endl;
    }
    return OK;
}

Status MiniSpanTree_Prim(MGraph G, VertexType u){
    //最小生成树Prim算法，找到点集U到点集V-U的最小边，直到所有点都在U中
    struct {
        VertexType adjvex;
        VRType lowcost;
    }closedge[MAX_VERTEX_NUM];//辅助数组,记录从顶点集U到V-U的最小边
    int i, j, k, min;
    k = LocateVex(G, u);
    for (j = 0; j < G.vexnum; j++) {
        if (j != k) { //初始化辅助数组
            closedge[j].adjvex = u;
            closedge[j].lowcost = G.arcs[k][j].adj; //将顶点u到其他顶点的边权值存入closedge
        }
        else {
            closedge[j].lowcost = 0;
        }
    }
    for(i=1;i<G.vexnum;i++){
        min=INFINITY;
        k=-1;//记录最小边的下标
        for(j=0;j<G.vexnum;j++){//找到最小边
            if(closedge[j].lowcost>0&&closedge[j].lowcost<min){
                min=closedge[j].lowcost;
                k=j;
            }
        }
        cout<<closedge[k].adjvex<<"->"<<G.vexs[k]<<endl;//输出生成树的边
        closedge[k].lowcost=0;                          //顶点k并入U集
        for(j=0;j<G.vexnum;j++){
            if(G.arcs[k][j].adj<closedge[j].lowcost){   //新顶点并入后重新选择最小边
                closedge[j].adjvex=G.vexs[k];           //修改closedge的值
                closedge[j].lowcost=G.arcs[k][j].adj;
            }
        }
    }
    cout<<endl;
    return OK;
}



int parent[MAX_VERTEX_NUM]; //记录每个元素的父节点

int Find(int x) { //查找根节点
    while (parent[x] != x) {
        x = parent[x];
    }
    return x;
}

void Union(int x, int y) { //合并两个集合
    int rootX = Find(x);
    int rootY = Find(y);
    if (rootX != rootY) {
        parent[rootY] = rootX;
    }
}

Status MiniSpanTree_Kruskal(MGraph G){
    //最小生成树Kruskal算法,找到最小的不成环的边
    for (int i = 0; i < G.vexnum; i++) {//初始化parent数组
        parent[i] = i;
    }
    struct {
        int begin;
        int end;
        int weight;
    }edges[MAX_EDGE_NUM];//辅助数组,记录边的信息
    int i, j, k, n, m;
    k=0;
    for(i=0;i<G.vexnum;i++){ //将边的信息存入数组
        for(j=0;j<i;j++){
            if(G.arcs[i][j].adj!=INFINITY){
                edges[k].begin=i;
                edges[k].end=j;
                edges[k].weight=G.arcs[i][j].adj;
                k++;
            }
        }
    }
    n=G.vexnum;
    m=G.arcnum;
    for(i=0;i<m-1;i++){ //对边按权值排序
        for(j=i+1;j<m;j++){
            if(edges[i].weight>edges[j].weight){
                swap(edges[i],edges[j]);
            }
        }
    }
    for(i=0;i<m;i++){ //对边进行判断，若不成环则输出
        k=Find(edges[i].begin);
        n=Find(edges[i].end);
        if(k!=n){
            cout<<G.vexs[edges[i].begin]<<"->"<<G.vexs[edges[i].end]<<endl;
            Union(k,n);
        }
    }
    return OK;
}

int main(){
    MGraph G;
    const char vex[]={'A','B','C','D','E','F'};
    int arcs[][3]={{0,1,10},{0,2,12},{0,4,15},{1,2,7},{1,3,5},{1,5,6},{2,4,12},{2,5,8},{3,5,6},{4,5,10}};
    CreateMGraph(UDN,G,6,10,vex,arcs);
    cout<<"The original graph is:"<<endl;
    OutMGraph(G);
    cout<<"The minimum spanning tree of the graph is:"<<endl;
    cout<<"Prim:"<<endl;
    MiniSpanTree_Prim(G,'A');

    cout<<"Kruskal:"<<endl;
    MiniSpanTree_Kruskal(G);
    return 0;

}