#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;
typedef int Status;
#define OK 0
#define ERROR -1
#define SUCCESS        1        //查找成功     
#define UNSUCCESS   0        //查找不成功
typedef int KeyType;             //关键字的数据类型
 //“线性探测再散列”哈希表的类型定义
typedef struct {                 
        KeyType *key;        //哈希表的基址（一维数组，数据元素只用一个关键字）
        int count;               //元素个数
        int size;                   //哈希表的长度
} HashTable;

//“链地址法”中同义词结点的类型定义
typedef struct KeyNode {    
        KeyType key;                //数据元素的关键字
        struct KeyNode *next;  //指向下一个同义词结点的指针
}KeyNode, *KeyLink;
//“链地址法”哈希表的类型定义 
typedef struct {
        KeyLink *head;   //哈希表的基址（一维指针数组，每个元素是同义词单链表的头指针）
        int count;            //元素个数
        int size;                //哈希表的长度（哈希函数“除留余数法”的除数）
}HashLink;
int Hash(KeyType key){
        return key % 13;  //除留余数法
}

Status CreateHashTable(HashTable &HT,int HT_Length,KeyType key[],int KeyNum){
    HT.key=(KeyType*)malloc(HT_Length*sizeof(KeyType));
    if(!HT.key) return ERROR;
    HT.size=HT_Length;
    HT.count=KeyNum;
    for(int i=0;i<HT_Length;i++) HT.key[i]=-1;
    for(int i=0;i<KeyNum;i++){
        int j=Hash(key[i]);
        if(HT.key[j]==-1) HT.key[j]=key[i];
        else{
            int k=1;
            while(HT.key[(j+k)%HT_Length]!=-1) k++;
            if(k>=HT_Length) return ERROR;
            HT.key[(j+k)%HT_Length]=key[i];
        }
    }
    return OK;
}

Status CreateHashLink(HashLink &HL,int HL_size,KeyType key[],int KeyNum){
    HL.head=(KeyLink*)malloc(HL_size*sizeof(KeyLink));
    if(!HL.head) return ERROR;
    HL.size=HL_size;
    HL.count=KeyNum;
    for(int i=0;i<HL_size;i++) HL.head[i]=NULL;
    for(int i=0;i<KeyNum;i++){ 
        int j=Hash(key[i]);
        KeyLink p=(KeyLink)malloc(sizeof(KeyNode));
        if(!p) return ERROR;
        p->key=key[i];
        p->next=HL.head[j];
        HL.head[j]=p;
    }
    return OK;

}

void OutHashLink(HashLink HL){
    for(int i=0;i<HL.size;i++){
        KeyLink p=HL.head[i];
        printf("%d :",i);
        while(p){
            printf("%d ",p->key);
            p=p->next;
        }
        printf("\n");
    }   
}

Status SearchHashTable(HashTable HT,KeyType key,int &p,int &c){
    p=Hash(key);
    c=0;
    while(HT.key[p]!=-1&&((p+1)%HT.size)!=Hash(key)&&HT.key[p]!=key){
        c++;
        p=(p+1)%HT.size;
    }
    if(HT.key[p]==key) {c++;return SUCCESS;}
    else {if(HT.key[p]==-1) c++;return UNSUCCESS;}
}

Status SearchHashLink(HashLink HL,KeyType key,KeyLink &p,int &c){
    p=HL.head[Hash(key)];
    c=0;
    while(p&&p->key!=key){
        c++;
        p=p->next;
    }
    c++;
    if(p) return SUCCESS;
    else return UNSUCCESS;
}
int main(){
	int i,j,k,total;
	int keys[12]={19,14,23,1,68,20,84,27,55,11,10,79};
	int keys1[13]={26,40,15,29,30,18,32,46,60,74,36,24,38};
	int n=12,n1=13; 
	int HT_Length=16;
	HashTable HT;

	printf("关键字表:\n");
	for(i=0;i<n;i++) printf("%2d ",keys[i]);
    //创建
	CreateHashTable(HT, HT_Length, keys, n);
	printf("\n\n线性探测再散列哈希表:\n");
	for(i=0;i<HT_Length;i++) printf("%d ",HT.key[i]);
	printf("\n");
    //查找
	total=0;
	for(i=0;i<n;i++){
		printf("\n查找关键字=%2d",keys[i]);
		SearchHashTable(HT, keys[i], j, k);
		total=total+k;
    		printf("  所在哈希表下标=%2d",j);
		printf("  关键字比较次数=%2d",k);
	}
	printf("\n\n查找成功ASL=%f\n",(float)total/n);

	total=0;
	for(i=0;i<n1;i++){
		printf("\n查找关键字=%2d",keys1[i]);
		SearchHashTable(HT, keys1[i], j, k);
		total=total+k;
		printf("  所在哈希表下标=%2d",j);
		printf("  关键字比较次数=%2d",k);
	}
	printf("\n\n查找不成功ASL=%f\n",(float)total/n1);

	HashLink HL;
	KeyLink p;
    //创建
	CreateHashLink(HL,13, keys, n);
	printf("\n链地址法哈希表:\n");
	OutHashLink(HL);
    //查找
	total=0;
	for(i=0;i<n;i++){
		printf("\n查找关键字=%2d",keys[i]);
		SearchHashLink(HL, keys[i], p, k);
		total=total+k;
		printf("  所在哈希表下标=%2d",(keys[i]) % 13);
		printf("  关键字比较次数=%2d",k);
	}
	printf("\n\n查找成功ASL=%f\n",(float)total/n);
	total=0;
	for(i=0;i<n1;i++){
		printf("\n查找关键字=%2d",keys1[i]);
		SearchHashLink(HL, keys1[i], p, k);
		total=total+k;
		printf("  所在哈希表下标=%2d",(keys1[i]) % 13);
		printf("  关键字比较次数=%2d",k);
	}
	printf("\n\n查找不成功ASL=%f\n",(float)total/n1);
	return 0;

}

