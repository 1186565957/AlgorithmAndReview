/*
*	1.������������ת���Σ�ɾ���������
*	2.�������ƽ����ȥ��ȡ���ٵ���ת����
*	3.����ֱ������ĸ��ڵ��Ǻ�ɫ��ʱ�򣬻��������Ϊ�˸��ڵ�ֹͣ
*	4.ɾ����ʱ��ֱ�������Ǹ��ڵ㣬���������Ϊ��ɫ�ڵ�֮��ɾ��������
*	5.����ʱ�����ձʼǽ�����ת�����е���ת���Ǵ���ײ��Ҷ�ڵ㿪ʼ�ġ�
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
	//������������Ե�ǰ�ڵ�����������ƶ�������
	//����ͬ��

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
	//��Բ���ڵ�ʱ�����������
	RBNode* InsertFixedUp(RBNode* Node) {
		if (Node == Root)
			return nullptr;
		if (Node->parentNode->color == Black)
			return;
		//���ڵ��ɫ������ڵ��ɫ
		if (Node->parentNode->color == Red && Node->parentNode->broNode->color == Red) {
			Node->parentNode->color = Black;
			Node->parentNode->broNode->color = Black;
			return InsertFixedUp(Node->parentNode->parentNode);
		}//���ڵ��ɫ�������ɫ����ǰ�ҽڵ�
		else if(Node->parentNode->rightNode == Node && Node->parentNode->color == Red
			&& Node->parentNode->broNode->color == Black) {
			leftRotate(Node->parentNode);
			return InsertFixedUp(Node);
		}//���ڵ��ɫ�������ɫ����ǰ��ڵ�
		else if (Node->parentNode->leftNode == Node && Node->parentNode->color == Red
			&& Node->parentNode->broNode->color == Black) {
			Node->parentNode->color = Black;
			rightRotate(Node->parentNode->parentNode);
			return InsertFixedUp(Node->parentNode);
		}
	}
	//���ɾ���ڵ��ʱ��Ľڵ���������
	RBNode* DeleteFixedUp(RBNode* Node) {
		if (Node == Root)
			return nullptr;
		if (Node->color == Red)
			return Node;
		//˫�ڣ��ֵ�Ϊ��ɫ
		if ( Node->color == Black) {
			Node->broNode->color = Black;
			Node->parentNode->color = Red;
			leftRotate(Node->parentNode);
			return DeleteFixedUp(Node->broNode);
		}
		else if ( Node->broNode->color == Black &&	//˫�ڣ��ֵ�Ϊ�ڣ�ֶ�Ӷ��Ǻ�
			Node->broNode->leftNode->color == Black && Node->broNode->rightNode->color == Black) {
			Node->broNode->color = Red;
			return DeleteFixedUp(Node->parentNode);
		}
		else if ( Node->broNode->color == Black		//˫�ڣ��ֵ�Ϊ�ڣ���ֶ�Ӻ�ɫ����ֶ�Ӻ�ɫ
			&& Node->broNode->leftNode->color == Red && Node->broNode->rightNode->color == Black) {
			Node->broNode->color = Red;
			Node->broNode->leftNode->color = Black;
			rightRotate(Node->parentNode, Node->parentNode);
			return DeleteFixedUp(Node->broNode);
		}
		else if (Node->broNode->color == Black		//˫�ڣ��ֵ�Ϊ�ڣ���ֶ�����⣬��ֶ�Ӻ�ɫ
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