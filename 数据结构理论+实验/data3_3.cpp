//图的遍历实现
#include <iostream>
#define OK 0;
#define ERROR 1;
using namespace std;
#define MAX_VERTEX_NUM  20//最大顶点数
typedef int Status;
typedef enum {DG,DN,UDG,UDN} GraphKind;  //图的类型
typedef char* VertexType; //顶点数据的类型(字符） 
//边(弧)结点数据类型 
typedef struct ArcNode {
       int             adjvex ;       //该边(弧)所依附(指向)的顶点在顶点数组的下标
       struct ArcNode *nextarc;       //指向下一条边(弧)的指针
       int            weight;        //边(弧)的权值，无权图其值为0
}ArcNode;
//顶点数组数据类型 
typedef struct VNode {
       VertexType  data;          //顶点数据 
       ArcNode  *firstarc;         //指向第一条依附该顶点的边(弧)的指针
}VNode, AdjList[MAX_VERTEX_NUM];

//图的邻接表数据类型 
typedef  struct {
       AdjList   vertices;           //图的顶点数组 
       int       vexnum, arcnum;     //图的顶点数和边(弧)数 
       GraphKind kind;               //图的类型 
}ALGraph;


Status CreateALGraph(ALGraph &G, int vexnum, int arcnum, char *vexs[], int *arcs) {
    //创建无向图的邻接表存储结构
       int i, k;
       ArcNode *p;
       for(k = 0; k < G.arcnum; k++){
              i = arcs[k*2];
              int j = arcs[k*2+1];
              p = new ArcNode;
              p->adjvex = j;
              p->nextarc = G.vertices[i].firstarc;
              G.vertices[i].firstarc = p;
              p = new ArcNode;
              p->adjvex = i;
              p->nextarc = G.vertices[j].firstarc;
              G.vertices[j].firstarc = p;
       }
       return OK;
}

Status UnitALGraph( GraphKind GKind, ALGraph &G, int vexnum, int arcnum, char *vexs[], int *arcs) {
       //初始化图的邻接表存储结构
       int i, k;
       ArcNode *p;
       G.vexnum = vexnum;
       G.arcnum = arcnum;
       G.kind = GKind;
       for(i = 0; i < G.vexnum; i++){
              G.vertices[i].data = vexs[i];
              G.vertices[i].firstarc = NULL;
       }
       return OK;
}

Status OutALGraph(ALGraph G){
       //输出图的邻接表存储结构
       int i;
       ArcNode *p;
       for(i=0;i<G.vexnum;i++){
              cout<<G.vertices[i].data;
              p = G.vertices[i].firstarc;
              while(p){
                     cout<<"->"<<G.vertices[p->adjvex].data;
                     p = p->nextarc;
              }
              cout<<endl;
       }       
       return OK;
}

Status VerDegree(ALGraph G){
       //求图的顶点度
       int i, count;
       ArcNode *p;
       for(i=0;i<G.vexnum;i++){
              count = 0;
              p = G.vertices[i].firstarc;
              while(p){
                     count++;
                     p = p->nextarc;
              }
              cout<<G.vertices[i].data<<"的度为"<<count<<endl;
       }
}


Status DFSTraverse(ALGraph G, int i){
       //深度优先遍历
       ArcNode *p;
       static bool visited[MAX_VERTEX_NUM] = {false};
       visited[i] = true;
       cout<<G.vertices[i].data<<" ";
       p = G.vertices[i].firstarc;
       while(p){
              if(!visited[p->adjvex])
              DFSTraverse(G, p->adjvex);
              p = p->nextarc;
       }
       return OK;
}

Status BFSTraverse(ALGraph G, int i){
       //广度优先遍历
       ArcNode *p;
       int queue[MAX_VERTEX_NUM], front = 0, rear = 0;
       static bool visited[MAX_VERTEX_NUM] = {false};
       cout<<G.vertices[i].data<<" ";
       visited[i] = true;
       queue[rear++] = i;
       while(front!=rear){ //队列不空
              i = queue[front++];
              p = G.vertices[i].firstarc; //找到第一个邻接点
              while(p){
                     if(!visited[p->adjvex]){ //未访问过
                            cout<<G.vertices[p->adjvex].data<<" ";
                            visited[p->adjvex] = true;
                            queue[rear++] = p->adjvex;
                     }
                     p = p->nextarc; //找下一个邻接点
              }
       }
       return OK;
}

int main(){
       ALGraph G;
       char *vexs[]={"c0","c1","c2","c3","c4","c5"};
       int arcs[][2]={{0,1},{0,2},{1,3},{1,4},{2,5},{3,4},{4,5}};
       UnitALGraph(UDG, G, 6, 7, vexs, (int *)arcs);
       CreateALGraph(G, 6, 7, vexs, (int *)arcs);
       OutALGraph(G);
       cout<<endl;
       VerDegree(G);
       cout<<endl;
       cout<<"深度优先遍历结果：";
       DFSTraverse(G, 0);
       cout<<endl;
       cout<<"广度优先遍历结果：";
       BFSTraverse(G, 0);
       cout<<endl;
       return 0; 

}