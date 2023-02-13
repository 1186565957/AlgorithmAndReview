#pragma once

#include <iostream>

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


