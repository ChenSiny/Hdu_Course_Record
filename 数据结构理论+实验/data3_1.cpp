//四种图的邻接矩阵表示法
#include <iostream>
#define INFINITY 32767                   //表示最大值
#define MAX_VERTEX_NUM  20       //最大顶点数
using namespace std;
const int OK = 0;
typedef int Status;
typedef enum {DG,DN,UDG,UDN} GraphKind;  //图的枚举类型，分别表示有向图、有向网、无向图、无向网
typedef int VRType; //顶点关系类型
typedef char InfoType;  //弧或边的相关信息类型
typedef struct ArcCell { 
        VRType adj  ;          //对无权图用1或0表示；对带权图，则为权值类型。
        InfoType *info;       //该边或弧相关信息的指针（字符串） 
} ArcCell, AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

typedef char VertexType;
typedef struct{
        VertexType vexs[MAX_VERTEX_NUM];   //顶点数组，存放顶点信息
        AdjMatrix   arcs;                  //邻接矩阵
        int vexnum, arcnum;           //图中顶点总数与弧数
        GraphKind  kind;                 //图的种类标志
} MGraph;      

Status CreateMGraph(GraphKind GKind, MGraph &G, int vexnum, int arcnum, const char *vexs[], int arcs[][3]) {
    int i, j, k;
    if (vexnum > MAX_VERTEX_NUM || arcnum > MAX_VERTEX_NUM * (MAX_VERTEX_NUM - 1)) {
        cout << "Error: Too many vertices or arcs!" << endl;
        return -1;
    }
    G.kind = GKind;
    G.vexnum = vexnum;
    G.arcnum = arcnum;

    for (i = 0; i < G.vexnum; i++) {
        strcpy(&G.vexs[i], vexs[i]); // 存储顶点信息
    }

    for (i = 0; i < G.vexnum; i++) { //初始化邻接矩阵,边的权值为∞,边的信息为空
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

Status OutMGraph(MGraph G) {
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


Status VerDegree(MGraph G){ //遍历求图的顶点度
    int i,j,indegree,outdegree;
    if(G.kind==DG||G.kind==DN){
    if(G.kind==DG){
        cout<<"该图为有向图"<<endl;
    }
    else if(G.kind==DN){
        cout<<"该图为有向网"<<endl;
    }   
    for(i=0;i<G.vexnum;i++){
        indegree=outdegree=0;
        for(j=0;j<G.vexnum;j++){
            if(G.arcs[i][j].adj!=INFINITY){
                outdegree++;
            }
            if(G.arcs[j][i].adj!=INFINITY){
                indegree++;
            }
        }
        cout<<G.vexs[i]<<"的入度为"<<indegree<<"，出度为"<<outdegree<<endl;
    }
    }
    if(G.kind==UDN||G.kind==UDG){
        if(G.kind==UDG){
            cout<<"该图为无向图"<<endl;
        }
        else if(G.kind==UDN){
        cout<<"该图为无向网"<<endl;}
        for(i=0;i<G.vexnum;i++){
            indegree=outdegree=0;
            for(j=0;j<G.vexnum;j++){
                if(G.arcs[i][j].adj!=INFINITY){
                    outdegree++;
                    indegree++;
                }
            }
            cout<<G.vexs[i]<<"的度为"<<indegree<<endl;
        }
    }
    return OK;
}

int main(){
    MGraph G1,G2,G3,G4;

    cout<<"G1"<<endl;
    const char* vexs1[] = {"v0", "v1", "v2", "v3"};
    int arcs1[][3]={{0,1,1},{0,2,1},{0,3,1},{1,2,1},{2,3,1}};
    CreateMGraph(UDG,G1,4,5,vexs1,arcs1);
    OutMGraph(G1);
    VerDegree(G1);

    cout<<"G2"<<endl;
    const char* vexs2[]={"v0", "v1", "v2", "v3"};
    int arcs2[][3]={{0,1,1},{2,0,1},{1,0,1},{1,2,1},{2,1,1},{3,1,1}};
    CreateMGraph(DG,G2,4,6,vexs2,arcs2);
    OutMGraph(G2);
    VerDegree(G2);

    cout<<"G3"<<endl;
    const char* vexs3[]={"v0", "v1", "v2", "v3"};
    int arcs3[][3]={{0,1,56},{0,2,34},{0,3,78},{2,3,25}};
    CreateMGraph(UDN,G3,4,4,vexs3,arcs3);
    OutMGraph(G3);
    VerDegree(G3);


    cout<<"G4"<<endl;
    const char* vexs4[]={"v0", "v1", "v2"};
    int arcs4[][3]={{0,2,50},{2,0,64},{1,2,45}};
    CreateMGraph(DN,G4,3,3,vexs4,arcs4);
    OutMGraph(G4);
    VerDegree(G4);
    return 0;
}

