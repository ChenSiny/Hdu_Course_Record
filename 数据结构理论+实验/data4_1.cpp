#include<iostream>
#include<fstream>
using namespace std;
#define MAXSIZE 100
#define OK 0
#define ERROR -1
typedef int KeyType;    
typedef int Status;
typedef struct{
    int key; //学号 
    char name[10]; //姓名 
    char sex;//性别
    int age; //年龄
}RecordType; //记录类型

typedef struct{
    RecordType *Record; //r[0]闲置或用作哨兵单元 
    int length; //顺序表的长度
}SSTable; //静态查找表类型


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


Status DestroySSTable(SSTable &ST){
    if(ST.Record){
        free(ST.Record);
        ST.Record=NULL;
    }
    ST.length=0;
    return OK;
}

void TraversSSTable(SSTable ST){
    int i;
    for(i=1;i<=ST.length;i++){
        cout<<ST.Record[i].key<<" "<<ST.Record[i].name<<" "<<ST.Record[i].sex<<" "<<ST.Record[i].age<<endl;
    }
}

int SearchSSTable_Seq(SSTable ST, KeyType key, int &c){
    int i;
    ST.Record[0].key=key; //设置哨兵
    i=ST.length;
    while(ST.Record[i].key!=key) i--;
    c=ST.length-i+1;
    return i;
}

void SortSSTable(SSTable &ST){
    int i,j;
    RecordType tmp;
    for(i=1;i<ST.length;i++){
        for(j=i+1;j<=ST.length;j++){
            if(ST.Record[i].key>ST.Record[j].key){
                tmp=ST.Record[i];
                ST.Record[i]=ST.Record[j];
                ST.Record[j]=tmp;
            }
        }
    }
}

int SearchSSTable_Bin(SSTable ST, KeyType key, int &c){
    int low=1,high=ST.length,mid;
    while(low<=high){
        c++;
        mid=(low+high)/2;
        if(ST.Record[mid].key==key) return mid;
        else if(ST.Record[mid].key>key) high=mid-1;
        else low=mid+1;
    }
    return 0;
}

void OutElem(SSTable ST, int i){
    cout<<ST.Record[i].key<<" "<<ST.Record[i].name<<" "<<ST.Record[i].sex<<" "<<ST.Record[i].age<<endl;
}


int main(){
	int i,key;
	int n=11;
	int c,total;
	SSTable ST1;
	//建立查找表
	if (CreateSSTable(ST1, "students.txt",n) == ERROR) {
        cout << "Error reading file!" << endl;
        return 1;
    }
	//输出初始查找表
	cout<<endl<<"学号无序的静态查找表"<<endl;
	TraversSSTable(ST1);

	//顺序查找特定学号，输出找到的元素和比较次数，或不存在与比较次数
	cout<<"---顺序查找---"<<endl;
	cout<<endl<<"请输入要查找的学号："<<endl; 
	cin>>key; 
	while (key!=-1){
		i=SearchSSTable_Seq(ST1,key,c);
		if(i==0)
			printf("当前要查找的记录不存在！\n");
		else 
			OutElem(ST1,i); 		//输出找到的记录 
		cout<<"查找时比较的次数="<<c<<endl;
		cout<<endl<<"请输入要查找的学号："<<endl;
		cin>>key; 
	}

	//依次查找输出成功asl
    printf("\n顺序查找各关键字的比较次数：\n");
	total=0;
	for(i=1;i<=ST1.length;i++){
		SearchSSTable_Seq(ST1,ST1.Record[i].key,c);
		total=total+ST1.length-c+1;
		cout<<"关键字="<<ST1.Record[i].key<<"  比较次数="<<ST1.length-c+1<<endl;
	}
	cout<<endl<<"查找成功的平均查找长度ASL="<<(float)total/ST1.length<<endl;
	cout<<"查找不成功的平均查找长度ASL="<<(float)ST1.length+1<<endl;

	//查找表排序并输出
	SortSSTable(ST1);
	printf("\n学号有序的静态查找表\n");
	TraversSSTable(ST1);

	//折半查找输出记录及次数
	cout<<"---折半查找(递归)---"<<endl; 
	cout<<"请输入要查找的学号："<<endl;
	cin>>key;

	while (key!=-1){
		int count=0; 
		i=SearchSSTable_Bin(ST1,key,count);
		if(i==0)
			printf("当前要查找的记录不存在！\n");
		else 
			OutElem(ST1,i); 		//输出找到的记录 
		printf("查找时比较的次数=%d\n",count);
		printf("\n请输入要查找的学号："); //下一次查找 
		scanf("%d",&key); 
	}


	printf("\n折半查找各关键字的比较次数：\n");
	total=0;
	//折半查找asl
	for(i=1;i<=ST1.length;i++){
		int count=0;
		SearchSSTable_Bin(ST1,ST1.Record[i].key,count);
		total=total+count;
		printf("关键字=%2d  比较次数=%d\n",ST1.Record[i].key,count);
	}
	cout<<endl<<endl<<"查找成功的平均查找长度ASL="<<(float)total/ST1.length<<endl;
	cout<<"折半查找各关键字的比较次数："<<endl;
	total=0;

	for(i=0;i<=ST1.length;i++){
		int count=0;
		if(i==0) SearchSSTable_Bin(ST1,4,count);
		else SearchSSTable_Bin(ST1,ST1.Record[i].key+1,count);
		total=total+count;
		cout<<"关键字="<<ST1.Record[i].key<<"  比较次数="<<count<<endl;
	}
	printf("\n查找不成功的平均查找长度ASL=%f\n",(float)total/(ST1.length+1));

	DestroySSTable(ST1);
	return 1;

}