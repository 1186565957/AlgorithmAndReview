#ifndef _TOOLS_REPO_H_
#define _TOOLS_REPO_H_

#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>


#include<random>


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

//字符分割函数
std::vector<std::string> Split(std::string &str,std::string flag){
    std::vector<std::string> vec;
    size_t last=0;
    //返回的是第一个字符的下标
    size_t index = str.find(flag,last);
    //npos是-1，是结束也就是找不到了的时候
    //查找失败返回2^64-1
    while(index != std::string::npos){
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

//sort and dedumplicate the element with linux c 
int sort_dedup(uint32_t* iplist, int* ipcnt)
{
    #define MAX_IP_CNT 65535
	if(*ipcnt <= 0 || iplist == NULL)
		return -1;

	//sort
	int i, j, cnt = *ipcnt;
	uint32_t key = 0;
	for (j = 1; j <*ipcnt; j++)
	{
		i = j - 1;
		key = iplist[j];
		while (i >= 0 && iplist[i] > key)
		{
			iplist[i + 1] = iplist[i];
			i--;
		}
		iplist[i + 1] = key;
	}

	//deduplicate
	i = 0, j = 1;
	while(j < cnt)
	{
		if(iplist[j] == iplist[i])
		{
			j++;
			(*ipcnt)--;
		}
		else
		{
			iplist[++i] = iplist[j];
			j++;
		}
	}

	bzero(&iplist[++i], (MAX_IP_CNT-j+1)* sizeof(uint32_t));
	return 0;
}

//Resolving the problem of buffer/cache increasing instantly
// due to callback fwriet() frequently


#endif // !_TOOLS_REPO_H_