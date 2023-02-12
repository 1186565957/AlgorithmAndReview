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

#pragma region 排序算法

//荷兰国旗问题，红白蓝国旗，给错综复杂的条，让你排序
//其实就是重复元素排序的问题
//快速排序就是解决很多次的国旗问题

///快排三重循环，头要小于尾；交换之后基准会变化，所以基准的值 意义不大
///还要保证在同一个位置的时候不会发生交换的问题。并且基准取得值不能再头和尾
//不能解决包括重复的问题，所以用到了三项排序。
//时间在nlogn,n^2之间
template<class T>
T* fast_Sort(T *array,size_t begin,size_t end){
    if(begin>=end)
        return nullptr;
    int randomNum =rand()%(end-begin+1)+begin;
    size_t i=begin,j=end;
    while(i!=j){
        while(array[i]<=array[randomNum]&&i<randomNum){           
            i++;
        }
        while(array[j]>=array[randomNum]&&j>randomNum){
            j--;
        }
        if(i!=j)                      
            swap(array[j],array[i]);              
    } 
    if(randomNum !=0)
        fast_Sort(array,begin,randomNum-1);
    if(randomNum!=end)
        fast_Sort(array,randomNum+1,end);  
    return array;
}

///为解决快速排序的重复元素问题，需要三项排序
///变成大于当前、等于当前、小于当前元素的三种排序
template<class T>
T *quickSortThreeTerms(T *array,size_t begin,size_t end){
    //三项切分部分，三个指针，分成三个部分，先把整体遍历一遍
    //第一个指针指向的永远比基准小、中间指针的时刻变动的、最后一个指针指向的比基准大，
    //所以在相同的情况下并不会对中间指针的指向数字发生任何改变
    //替换末尾的元素有可能会只换一个也大的，所以中间指针不变动。
    //基准元素选最边界的就可以改变一个边界判断的问题
    if(begin>=end)
        return nullptr;
    size_t m=begin,n=end,index=begin+1;
    //只能用边界来做，采用随机数的方式目前没有发现，会导致第一个数组大于基准的时候无法替换掉
    int Benchmarking = array[m];
    while(index<=n){

        if(array[index]<Benchmarking){
            swap(array[m++],array[index++]);
        }else if(array[index]>Benchmarking){
            swap(array[index],array[n--]);
        }else{
            index++;
        }
    }
    if(n>0)
        quickSortThreeTerms(array,begin,n-1);
    
    quickSortThreeTerms(array,n+1,end);
}



///归并排序，分治思维的体现
//需要借助额外的内存空间，是稳定排序，复杂度为nlogn
//非递归版本
//所有的递归都会在尾递归优化的过程中变成循环结构
//最后归并的块是相对有序的所以，也已直接一点一点的来剥离
//上面的两重循环就是归并中分割的过程
//稳定排序 nlogn  最好是n
template<class T>
void MergeSort(T *array,size_t len){
    //动态规划思考整体流程
    //分片次数，一次多两个
    //要倒着从后面开始归并的排出来
    T *a = array;
    T *b = new T[len];
    //第一个循环是有多少个块
    for(int seg =1;seg<len;seg*=2){
        //开始的地方
        for(int start=0;start <len;start+=seg*2){
            //内部是相对有序的快速排序的样子
            int low=start,mid = std::min(size_t(start+seg),len),high = std::min(size_t(start+seg*2),len);
            int k=low;
            int start1=low,end1=mid;
            int start2=mid,end2=high;

            while(start1<end1 && start2<end2)
                b[k++]=a[start1]<a[start2]?a[start1++]:a[start2++];
            while(start1<end1)
                b[k++] = a[start1++];
            while(start2<end2)
                b[k++]=a[start2];
        }
        swap(a,b);       
    }
    if (a != array) {
        int i;
        for (i = 0; i < len; i++)
            b[i] = a[i];
        b = a;
    }
    delete b;
}

