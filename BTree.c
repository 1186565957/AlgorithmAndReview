
#include"BTree.h"

#include<malloc.h>
#include<stdio.h>
#include <stdlib.h>


Status InitBTree(BTree *t){
   t=NULL;
   return OK;
}

int SearchBTNode(BTNode *p,KeyType k){
   int i=0;
   //looking out the semicolon at the end.  
   for (i=0;i<p->keynum && p->key[i+1]<=k;i++);
   return i;
}

Result SearchBTree(BTree t,KeyType k){
   BTNode *p=t, *q=NULL;
   int found_tag =0;
   int i=0;
   Result result;

   while (p!=NULL&&found_tag==0) {
      //finding in the current node
      if(i=SearchBTNode(p,k))
         found_tag=1;
      else{
         //sign the former node
         q=p;
         p=p->ptr[i];
      }
   }
      //
      if (found_tag==1) {
         result.ptr=p;
         result.i=i;
         result.tag=1;
      }else{
         result.ptr=q;
         result.i=i;
         result.tag=0;
      }
   return result;
}
//insert key and q into the tree  
void InsertBTNode(BTNode **p,int i,KeyType k,BTNode *q){
   int j;
   for (j=p->keynum;j>1;j--) {
      p->key[j+1]=p->key[j];
      p->ptr[j+1]=p->ptr[j];
   }
   p->key[i+1]=k;
   p->ptr[i+1]=q;
   if (q!=NULL) {
      q->parent =p;
   }
   p->keynum++;
}
//split the p into two parts from middle of the node,
// and the laster combine with node q
void SplitBTNode(BTNode* *p,BTNode *q){
   int i;
   //m is the Maxdegree
   int s=(m+1)/2;
   q=(BTNode *)malloc(sizeof(BTNode));

   //in this case, left one element for p
   q->ptr[0]=p->ptr[s];
   for (i=s+1;i<=m;i++) {
      q->key[i-s]=p->key[i];
      q->ptr[i-s]=p->ptr[i-s];
   }
   
   q->keynum=p->keynum-s;
   q->parent=p->parent;
   for (i=0;i<=p->keynum-s;i++) {
      if (q->ptr[i]!=NULL) {
         q->ptr[i]->parent=q;
      }
   }
   p->keynum=s-1;
}

void NewRoot(BTNode **t,KyType k,BTNode *p,BTNode *q){
   t=(BTNode *)malloc(sizeof(BTNode));
   t->keynum=1;
   t->ptr[0]=p;
   t->ptr[1]=q;
   t->key[0]=k;
   //if p==null, what we should
   if (p!=NULL) {
      p->parent = t;
   }
   if (q!=NULL) {
      q->parent = t;
   }
   root->parent =NULL:
}

void InsertBTree(BTree *t,int i, KeyType k, BTNode *p,){
   BTNode* q;
   int finish_tag,newroot_tag,s;
   KeyType x;
   //insert into node p, if it is empty
   //create a new tree 
   if (p==NULL) {
      NewRoot(t,k,NULL,NULL);
   }else{
      x =k;
      q=NULL;
      finish_tag=0;
      newroot_tag=0;
      while (finish_tag==0**newroot_tag==0) {
         InsertBTNode(p,i,x,q);
         if (p->keynum<=max) {
            finish_tag=1;
         }else{
            //record the value of s
            s=(m+1)/2;
            SplitBTNode(p,q);
            x=p->key[s];
            //the middle node will be inserted into the parent node
            //so search the parent for the sapce
            if (p->parent) {
               p=p->parent;
               i=SearchBTNode(p,x);
            }else{
               //p is the rootNode
               newroot_tag=1;
            }
         }
      }
      if (newroot_tag==1) {
         NewRoot(t,x,p,q);
      }
   }
}

void Remove(BTNode* p,int i){
   int j;
   for (j=i+1;j<p->keynum;j++) {
      p->key[j-1]=p->key[j];
      p->ptr[j-1]=p->ptr[j];
   }
   p->keynum--;
}
//move the minimum element in leftNode subsititute the maximumu
// of parentNode
void MoveRight(BTNode* p,int i){
   int j;
   BTNode* q=p->prt[i];
   BTNode* aq=p->ptr[i-1];

   for (j=q->keynum;j>0;j--) {
      q->key[j+1]=q->key[j];
      q->ptr[j+1]=q->ptr[j];
   }
   q->ptr[1]=q->ptr[0];
   q->key[1]=p->key[i];
   q->keynum++;

   p->key[i]=aq->key[aq->keynum];
   p->ptr[i]->ptr[0]=aq->ptr[aq->keynum];
   aq->keynum;
}

