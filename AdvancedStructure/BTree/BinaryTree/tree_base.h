#pragma once
#include <iostream>


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