#include<stdio.h>
#include<stdlib.h>
using namespace std;
#define OK 0
#define ERROR -1
typedef int Status;

#define MAXSIZE 20         //文件中记录个数的最大值
typedef int KeyType;          //定义关键字类型为整数类型
//记录类型
typedef struct  {
        KeyType  key;             //学号（记录的关键字）
        const char *name;     //姓名
        const char *sex;         //性别 
        int  age;                     //年龄 
} RecordType;                     
//记录表的类型
typedef struct{
          RecordType  r[MAXSIZE+1];      //r[0]闲置或用作“哨兵”单元
          int length;                                  //记录的个数
}RecordTable;                                     
int ccount,mcount;

Status CreateRecordTable(RecordTable &L) {	//创建顺序表 
	int keys[]={56,19,80, 5,21,64,88,13,37,75,92};
	//int keys[]={25,12,9,20,7,31,24,35,17,10,5};
	const char *names[]={"Zhang","Wang","ZHou","Huang","Zheng","Li","Liu","Qian","Sun","Zhao","Chen"};
	const char *sexs[]={"F","F","F","F","M","M","M","M","M","M","M"};
	int ages[]={19,18,19,18,19,20,20,19,18,19,18};
	int i,n=11;
	for(i=1;i<=n;i++){
		L.r[i].key=keys[i-1];
		L.r[i].name=names[i-1];
		L.r[i].sex=sexs[i-1];
		L.r[i].age=ages[i-1];
	}
	L.length=n;
	return OK;	
}

void OutRecordTable(RecordTable L){ 	//输出顺序表的各个记录
	int i;
	printf("学号 姓名 性别 年龄\n");
	for(i=1;i<=L.length;i++){
		printf(" %2d ",L.r[i].key);
		printf("%5s   ",L.r[i].name);
		printf("%1s   ",L.r[i].sex);
		printf("%2d\n",L.r[i].age);
	}
}

Status SelectSort(RecordTable &RT){
    int i,j,k;
    ccount=0;mcount=0;
    RecordType temp;
    for(i=1;i<RT.length;i++){
        k=i;
        for(j=i+1;j<=RT.length;j++){
            ccount++;
            if(RT.r[j].key<RT.r[k].key){
                k=j;
            }
        }
        if(k!=i){
            temp=RT.r[i];
            RT.r[i]=RT.r[k];
            RT.r[k]=temp;
            mcount++;
        }
    }
    return OK;
}


Status HeapAdjust(RecordTable &H,int s,int m){
    int i;
    RecordType rc=H.r[s];
    for(i=2*s;i<=m;i*=2){
        if(i<m && H.r[i].key<H.r[i+1].key){
            i++;
        }
        if(rc.key>=H.r[i].key){
            break;
        }
        H.r[s]=H.r[i];
        s=i;
        mcount++;
    }
    H.r[s]=rc;
    mcount++;
    return OK;
}

Status HeapSort(RecordTable &H){
    int i;int pcount=0;
    ccount=0;mcount=0;
    for(i=H.length/2;i>0;i--){ //建立初始堆
        HeapAdjust(H,i,H.length);
    }
    printf("\n初始堆：\n");
    OutRecordTable(H);
    for(i=H.length;i>1;i--){
        RecordType temp=H.r[1];
        H.r[1]=H.r[i];
        H.r[i]=temp;
        mcount++;
        HeapAdjust(H,1,i-1);
        pcount++;
        printf("\n第%d次排序结果：\n",pcount);
        OutRecordTable(H);
    }
    return OK;
}

int main(){
    RecordTable L;
    CreateRecordTable(L);
    printf("原始记录表：\n");
    OutRecordTable(L);
    printf("\n");
    SelectSort(L);
    printf("选择排序后的记录表：\n");
    OutRecordTable(L);
    printf("\n");

    RecordTable H;
    CreateRecordTable(H);
    printf("原始记录表：\n");
    OutRecordTable(H);
    printf("\n");
    HeapSort(H);
    printf("堆排序后的记录表：\n");
    OutRecordTable(H);
    printf("\n");
    return 0;   
}