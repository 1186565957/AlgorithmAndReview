#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stack>
#include<map>
#include<vector>
#include<assert.h>
#include<string>
#include<algorithm>
#include<functional>
#include<iostream>



#pragma region 回溯算法的部分
//回溯算法里面对于小火柴正方形的解法
bool backtrack(std::vector<int> nums, int squareLen, int index, std::vector<int> square);
bool makesquare(std::vector<int> &nums)
{
    int Count = 0;
    int squareLen = 0;
    for (int i = 0; i < nums.size(); i++)
    {
        Count += nums[i];
    }
    if (Count % 4 != 0)
        return false;
    squareLen = Count >>2;
    std::vector<int> square(4, 0);
    //不排序的话可能遇到大数，运行内存会炸
    std::sort(nums.begin(),nums.end(),std::greater<int>());
    return  backtrack(nums, squareLen, 0, square);
}

bool backtrack(std::vector<int> nums, int squareLen, int index, std::vector<int> square)
{
    if (index == (nums.size()))
    {
        //开头检测是否成功了的案例
        if (square[1] == square[0] && square[1] == square[2] && square[2] == square[3])
            return true;
        return false;
    }
    //一根火柴有自己的摆放位置，看看摆在哪里
    for (int i = 0; i < 4; i++)
    {
        if (square[i] + nums[index] > squareLen)
        {
            continue;
        }
        square[i] += nums[index];    
        //放下一根火柴
        if (backtrack(nums,squareLen ,index + 1 , square))
            return true;
        //火柴放在这里不合适，剔除
        square[i] -= nums[index];
    }
    return false;
}
#pragma endregion

#pragma region LeetCode
///单调栈解下一个更大元素的算法方式
std::vector<int> findBiggerElement(int num1[],const int *num2,const int len1, const size_t len2){
    
    std::stack<int> tempRelationStack;
    std::map<int,int> comparsion_table;
    std::vector<int> result(len1,0);
    for(int i=0;i<len2;i++){
        if(tempRelationStack.empty())
            tempRelationStack.push(num2[i]);
        else{            
            while(num2[i]>tempRelationStack.top()){
                comparsion_table.insert(std::make_pair(tempRelationStack.top(),num2[i]));
                tempRelationStack.pop();
                if(tempRelationStack.empty())
                    break;
            }
            tempRelationStack.push(num2[i]);            
        }
    }

    for(int i=0;i<len1;i++){
        if(comparsion_table[num1[i]]){
            result[i] = comparsion_table[num1[i]];
        }else{
            result[i] = -1;
        }
    }

    return result;
}


///单调栈进阶类型，现在是循环数组
std::vector<int> nextGreaterElements(std::vector<int>& nums) {
    //记录当前元素的位置

    int Length = nums.size();
    int index = 0;
    std::stack<std::pair<int,int>> tempStack;
    std::vector<int>result(nums.size(),-1);
    for(int i = 0;i<Length*2;i++){
        index = i%Length;
        if(tempStack.empty()){
            tempStack.push(std::make_pair(index,nums[index]));
            continue;
        }
        else{
            while(!tempStack.empty() && nums[index]>tempStack.top().second){
                result[tempStack.top().first] = nums[index];
                tempStack.pop();
            }
            tempStack.push(std::make_pair(index,nums[index]));
        }
    }

    return result;
}


//普通的方式，和普通的题目
int findMaxConsecutiveOnes(std::vector<int> &nums)
{
    if (nums.size() == 0)
        return 0;
    if(nums.size() == 1 && nums[0] ==0)
        return 0;

    int first = 0;
    int p = 0;
    while (first!=nums.size()&& nums[first]==0)
        first++;
    p = first;

    int maxcount = 0;
    int count = 0;

    while (p < nums.size())
    {
        while ((nums[first] == nums[p]))
        {
            count++;
            p++;
            if(p == nums.size()){
                if (count >= maxcount)
                    maxcount = count;
                return maxcount;
            }
        }
        if (count >= maxcount)
            maxcount = count;
        count = 0;
        while (p!=nums.size()&&nums[p] == 0)
            p++;
        first = p;
        p = first;
    }
    return maxcount;
}

//可以用K个字符来置换的方式
//采取滑动窗口的方式来实现
int longestOnes(std::vector<int>& A, int K) {

    int first = 0;
    int p=0;
    int zeroNum = 0;
    int maxCount;
    for(;p<A.size();p++){
        if(A[p] == 0){
            zeroNum++;
        }
        while(zeroNum>K){
            if(A[first++] == 0)
                zeroNum--;
        }
        maxCount = std::max(maxCount,p-first+1);

    }
    return maxCount;
}

