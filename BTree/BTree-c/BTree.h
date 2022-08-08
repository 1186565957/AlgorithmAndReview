#ifndef _BTREE_H
#define _BTREE_H
#define MAXM 10
/*
    BTree is onee of balanced binary tree, the leftNode's key is less than the keynum
    but more than ceil(m/2)-1.Date can store in any node of the tree.
 
*/
//define the MaxDegree of BTree,the max 
//nad min num of keyword,and the type of key;
#define m 4
#define max (m-1)
#define min ((m-1)/2)

typedef int KeyType;

//Define the TreeNode struct
typedef struct node{
   int keynum;
   KeyType key[max];
   struct node* parent;
   struct node* ptr[max];
}BTNode,*BTree;

//Result of BTreeSearch struct;
typedef struct{
   //point to the node
   BTNode* ptr;
   //index of key
   int i;
   //symbol of whether success
   int tag;
}Result;
//the listset why?
typedef struct LNode{
   BTree data;
   struct LNode *next;
}LNode,*LinkList;

//init the status of code
typedef enum status{
   TRUE,
   FALSE,
   OK,
   ERROR,
   OVERFLOW,
   EMPTY,

}Status;

//Define the function of the BTree algorithm

//Focus on this function,don't see it before
Status InitBTree(BTree *t);
//search the node of p to insert key
int SearchBTNode(BTNode* p,KeyType k);
Result SearchBTree(BTree t,KeyType k);
//regard the first parameter
//insert the k and q to the tree
void InsertBTNode(BTNode* *p,int i,KeyType k,BTNode *q);
void SpliteBTNode(BTNode **p,BTNode **q);
void NewRoot(BTNode **t,KeyType k,BTNode *p,BTNode *q);
//i is the tag where to insert the node-p
void InsertBTree(BTree *t,int i,KeyType k,BTNode *p);
void Remove(BTNode *p,int i);

void Substitution(BTNode *p,int i);
void MoveRight(BTNode* p,int i);
void MoveLeft(BTNode* p, int i);
//combine right and parent node into left node
//and adjust the index of key
void Combine(BTNode* p,int i);
//delete key of index-i,and adjust the tree to balance
void AdjustBTree(BTNode* p,int i);

int FindBTNode(BTNode* p,KeyType k);
int BTNodeDelete(BTNode* p,KeyType k);

void DestoryBTree(BTree *t);

Status InitQueue(LinkList *l);
LNode* CreateNode(BTree t);
Status IfEmpty(LinkList L);
void DestoryQueue(LinkList L);
//???
Status Traverse(BTree t,LinkList L,int newline,int sum);
void Test();
#endif
