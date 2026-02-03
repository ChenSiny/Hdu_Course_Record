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
int ccount,mcount,pcount;	        //全局变量，ccount关键字比较次数，mcount记录移动次数 

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


Status BubbleSort(RecordTable &RT){
    int i,j;
    RecordType temp;
    for(i=1;i<RT.length;i++){
        for(j=RT.length;j>i;j--){
            ccount++;
            if(RT.r[j].key<RT.r[j-1].key){
                temp=RT.r[j];
                RT.r[j]=RT.r[j-1];
                RT.r[j-1]=temp;
                mcount++;
            }
        }
    }
    printf("ccount=%d,mcount=%d\n",ccount,mcount);
    ccount=0;mcount=0;
    return OK;
}

void QuickSort(RecordTable &RT, int low, int high) {
    if (low < high) {
        int i = low, j = high;
        RecordType temp = RT.r[low];
        while (i < j) {
            while (i < j && RT.r[j].key >= temp.key) {
                j--;
                ccount++;
            }
            if (i < j) {
                RT.r[i] = RT.r[j];
                mcount++;
            }
            while (i < j && RT.r[i].key <= temp.key) {
                i++;
                ccount++;
            }
            if (i < j) {
                RT.r[j] = RT.r[i];
                mcount++;
            }
        }
        RT.r[i] = temp;
        mcount++;
        
        pcount++;
        printf("第%d次排序结果：\n", pcount);
        OutRecordTable(RT);

        QuickSort(RT, low, i - 1);
        QuickSort(RT, i + 1, high);

    }

}

int main(){
    RecordTable L;
    CreateRecordTable(L);
    printf("原始记录表：\n");
    OutRecordTable(L);
    printf("\n");

    BubbleSort(L);
    printf("冒泡排序后的记录表：\n");
    OutRecordTable(L);
    printf("\n");

    CreateRecordTable(L);
    printf("快速排序后的记录表：\n");
    QuickSort(L, 1, L.length);
    OutRecordTable(L);
    printf("ccount=%d,mcount=%d\n",ccount,mcount);

    return 0;
}