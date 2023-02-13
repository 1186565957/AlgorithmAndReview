#include <unordered_map>
#include <iostream>

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

