//图的邻接表存储结构
#include <iostream>
#define OK 0
#define ERROR 1
using namespace std;
#define MAX_VERTEX_NUM  20//最大顶点数
typedef int Status;
typedef enum {DG,DN,UDG,UDN} GraphKind;  //图的类型
typedef char VertexType; //顶点数据的类型(字符） 

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




Status CreateDG(ALGraph &G, int vexnum, int arcnum, char *vexs, int *arcs) {
    //创建有向图的邻接表存储结构
       int i, k;
       ArcNode *p;
       for(k = 0; k < G.arcnum; k++){
              i = arcs[k*2]; 
              int j = arcs[k*2+1];
              p = new ArcNode;
              p->adjvex = j; //边的终点
              p->nextarc = G.vertices[i].firstarc; //头插法
              G.vertices[i].firstarc = p;
       }
       return OK;
}

Status CreateDN(ALGraph &G, int vexnum, int arcnum, char *vexs, int *arcs) {
    //创建有向网的邻接表存储结构
       int i, k;
       ArcNode *p;
       for(k = 0; k < G.arcnum; k++){
              i = arcs[k*3];
              int j = arcs[k*3+1];
              p = new ArcNode;
              p->adjvex = j;
              p->weight = arcs[k*3+2];
              p->nextarc = G.vertices[i].firstarc;
              G.vertices[i].firstarc = p;
       }
       return OK;
}

Status CreateUDG(ALGraph &G, int vexnum, int arcnum, char *vexs, int *arcs) {
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

Status CreateUDN(ALGraph &G, int vexnum, int arcnum, char *vexs, int *arcs) {
    //创建无向网的邻接表存储结构
       int i, k;
       ArcNode *p;
       for(k = 0; k < G.arcnum; k++){
              i = arcs[k*3];
              int j = arcs[k*3+1];
              p = new ArcNode;
              p->adjvex = j; //边的终点
              p->weight = arcs[k*3+2];
              p->nextarc = G.vertices[i].firstarc;
              G.vertices[i].firstarc = p;
              p = new ArcNode;
              p->adjvex = i;
              p->weight = arcs[k*3+2];
              p->nextarc = G.vertices[j].firstarc;
              G.vertices[j].firstarc = p;
       }
       return OK;
}

Status UnitALGraph( GraphKind GKind, ALGraph &G, int vexnum, int arcnum, char *vexs, int *arcs) {
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


Status CreateALGraph(GraphKind GKind, ALGraph &G, int vexnum, int arcnum, char *vexs, int* arcs) {
    //创建4种不同类型图的邻接表存储结构
       UnitALGraph(GKind,G, vexnum, arcnum, vexs, arcs);
       switch(G.kind){
              case DG: return CreateDG(G, vexnum, arcnum, vexs, arcs);
              case DN: return CreateDN(G, vexnum, arcnum, vexs, arcs);
              case UDG: return CreateUDG(G, vexnum, arcnum, vexs, arcs);
              case UDN: return CreateUDN(G, vexnum, arcnum, vexs, arcs);
              default: return ERROR;
       }
       return OK;
}


Status OutALGraph(ALGraph G){
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

Status VerDegree(ALGraph G) {
    if (G.kind == UDG || G.kind == UDN) {
        int i, count;
        ArcNode *p;
        for (i = 0; i < G.vexnum; i++) {
            count = 0;
            p = G.vertices[i].firstarc;
            while (p) {
                count++;
                p = p->nextarc;
            }
            cout << G.vertices[i].data << " 的度为 " << count << endl;
        }
    } else if (G.kind == DG || G.kind == DN) {
        int indegree[MAX_VERTEX_NUM] = {0}, outdegree[MAX_VERTEX_NUM] = {0};
        ArcNode *p;
        for (int i = 0; i < G.vexnum; i++) {
            p = G.vertices[i].firstarc; 
            while (p) {
                outdegree[i]++;
                indegree[p->adjvex]++;
                p = p->nextarc;
            }
        }
        for (int i = 0; i < G.vexnum; i++) {
            cout << G.vertices[i].data << " 的入度为 " << indegree[i]
                 << "，出度为 " << outdegree[i] << endl;
        }
    } else {
        cout << "图类型无效！" << endl;
        return ERROR;
    }
    return OK;
}


void DestroyALGraph(ALGraph &G) {
    for (int i = 0; i < G.vexnum; i++) {
        ArcNode *p = G.vertices[i].firstarc;
        while (p) {
            ArcNode *temp = p;
            p = p->nextarc;
            delete temp;
        }
    }
}

int main(){
       ALGraph G1,G2,G3,G4;

       int vexnum1=4,arcnum1=5;
       char vexs1[]={'v0','v1','v2','v3'};
       int arcs1[5][2]={{0,1},{0,2},{1,2},{0,3},{2,3}};
       CreateALGraph(UDG, G1, vexnum1, arcnum1, vexs1, (int*)arcs1);
       cout<<"G1:"<<endl;
       OutALGraph(G1);
       VerDegree(G1);

       int vexnum2=4,arcnum2=6;
       char vexs2[]={'v0','v1','v2','v3'};
       int arcs2[6][2]={{0,1},{1,0},{1,2},{2,0},{2,1},{3,1}};
       CreateALGraph(DG, G2, vexnum2, arcnum2, vexs2,(int*) arcs2); 
       cout<<"G2:"<<endl;
       OutALGraph(G2);
       VerDegree(G2);

       int vexnum3=4,arcnum3=4;
       char vexs3[]={'v0','v1','v2','v3'};
       int arcs3[4][3]={{0,1,56},{0,2,34},{0,3,78},{2,3,25}};
       CreateALGraph(UDN, G3, vexnum3, arcnum3, vexs3,(int*) arcs3);
       cout<<"G3:"<<endl;
       OutALGraph(G3);
       VerDegree(G3);

       int vexnum4=3,arcnum4=3;
       char vexs4[]={'v0','v1','v2'};
       int arcs4[3][3]={{0,2,50},{2,0,64},{1,2,45}};
       CreateALGraph(DN, G4, vexnum4, arcnum4, vexs4,(int*) arcs4);
       cout<<"G4:"<<endl;
       OutALGraph(G4);
       VerDegree(G4);

       DestroyALGraph(G1);
       return 0;


}