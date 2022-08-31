

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

#pragma region 字符匹配算法
///KMP字符串匹配算法
///基本方式：里面next的值是前缀集合和后缀集合的交集的最大长度。
///把所有拆分更好看出来，一个开头算，一个结尾算，算的是模式字符串的长度
///主字符串到当前卡主位置是和模式字符串相同的，那么这个最大匹配长度就可以直接移动自己的距离了
///因为是倒着算的，所以没问题
///next数组获取就是对自身重复匹配的过程，第一个值一定是-1，编程方便
///一直向后移动，相同过的就开始匹配，不同的标零，相同的就开始置一，后面的相加。
/// next数组需要全体的数向后移动一位，因为是数组中的位置
int KMPStringMatch(const std::string desString, const std::string flag,const int flagLen){
    std::vector<int> next(flagLen,0);
    int i=0,j=0;
    getNext(flag.c_str(),next);
    
    while(i<desString.length() && j<flag.length()){
        if(j == -1 || desString[i] == flag[j]){
            j++;i++;
        }else{
            j=next[j]; 
        }
    }
    if(j == flag.length())
        return i-j;
    else
    {
        return -1;
    }
    
    return ;
}
//
void getNext(const char *str,std::vector<int> &next){
    next[0] = -1;
    int index=0, j=-1;
    while( index < strlen(str)){
        if(j == -1 || str[index] == str[j]){
            index++;
            j++;
            next[index] = j;
        }else   //初始化都是0
        {
            j = next[j];
        }
    }
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