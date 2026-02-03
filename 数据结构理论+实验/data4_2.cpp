#include <iostream>
#include<fstream>
using namespace std;
typedef int Status;
#define OK 0
#define ERROR -1
typedef int KeyType;        //关键字的数据类型
//数据元素（记录）的类型定义
typedef struct {
        KeyType  key;            //学号(关键字)
        char name[10];     //姓名
        char sex;         //性别 
        int  age;                     //年龄 
} RecordType;

typedef struct{
    RecordType *Record; //r[0]闲置或用作哨兵单元 
    int length; //顺序表的长度
}SSTable; //静态查找表类型


//动态查找表BST的类型定义
typedef  struct BSTNode {   
        RecordType  data;
        struct BSTNode *lchild, *rchild; //指向左右孩子结点的指针
}BSTNode, *BSTree;
void OutElem_BST(BSTree p){
	cout<<p->data.key<<" "<<p->data.name<<" "<<p->data.age<<" "<<p->data.sex<<endl;
}

Status CreateSSTable(SSTable &ST, const char* filename,int n){
    ifstream infile(filename);
    if (!infile) return ERROR;
    ST.Record = (RecordType*)malloc((n + 1) * sizeof(RecordType));
    if (!ST.Record) return ERROR;
    ST.length = n;

    for (int i = 1; i <= n; i++) {
        infile >> ST.Record[i].key >> ST.Record[i].name >> ST.Record[i].sex >> ST.Record[i].age;
    }

    infile.close();
    return OK;
}

void TraversSSTable(SSTable ST){
    int i;
    for(i=1;i<=ST.length;i++){
        cout<<ST.Record[i].key<<" "<<ST.Record[i].name<<" "<<ST.Record[i].sex<<" "<<ST.Record[i].age<<endl;
    }
}

Status InsertDSTable_BST(BSTree &T,RecordType e){
	BSTree s;
	if(!T){
		s=(BSTree)malloc(sizeof(BSTNode));
		s->data=e;
		s->lchild=s->rchild=NULL;
		T=s;
		return OK;
	}
	else if(e.key==T->data.key) {
		return ERROR;
	}
	else if(e.key<T->data.key) 
		return InsertDSTable_BST(T->lchild,e);
	else return InsertDSTable_BST(T->rchild,e);

}
Status CreateDSTable_BST(BSTree &DT,const SSTable ST){
	int i;
	DT=NULL;
	for(i=1;i<=ST.length;i++){
		InsertDSTable_BST(DT,ST.Record[i]);
	}
	return OK;
}

void TraversDSTable_InOrder(BSTree T){
	if(T){
		TraversDSTable_InOrder(T->lchild);
		OutElem_BST(T);
		TraversDSTable_InOrder(T->rchild);
	}
}


Status DeleteBST(BSTree &T,KeyType key){
	BSTNode *p=NULL;
	BSTNode *q=T;
	while((q)&&(q->data.key!=key)){
		p=q;q=(key<q->data.key)?q->lchild:q->rchild;
	}
	if(!q) return ERROR;
	if(!q->lchild&&!q->rchild){
		if(p){
			if(p->lchild==q) p->lchild=NULL;
			else p->rchild=NULL;
		}
		else T=NULL;
		free(q);	
	}
	else if(!q->lchild){
		if(p){
			if(p->lchild==q) p->lchild=q->lchild;
			else p->rchild=q->lchild;
		}
		else T=q->lchild;
		free(q);
	}
	else if(!q->rchild){
		if(p){
			if(p->lchild==q)p->lchild=q->lchild;
			else p->rchild=q->lchild;
		}
		else T=q->rchild;
		free(q);
	}
	else if(q->lchild&&q->rchild){
		BSTNode *child=q->lchild;
		BSTNode *parent=q;
		while(child->rchild){
			parent=child;
			child=child->rchild;
		}
		q->data=child->data;
		if(child->lchild) {
			if(child==parent->rchild)parent->rchild=child->lchild;
			else parent->lchild=child->lchild;
		}
		else parent->rchild=NULL;
		free(child);
	}
	return OK;

}