//桶排序方式
//通过一定的算法表达式来确定要放入的桶的位置
//用最大值和最小值的区间/给的数组的元素个数，也就是每一个桶都可以装元素个数个空间
//然后通过计算桶的代表区间，进入每一个桶，然后分别对桶进行排序
template<class T>
void BucketSort(T A[],int left,int right){
    //前面还可以加一个自己计算桶数目的过程
    size_t max = LONG_MAX;
    size_t min = LONG_MIN;
    
    for(size_t i =0;i<right-left+1;i++){
        max=max(max,A[i]);
        min=min(min,A[i]);
    }

    int bucketNum = (max -min)/(right-left+1);

    size_t size = right -left+1;
   std::vector<std::vector<T>> buckets(bucketNum,0);
    
    if(A == NULL || left<1||right<1)
        return ;  

    for(int i=0;i<right-left+1;i++){
        buckets.at(A[i]-min/(right-left+1)).push_back(A[i]);
    }

    for(int i=0;i<buckets.size();i++){
        
    }
}
#pragma endregion

#pragma region 检索算法
///二分查找，判定条件就是头和尾还在一前一后或者重合的时候。
//取出重复只需要在找到之后左右遍历一下就好了
template<class T>
std::vector<T> binarySearch(T *array,size_t low,size_t high,size_t flag){
    size_t mid ;
    std::vector<T> result;
    quickSortThreeTerms(array,0,9); 

    while(low<=high){
        mid = (high+low)/2+1;
        if(array[mid] == flag){
            result.push_back(mid);
            size_t temp =mid;
            while(array[++temp] == flag){
                result.push_back(temp);               
            }
            temp=mid;
            while(array[--temp] == flag){
                result.push_back(temp);              
            }
            return result;
        }else if(array[mid]<flag){
            low = mid+1;
        }else if(array[mid]>flag){
            high = mid-1;
        }
    }

    return result;
}

///插值查找的方式
//一种自适应的区域性查找的方式
//mid=low+(key-a[low])/(a[high]-a[low])*(high-low)
//复杂度为loglogn
//这要求分布均匀一点，不均匀的话时间复杂度会更高
template<class T>
std::vector<T> InsertionSearch(T *array,size_t low,size_t high,size_t flag){
     size_t mid;
    std::vector<T> result;
    quickSortThreeTerms(array,0,9); 

    while(low<=high){
        mid = low+(flag-array[low])/(array[high]-array[low])*(high-low);
        if(array[mid] == flag){
            result.push_back(mid);
            size_t temp =mid;
            while(array[++temp] == flag){
                result.push_back(temp);               
            }
            temp=mid;
            while(array[--temp] == flag){
                result.push_back(temp);              
            }
            return result;
        }else if(array[mid]<flag){
            low = mid+1;
        }else if(array[mid]>flag){
            high = mid-1;
        }
    }

    return result;
 }

///斐波那契查找方式
//
#pragma endregion

#pragma region 哈希表相关算法
///哈希插入函数，链表法的时候里面存的就是一个
//链表的形势
enum FlagTyte{
    FLAG_EMPTY,
    FLAG_LIST,
};

///整体哈希表实现
template<class KEY>
class HashTable{

    typedef struct Node{
        KEY key;
        Node *next;
        Node(KEY key,Node *next){
            key = this->key;
            next = this->next;
        }
    }ListNode;

private:
    //一个存数，一个存标志位的
    vector<ListNode> Array;
    FlagTyte *flag;
    size_t hashSize;
    size_t hashLen;
    size_t prime;//取一个质数作为除留余数的被除数
public:
    HashTable(size_t Len,size_t prime){    

        Array.resize(Len,0);
        flag = new FlagTyte[Len];
        hashLen = Len;
        hashSize = 0;
        this.prime = prime;

        for(int i=0;i<Len;i++){
            flag[i]=FLAG_EMPTY;
        }
    }
    ~HashTable(){
        delete Array;
        delete[]flag;
    }

    void Clear(){
        for(int i=0;i<hashLen;i++){
            flag[i]=FLAG_EMPTY;
        }
        hashSize=0;
    }

    inline size_t Hash(const KEY key){

        return key%prime;
    }

    inline ListNode* JudgeExist(KEY key){

        size_t i = Hash(key);

        ListNode *p = Array[i];
        while(p.next){
            if(p->key == i)
                return p;
            p=p.next;
        }
        return nullptr;
    }

