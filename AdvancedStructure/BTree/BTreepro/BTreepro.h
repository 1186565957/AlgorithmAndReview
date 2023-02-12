#ifndef BTreepro_h
#define BTreepro_h

/**
 * @file BTreepro.h
 * @author yanmo 
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * 
 * 
 * @copyright Copyright (c) 2023
 * 
 */




#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

#define M (4)
#define LIMIT_M_2 (M%2?(M+1)/2:M/2)

typedef struct BPlusNode *BPlusTree,*Position;
typedef int KeyType;
//key[0] is abandoned,
struct BPlusNode{
   int KeyNum;
   KeyType key[M+1];
   BPlusTree Children[M+1];
   BPlusTree Next;
};
extern BPlusTree Initialize();
extern BPlusTree Insert(BPlusTree T,KeyType key);
extern BPlusTree Remove(BPlusTree T,KeyType key);
extern BPlusTree Destory(BPlusTree T);
extern void Travel(BPlusTree T);

extern void TravelData(BPlusTree T);

#endif
