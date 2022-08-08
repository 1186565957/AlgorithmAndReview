#include<stdlib.h>
#include<stdio.h>
#include<memory.h>
#include<string.h>

#pragma region 斐波那契类型
//斐波那契数列是从1开始，下一个的值是前两个值的和
//求斐波那契数列的第N项
int FibonacciSequence(int n){
    if(n<=0)
        return 0;
    if(n == 1)
        return 1;
    
    return FibonacciSequence(n-1)+FibonacciSequence(n-2);

}
int FibonacciSequence_Unrecursion(int n){
    if(n<=0)
        return 0;
    if(n == 1)
        return 1;
    
    int tempFirst =0;
    int tempSecond =1;
    int tempResult =0;
    for(int i=0;i<=n;i++){
        tempResult = tempFirst + tempSecond;
        tempFirst = tempSecond;
        tempSecond = tempResult;
    }
    return tempResult;
}

//青蛙跳台阶的问题
//n级台阶的跳法
//本质上和上面就是相同的f(n) = f(n-1)+f(n-2)
//只是基数变动了f(1) = 1 f(2) = 2罢了
int FrogSteps(int n){


    return 0;
}

//青蛙要是一次可以跳1~n个台阶，就变成了递推公式
//f(n) = 2^(n-1)    

#pragma endregion

#pragma region 排序类型

//旋转数组的最小数字
//有一个是否有重复的问题
int RotatoMinDigit(int *RArray,int len){

    if(RArray == nullptr || len <=0)
        return -1;
    int *begin = RArray;
    int *end = RArray+len-1;
    int *ptr = nullptr;

    int tempLen = len/2;
    ptr = RArray + tempLen;
    while (ptr!=begin && ptr != end)
    {
        if (*ptr >= *end)
        {
            tempLen += (end - ptr) / 2;
            begin = ptr;
            ptr = RArray + tempLen;
        }
        else
        {
            tempLen -= (ptr - begin) / 2;
            end = ptr;
            ptr = RArray + tempLen;
        }

    }
    return *ptr;
}


#pragma endregion


int main(){

    //FibonacciSequence_Unrecursion(6);

    int Array[] = {6,6,7,8,1,1,2,3,3,4,5};
    RotatoMinDigit(Array,sizeof(Array)/sizeof(Array[0]));
    system("pause");
    return 0;
}