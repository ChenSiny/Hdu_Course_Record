//关键路径
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 32767                   //表示最大值∞
#define MAX_VERTEX_NUM  20       //最大顶点数

typedef int Status;
typedef enum {DG,DN,UDG,UDN} GraphKind;  //图的枚举类型
typedef int VRType;
typedef char InfoType;  

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


void CreateMGraph(GraphKind GKind, MGraph &G, int vexnum, int arcnum, char *vexs, int *arcs){
	G.vexnum = vexnum;
    G.arcnum = arcnum;
    G.kind = GKind;

    // 初始化顶点数组
    for (int i = 0; i < vexnum; i++) {
        G.vexs[i] = vexs[i];
    }

    // 初始化邻接矩阵
    for (int i = 0; i < vexnum; i++) {
        for (int j = 0; j < vexnum; j++) {
            G.arcs[i][j].adj = INFINITY;  // 初始化为∞
            G.arcs[i][j].info = NULL;
        }
    }

    // 填充邻接矩阵
    for (int k = 0; k < arcnum; k++) {
        int i = arcs[k * 3];     // 边的起点
        int j = arcs[k * 3 + 1]; // 边的终点
        int w = arcs[k * 3 + 2]; // 权值

        // 根据图的类型填充邻接矩阵
        if (GKind == DG || GKind == DN) {  // 有向图
            G.arcs[i][j].adj = w;
        } else if (GKind == UDG || GKind == UDN) {  // 无向图
            G.arcs[i][j].adj = w;
            G.arcs[j][i].adj = w;  // 对称
        }
    }
}

void OutMGraph(MGraph G){
    switch (G.kind) {
        case DG: printf("有向图\n"); break;
        case DN: printf("有向网\n"); break;
        case UDG: printf("无向图\n"); break;
        case UDN: printf("无向网\n"); break;
    }

    // 输出顶点数组
    printf("\n顶点数组: ");
    for (int i = 0; i < G.vexnum; i++) {
        printf(" %c ", G.vexs[i]);
    }
    printf("\n");

    // 输出邻接矩阵
    printf("邻接矩阵:\n");
    for (int i = 0; i < G.vexnum; i++) {
        for (int j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j].adj == INFINITY && (G.kind == DG || G.kind == DN)) {
                printf("∞ ");
            } else {
            	if(G.arcs[i][j].adj == INFINITY && (G.kind == UDG || G.kind == UDN)){
            		printf("0 ");
				}
                else 
					printf("%d ", G.arcs[i][j].adj);
            }
        }
        printf("\n");
    }
}


// 拓扑排序函数
int TopologicalSort(MGraph &G, int *indegree, int *sorted) {
    int i, j, k;
    int count = 0;
    int stack[MAX_VERTEX_NUM];
	int top = -1;
    
    // 计算入度
    for (i = 0; i < G.vexnum; i++) {
        indegree[i] = 0;
    }
    for (i = 0; i < G.vexnum; i++) {
        for (j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j].adj != INFINITY && G.arcs[i][j].adj != 0) {
                indegree[j]++;
            }
        }
    }

    // 将入度为0的节点入栈
    for (i = 0; i < G.vexnum; i++) {
        if (indegree[i] == 0) {
            stack[++top] = i;
        }
    }

    // 进行拓扑排序
    while (top != -1) {
        k = stack[top--];
        sorted[count++] = k;

        for (i = 0; i < G.vexnum; i++) { // 更新入度
            if (G.arcs[k][i].adj != INFINITY && G.arcs[k][i].adj != 0) {
                indegree[i]--;
                if (indegree[i] == 0) {
                    stack[++top] = i;
                }
            }
        }
    }

    return count;  // 返回拓扑排序的节点数
}

// 计算关键路径
void CriticalPath(MGraph &G) {
    int sorted[MAX_VERTEX_NUM];  // 拓扑排序结果
    int indegree[MAX_VERTEX_NUM]; // 各节点的入度
    int et[MAX_VERTEX_NUM];      // 最早发生时间
    int lt[MAX_VERTEX_NUM];      // 最晚发生时间
    int slack[MAX_VERTEX_NUM];   // 松弛时间
    
    // 拓扑排序
    int count = TopologicalSort(G, indegree, sorted);
    
    if (count != G.vexnum) {
        printf("有回路。\n");
        return;
    }

    // 初始化最早发生时间 et
    for (int i = 0; i < G.vexnum; i++) {
        et[i] = 0;
    }

    // 计算最早发生时间
    for (int i = 0; i < G.vexnum; i++) {
        int u = sorted[i];
        for (int v = 0; v < G.vexnum; v++) {
            if (G.arcs[u][v].adj != INFINITY) { //如果从节点 u 到节点 v 存在边且权重不为 INFINITY，则更新节点 v 的最早发生时间。
                if (et[v] < et[u] + G.arcs[u][v].adj) {
                    et[v] = et[u] + G.arcs[u][v].adj;
                }
            }
        }
    }

    // 初始化最晚发生时间 lt
    for (int i = 0; i < G.vexnum; i++) {
        lt[i] = et[G.vexnum - 1];
    }

    // 计算最晚发生时间
    for (int i = G.vexnum - 1; i >= 0; i--) {
        int u = sorted[i];
        for (int v = 0; v < G.vexnum; v++) {
            if (G.arcs[u][v].adj != INFINITY) { //如果从节点 u 到节点 v 存在边且权重不为 INFINITY，则更新节点 u 的最晚发生时间。
                if (lt[u] > lt[v] - G.arcs[u][v].adj) {
                    lt[u] = lt[v] - G.arcs[u][v].adj;
                }
            }
        }
    }

    printf("关键路径: ");
    for (int i = 0; i < G.vexnum; i++) {
        for (int j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j].adj != INFINITY) {
                slack[i] = lt[j] - et[i] - G.arcs[i][j].adj; // 计算松弛时间
                if (slack[i] == 0) {
                    printf("%c->", G.vexs[i]);
                }
            }
        }
    }
    printf("%c",G.vexs[G.vexnum-1]);
    printf("\n");
}

int main(){
	MGraph G;
	char vexs[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
	int arcs[]={
				0, 1, 8,//A0
	 			0, 2, 6,//A1
	 			0, 4, 7,//A2
	 			1, 3, 3,//A3
	 			2, 3, 10,//A4
	 			2, 6, 9,//A5
				4, 6, 9,//A6
				4, 7, 13,//A7
				3, 5, 4,//A8
				3, 8, 19,//A9
				6, 8, 8,//A10
				6, 7, 2,//A11
				7, 8, 6,//A12
				5, 9, 14,//A13
				8, 9, 10,//A14
				};
	
	CreateMGraph(DN, G, 10, 15, vexs, arcs);
	OutMGraph(G);
	CriticalPath(G);
}








