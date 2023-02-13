#incldue <iosream>

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

/**
 * The KMP character-matching algorithm. Effective: O(n)
 * The essence is the transfer of state-machine
 * Key-point: keeping the dp-array which is only releated with the flag-str
 * DP-ARRAY: Value in array is meaning the maximum Length of the substring,
 * which matched by the reverser order
 * The next dp-value is depending on the previous value, because the new char is only compared
 * with the next charactor
 *
 * @return The first starting position of flag-str in string
 */
int KMP_strmatch(char *string, uint32_t str_len, char *flag, uint32_t flag_len)
{
    // All values in dp-array mimnus one. Then the val is according to the compare-idx
    int i = 2, comp_idx = 0;
    int dp[flag_len] = {-1, 0};
    while (i < flag_len)
    {
        if (string[i - 1] == string[comp_idx])

            dp[i++] = ++comp_idx;
        else
        {
            if (comp_idx > 0)
                comp_idx = dp[comp_idx];
            else
                comp_idx = 0;
        }
    }

    // Doing the match progress while the idx is coninclude with the str-end
    int ii, iii, idx;
    while (ii < str_len && iii < flag_len)
    {
        if (string[ii] == flag[iii])
        {
            ii++;
            iii++;
        }
        else
        {
            if (dp[iii] == -1)
                ii++;
            else
                iii = dp[iii];
        }
    }

    // Determine whether the iii has moved to end of the fla-str
    idx = (iii == flag_len ? ii - iii : -1);
    return idx;
}