    bool insertIntoHashTable(KEY key){
        size_t i = Hash(key);

        if(flag[i] == FLAG_EMPTY)
        {
            Array[i] =key;
            flag[i] =FLAG_DATA;
            hashSize++;
            return true;
        }else {
            ListNode *p=Array[i];
            while(p->next != nullptr){
                p = p->next;                
            }

            ListNode newNode = new ListNode(NULL,nullptr);
            p->next = newNode;
            return true;
        }
        return false;
    }

    bool removeHashNode(KEY key ){

        size_t i = Hash(key);

        ListNode *currNode;
        ListNode *tempNode;
        ListNode *p = Array[i];
        while (p.next)
        {
            if (p->key == i)
            {
                currNode = p;
                if (currNode == nullptr)
                    return false;
                if (Array[Hash[key]]->key == key)
                    Array[Hash[key]] = NULL;
                else
                {
                    tempNode->next = p->next;
                    delete p;
                }
            }
            tempNode = p;
            p = p.next;

        }

        return false;
    }
};

#include <unordered_map>
/**
 * @brief LRU, one algorithm which was usually used in the memory manager
 *  to storge the data with the limited space.
 *  Insert the node into the left edge, and if the existed node 
 *  be required, move it the left edge. If the list is full, 
 *  change the rightmost one. 
 * 
 */
typedef struct lru_node
{
    void* value;

    struct lru_node* left;
    struct lru_node* right;
}LRUNode;

class LRUCache
{
private:
    //the number of LRUNode
    uint32_t cnt;
    //the capacity of the map
    uint32_t capacity;

    //can be replaced by list and list::iterator
    LRUNode* head;
    LRUNode* tail;

    //the map
    std::unordered_map<int, LRUNode> hashmap{};
private:
    void mvLeftMost(LRUNode* node){
        node->left->right = node->right;
        node->right->left = node->left;
        node->left = nullptr;
        node->right = nullptr;
        
        //insert a new one into list
        if(this->head == node){
            
        }else if(this->tail == node){
            this->tail->left->right = nullptr;
            this->tail = node->left;

            node->left = nullptr;
            node->right = this->head;
            this->head->left = node;
            this->head = node;
        }else{
            node->right = this->head;
            this->head->left = node;
            this->head = node;
        }
    }

    //Inserting into the most right when adding a new one
    void MoveToRight(LRUNode* node){
        if(this->head == nullptr){
            this->head = node;
            this->tail = node;
        }
        else{
            //storage is less than the capacity
            //insert into the tail
            if(this->cnt < this->capacity){
                this->tail->right = node;
                node->left = this->tail;
                this->tail = node;
            }else{//replace the tail one
                node->left = this->tail->left;
                node->right = this->tail->right;
                this->tail = node;
            }
        }
    }

public: 
    explicit LRUCache(int _cap) {
        if(_cap <= 1)
            std::cout << "Please set the capacity more than one" << std::endl;
        else
            this->capacity = _cap;
    }

    int putValue(int key, void* value)
    {
        //judge the storage and is exist or not
        if((this->cnt >= this->capacity) || (hashmap.find(key) != hashmap.end()))
            return -1;
        
        LRUNode* node = new LRUNode();
        MoveToRight(node);
        return 0;
    }

    void* getValue(int key)
    {
        if(hashmap.find(key) == hashmap.end())
        {
            return NULL;
        }
        LRUNode* node = &hashmap.at(key);
        mvLeftMost(node);
        return node->value;
    }

    int deleteValue(int key){
        LRUNode* node = nullptr;
        if(hashmap.find(key) != hashmap.end()){
            node = &hashmap.at(key);
            if(node->left != nullptr)
                node->left->right = node->right;
            if(node->right != nullptr)
                node->right->left = node->left;
            
            delete node;
            //free(node);
        }else{
            return -1;
        }
    }
};


#pragma endregion

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
#pragma endregion

