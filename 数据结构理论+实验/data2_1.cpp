#include<stdio.h>
#include <iostream>

#define OK 0
#define ERROR 1
#define OVERFLOW -2

using namespace std;

typedef int Status;    
typedef  char TElemType; 
typedef  struct BiTNode {  
    TElemType  		data;
    struct BiTNode  *lchild, *rchild; 
} BiTNode, *BiTree;


Status CreateBiTree(BiTree &T) {
    //根据前序输入建立二叉树
    char ch;
    cin>>ch;
    if(ch=='#') T=NULL;
    else
    {
        T=new BiTNode;
        T->data=ch;   
        CreateBiTree(T->lchild);
        CreateBiTree(T->rchild);   
    }
    return OK;
}

Status PreOrderTraverse(BiTree T) {
    //先序遍历二叉树
    if (T) {
        printf("%c", T->data);
        PreOrderTraverse(T->lchild);
        PreOrderTraverse(T->rchild);
    }
    return OK;
}

Status InOrderTraverse(BiTree T) {
    //中序遍历二叉树,递归算法,对每个结点调用一次visit(),左中右
    if (T) {
        InOrderTraverse(T->lchild);
        printf("%c", T->data);
        InOrderTraverse(T->rchild);
    }
    return OK;
}

Status PostOrderTraverse(BiTree T) {
    //后序遍历二叉树,递归,左右中
    if (T) {
        PostOrderTraverse(T->lchild);
        PostOrderTraverse(T->rchild);
        printf("%c", T->data);
    }
    return OK;
}

int TNodes(BiTree T, int d) {
    if (T == NULL) return 0;

    int count = 0;
    if (d == 0 && T->lchild == NULL && T->rchild == NULL) {
        count = 1;
    } else if (d == 1 && ((T->lchild == NULL && T->rchild != NULL) || (T->lchild != NULL && T->rchild == NULL))) {
        count = 1;
    } else if (d == 2 && T->lchild != NULL && T->rchild != NULL) {
        count = 1;
    } 
    return count + TNodes(T->lchild, d) + TNodes(T->rchild, d);
}
int High(BiTree T) {
    if (T == NULL) return 0;
    int l = High(T->lchild);
    int r = High(T->rchild);
    return l > r ? l + 1 : r + 1;
}
Status InsertBST(BiTree &T, TElemType e){
    if(T == NULL){
        T = new BiTNode;
        T->data = e;
        T->lchild = T->rchild = NULL;
        return OK;
    }
    if(e < T->data) return InsertBST(T->lchild, e);
    if(e > T->data) return InsertBST(T->rchild, e);
    return ERROR;
}
Status CreateBST(BiTree &T, const char *chars){
    T = NULL;
    int i = 0;
    while(chars[i] != '\0'){
        InsertBST(T, chars[i]);
        i++;
    }
    return OK;
}



int main(){
    BiTree T;
    BiTree T1,T2;
    int h1,h2;
    CreateBiTree(T);
    cout<<"先序遍历：";
    PreOrderTraverse(T);
    printf("\n");
    cout<<"中序遍历：";
    InOrderTraverse(T);
    printf("\n");
    cout<<"后序遍历：";
    PostOrderTraverse(T);
    printf("\n");
    printf("叶子结点数：%d\n", TNodes(T, 0));
    printf("度为1的结点数：%d\n", TNodes(T, 1));
    printf("度为2的结点数：%d\n", TNodes(T, 2));
    CreateBST(T1, "DBFCAEG");
    InOrderTraverse(T1);
    printf("\n");
    CreateBST(T2, "DBFCAEG");
    InOrderTraverse(T2);
    printf("\n");
    h1=High(T1);
    h2=High(T2);
    printf("树1的高度：%d\n", h1);
    printf("树2的高度：%d\n", h2);

    return 0;
    
}


