#include<iostream>
#include<cstring>
#include<cstdlib>

typedef int Status;
#define OVERFLOW 3 
#define OK 0
#define ERROR 1
using namespace std;

typedef struct {
      char *ch;       //若是非空串，则按串长分配存储区，否则ch为NULL。
       int  length;   //串长度
} HString;

Status StrAssign(HString &S, const char *chars) {//生成一个其值等于chars的串T

    int len = strlen(chars);
    if (len == 0) {
        S.ch = NULL;
        S.length = 0;
    } else {
        S.ch = (char *)malloc(len * sizeof(char));
        if (!S.ch) return ERROR;
        for(int i=0;i<len;i++){
            *(S.ch+i)=chars[i];
        }
        S.length = len;
    }
    return OK;
}

Status StrCompare(HString S,HString T){
    for (int i = 0; i < S.length && i < T.length; ++i) {
        if (S.ch[i] != T.ch[i])
            return S.ch[i] - T.ch[i];
    }
    if (S.length < T.length) return -1;
    if (S.length > T.length) return 1;
    return 0;

}

int StrLength(HString S){
    return S.length;
}

Status Concat(HString &S,HString T1, HString T2){
    S.length=T1.length+T2.length;
    S.ch=(char *)malloc(S.length*sizeof(char));
    for (int i=0;i<T1.length;i++){
        *(S.ch+i)=*(T1.ch+i);
    }
    for(int j=0;j<T2.length;j++){
        *(S.ch+T1.length+j)=*(T2.ch+j);
    }
    return OK;
}

Status SubString(HString &sub,HString S,int pos,int len){
    int lengths=StrLength(S);
    if(pos+len-1>lengths) return ERROR;
    if(lengths==0){
        sub.ch=NULL;
        sub.length=0;
    }
    else{
        sub.ch=(char*)malloc(len*sizeof(char));
        if(!sub.ch) return ERROR;
        for(int i=0;i<len;i++){
            *(sub.ch+i)=*(S.ch+pos+i-1);
        }
        sub.length=len;
    }
    return OK;
}

Status StrTraverse(HString S){
    int length=StrLength(S);
    if(length==0) return ERROR;
    for (int i=0;i<length;i++){
        cout<<*(S.ch+i);
    }
    cout<<endl;
    return OK;
}

Status StrCopy(HString &S,HString T){
    int length=StrLength(T);
    if(S.ch) free(S.ch);
    S.length=length;
    S.ch=(char *)malloc(S.length*sizeof(char));
    if(!S.ch) return ERROR;
    for(int i=0;i<length;i++){
        *(S.ch+i)=*(T.ch+i);
    }
    return OK;
}

int Index(HString S,HString T,int pos){
    HString sub;
    for (int i = pos; i <= S.length - T.length + 1; ++i) {
        SubString(sub, S, i, T.length);
        if (StrCompare(sub, T) == 0) return i;
    }
    return -1;

}

Status Replace(HString &S, HString T1, HString T2) {
    int pos = 1;              // 搜索起始位置
    HString result, temp;     // result 存放最终结果，temp 存放不匹配的部分
    StrAssign(result, "");    // 初始化result为空串

    while (pos <= S.length) {
        // 从当前位置提取与 T1 等长的子串
        SubString(temp, S, pos, T1.length);
        
        // 如果当前子串与 T1 相等，则用 T2 替换 T1
        if (StrCompare(temp, T1) == 0) {
            Concat(result, result, T2);   // 将 T2 拼接到 result 中
            pos += T1.length;             // 跳过 T1 的长度
        } else {
            // 如果不相等，将当前字符拼接到 result 中
            SubString(temp, S, pos, 1);   // 获取当前字符
            Concat(result, result, temp); // 拼接当前字符
            pos += 1;                     // 移动到下一个字符
        }
    }

    // 将 result 复制回 S
    StrCopy(S, result);
    return OK;
}


Status StrInsert(HString &S, int pos, HString T) {
    if (pos < 1 || pos > S.length + 1) return ERROR;
    HString sub1, sub2,result;
    SubString(sub1, S, 1, pos - 1);
    SubString(sub2, S, pos, S.length - pos + 1);
    Concat(result, sub1, T);
    Concat(result, result, sub2);
    StrCopy(S, result);
    return OK;
}

Status StrDelete(HString &S, int pos, int len) {
    if (pos < 1 || pos > S.length - len + 1) return ERROR;
    HString sub1, sub2;
    SubString(sub1, S, 1, pos - 1);
    SubString(sub2, S, pos + len, S.length - pos - len + 1);
    Concat(S, sub1, sub2);
    return OK;
}

int main() {
    HString T1, T2, T3, S;
    
    // 数据准备
    StrAssign(T1, "T1T1");
    StrAssign(T2, "T2T2");
    StrAssign(T3, "T3T3");
    cout << "T1: "; StrTraverse(T1);
    cout << "T2: "; StrTraverse(T2);
    cout << "T3: "; StrTraverse(T3);
    // 将T1赋给S，并输出S
    StrCopy(S, T1);
    cout << "S: "; StrTraverse(S);

    // 将T2插入到S的尾部，并输出S
    StrInsert(S, StrLength(S) + 1, T2);
    cout << "S after inserting T2: "; StrTraverse(S);

    // 将S中的T1替换成T3，并输出S
    Replace(S, T1, T3);
    cout << "S after replacing T1 with T3: "; StrTraverse(S);

    // 查找T2在S中的位置，并输出位置结果
    int pos = Index(S, T2, 1);
    cout << "Position of T2 in S: " << pos << endl;

    // 删除S中子串T2，并输出删除后S的结果
    StrDelete(S, pos, StrLength(T2));
    cout << "S after deleting T2: "; StrTraverse(S);

    return 0;
}
