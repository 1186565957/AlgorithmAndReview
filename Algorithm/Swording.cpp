#include "common_c_plus.h"


struct listNode{
    int val;
    listNode *next;
};

bool reverlist(listNode *head){
    if(head == nullptr)
        return false;

    listNode *cur = head;
    listNode *pre = nullptr;

    while(cur != nullptr){
        listNode *temp = cur->next;
        cur->next = pre;
        pre = cur;
        cur = temp;
    }
    
    return true;
}


//添加复制运算符函数
//这样还有异常处理的问题的
class CMyString 
{
    public:
        CMyString(char *pData = nullptr);
        CMyString(const CMyString &str);
        ~CMyString(void);

    private:
        char *m_pData;

    private:
        CMyString& operator =(const CMyString &str){
            if(this == nullptr)
                return *this;
            
            delete []m_pData;
            m_pData = nullptr;

            m_pData = new char[strlen(str.m_pData)+1];
            strcpy(this->m_pData, str.m_pData);
            
            return *this;
        }
};


/// @brief 修造重复数字：常规使用排序；此次使用哈希概念找数字对应位置
///
/// @param array 
bool find_duplicata(int *array, int len){

    if(array == nullptr || len <= 0)
        return false;
    
    int i =0, temp = 0;
    for(; i< len; ){
        temp = array[i];
        if(temp == i){
            i++;
            continue;
        }
            
        if(array[i] == array[temp])
            return true;
        else
            std::swap(array[i], array[temp]);
    }
    return false;
}

//不修改原有数组找重复数字
int main()
{
    int array[] = {2,3,5,4,3,2,6,7};
    find_duplicata(array, 8);

    printf("%d", array[2]);

    //sleep(20);

    return 0;
}