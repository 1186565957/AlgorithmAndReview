#ifndef _BTREE_H
#define _BTREE_H

/*
 *  BTree is one of balanced binary tree, the key of leftNode is less than the keynum
 *  but more than ceil(m/2)-1.Date can store in any node of the tree.
 *
 * 需要重新整理这颗B树的结构了。
*/
//define the MaxDegree of BTree,the max 
//nad min num of keyword,and the type of key;
#define MAXM 10
#define m 4
#define max (m-1)
#define min ((m-1)/2)


//Define the TreeNode struct
typedef struct node{
   int keynum;
   int  key[max];
   struct node* parent;
   struct node* ptr[max];
}BTNode,*BTree;

//Result of BTreeSearch struct;
typedef struct{
   BTNode* ptr;
   int i;
   int tag;
}Result;

typedef struct LNode{
   BTree data;
   struct LNode *next;
}LNode,*LinkList;


int  InitBTree(BTree *t);
int SearchBTNode(BTNode* p,int k);
Result SearchBTree(BTree t,int k);
void InsertBTNode(BTNode *p,int i,int k,BTNode *q);
void InsertBTree(BTree *t,int i,int k,BTNode *p);
void Remove(BTNode *p,int i);

void SpliteBTNode(BTNode *p,BTNode **q);
void NewRoot(BTNode *t,int k,BTNode *p,BTNode *q);

void Substitution(BTNode *p,int i);
void MoveRight(BTNode* p,int i);
void MoveLeft(BTNode* p, int i);

void Combine(BTNode* p,int i);
void AdjustBTree(BTNode* p,int i);

int FindBTNode(BTNode* p,int k, int i);
int BTNodeDelete(BTNode* p,int k);

void DestoryBTree(BTree *t);

int InitQueue(LinkList l);
LNode* CreateNode(BTree t);
int IfEmpty(LinkList L);
void DestoryQueue(LinkList L);
int Traverse(BTree t,LinkList L,int newline,int sum);


#endif