//动态规划求解最长上升子序列
int lengthOfLIS(std::vector<int> &nums)
{
    if(nums.size() == 0)
        return 0;
    if(nums.size() == 1)
        return 1;

    int maxCount =0;
    std::vector<int> dp(nums.size(),1);
    for(int i=0;i<nums.size();i++){
        for(int j=0;j<i;j++){   
            if(nums[i]>nums[j])
                dp[i] =std::max(dp[i],dp[j]+1) ;
            
            maxCount = std::max(maxCount,dp[i]);
        }
    }
    return maxCount;
}

//俄罗斯套娃信封
//排序，找宽和高其中一个最小的，套进去
//利用上面，针对一个边排序再求最大的连续问题
int maxEnvelopes(std::vector<std::vector<int>>& envelopes) {

    if(envelopes.size() == 0)
        return 0;
    if(envelopes.size() == 1)
        return 1;

    //冒泡排序,优先放置一个元素的合适位置
    // for(int i=0;i<envelopes.size();i++){
    //     for(int j=i+1;j<envelopes.size();j++){
    //         if(envelopes[i][0]>envelopes[j][0])
    //             std::swap(envelopes[i],envelopes[j]);
    //     }

    // }

    sort(envelopes.begin(), envelopes.end(), [](const auto &e1, const auto &e2) {
        return e1[0] < e2[0] || (e1[0] == e2[0] && e1[1] > e2[1]);
    });

    int maxCount =0;
    std::vector<int> dp(envelopes.size(),1);
    for(int i=0;i<envelopes.size();i++){
        for(int j=0;j<i;j++){   
            if(envelopes[i][1]>envelopes[j][1]&&envelopes[i][0]>envelopes[j][0] )
                dp[i] =std::max(dp[i],dp[j]+1) ;
            
            maxCount = std::max(maxCount,dp[i]);
        }
    }

    return maxCount;
}

//爱生气的书店老板，想要顾客开心,又几分钟可以保持冷静，这几分钟连续
//找连续的X最大数值,并且是生气的三个的数值最大
int maxSatisfied(std::vector<int>& customers, std::vector<int>& grumpy, int X) {

    int first = 0;
    int p = first+X-1;
    int count = 0;
    int maxCount=0;
    int satisfied = 0;
    std::vector<int> grumpyEffect(customers.size(),0);

    for(int i=0;i<customers.size();i++){
        if(grumpy[i])
            grumpyEffect[i] = customers[i];
        else{
            satisfied+=customers[i];
        }
    }

    while(p<customers.size()){
        for(int j=first;j<=p;j++){
            count+=grumpyEffect[j];
        }
        
        maxCount = std::max(count,maxCount); 
        count = 0; 
        p++;
        first++; 
    }

    return maxCount+satisfied;
}

//青蛙跳台阶的问题
//n级台阶的跳法
//本质上和上面就是相同的f(n) = f(n-1)+f(n-2)
//只是基数变动了f(1) = 1 f(2) = 2罢了
int FrogSteps(int n){


    return 0;
}

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


#pragma region 背包问题

///动态规划背包问题:一定大小的背包装进去最重的货物
///货物的重要要保持最大,B(n,c) = Max[B(n-1,c),B(n-1,c-1)+W]
///对于前 i 个物体，在背包容量为w的时候，可以装的最大价值是dp[i][w]。
///所以针对每一个物体的时候都要评测背包的容量的大小。
///效率较低，完成另一种效率更高的方式
int dynamicBackpack(int maxWeight, std::vector<std::pair<int,int>> goods){
    std::vector<std::vector<int>> 
        tempt(goods.size()+1,std::vector<int>(goods.size()+1,0));
    
    for(int i=1;i<=goods.size();i++){
        for(int w = 1;w<=maxWeight;w++){
            if(w - goods[i-1].second<0)
                tempt[i][w] = tempt[i-1][w];
            else //足够存放，比较放空余空间大小的之后再放当前重量的，最大的价值
                tempt[i][w] = std::max(tempt[i-1][w - goods[i-1].second]+
                    goods[i].second,tempt[i-1][w]);
        }
    }
    return tempt[maxWeight][goods.size()];
}
#pragma endregion
