/*
*	1.红黑树插入最多转两次，删除最多三次
*	2.用自身的平衡性去换取更少的旋转性质
*	3.插入直到自身的父节点是黑色的时候，或者自身成为了根节点停止
*	4.删除的时候，直到自身是根节点，或者自身成为红色节点之后，删除自身即可
*	5.其他时候依照笔记进行旋转，所有的旋转都是从最底层的叶节点开始的。
*	@ym
**/
#pragma once
#include<stdio.h>
#include<stdlib.h>

enum ColorType
{
	Red,
	Black,
};


template<class T>
struct RBNode
{
	ColorType color ;
	T data;
	RBNode* leftNode;
	RBNode* rightNode;
	RBNode* parentNode;
	RBNode* broNode;
};

template<class T>
class RBTree {

public:
	RBTree(const T tdata) {
		Root = (RBNode*)malloc(sizeof(RBNode));
		Root->color = true;
		Root->data = tdata
		Root->leftNode = nullptr;
		Root->rightNode = nullptr;
		Root->parentNode = nullptr;
		Root->broNode  = nullptr;
	}

	virtual ~RBTree() {
		free(Root);
	}
private:
	RBNode* Root;
private:
	//定义的左旋是以当前节点进行向左下移动的左旋
	//右旋同理

	RBNode* CreateNode(T tdata) {
		RBNode<T>* newNode = (RBNode*)malloc(sizeof(RBNode));
		newNode->color = Red;
		newNode->data = tdata;
		newNode->leftNode = nullptr;
		newNode->rightNode = nullptr;
		newNode->parentNode = nullptr;
	}

	void leftRotate(RBNode<T>* Node,RBNode<T>*parent) {
		if (Node == nullptr)
			return;
		if (parent == nullptr)
			Root = Node->rightNode;


		if (Node->rightNode->leftNode != nullptr) {
			RBNode<T>* tempNode = Node->rightNode->leftNode;			
			Node->rightNode->leftNode = Node;
			Node->rightNode->parentNode = Node->parentNode;
			if (parent->leftNode == Node)
				parent->leftNode = Node->rightNode;
			else
				parent->rightNode = Node->rightNode;

			Node->parentNode = Node->rightNode;
			Node->broNode = Node->rightNode->rightNode;			
			Node->rightNode->rightNode->broNode = Node;

			Node->rightNode = tempNode;
			tempNode->parentNode = Node;
			tempNode->broNode = Node->leftNode;
			Node->leftNode->broNode = tempNode;

		}
		else {

			Node->rightNode->leftNode = Node;
			Node->rightNode->parentNode = Node->parentNode;
			if (parent->leftNode == Node)
				parent->leftNode = Node->rightNode;
			else
				parent->rightNode = Node->rightNode;

			Node->parentNode = Node->rightNode;
			Node->broNode = Node->rightNode->rightNode;
			Node->rightNode->rightNode->broNode = Node;

			Node->rightNode = nullptr;
			Node->leftNode->broNode = nullptr;
		}
	}


	void rightRotate(RBNode<T>* Node,RBNode<T>* parent) {
		if (Node == nullptr)
			return;
		if (parent == nullptr)
			Root = Node->leftNode;

		if (Node->leftNode->rightNode != nullptr) {

			RBNode<T>* tempNode = Node->leftNode->rightNode;
			Node->leftNode->rightNode = Node;
			Node->parentNode = Node->leftNode;
			Node->leftNode->parentNode = parent;
			if (parent->leftNode == Node)
				parent->leftNode = Node->leftNode;
			else
				parent->rightNode = Node->leftNode;

			Node->broNode = Node->leftNode->leftNode;
			Node->leftNode->leftNode->broNode = Node;

			Node->leftNode = tempNode;
			tempNode->broNode = Node->rightNode;
			tempNode->parentNode = Node;
			Node->rightNode->broNode = tempNode;



		}
		else {
			Node->leftNode->rightNode = Node;
			Node->parentNode = Node->leftNode;
			Node->leftNode->parentNode = parent;
			if (parent->leftNode == Node)
				parent->leftNode = Node->leftNode;
			else
				parent->rightNode = Node->leftNode;

			Node->broNode = Node->leftNode->leftNode;
			Node->leftNode->leftNode->broNode = Node;

			Node->letNode = nullptr;
			Node->rightNode->broNode = nullptr;

		}
	}
	//针对插入节点时候的修正问题
	RBNode* InsertFixedUp(RBNode* Node) {
		if (Node == Root)
			return nullptr;
		if (Node->parentNode->color == Black)
			return;
		//父节点红色，叔叔节点红色
		if (Node->parentNode->color == Red && Node->parentNode->broNode->color == Red) {
			Node->parentNode->color = Black;
			Node->parentNode->broNode->color = Black;
			return InsertFixedUp(Node->parentNode->parentNode);
		}//父节点红色，叔叔黑色，当前右节点
		else if(Node->parentNode->rightNode == Node && Node->parentNode->color == Red
			&& Node->parentNode->broNode->color == Black) {
			leftRotate(Node->parentNode);
			return InsertFixedUp(Node);
		}//父节点红色，叔叔黑色，当前左节点
		else if (Node->parentNode->leftNode == Node && Node->parentNode->color == Red
			&& Node->parentNode->broNode->color == Black) {
			Node->parentNode->color = Black;
			rightRotate(Node->parentNode->parentNode);
			return InsertFixedUp(Node->parentNode);
		}
	}
	//针对删除节点的时候的节点修正问题
	RBNode* DeleteFixedUp(RBNode* Node) {
		if (Node == Root)
			return nullptr;
		if (Node->color == Red)
			return Node;
		//双黑，兄弟为红色
		if ( Node->color == Black) {
			Node->broNode->color = Black;
			Node->parentNode->color = Red;
			leftRotate(Node->parentNode);
			return DeleteFixedUp(Node->broNode);
		}
		else if ( Node->broNode->color == Black &&	//双黑，兄弟为黑，侄子都是黑
			Node->broNode->leftNode->color == Black && Node->broNode->rightNode->color == Black) {
			Node->broNode->color = Red;
			return DeleteFixedUp(Node->parentNode);
		}
		else if ( Node->broNode->color == Black		//双黑，兄弟为黑，左侄子红色，右侄子黑色
			&& Node->broNode->leftNode->color == Red && Node->broNode->rightNode->color == Black) {
			Node->broNode->color = Red;
			Node->broNode->leftNode->color = Black;
			rightRotate(Node->parentNode, Node->parentNode);
			return DeleteFixedUp(Node->broNode);
		}
		else if (Node->broNode->color == Black		//双黑，兄弟为黑，左侄子任意，右侄子红色
			&& Node->broNode->rightNode->color == Red) {
			Node->broNode->color = Node->parentNode->color;
			Node->parentNode->color = Black;
			Node->broNode->rightNode = Black;
			leftRotate(Node->parentNode);
			return DeleteFixedUp(Node->parentNode);
		}
	}

public:
	void InsertNode(T data);
	int DeleteNode(RBNode<T>* Node);
	RBNode<T>* SearchNode(T data);
};