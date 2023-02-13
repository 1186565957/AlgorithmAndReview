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