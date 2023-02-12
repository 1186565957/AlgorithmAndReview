#include"graph_directed.h"
using namespace std;

/**
 * @brief Construct a new graph matrix::graph matrix object  待完善
 * 
 */


graph_matrix::graph_matrix() {
	if (!m_vertexArray.empty()) {
		m_vertexArray.clear();
	}
}


graph_matrix::~graph_matrix() {

	for (int i = 0;i < m_vertexArray.size();i++) {
		Vertex tempt =  m_vertexArray[i];
		Edge* p = tempt.pAdjEdges;
		while (p != nullptr)
		{
			tempt.pAdjEdges = p->pNextEdge;
			delete p;
			p = tempt.pAdjEdges;
		}
	}
	if (m_vertexArray.empty())
		m_vertexArray.clear();


	//外连接错误,不知原因的找不到
	//typename vector<Vertex>::iterator vIter;

	//for (vIter = m_vertexArray.begin();vIter != m_vertexArray.end();vIter++) {
	//	//点边
	//	Edge* p = (*vIter).pAdjEdges;
	//	while (p != nullptr)
	//	{
	//		vIter->pAdjEdges = p->pNextEdge;
	//		delete p;
	//		p = vIter->pAdjEdges;
	//	}
	//}
	//if (m_vertexArray.empty()) {
	//	m_vertexArray.clear();
	//}
}


bool graph_matrix::insertVertex(IN const char vertexData) {
	Vertex VertexInstance(vertexData, nullptr);
	m_vertexArray.push_back(VertexInstance);
	return true;
}


bool graph_matrix::insertEdge(IN const char vertexData1, IN const char vertexData2, IN const size_t weight) {

	int ver1Index = getVertexIndex(vertexData1);
	if (ver1Index == -1)
		return false;

	int ver2Index = getVertexIndex(vertexData2);
	if (ver2Index == -1)
		return false;

	Vertex tempt = m_vertexArray[ver1Index];

	Edge* p = tempt.pAdjEdges;
	while (p!=nullptr && p->nDestVertex != ver2Index)
	{
		//
		p = p->pNextEdge;
	}
	if (p == nullptr) {
		
		p = new Edge(ver2Index, weight, tempt.pAdjEdges);
		tempt.pAdjEdges = p;
		return true;
	}

	if (p->nDestVertex == vertexData2) {
		Edge* q = p;
		p = new Edge(ver2Index, weight, q->pNextEdge);
		q->pNextEdge = p;
		return true;
	}

	return false;
}


bool graph_matrix::removeEdge(IN const char vertexData1, IN const char vertexData2, IN const size_t weight) {

	int ver1Index = getVertexIndex(vertexData1);
	if (ver1Index == -1)
		return false;

	int ver2Index = getVertexIndex(vertexData2);
	if (ver2Index == -1)
		return false;
	Vertex tempt = m_vertexArray[ver1Index];

	Edge* p = tempt.pAdjEdges;
	Edge* q = nullptr;

	while (p != nullptr && p->nDestVertex != ver2Index) {
		q = p;
		p = p->pNextEdge;
	}

	if (p == nullptr)
		return false;

	while (weight!=p->edgeWeight && p->nDestVertex == ver2Index)
	{
		q = p;
		p = p->pNextEdge;
	}

	if (p->nDestVertex != ver2Index)
		return false;
	q->pNextEdge = p->pNextEdge;
	delete p;
	return true;
	
}


int graph_matrix::getVertexNumber() {

	return m_vertexArray.size();
}


char graph_matrix::getData(IN int Index) {

	return m_vertexArray[Index].vertexData;
}


Vertex graph_matrix::getBeginVertex() {
	if (!m_vertexArray.empty())
		return m_vertexArray[0];
}


int graph_matrix::getVertexIndex(IN const char vertexData) {

	for (int i = 0;i < m_vertexArray.size();i++)
		if (vertexData == getData(i))
			return i;
	return -1;
}



size_t graph_matrix::getEdgeWeight(IN const Edge* pEdge) {
	return pEdge->edgeWeight;
}


//返回一个点的所有边权重
void graph_matrix::getVertexEdgeWeight(IN const int v1, OUT std::vector<size_t>& distanceArray) {

	Vertex tempt = m_vertexArray[v1];
	Edge* p = tempt.pAdjEdges;
	size_t temp;
	//重复边问题
	int preIndex = -1;

	while (p != nullptr) {

		if (preIndex == p->nDestVertex) {
			if (temp > p->edgeWeight) {
				distanceArray[preIndex] = p->edgeWeight;
			}
		}
		else {
			distanceArray[p->nDestVertex] = p->edgeWeight;
			preIndex = p->nDestVertex;
			temp = p->edgeWeight;
		}
		p = p->pNextEdge;

	}
}


std::ostream& operator <<(OUT ostream& out, IN graph_matrix& graphInstance){

	int vertexCount = graphInstance.getVertexNumber();
	out << "The graph has" << vertexCount << "vertexes" << endl;
	for (int i = 0;i < vertexCount;i++) {
		char Data = graphInstance.getData(i);
		out << Data << ": ";

		Vertex tempt = graphInstance.m_vertexArray[i];
		Edge* p = tempt.pAdjEdges;

		while (p != nullptr)
		{
			out << "(" << Data << "," << graphInstance.getData(p->nDestVertex) << "," << p->edgeWeight << ")  ";
			p->pNextEdge;
		}
		out << endl;
	}
	return out;
}



bool Graph_topo::kahn(graph_matrix& matrixSharePtr)
{

	return true;
}


void Graph_topo::Dijkstra(graph_matrix& matrixSharePtr) {
	
	std::stack<Vertex> vertexStack;
	//首边
	Vertex tempVertex = matrixSharePtr.getBeginVertex();

	bool flag = true;
	//深度遍历
	Edge* pNext = tempVertex.pAdjEdges;
	printf("Vertex DataValue:%d    ", tempVertex.vertexData);
	vertexStack.push(tempVertex);

	while (!vertexStack.empty()) {
		if (!flag) {
			pNext = vertexStack.top().pAdjEdges->pNextEdge;
			vertexStack.pop();
		}
		while (pNext != nullptr) {
			
			printf("Vertex DataValue:%d    ", pNext->nDestVertex);
			vertexStack.push(tempVertex);
			pNext = matrixSharePtr.m_vertexArray[matrixSharePtr.getVertexIndex(pNext->nDestVertex)].pAdjEdges->pNextEdge;
			flag = false;
		}
	}

	return ;
}