#pragma region 搜索二叉树的相关部分
//搜索二叉树的整体实现方式以及红黑树的整体实现方式
//typedef需要类型完全可知
template <class T>
struct BSearchTreeNode
{
    T tdata;
    struct BSearchTreeNode *pLeft;
    struct BSearchTreeNode *pRight;
};
template<class T>
BSearchTreeNode<T> *CreateBSNode(T data,BSearchTreeNode<T> *Left = nullptr,BSearchTreeNode<T> *Right = nullptr){

    BSearchTreeNode<T> *newBSNode =(BSearchTreeNode<T> *) malloc(sizeof(BSearchTreeNode<T>));
    newBSNode->tdata = data;
    newBSNode->pLeft = Left;
    newBSNode->pRight = Right;
    return newBSNode;
}
//建立二叉树，如果可以还是用递归的方式老实写，思路更清晰一点，
//只需要判断一个节点是否插入就可以了
template <class T>
BSearchTreeNode<T> *CreateBSTree(T *Array,size_t len){
    int i=1;
    BSearchTreeNode<T> *Root =(BSearchTreeNode<T> *) malloc(sizeof(BSearchTreeNode<T>));
    bool flag=false;
    Root->tdata = Array[0];
    Root->pLeft = nullptr;
    Root->pRight =nullptr;
    BSearchTreeNode<T> *currentNode = Root;
    while(i<len){
        currentNode = Root;
        BSearchTreeNode<T> *newNode = CreateBSNode(Array[i]);
        flag = true;
        while(flag){
            if(newNode->tdata > currentNode->tdata){
                if(currentNode->pRight == nullptr){
                    currentNode->pRight = newNode;
                    flag = false;
                    i++;
                    continue;
                }
                else
                    currentNode = currentNode->pRight;
            }else if(newNode->tdata < currentNode->tdata){
                if(currentNode->pLeft == nullptr){
                    currentNode->pLeft = newNode;
                    flag = false;
                    i++;
                    continue;
                }
                else{
                    currentNode = currentNode->pLeft;               
                }
            }else
            {
                flag = false;
                free(newNode);
                i++;
                break;
            }
            
        }
    }

    return Root;
}

template<class T>
void DeleteNodeOfBSTree(BSearchTreeNode<T> *Root,T tdata){
    BSearchTreeNode<T> *currentNode = Root;
    while(currentNode->tdata != tdata){
        if(currentNode->tdata <tdata){
            currentNode = currentNode->pRight;
        }else {
            currentNode = currentNode->pLeft;
        }
    }
    //开始找右子树最小的点
    BSearchTreeNode<T> *MinOfRight = nullptr;
    while(MinOfRight->pLeft !=nullptr){
        MinOfRight = MinOfRight->pLeft; 
    }

    // currentNode->tdata = MinOfRight->tdata;
    // currentNode->pLeft = MinOfRight->pLeft;
    // currentNode->pRight = MinOfRight->pRight;
    // free(MinOfRight);
    //这里删除操作需要区分左右子树的情况，后面来补
}

template<class T>
void DeleteNodeOfBSTree(BSearchTreeNode<T> *Node,BSearchTreeNode<T> *Parent,T data){

    if(Node == nullptr)
        return ;
    
}


//本质上是根本没有右子树这个概念的,只有当前树节点
//这个事中序,后面补充后续
template<class T>
void InorderTraversing(BSearchTreeNode<T> *Root){
    std::stack<BSearchTreeNode<T> *> mStack;
    BSearchTreeNode<T> *currentNode = Root;
    while(!mStack.empty() || currentNode!=nullptr){
        while(currentNode !=nullptr){
            mStack.push(currentNode);
            currentNode = currentNode->pLeft;
        }       
        currentNode = mStack.top();
        mStack.pop();
        printf("%d ",currentNode->tdata);
        currentNode = currentNode->pRight;
        
    }
}
#pragma endregion

