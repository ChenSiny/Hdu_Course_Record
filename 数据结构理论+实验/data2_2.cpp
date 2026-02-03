#include <iostream>
#define OK 0
#define ERROR 1
// #define OVERFLOW -2
using namespace std;
typedef int Status;              //声明函数类型名

typedef  char TElemType;    //声明结点元素值得类型

typedef struct BiThrNode {  //定义线索二叉链表的结点结构和类型

      TElemType data;

       struct BiThrNode *lchild, *rchild;

       int  LTag, RTag;
}BiThrNode,*BiThrTree;
BiThrNode *pre;  //全局变量，始终指向刚刚访问过的结点

Status InitBiThrTree(BiThrTree &T) {
    //根据前序输入建立二叉树
    char ch;
    cin>>ch;
    if(ch=='#') T=NULL;
    else
    {
        T=new BiThrNode;
        T->data=ch;   
        InitBiThrTree(T->lchild);
        InitBiThrTree(T->rchild);   
    }
    return OK;
}

Status InOrderTraverse(BiThrTree  T) {
    //中序遍历二叉树,递归算法,对每个结点调用一次visit(),左中右
    if (T) {
        InOrderTraverse(T->lchild);
        printf("%c ", T->data);
        InOrderTraverse(T->rchild);
    }

    return OK;
}
void InThreading(BiThrTree p)//中序遍历线索化二叉树的递归算法
{//pre 是全局变量，初始化时其右孩子指针为空，便于在树的最左点开始建线索   
if(p)
   {
   InThreading(p-> lchild);//递归左孩子线索化
   if(!p-> lchild)//左孩子为空，建立前驱线索
   {
     p->LTag=1;
     p->lchild=pre;
   }
   else p->LTag=0;  //左孩子不为空
   if(!pre-> rchild)
   {
     pre->RTag=1;
     pre->rchild=p;
   }
   else pre->RTag=0;  
   pre=p;
   InThreading(p-> rchild);//递归右孩子线索化
   }
}
void InOrderThreading(BiThrTree &Thrt,BiThrTree T) {// 中序遍历二叉树 T，并将其中序线索化，Thrt 指向头节点
    Thrt=new BiThrNode ;   //创建头结点
    Thrt->data='@';
    Thrt->LTag=0;
    Thrt->RTag=1;
    Thrt->rchild=Thrt;
if(!T) Thrt->lchild=Thrt;   //若二叉树为空，头结点的左孩子指向自己
else //不空时，头结点的左孩子指向二叉树的根结点
 {
   Thrt-> lchild=T; 
   pre=Thrt;    
   InThreading(T);
   pre-> rchild=Thrt;                        
   pre-> RTag=1;
   Thrt-> rchild=pre;
 }
}

Status InOrderTraverse_Thr(BiThrTree T) {//中序遍历二叉线索链表T的非递归算法
    BiThrTree p;
    p=T->lchild;
    cout<<endl;
    while(p!=T)
    {
        while(p->LTag==0) p=p->lchild;// 找到中序遍历的第一个结点
        cout<<'|'<<p->LTag<<'|'<<p->lchild->data<<'|'<<p->data<<'|'<<p->rchild->data<<'|'<<p->RTag<<'|'<<endl;
        while(p->RTag==1&&p->rchild!=T)
        {
            p=p->rchild;
            cout<<'|'<<p->LTag<<'|'<<p->lchild->data<<'|'<<p->data<<'|'<<p->rchild->data<<'|'<<p->RTag<<'|'<<endl;
        }
        p=p->rchild;
    }
    return OK;
}

int main() {
    BiThrTree T,Thrt;
    InitBiThrTree(T); // Ensure the string contains only ASCII characters
    InOrderTraverse(T);
    InOrderThreading(Thrt,T);
    InOrderTraverse_Thr(Thrt);
    return 0;
}