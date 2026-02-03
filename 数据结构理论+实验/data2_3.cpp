
#include <iostream>
#include <cstring>
#define OK 0
#define ERROR 1
#define OVERFLOW 3 

using namespace std;
typedef int Status;    

typedef  struct  {  // 定义二叉链表结点类型 
    int weight;
    int parent, lchild, rchild; 
} HTNode, *HTree;           // 动态分配数组存储赫夫曼树

typedef  char **HTCode; 

Status InitHTree(HTree &HT, int *w, int n){
    HT = new HTNode[2 * n - 1]; // 动态分配数组
    for (int i = 0; i < n; i++) {
        HT[i].weight = w[i];
        HT[i].parent = -1;
        HT[i].lchild = -1;
        HT[i].rchild = -1;
    }
    for (int i = n; i < 2 * n - 1; i++) {
        HT[i].weight = -1;
        HT[i].parent = -1;
        HT[i].lchild = -1;
        HT[i].rchild = -1;
    }
    return OK;
}

Status Select(HTree HT, int n, int &s1, int &s2){//选择权值最小的两个结点
    int min1 = INT_MAX, min2 = INT_MAX; // 使用INT_MAX
    int k1 = -1, k2 = -1;
    for(int i = 0; i < n; ++i) {
        if(HT[i].parent == -1 && HT[i].weight < min1) {
            min2 = min1;
            k2 = k1;
            min1 = HT[i].weight;
            k1 = i;
        } else if(HT[i].parent == -1 && HT[i].weight < min2) {
            min2 = HT[i].weight;
            k2 = i;
        }
    }
    s1 = k1;
    s2 = k2;
    return OK;
}

Status CreateHTree(HTree &HT, int n){
    for(int i = n; i < 2 * n - 1; ++i){  
        int s1, s2;
        Select(HT, i, s1, s2); 
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;    
        HT[i].weight = HT[s1].weight + HT[s2].weight;   
    }
	cout<<"HT的内容是："<<endl;
    for(int i = 0; i < 2 * n - 1; ++i) { // 修改索引
        cout << HT[i].weight << ":" << HT[i].parent << ":" << HT[i].lchild << ":" << HT[i].rchild << endl;
    }
    return OK;
}

Status HTCoding(HTree HT, HTCode &HC, int n) {//根据赫夫曼树HT求赫夫曼编码HC
    char *cd;
    int start;
    int c, p;
    cd = new char[n]; 
    if (!cd) exit(OVERFLOW);
    cd[n-1] = '\0';
    for (int i = 0; i < n; ++i) {
        start = n - 1;
        c = i;
        p = HT[c].parent;
        while (p != -1) {// 从叶子结点向上直到根结点
            if (HT[p].lchild == c)
                cd[--start] = '0';
            else
                cd[--start] = '1';
            c = p;
            p = HT[c].parent;
        }
        HC[i] = new char[n - start]; // 动态分配编码字符串
        if (!HC[i]) exit(OVERFLOW);
        strcpy(HC[i], &cd[start]);
    }
    delete[] cd; // 释放临时存储内存
    return OK;
}

Status HTprint(HTree HT, HTCode HC, int n){
	cout<<"输出叶子结点哈夫曼编码"<<endl;
    for(int i = 0; i < n; ++i) {
        cout << HT[i].weight << ": ";
        cout << HC[i] << endl;
    }
    return OK;
}

int main() {
    int w[] = {5, 29, 7, 8, 14, 23, 3, 11};
    HTree HT;
    HTCode HC = new char*[8]; // 创建数组来存储编码
    InitHTree(HT, w, 8);
    CreateHTree(HT, 8);
    HTCoding(HT, HC, 8);
    HTprint(HT, HC, 8);

    // 释放内存
    for (int i = 0; i < 8; ++i) {
        delete[] HC[i];  // 释放每个编码字符串
    }
    delete[] HC; // 释放编码数组
    delete[] HT; // 释放赫夫曼树数组
}
