#include "common_c_plus.h"

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