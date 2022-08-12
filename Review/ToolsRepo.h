#include<stdio.h>
#include<ctype.h>
#include<assert.h>
#include<random>
using namespace std;

//字节的拷贝函数
//要考虑这之间的内存要是重叠了怎么办
/*
    不重叠：随便怎么拷贝都行
    源内存再后面：从头像后进行拷贝
    源内存再前面：从后往前拷贝
*/
void *memmove(void *dst,const void *src,size_t n){

    assert(dst&&src);
    char *dst_t = (char *)dst;
    char *src_t = (char*)src_t;

    if(src_t>dst_t){
        while(n--)
            *dst_t++ = *src_t++;
        
    }else if(src_t<dst_t){
        dst_t += n - 1;
        src_t += n - 1;
        while(n--)
            *dst_t-- = *src_t--;
    }
    return dst;
}

///复制字符串到指定位置的方式
char *strcpy(char *dst,const char *src,size_t n){

    assert(dst&&src);
    ///和上面是完全一样的东西的构成结构。
}

//实现字符向整形类型转换的程序
int atoi(char *nptr){
    int Iterger_sin =1;
    int Result_Interger=0;

    assert(nptr);
    while(isspace( *nptr)){
        nptr++;
    }
    if(*nptr == '-')
        Iterger_sin =-1;
    if(*nptr=='-'||*nptr == '+')
        nptr++;

    while(*nptr>='0' && *nptr<='9')
    {
        Result_Interger +=Result_Interger*10+*nptr-'0';
        nptr++;
    }
    Result_Interger*=Iterger_sin;
    return Result_Interger;
}

//实现快速排序的优化程序
template<class T>
T* fast_Sort(T *array,size_t begin,size_t end){
    int randomNum =rand()%(end -begin+1);
    size_t i=begin,j=end;
    while(i!=j){
        while(array[i]<=array[randomNum]){           
            begin++;
        }
        while(array[j]>=array[randomNum]){
            end--;
        }                      
        swap(array[j],array[i]);
        j--,i++;        
    } 
    fast_Sort(array,begin,randomNum-1);
    fast_Sort(array,randomNum+1,end);  
}

vector<string> Split(string &str,string flag){

    vector<string> vec;
    size_t last=0;
    //返回的是第一个字符的下标
    size_t index = str.find(flag,last);
    //npos是-1，是结束也就是找不到了的时候
    //查找失败返回2^64-1
    while(index != string::npos){
        if(index == last){
            last = index+flag.size();
            index = str.find(flag,last);
            continue;
        }
        vec.push_back(str.substr(last,index-last));
        last = index+flag.size();
        index = str.find(flag,last);
    }
    //末端没有标记函数的情况
    if(index>last && index!=last){
        vec.push_back(str.substr(last,str.size()-last));
    }
    return vec;
}