#pragma region 平衡二叉树
//为后面红黑树做铺垫，设计平衡二叉树的整体结构
template <class T>
class BlanceBTree
{
public:
    BlanceBTree(const T data,BlanceBTree<T> *left = nullptr, BlanceBTree<T> *right = nullptr):
        tdata(data),pLeft(left),pRight(right){

    }

private:
    T tdata;
    BlanceBTree<T> *pLeft;
    BlanceBTree<T> *pRight;
    BlanceBTree<T> *newNode = nullptr;
    size_t mDepth;

private:
    BlanceBTree<T> *createBlanceBNode(T tdata)
    {
        BlanceBTree<T> *newNode = new BlanceBTree<T>(tdata);
        return newNode;
    }
    //左旋
    BlanceBTree<T> *LRevolve(BlanceBTree<T> *fulcrum){
        BlanceBTree<T> *p = fulcrum->pRight;
        fulcrum->pRight =  p->pLeft;
        p->pLeft = fulcrum;
        return p;
    }
    //右旋,我用的是支点
    BlanceBTree<T> *RRevolve(BlanceBTree<T> *fulcrum){
        BlanceBTree<T> *p = fulcrum->pLeft;
        fulcrum->pLeft =  p->pRight;
        p->pRight = fulcrum;
        return p;
    }
    //左右旋,我用的是第二步的支点
    BlanceBTree<T> *LRRevolve(BlanceBTree<T> *fulcrum){
        LRevolve(fulcrum->pLeft);
        return RRevolve(fulcrum);
    }
    //右左旋
    BlanceBTree<T> *RLRevolve(BlanceBTree<T> *fulcrum){

        RRevolve(fulcrum->pRight);
        return LRevolve(fulcrum);
    }

    size_t getDepth(BlanceBTree<T> *node){
        if(node == nullptr)
            return 0;
        //有节点就加一，一直往后退
        return std::max(getDepth(node->pLeft),getDepth(node->pRight))+1;
    }
    //获取平衡因子
    size_t getBlanceCharacter(BlanceBTree<T> *Node){

        return getDepth(Node->pLeft) - getDepth(Node->pRight);
    }
    //从顶部到底部开始调整整体树的平衡性
    //寻找失衡的节点，然后进行调整
    void fixUpBlance(BlanceBTree<T> *Root){
        BlanceBTree<T> *currentNode = Root;
        while(true){
            //我用的都是左减右
            if(getBlanceCharacter(currentNode) == 2){
                if(std::abs(getBlanceCharacter(currentNode->pLeft)) == 1)
                    break;
                else
                    currentNode = currentNode->pLeft;
            }else if(getBlanceCharacter(currentNode) == -2){
                if(std::abs(getBlanceCharacter(currentNode->pRight)) == 1)
                    break;
                else
                    currentNode = currentNode->pRight;
            }else{
                break;
            }
        }

        //对当前的节点进行平衡操作
        size_t bC = getBlanceCharacter(currentNode);
        //左子树的情况
        //返回值是整个树的根节点，因为每次插入都会检查
        if(bC>1){
            if(getBlanceCharacter(currentNode->pLeft>0))
                this->RRevolve(currentNode);
            else
                this->LRRevolve(currentNode);
        }else if(bC <-1){
            if(getBlanceCharacter(currentNode->pRight>0))
                this->RLRevolve(currentNode);
            else
                this->LRevolve(currentNode);
        }
    }

public:
    //根据数组创建初始的平衡二叉树结构
    BlanceBTree<T> *createBlanceBNode(T *Array,size_t len){
        int i =0;
        BlanceBTree<T> *Root;
        while(i<len){
            InsertBlanceNode(Array[i],Root);
        }
    }
    //向平衡二叉树中插入节点
    void InsertBlanceNode(T data,BlanceBTree<T> &Root){
        BlanceBTree<T> *newNode =createBlanceBNode(tdata);
        if(Root == nullptr){
            Root = newNode;
            return ;
        }
        BlanceBTree<T> *currentNode = Root;
        while(true){
            if(currentNode->tdata <data ){
                if(currentNode->pRight == nullptr){
                    currentNode->pRight = newNode;
                    this->fixUpBlance();
                    return ;
                }else
                    currentNode = currentNode->pRight;
            }else if(currentNode->tdata > data){
                if(currentNode->pLeft == nullptr){
                    currentNode->pLeft = newNode;
                    this->fixUpBlance();
                    return ;
                }else
                    currentNode = currentNode->pLeft;
            }else
                return ;
        }
    }

