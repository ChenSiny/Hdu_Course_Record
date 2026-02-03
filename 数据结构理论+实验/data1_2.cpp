#include <iostream>
using namespace std;

#define MAXSIZE 100
#define OK 1
#define ERROR 0
typedef int ElemType;
typedef int Status;

typedef struct {
    ElemType *data;
    int length;
} SqList;

// 初始化顺序表
Status InitList(SqList &L) {
    L.data = new ElemType[MAXSIZE];
    if (!L.data) return ERROR;
    L.length = 0;
    return OK;
}

// 清空顺序表
Status ClearList(SqList &L) {
    L.length = 0;
    return OK;
}

// 返回顺序表长度
int Listlength(SqList L) {
    return L.length;
}

// 获取第i个元素
Status GetElem(SqList L, int i, ElemType &e) {
    if (i < 1 || i > L.length) return ERROR;
    e = L.data[i - 1];
    return OK;
}

// 查找元素位置
int LocateList(SqList L, ElemType e) {
    for (int i = 0; i < L.length; i++) {
        if (L.data[i] == e) return i + 1;
    }
    return 0;
}

// 插入元素
Status ListInsert(SqList &L, int i, ElemType e) {
    if (i < 1 || i > L.length + 1 || L.length == MAXSIZE) return ERROR;
    for (int j = L.length; j >= i; j--) {
        L.data[j] = L.data[j - 1];
    }
    L.data[i - 1] = e;
    L.length++;
    return OK;
}

// 删除元素
Status ListDelete(SqList &L, int i, ElemType &e) {
    if (i < 1 || i > L.length) return ERROR;
    e = L.data[i - 1];
    for (int j = i; j < L.length; j++) {
        L.data[j - 1] = L.data[j];
    }
    L.length--;
    return OK;
}

// 遍历顺序表
Status ListTraverse(SqList L) {
    for (int i = 0; i < L.length; i++) {
        cout << L.data[i] << " ";
    }
    cout << endl;
    return OK;
}

// 并运算
Status Union(SqList &La, SqList Lb) {
    for (int i = 0; i < Lb.length; i++) {
        if (!LocateList(La, Lb.data[i])) {
            ListInsert(La, La.length + 1, Lb.data[i]);
        }
    }
    return OK;
}

// 交运算
Status Intersection(SqList &La, SqList Lb) {
    int i = 0;
    while (i < La.length) {
        if (!LocateList(Lb, La.data[i])) {
            ElemType e;
            ListDelete(La, i + 1, e);
        } else {
            i++;
        }
    }
    return OK;
}

// 差运算
Status Difference(SqList &La, SqList Lb) {
    int i = 0;
    while (i < La.length) {
        if (LocateList(Lb, La.data[i])) {
            ElemType e;
            ListDelete(La, i + 1, e);
        } else {
            i++;
        }
    }
    return OK;
}

// 归并排序
Status MergeList(SqList La, SqList Lb, SqList &Lc) {
    int i = 0, j = 0, k = 0;
    while (i < La.length && j < Lb.length) {
        if (La.data[i] <= Lb.data[j]) {
            ListInsert(Lc, ++k, La.data[i++]);
        } else {
            ListInsert(Lc, ++k, Lb.data[j++]);
        }
    }
    while (i < La.length) {
        ListInsert(Lc, ++k, La.data[i++]);
    }
    while (j < Lb.length) {
        ListInsert(Lc, ++k, Lb.data[j++]);
    }
    return OK;
}

// 去重
Status Purge(SqList &Lc) {
    int i = 0;
    while (i < Lc.length - 1) {
        if (Lc.data[i] == Lc.data[i + 1]) {
            ElemType e;
            ListDelete(Lc, i + 2, e);
        } else {
            i++;
        }
    }
    return OK;
}

int main() {
    SqList La, Lb, Lc;
    InitList(La);
    InitList(Lb);
    InitList(Lc);

    ElemType ea[] = {2, 8, 27, 39, 66, 77, 89};
    ElemType eb[] = {6, 18, 27, 59, 65, 77, 89, 120, 140};

    for (int i = 0; i < 7; i++) {
        ListInsert(La, i + 1, ea[i]);
    }
    for (int i = 0; i < 9; i++) {
        ListInsert(Lb, i + 1, eb[i]);
    }

    cout << "La: ";
    ListTraverse(La);
    cout << "Lb: ";
    ListTraverse(Lb);

    // 并运算
    Union(La, Lb);
    cout << "La U Lb: ";
    ListTraverse(La);

    // 恢复La
    ClearList(La);
    for (int i = 0; i < 7; i++) {
        ListInsert(La, i + 1, ea[i]);
    }

    // 交运算
    Intersection(La, Lb);
    cout << "La ∩ Lb: ";
    ListTraverse(La);

    // 恢复La
    ClearList(La);
    for (int i = 0; i < 7; i++) {
        ListInsert(La, i + 1, ea[i]);
    }

    // 差运算
    Difference(La, Lb);
    cout << "La - Lb: ";
    ListTraverse(La);

    // 归并排序
    ClearList(La);
    for (int i = 0; i < 7; i++) {
        ListInsert(La, i + 1, ea[i]);
    }
    MergeList(La, Lb, Lc);
    cout << "MergeList: ";
    ListTraverse(Lc);

    // 去重
    Purge(Lc);
    cout << "Purge: ";
    ListTraverse(Lc);

    // 释放内存
    delete[] La.data;
    delete[] Lb.data;
    delete[] Lc.data;

    return 0;
}