BSTree SearchDSTable_BST(BSTree T,KeyType key,int &count){
	BSTNode *p=T;
	while(p){
		count++;
		if(key==p->data.key) return p;
		else if(key<p->data.key) p=p->lchild;
		else p=p->rchild;
	}
	return 0;
}

void InputElem(RecordType &e){
	cout<<"学号 姓名 性别 年龄"<<endl;
	cin>>e.key;
	if(e.key!=-1){
		cin>>e.name>>e.sex>>e.age;
	}
}
int main(){
	int i,key;
	int n=11;
	SSTable ST1;
	BSTree DT1,p;
	//创建静态查找表ST1 
	if (CreateSSTable(ST1, "students.txt",n) == ERROR) {
        cout << "Error reading file!" << endl;
        return 1;
    }
	//输出初始查找表
	cout<<endl<<"学号无序的静态查找表"<<endl;
	TraversSSTable(ST1);

	//创建动态查找表DT1 
	CreateDSTable_BST(DT1,ST1);
	cout<<endl<<"学号有序的动态查找表(中序遍历)"<<endl;
	cout<<"学号 姓名 性别 年龄"<<endl;
	TraversDSTable_InOrder(DT1);

	//查找动态查找表元素 
	cout<<endl<<"---元素查找---"<<endl;
	cout<<"请输入要查找记录的学号(输入-1结束)："<<endl;
	scanf("%d",&key); 
	while (key!=-1){
		int count=0;
		p=SearchDSTable_BST(DT1,key,count);
		if(!p)
			cout<<"未找到记录"<<endl; //未找到记录
		else 
			OutElem_BST(p); 		//输出找到的记录 
		cout<<"查找长度为："<<count<<endl;
		cout<<"请输入要查找记录的学号(输入-1结束)："; //下一次查找
		scanf("%d",&key); 
	} 

	int total=0;
	for(i=1;i<=ST1.length;i++){
		int count=0;
		SearchDSTable_BST(DT1, ST1.Record[i].key,count);
		total=total+count;
	}
	cout<<endl<<"查找成功的平均查找长度ASL="<<total/(ST1.length)<<endl;
	total=0;
	for(i=0;i<=ST1.length;i++){
		int count=0;
		if(i==0) SearchDSTable_BST(DT1,4,count);
		else SearchDSTable_BST(DT1,ST1.Record[i].key+1,count);
		total=total+count;
	}
	printf("\n查找不成功的平均查找长度ASL=%f\n",(float)total/(ST1.length+1));

	//构造动态查找表（插入新元素 ） 
	cout<<endl<<"---元素插入---"<<endl;
	RecordType e;
	cout<<endl<<"请输入要插入的新记录(输入-1结束)"<<endl;
	InputElem(e); 
	while (e.key!=-1) {
		if(!InsertDSTable_BST(DT1,e)){
			cout<<endl<<"插入元素后的动态查找表(中序遍历)"<<endl;
			cout<<"学号  姓名  年龄  性别"<<endl;
			TraversDSTable_InOrder(DT1);
		}
		else cout<<"当前要插入的记录已存在！"<<endl;
		cout<<endl<<"请输入要插入的新记录(输入-1结束)"<<endl;
		InputElem(e); 
	}

	//删除动态查找表元素 
	printf("\n---元素删除---\n");
	printf("\n请输入要删除记录的学号(输入-1结束)："); 
	cin>>key;
	while (key!=-1){
		if(!DeleteBST(DT1,key)){
			printf("\n删除元素后的动态查找表(中序遍历)\n");
			printf("学号  姓名  年龄  性别 \n");
			TraversDSTable_InOrder(DT1);
		}
		else cout<<"当前要删除的记录不存在！"<<endl;
		printf("\n请输入要删除记录的学号(输入-1结束)："); //下一次删除 
		cin>>key;
	} 
	return 0;

}