    //删除二叉树中的某个节点
    //一个节点一个节点的向下去搜索位置
    void deleteNodeBT(T data,BlanceBTree<T> *Node,BlanceBTree<T> *parent){
        BlanceBTree<T> *current ;
        // while(current->tdata != data || current != nullptr){
        //     if(current->tdata >data){
        //         current = current->pLeft;
        //     }else if(current->tdata <data){
        //         current = current->pRight;
        //     }
        // }
        // if(current == nullptr)
        //     return ;
        // //当左子树和右子树都有的时候，取下面最长那棵树的一部分
        // if(current->pLeft!=nullptr && current->pRight!=nullptr){
        //     if(this->getDepth(current->pLeft)>this->getDepth(current->pRight)){
        //         //获取左子树最大节点
        //         BlanceBTree<T> *tempNode = current;
        //         while(current->pRight != nullptr)
        //             current = current->pRight;           
        //     }
        // }

        if(Node == nullptr)
            return ;
        else if(data < current->tdata){
            return deleteNodeBT(data,Node->pLeft,Node);
        }else if(data>current->tdata){
            return deleteNodeBT(data,current->pRight,Node);
        }
        //当左子树和右子树都有的时候，取下面最长那棵树的一部分
        if(current->pLeft!=nullptr && current->pRight != nullptr){
            if(this->getDepth(current->pLeft)>this->getDepth(current->pRight)){
                BlanceBTree<T> *tempNode = current->pRight;
                while(current->pRight != nullptr)
                    current = current->pRight;
                Node->tdata == current->tdata;
                //最终删除的还是原本的值，只是把最下面的节点挪上来了
                return deleteNodeBT(Node->tdata,Node->pLeft,Node);
            }else{
                BlanceBTree<T> *tempNode = current;
                while(current->pLeft != nullptr)
                    current = current->pLeft;
                Node->tdata = current->tdata;
                return deleteNodeBT(data,Node->pRight,Node);
            }
        }else{
            //有左边没右边
            if(Node->pRight == nullptr && Node->pLeft){
                BlanceBTree<T> *tempNode = current->pLeft;
                while(current->pRight != nullptr)
                    current = current->pRight;
                Node->tdata = current->tdata;
                return deleteNodeBT(Node->tdata,Node->pLeft,Node);
            }else if(Node->pRight && Node->pLeft == nullptr){ //有右边没左边的情况
                BlanceBTree<T> *tempNode = current->pRight;
                while(current->pLeft != nullptr)
                    current = current->pLeft;
                Node->tdata = current->tdata;
                return deleteNodeBT(Node->tdata,Node->pRight,Node);
            }else if(Node->pRight == nullptr && Node->pLeft == nullptr){//本身是叶子节点的情况
                if(Node == parent->pLeft){//
                    parent->pLeft == nullptr;
                }else if(Node == parent->pRight){
                    parent->pRight == nullptr;
                }
                delete Node;
                Node == nullptr;
                fixUpBlance();
            }
        }
        return ;
    }
    
};
#pragma endregion



int main(){

    //int nums1[]={4,2,7,1,10,7,234},nums2[]={1,3,4,2};
    //findBiggerElement(nums1,nums2,sizeof(nums1)/sizeof(nums1[0]),sizeof(nums2)/sizeof(nums2[0]));
    //fast_Sort(nums1,0,8);
    //quickSortThreeTerms(nums1,0,8);
    //string str="stirngmmmmmjsjjmmmsjjjmmmmmmm123";
    //string flag="mmm";
    //vector<string> m=Split(str,flag);
    //binarySearch(nums1,0,8,11);

    // std::vector<int> m ={3};
    // std::vector<int> s={1};
    // int x=1;

    //std::vector<std::vector<int>> n={{4,5},{4,6},{6,7},{2,3},{1,1}};
    //findMaxConsecutiveOnes(n);
    //longestOnes(n,2);
    //InorderTraversing(CreateBSTree(nums1,7));
    //makesquare(m);
    //lengthOfLIS(n);
    //maxEnvelopes(n);
    //nextGreaterElements(m);
    //maxSatisfied(m,s,x);

    return 0;
}