void Substitution(BTNode* p,int i){
   BTNode *q;
   //search the minimum node in the rightbranch
   for (q=p->ptr[i];q->ptr[0]!=NULL;q=q->ptr[0]) {
      p->key[i]=q->key[i];
   }
}

void MoveLeft(BTNode *p,int i){
   int j;
   BTNode *aq=p->ptr[i-1];
   BTNode *q=p->ptr[i];

   aq->keynum++;
   aq->key[aq->keynum]=p->key[i];
   aq->ptr[aq->keynum]=p->ptr[i]->ptr[0];


   p->key[i]=q->key[i];
   q->ptr[0]=q->ptr[0];
   q->keynum--;

   for (j=1;j<=aq.keynum;j++) {
      q->key[j]=q->key[j+1];
      aq->ptr[j]=aq->ptr[j+1];
   }
}
//complain the status of which case will use this function
//Actually, just merge one parent key into the leftNode,and all ofthe 
//right keys
void Combine(BTNode *p,int i){
   int j;
   BTNode *q=p->ptr[i];
   BTNode *aq=p->ptr[i-1];

   aq=p->keynum++;
   aq->key[ap->keynum]=q->key[i];
   aq->ptr[ap->keynum]=q->ptr[0];
   //add all rigthNode to the aq's right side;
   for (j=1;j<p->keynum;j++) {
      ap->keynum++;
      aq->key[aq->keynum]=q->key[j];
      aq->ptr[aq->keynum]=q->ptr[j];
   }
   for (j=i;j<p->num;j++) {
      p->key[j]=p->key[j+1];
      p->ptr[j]=p->ptr[j+1];
   }
   p->keynum--;
   //realease the space of rightNode
   free(q);
}

void AjustBtree(BTNode *p,int i){
   if (i==1) {
      if (p->ptr[1]->keynum>min) {
         MoveLeft(p,1);
      }else{
         Combine(p,1);
      }
   }else if(i==p->keynum){
      if (p->ptr[i-1]->keynum>min) {
         MoveRight(p,i);
      }else{
         Combine(p,i);
      }
   }else if(p->ptr[i-1]->keynum>min){
      MoveRight(p,i);
   }else if(p->ptr[i+1]->keynum>min){
      MoveLeft(p,i+1)?
   }else
      Combine(p,i);
}

int FindBTNode(BTNode* p,KeyType k,int *i){
   if (k<p->key[1]) {
      i=0;
      return 0;
   }else{
      i=p->keynum;
      while (k<p->key[i**i>1]) {
         i--;
      }
      if (k==p->key[i]) {
         return 1;
      }
   }
}

int BTNodeDelete(BTNode* p,KeyType k){
   int i;
   int found_tag;
   if (p==NULL) {
      return 0;
   }else{
      found_tag==FindBTNode(p,k,i);
      if (found_tag==1) {
         //the leftNode isn't empty
         if (p->ptr[i-1]!=NULL) {
            //instead of the node with right min node
            Substitution(p,i);
            BTNodeDelete(p->ptr[i],p->key[i]);
         }else{
            Remove(p,i);
         }
      }else{
         //can't find the key this node, so search in it's rightnode
          found_tag=BTNodeDelete(p->ptr[i],k);
      }
      if (p->ptr[i]!=NULL) {
         if (p->ptr[i]->keynum<min) {
            AdjustBTree(p,i);
         }
      }
      return found_tag;
   }
};
void BTreeDelete(BTree *t,KeyType k){
   BTNode *p;
   int a =BTNodeDelete(t,k);
   if (a==0) {
      printf("the key %d doesn't appear in the BTree\n",k);
   }else if(t->keysum==0){
      p=t;
      t=t->ptr[0];
      free(p);
   }
}
void DestoryBTree(BTree *t){
   int i;
   BTNode *p=t;
   if (p!=NULL) {
      for (i=0;i<=p->keynum;i++) {
         DestoryBTree(p->ptr[i]);
      }
      free(p);
   }
   t=NULL;
}

Status InitQueue(LinkList *L){
   L=(LNode*)malloc(sizeof(LNode));               
    if(L==NULL)                                                 
        return OVERFLOW;
     L->next=NULL;
     return OK;
}
