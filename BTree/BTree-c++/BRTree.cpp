#include "BRTree.h"


template<class T>
void RBTree<T>::InsertNode(T data) {

	RBNode<T>* newNode =  CreateNode(data);
	RBNode<T>* currentNode = Root;
	while (currentNode != nullptr) {
		if (currentNode->data >= data)
			currentNode = currentNode->rightNode;
		else
			currentNode = currentNode->leftNode;
	}
	if (currentNode->parentNode->leftNode == currentNode) {
		currentNode->parentNode->leftNode = newNode;
		currentNode->parentNode->rightNode->broNode = newNode;
		newNode->broNode = currentNode->parentNode->rightNode;
	}
	else {
		currentNode->parentNode->rightNode = newNode;
		currentNode->parentNode->leftNode->broNode = newNode;
		newNode->broNode = currentNode->parentNode->leftNode;
	}

	newNode->parentNode = currentNode->parentNode;
	free(newNode);

	InsertFixedUp(newNode);
}

template<class T>
int  RBTree<T>::DeleteNode(RBNode<T>* Node) {
	if (Root->data == Node->data)
		free(Root);
	//this.InsertNode(data);

	RBNode<T>* currentNode = Node;
	RBNode<T>* temptNode = currentNode;
	//��һ������������Ҷ��������
	if (currentNode->color == Red) {//���±غ�
		if (currentNode->leftNode == nullptr) {
			//������
			if (currentNode->rightNode == nullptr) {
				free(currentNode);
				return 0;
			}
			//��������
			//temptNode = currentNode;
			currentNode = currentNode->rightNode;
			free(temptNode)
			return 0;
		}
		else {
			//��������
			if (currentNode->rightNode == nullptr) {
				//temptNode = currentNode;
				currentNode = currentNode->leftNode;
				free(temptNode)
				return 0;
			}
			//������������
			/*temptNode = currentNode;
			while (temptNode->leftNode != nullptr) {

				if (temptNode->color == Red) {
					currentNode->data = temptNode->data;
					free(temptNode);
					return 0;
				}
				else {
					currentNode->data = temptNode->data;
					free(temptNode);
					
					return 0;
				}
			}*/
		}
	}
	//�ڲ�������ɫ˫�����ͺ�ɫ�ڵ�����
	//����ֻ����Ҷ�ӽڵ�
	while (temptNode->leftNode != nullptr || temptNode != nullptr) {
		if (temptNode->leftNode != nullptr)
			temptNode = temptNode->leftNode;
		if (temptNode->rightNode != nullptr)
			temptNode = temptNode->rightNode;
	}
	currentNode->data = temptNode->data;
	tempNode = DeleteFixedUp(temptNode);
	
	if(temptNode!=nullptr)
		free(temptNode);
	return 0;

}

template<class T>
RBNode<T>* RBTree<T>::SearchNode(T data) {

	RBNode<T>* currentNode = nullptr;
	while (data != currentNode->data) {
		if (currentNode->data >= data) {
			currentNode = currentNode->rightNode;
		}
		else {
			currentNode = currentNode->leftNode;
		}
		if (currentNode == nullptr)
			return -1;
	}
	return currentNode;
}