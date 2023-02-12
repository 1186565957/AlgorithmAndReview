#include"BTreepro.h"
/*
   BTreePro is different from BTree, the BP's key can equal the Maxsize, and the childrenNum
   is also equal the keynum in some case;the leftNode is a linklist, which is connected with ptr;
*/

//INT_MIN is the minimum of int declared in limits.h
static KeyType Unavailable = INT_MIN;

static BPlusTree MallocNewNode(){
   BPlusTree NewNode;
   int i;
   NewNode= malloc(sizeof(struct BPlusNode));
   if (NewNode==NULL) {
      exit((EXIT_FAILURE));
   }
   i=0;
   while (i<M+1) {
      NewNode->key[i]=Unavailable;
      NewNode->Children[i]=NULL;
      i++;
   }
   NewNode->Next=NULL;
   NewNode->KeyNum=0;
   return NewNode;
}

extern BPlusTree Initialize(){
   BPlusTree T;
   //there was a brackets
   if (M< 3 ) {
      printf("M is less than 3");
      exit(EXIT_FAILURE);
   }
   //set ROOTNODE
   T=MallocNewNode();
   return T;
}

static Position FindMostLeft(Position P){
   Position Temp;
   Temp = P;
   while (Temp!=NULL && Temp->Children[0]!=NULL) {
      Temp=Temp->Children[0];
   }
   return Temp;
}
static Position FindMostRight(Position p){
   Position Temp;
   Temp =p;
   while (Temp!=NULL && Temp->Children[Temp->KeyNum-1]!=NULL) {
      Temp=Temp->Children[Temp->KeyNum-1];
   }
   return Temp;
}

//Searchi for a empty brotherNode, if it disappeared, return NULL;
static Position FindSibiling(Position parent, int i){
   Position Sibling;
   int Limit;
   Limit = M;
   if (i==0) {
      if (parent->Children[1]->KeyNum<Limit) {
         Sibling = parent->Children[1];
      } 
   }else if (parent->Children[i-1]->KeyNum<Limit) {
         Sibling = parent->Children[i-1];
      }else if(i+1<parent->KeyNum && parent->Children[i+1]->KeyNum<Limit){
         Sibling =parent->Children[i+1];
      }
   return Sibling;
}
//Finding a Sibling Node whose keynum more than two
//why define a point j?
static Position FindSiblingKeyNum_M_2(Position parent,int i,int *j){
   int limit;
   Position Sibling=NULL;
   limit=LIMIT_M_2;
   if (i==0) {
      if (parent->Children[1]->KeyNum > limit) {
         Sibling=parent->Children[1];
         *j=1;
      }
   }else{
      //i+1 and i-1 means the two side of the iNode
      if (parent->Children[i-1]->KeyNum>limit) {
         Sibling=parent->Children[i-1];
         *j=i-1;
      }else if(i+1<parent->KeyNum && parent->Children[i+1]->KeyNum>limit){
         Sibling=parent->Children[i+1];
         *j=i+1;
      }
   }
   return Sibling;
}
//Two model
//Insert k into Nodex, i is the index of node in parentNode,j is the key's index;
//and insert x into parentNode, judge it by the value of isKey
static Position InsertElement(int isKey, Position parent, Position X, KeyType k,int i,int j){
   int knum;
   //remove the node whose key is biger than j
   //Empty a space for the new node
   if (isKey) {
      knum=X->KeyNum - 1;
      while (knum>=j) {
         X->key[knum+1]=X->key[knum];
         knum--;
      }
   }else{
      //Judge the x is LeafNode, and link them together.
      if (X->Children[0]==NULL) {
         if (i>0) {
            parent->Children[i-1]=X;
         }
         X->Next=parent->Children[i];
      }
      knum=parent->KeyNum-1;
      while (knum>=i) {
         parent->Children[knum+1]=parent->Children[knum];
         parent->key[knum+1]=parent->key[knum];
         k--;
      }
      parent->key[i]=X->key[0];
      parent->Children[i]=X;
      parent->KeyNum++;
   }
   return X;
}
//delete the node of jIndex
static Position RemoveElement(int isKey, Position parent, Position X, int i, int j){
   int knum, limit;
   if (isKey) {
      limit=X->KeyNum;
      knum=j+1;
      while (knum<limit) {
         X->key[knum-1]=X->key[knum];
         knum++;
      }
      X->key[X->KeyNum-1]=Unavailable;
      parent->key[i]=X->key[0];
      X->KeyNum--;
   }else{
      //remove the node of iLeaveNode,and combine the rest;
      if (X->Children[0]==NULL&&i>0) {
         parent->Children[i-1]->Next=parent->Children[i+1];
      }
      limit = parent->KeyNum;
      knum=i+1;
      while (knum<limit) {
         parent->Children[knum-1]=parent->Children[knum];
         parent->key[knum-1]=parent->key[knum];
         knum++;
      }
      parent->Children[parent->KeyNum-1]=NULL;
      parent->key[parent->KeyNum-1]=Unavailable;
      parent->KeyNum--;
   }
   return X;
}
//src and dst is next to each other, move src into dst ,n is the distance of movement.
static Position MoveElement(Position src, Position dst, Position parent,int i, int n){
   KeyType tempkey;
   Position child;
   int j, srcInfront = 0;

   if (src->key[0]<dst->key[0]) {
      srcInfront=1;
   }
   j = 0;

   if (srcInfront) {
      if (src->Children[0]!=NULL) {
         while (j<n) {
            child=src->Children[src->KeyNum -1];
            //remove sec'child node and insert empty childNode into dstNode
            RemoveElement(0,src,child,src->KeyNum,Unavailable);
            InsertElement(0,dst,child,Unavailable,0,Unavailable);
            j++;
         }
      }else{
         while (j<n) {
            tempkey = src->key[src->KeyNum-1];
            RemoveElement(1,parent,src,i,src->KeyNum-1);
            InsertElement(1,parent,dst,tempkey,+1,0);
            j++;
         }
         //------------------------------------------
      }
   }
}

