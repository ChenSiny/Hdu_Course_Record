#include <cstdlib>
#include<iostream>
using namespace std;
typedef int Status;         //声明函数类型
#define STACK_INIT_SIZE    10
#define STACKINCREMENT  5
#define OVERFLOW 3 
#define OK 0
#define ERROR 1
#define EMPTY 1
#define FULL 1

typedef int SElemType;  //声明SElemType的类型
typedef struct  { 
        SElemType *base;  //顺序栈数据存储空间基址 
        SElemType *top;    //顺序栈栈顶指针 
        int stacksize;
}SqStack;  // SqStack为用户定义的顺序栈类型

#define MAXQSIZE 10    //队列的最大长度
typedef int QElemType;  //声明SElemType的类型
typedef struct {
       QElemType *base;    //初始化队列时动态分配存储空间的地址(数组名)
       int front;                 //头元素的下标变量
       int rear;                   //尾元素的下标变量
} SqQueue;   // 循环队列类型

Status InitStack(SqStack &S)
{// 构造一个空栈 S
  S.base = (SElemType*)malloc(STACK_INIT_SIZE * sizeof(SElemType));
  if(!S.base) exit(OVERFLOW);
  S.top=S.base;
  S.stacksize=STACK_INIT_SIZE;
  return OK;
}

Status ClearStack(SqStack &S){//清空栈
    S.top=S.base;
    return OK;
}

Status StackLength(SqStack &S){//返回栈长度
    return S.top-S.base;
}

Status GetTop(SqStack S,SElemType &e){//取栈顶元素
    if (S.top!=S.base) e=*(S.top-1);
    return OK;
}

Status Push(SqStack &S,SElemType e){//入栈
    if(S.top-S.base!=STACK_INIT_SIZE) *S.top++=e;
    else return ERROR;

    return OK;
}

Status Pop(SqStack &S, SElemType &e) {//出栈
    if (S.top == S.base)
        return ERROR;
    e = *--S.top;
    return OK;
}

Status InitQueue(SqQueue &Q){//初始化队列
    Q.base=(QElemType*)malloc(sizeof(QElemType)*MAXQSIZE);
    if(!Q.base) exit(OVERFLOW);
    Q.front=Q.rear=0;
    return OK;
}

Status ClearQueue(SqQueue &Q){//清空队列
    Q.front=Q.rear=0;
    return OK;
}

int QueueLength(SqQueue &Q){//返回队列长度
    return (Q.rear-Q.front+MAXQSIZE)%MAXQSIZE;
}

Status EnQueue(SqQueue &Q,QElemType e){//入队
    if ((Q.rear+1)%MAXQSIZE==Q.front) return FULL;
    Q.base[Q.rear]=e;
    Q.rear=(Q.rear+1)%MAXQSIZE;
    return OK;
}

Status DeQueue(SqQueue &Q,QElemType &e){//出队
    if(Q.front==Q.rear) return EMPTY;
    e=Q.base[Q.front];
    Q.front=(Q.front+1+MAXQSIZE)%MAXQSIZE;
    return OK;
}

Status QueueEmpty(SqQueue &Q){//判断队列是否为空
    if(Q.front==Q.rear) return EMPTY;
    return OK;
}

Status QueueFull(SqQueue Q){//判断队列是否为满
    if((Q.rear+1)%MAXQSIZE==Q.front) return FULL;
    return OK;
}

Status Conversion(int m,int n){//进制转换
    //进制转换
    int m_n;
    SqStack S;
    InitStack(S);
    while(m>0){
        Push(S,m%n);
        m=m/n;
    }
    while(S.top!=S.base){//出栈
        int top;
        Pop(S,top);
        cout<<top;
    }
    cout<<endl;
    return OK;
}

Status QueueReserve(SqQueue Q,SqQueue &re) {
    //队列逆置

    SqStack S;
    InitStack(S);
    while (Q.front != Q.rear) {
        int e;
        DeQueue(Q, e);
        Push(S, e);
    }
    while (S.top != S.base) {
        int e;
        Pop(S, e);
        EnQueue(re, e);
    }
    return OK;
}
 
int main(){
    int m;
    int n;
    cout<<"请输入待转换十进制整数"<<endl;
    cin>>m;
    cout<<"二进制结果"<<endl;
    Conversion(m,2);
    cout<<"八进制结果"<<endl;
    Conversion(m,8);

    SqQueue Q1;
    InitQueue(Q1);
    int p,q;
    //入队9
    for (int i=0;i<9;i++){
        cin>>p;
        EnQueue(Q1,p);
    }
    cout<<"front:"<<Q1.base[Q1.front]<<endl;
    cout<<"rear:"<<Q1.base[Q1.rear]<<endl;
    //入队1
    cin>>p;
    EnQueue(Q1,p);
    //输出此时队列状态
    for (int i = Q1.front; i != Q1.rear; i = (i + 1) % MAXQSIZE) {
        cout << Q1.base[i] << " ";
    }
    cout << endl;
    //出队5
    for(int i=0;i<5;i++){
        DeQueue(Q1,q);
    }
    //入队2
    for (int i=0;i<2;i++){
        cin>>p;
        EnQueue(Q1,p);
    }
    int length;
    length=QueueLength(Q1);
    cout<<"length:"<<length<<endl;
    cout<<"front:"<<Q1.base[Q1.front]<<endl;
    cout<<"rear:"<<Q1.base[Q1.rear]<<endl;

    SqQueue re;
    InitQueue(re);
    QueueReserve(Q1,re);
    for (int i=0;i<length;i++){
        int e;
        DeQueue(re,e);
        cout<<e<<" ";
    }

    return 0;
}