#include "common_c_plus.h"

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

int main(){

    //FibonacciSequence_Unrecursion(6);

    int Array[] = {6,6,7,8,1,1,2,3,3,4,5};
    RotatoMinDigit(Array,sizeof(Array)/sizeof(Array[0]));
    system("pause");
    return 0;
}