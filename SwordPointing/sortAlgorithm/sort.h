#pragma once

#include <iostream>
#include <vector>

//为解决快速排序的重复元素问题，需要三项排序
//变成大于当前、等于当前、小于当前元素的三种排序
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