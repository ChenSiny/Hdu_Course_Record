#include <cstdio>
#include <cstdlib>
#define OK      1
#define ERROR   0
#define TRUE    1
#define FALSE    0
#define INFEASIBLE -1
#define OVERFLOW -2
#define MAXSIZE    100
typedef int ElemType;  //声明ElemType的类型
typedef int Status;  //声明函数类型名

typedef struct SqList {
        ElemType *elem;  //顺序表数据存储空间基址
        int length;
}SqList;

Status InitList(SqList &L) {
        // 构造一个空的线性表L
        L.elem = new ElemType[MAXSIZE];  //为顺序表分配一个大小为MAXSIZE的数组空间
        if (!L.elem) exit(OVERFLOW);  //存储分配失败
        L.length = 0;  //空表长度为0
        return OK;
}
Status ClearList(SqList &L) {
        //将L重置为空表
        L.length = 0;
        return OK;
}
int ListLength(SqList L) {
        //返回L中数据元素的个数
        return L.length;
}
Status GetElem(SqList L, int i, ElemType &e) {
        //用e返回L中第i个数据元素的值
        if (i < 1 || i > L.length) return ERROR;
        e = L.elem[i - 1];
        return OK;
}
int LocateList(SqList L, ElemType e) {
        //返回L中第一个与e满足关系compare()的数据元素的位序
        int i;
        for (i = 0; i < L.length; i++) {
                if (L.elem[i] == e) return i + 1;
        }
        return 0;
}
Status ListInsert(SqList &L, int i, ElemType e) {
        //在L中第i个位置之前插入新的数据元素e，L的长度加1
        int j;
        if (i < 1 || i > L.length + 1) return ERROR;  //i值不合法
        if (L.length >= MAXSIZE) return ERROR;  //当前存储空间已满
        for (j = L.length - 1; j >= i - 1; j--) {
                L.elem[j + 1] = L.elem[j];  //插入位置及之后的元素后移
        }
        L.elem[i-1]=e;
        ++L.length;
        return OK;
}

Status ListDelete(SqList &L, int i, ElemType &e) {
        //在L中删除第i个数据元素，并用e返回其值，L的长度减1
        int j;
        if (i < 1 || i > L.length) return ERROR;  //i值不合法
        e = L.elem[i - 1];
        for (j = i; j < L.length; j++) {
                L.elem[j - 1] = L.elem[j];  //删除位置之后的元素前移
        }
        --L.length;
        return OK;
}
Status ListTraverse(SqList L) {
    for (int i=0;i<L.length;i++){
        printf("%d ",L.elem[i]);
    }
    printf("\n");
    return OK;
}
Status Union(SqList &La,SqList Lb){
    //将所有在线性表Lb中但不在La中的数据元素插入到La中
    int La_len,Lb_len;
    ElemType e;
    La_len=ListLength(La);
    Lb_len=ListLength(Lb);
    for (int i=1;i<=Lb_len;i++){
        GetElem(Lb,i,e);
        if (!LocateList(La,e)){
            ListInsert(La,La_len+1,e);
            La_len++;
        }
    }
    return OK;
}
Status Intersection(SqList &La, SqList Lb) {
    int La_len = ListLength(La);
    ElemType e;
    for (int i = La_len; i >= 1; i--) {  // 从后往前遍历
        GetElem(La, i, e);
        if (!LocateList(Lb, e)) {
            ListDelete(La, i, e);
        }
    }
    return OK;
}

Status Difference(SqList &La,SqList Lb){
    int La_len;
    ElemType e;
    La_len=ListLength(La);
    for (int i=1;i<=La_len;i++){
        GetElem(La,i,e);
        if(LocateList(Lb,e)){
            ListDelete(La,i,e);
            i--;
            La_len--;
        }
    }
    return OK;
}
Status MergeList(SqList La,SqList Lb,SqList &Lc){
    //有序表合并
    int La_len,Lb_len,i,j;
    ElemType e1,e2;
    La_len=ListLength(La);
    Lb_len=ListLength(Lb);
    i=1;j=1;
    InitList(Lc);
    while(i<=La_len&&j<=Lb_len){
        GetElem(La,i,e1);
        GetElem(Lb,j,e2);
        if(e1<=e2){
            ListInsert(Lc,Lc.length+1,e1);
            i++;
        }
        else{
            ListInsert(Lc,Lc.length+1,e2);
            j++;
        }
    }
    while(i<La_len){
        GetElem(La,i++,e1);
        ListInsert(Lc,Lc.length+1,e1);
    }
    while(j<Lb_len){
        GetElem(Lb,j++,e2);
        ListInsert(Lc,Lc.length+1,e2);
    }
    return OK;
}
Status Purge(SqList &Lc){
    ElemType e;
    for (int i=1;i<=Lc.length;i++){
        GetElem(Lc,i,e);
        for (int j=i+1;j<=Lc.length;j++){
            if (e==Lc.elem[j-1]){
                ListDelete(Lc,j,e);
                j--;
            }
        }
    }
    return OK;
}
int main(){
    //初始化
    SqList La,Lb,Lc;
    ElemType ea[]={2,8,27,39,66,77,89};
    ElemType eb[]={6,18,27,59,65,77,89,120,140};
    InitList(La);InitList(Lb);InitList(Lc);
    for (int i=0;i<7;i++){
        ListInsert(La,i+1,ea[i]);
    }
    for (int i=0;i<9;i++){
        ListInsert(Lb,i+1,eb[i]);
    }
    printf("La: ");ListTraverse(La);
    printf("Lb: ");ListTraverse(Lb);
    //集合并

    Union(La,Lb);
    printf("La U Lb: ");ListTraverse(La);
    InitList(La);
    for (int i=0;i<7;i++){
        ListInsert(La,i+1,ea[i]);
    }
    //集合交

    Intersection(La,Lb);
    printf("La ∩ Lb: ");ListTraverse(La);
    InitList(La);
    for (int i=0;i<7;i++){
        ListInsert(La,i+1,ea[i]);
    }
    //集合差

    //ListTraverse(La);printf("\n");
    Difference(La,Lb);
    printf("La - Lb: ");ListTraverse(La);
    InitList(La);
    for (int i=0;i<7;i++){
        ListInsert(La,i+1,ea[i]);
    }
    //有序表合并
    MergeList(La,Lb,Lc);
    printf("MergeList: ");ListTraverse(Lc);
    //去重
    Purge(Lc);
    printf("Purge: ");ListTraverse(Lc);

    return 0;

}