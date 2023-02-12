
#pragma once

#include<algorithm>
#include<memory>
#include<fstream>
#include<vector>
#include<stack>

#define IN 
#define OUT 
#define INOUT 
#define MAX_VERTEX_NUM 200

struct graph_Edge    
{
//数据成员:
	int mWeight;
	int mAdjIndex;
	graph_Edge *pNextEdge;

//构造函数:
	graph_Edge():
		mAdjIndex(-1),pNextEdge(nullptr),mWeight(0){}
	graph_Edge(int _adjVex,graph_Edge *_nextArc=nullptr):
		mAdjIndex(_adjVex),pNextEdge(_nextArc),mWeight(0){}
};

template<typename DataType>
struct graph_Vertex   
{
//数据成员:
	DataType mdata;
	graph_Vertex *pAdjEdge;

//构造函数:
	graph_Vertex():
		pAdjEdge(nullptr){}	 

	graph_Vertex(DataType VexValue,graph_Edge *_firstArc=nullptr){
		mdata = VexValue;
		pAdjEdge = _firstArc;
	}
};


enum g_visitStatus{VISIT,UNVISTI};

template<typename DataType>
class Directed_graph{
private:
	g_visitStatus* mTag;
	std::vector<graph_Vertex<DataType>> *mVexList;
	int vexNum;
	int EdgeNum;
	int containerLenght;
public:
	Directed_graph(int length);
	Directed_graph(DataType* vexList,int _vexNum);
	~Directed_graph(){}

	void Clear();
	bool isEmpty();
	DataType getVexContent(int Index);
	size_t getIndexOf(DataType &data);

	void InsertEdge(size_t firstVex,size_t secondVex);
	void InsertVex(DataType &data);
	void DeleteEdge(size_t firstVex,size_t secondVex);
	void DeleteVex(DataType &data);

	int GetVexNum();
	int getArcNum();
	int FirstAdjVex(size_t vex);
	int NextAdjVex(size_t firstVex,size_t secondVex);

	void show();
};

template<typename DataType>
Directed_graph<DataType>::Directed_graph(int length){
	mTag = new g_visitStatus[length];
	vexNum = 0;
	EdgeNum = 0;
	containerLenght = length;
	mVexList = new std::vector<graph_Vertex<DataType>>(MAX_VERTEX_NUM);
}

template<typename DataType>
Directed_graph<DataType>::Directed_graph(DataType* vexList,int _vexNum){
	vexNum = _vexNum;
	EdgeNum = 0;
	mTag = new g_visitStatus[MAX_VERTEX_NUM];
	mVexList = new std::vector<graph_Vertex<DataType>>(MAX_VERTEX_NUM);

	for(int i =0;i<vexNum;i++){
		mTag[i] = UNVISTI;
		mVexList->at(i).mdata = vexList[i];
	}
}

template<typename DataType>
Directed_graph<DataType>::~Directed_graph(){
	if(mTag != nullptr)
		delete[] mTag;
	if(mVexList != nullptr)
		mVexList->clear();
}

template<typename DataType>
void Directed_graph<DataType>::Clear(){
	graph_Edge *p;
	for(int i= 0;i<vexNum;i++){
		while(mVexList->at(i).pAdjEdge != nullptr){
			p = mVexList->at(i).pAdjEdge;
			mVexList->at(i).pAdjEdge = p->pNextEdge;
			delete p;
		}
	}
	EdgeNum = 0;
}

template<typename DataType>
bool Directed_graph<DataType>::isEmpty(){

	return vexNum == 0;
}

template<typename DataType>
DataType Directed_graph<DataType>:: getVexContent(int Index){
	if(Index <0 || Index> vexNum-1)
		return -1;

	return mVexList->at[Index].mdata;
}

template<typename DataType>
size_t Directed_graph<DataType>::getIndexOf(DataType &data){
	for(int i=0;i<vexNum;i++)
		if(mVexList->at(i).mdata == data)
			return i;
	return -1;
}

template<typename DataType>
void Directed_graph<DataType>::InsertEdge(size_t firstVex,size_t secondVex){
	if(firstVex <0 || firstVex >vexNum)
		return false;
	if(secondVex<0 || secondVex >vexNum)
		return false;
	
	mVexList->at(firstVex).pAdjEdge = new graph_Edge(secondVex,mVexList->at(firstVex).pAdjEdge);
	EdgeNum++;

}

template<typename DataType>
void Directed_graph<DataType>:: InsertVex(DataType &data){
	if(vexNum == MAX_VERTEX_NUM)
		return ;
	mVexList->at(vexNum).mdata = data;
	mVexList->at(vexNum).mAdjEdge = nullptr;
	mTag[vexNum] = UNVISTI;
	vexNum++;
}

template<typename DataType>
void Directed_graph<DataType>::DeleteEdge(size_t firstVex,size_t secondVex){
	if(firstVex <0 || firstVex >vexNum)
		return false;
	if(secondVex<0 || secondVex >vexNum)
		return false;

	graph_Edge *p ,*q;
	p = mVexList->at(firstVex).mAdjEdge;
	while(p!= nullptr && p->mAdjIndex != secondVex){
		q = p;
		p = p->pNextEdge;
	}

	if(p!= nullptr){
		if(mVexList->at(firstVex).mAdjEdge == p)
			mVexList->at(firstVex).mAdjEdge = p->pNextEdge;
		else
		{
			q->pNextEdge = p->pNextEdge;
		}
		delete p;
		EdgeNum--;
	}
}


template<typename DataType>
void Directed_graph<DataType>::DeleteVex(DataType &data){
	int Indexof = getIndexOf(data);
	int temp;

	if(v == -1)
		return ;
	//删除终点边
	for(temp=0;temp<vexNum;temp++){
		DeleteEdge(temp,Indexof);
	}
	//删除起点边
	graph_Edge *p = mVexList->at(Indexof).mAdjEdge;
	while(p != nullptr){
		mVexList->at(Indexof).mAdjEdge = p->pNextEdge;
		delete p;
		p = mVexList->at(Indexof).mAdjEdge;
		EdgeNum--;
	}
	//末尾点
	if(Indexof == vexNum-1)
		vexNum --;
	else{
		vexNum--;
		mVexList[Indexof] = mVexList[vexNum];
		mVexList[vexNum].mAdjEdge = nullptr;
		mTag[Indexof] = mTag[vexNum];
		for(temp =0;temp<vexNum;temp++){
			//给其他的节点的指向重排
			p = mVexList->at(temp).mAdjEdge;
			while(p!= nullptr){
				if(p->mAdjIndex == vexNum)
					p->mAdjIndex = Indexof;
				p = p->pNextEdge;
			}
		}
	}
}




