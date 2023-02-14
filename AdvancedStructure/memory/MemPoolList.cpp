#include "MemPoolList.h"
#include <stdio.h>

CMemoryPools* CMemoryPools::m_pMemoryPools = nullptr;

CMemoryPools::CMemoryPools(void) {

	Init();
}

CMemoryPools::~CMemoryPools(void) {
	
	Close();
}

void CMemoryPools::Init() {

	m_pMemoryList = nullptr;
	m_pMemoryListLast = nullptr;
	m_ThreadLock.Init();
}

void CMemoryPools::Close() {

	CAutoLock autolock(&m_ThreadLock);

	//删除以及分配了的文件快
	_MemoryList* pCurrentMemoryList = m_pMemoryList;
	while (pCurrentMemoryList != nullptr)
	{
		_MemoryBlock* pMemoryUsed = m_pMemoryList->m_pMemoryUsed;
		while (pMemoryUsed != nullptr) {
			if (pMemoryUsed->m_pBrick != nullptr) {
				free(pMemoryUsed->m_pBrick);
				pMemoryUsed->m_pBrick = nullptr;
			}

			pMemoryUsed = pMemoryUsed->m_pNext;
		}

		_MemoryBlock* pMemoryFree = m_pMemoryList->m_pMemoryFree;
		while (pMemoryFree != nullptr) {

			if (pMemoryFree->m_pBrick != nullptr) {
				free(pMemoryFree->m_pBrick);
				pMemoryFree->m_pBrick = nullptr;
			}

			pMemoryFree = pMemoryFree->m_pNext;
		}
		pCurrentMemoryList = pCurrentMemoryList->m_pMemoryNext;
	}
}


//设置内存的包头，记录用到的内存地址信息 缓冲区、链表、块
void* CMemoryPools::SetMemoryHead(void* pBuff, _MemoryList* pList, _MemoryBlock* pBlock) {

	if (pBuff == nullptr) {
		return nullptr;
	}

	unsigned int* plData = (unsigned int*)pBuff;

	plData[0] = (unsigned int)pList;	//链表首地址
	plData[1] = (unsigned int)pBlock;	//所在链表的地址
	plData[2] = (unsigned int)MAGIC_CODE;	//验证码

	return &plData[3];

}


void* CMemoryPools::GetMemoryHead(void *pBuff){
	
	//直接把指向的内存块赋值到一个Long类型的指针的地址的地方
	if (pBuff == nullptr)
		return nullptr;

	long* plData = (long*)pBuff;
	return &plData[3];
}


bool CMemoryPools::GetHeadMemoryBlock(void* pBuff, _MemoryList*& pList, _MemoryBlock*& pBlock) {

	//缓冲区末尾地址
	char* szbuf = (char*)pBuff;
	//得到首地址位置
	unsigned int* plData = (unsigned int*)(szbuf - MAX_MEMORY_SIZE);

	if (plData[2] != (long)MAGIC_CODE) {
		return false;
	}
	else {
		pList =(_MemoryList*) plData[0];
		pBlock = (_MemoryBlock*)plData[1];
		return true;
	}
}

void* CMemoryPools::GetBuff(size_t szBuffSize) {

	//线程安全
	CAutoLock autolock(&m_ThreadLock);
	void* pBuff = nullptr;

	//判断首次使用
	if (m_pMemoryList == nullptr) {
		//需要的空间，加上之后备用的最大空间
		pBuff = malloc(szBuffSize + MAX_MEMORY_SIZE);
		if (pBuff == nullptr) {
			printf("GetBuff() function error: pBuff malloc == null");
			return nullptr;
		}

		m_pMemoryList = (_MemoryList*)malloc(sizeof(_MemoryList));
		if (m_pMemoryList == nullptr) {
			printf("GetBuff function error: m_pMemoryList malloc == null");
			return nullptr;
		}
		m_pMemoryList->Init();

		//新建一个内存块的链表
		_MemoryBlock* pMemoryUsed = (_MemoryBlock*)malloc(sizeof(_MemoryBlock));
		if (pMemoryUsed == nullptr) {
			free(pBuff);
			return nullptr;
		}

		pMemoryUsed->Init();

		//这个内存块指向之前的申请的地方
		pMemoryUsed->m_pBrick = pBuff;

		m_pMemoryList->m_iSize = (int)szBuffSize;
		m_pMemoryList->m_pMemoryUsed = pMemoryUsed;
		m_pMemoryList->m_pMemoryUsedLast = pMemoryUsed;

		m_pMemoryListLast = m_pMemoryList;

		return SetMemoryHead(pBuff, m_pMemoryList, pMemoryUsed);
	}

	//查找链表中的空余数据块
	_MemoryList* pCurrentMemoryList = m_pMemoryList;
	while (pCurrentMemoryList !=nullptr)
	{
		//没有剩余的自由内存块
		//m_iSize代表剩余空间
		if (pCurrentMemoryList->m_iSize == (int)szBuffSize) {
			_MemoryBlock* pMemoryFree = pCurrentMemoryList->m_pMemoryFree;
			if (pMemoryFree == nullptr) {
				pBuff = malloc(szBuffSize + MAX_MEMORY_SIZE);
				if (pBuff == nullptr)
					return nullptr;

				//创建一个内存块链表
				_MemoryBlock* pMemoryUsed = (_MemoryBlock*)malloc(sizeof(_MemoryBlock));
				if (pMemoryUsed == nullptr)
					return nullptr;

				pMemoryUsed->Init();

				pMemoryUsed->m_pBrick = pBuff;
				_MemoryBlock* pMemoryUsedLast = m_pMemoryList->m_pMemoryUsedLast;
				if (pMemoryUsedLast == nullptr) {
					//内存申请没有成功的话
					pCurrentMemoryList->m_iSize = (int)szBuffSize;
					pCurrentMemoryList->m_pMemoryUsed = pMemoryUsed;
					pCurrentMemoryList->m_pMemoryUsedLast = pMemoryUsed;
					return SetMemoryHead(pBuff, pCurrentMemoryList, pMemoryUsed);
				}
				else {
					pMemoryUsed->m_pPrev = pCurrentMemoryList->m_pMemoryUsedLast;
					pCurrentMemoryList->m_pMemoryUsedLast->m_pNext = pMemoryUsed;
					pCurrentMemoryList->m_pMemoryUsedLast = pMemoryUsed;
					return SetMemoryHead(pBuff, pCurrentMemoryList, pMemoryUsed);
				}


			}//存在剩余空间的位置
			else {
				//把这个内存块单独取出来了
				_MemoryBlock* pMemoryTemp = pMemoryFree;
				pCurrentMemoryList->m_pMemoryFree = pMemoryFree->m_pNext;
				pBuff = pMemoryFree->m_pBrick;

				pMemoryTemp->m_pPrev = pCurrentMemoryList->m_pMemoryUsedLast;
				pMemoryFree->m_pNext = nullptr;

				//列表首次使用
				if (pCurrentMemoryList->m_pMemoryUsedLast == nullptr) {
					pCurrentMemoryList->m_pMemoryUsedLast = pMemoryTemp;
					pCurrentMemoryList->m_pMemoryUsed = pMemoryTemp;
				}
				else {
					pCurrentMemoryList->m_pMemoryUsedLast->m_pNext = pMemoryTemp;
					pCurrentMemoryList->m_pMemoryUsedLast = pMemoryTemp;
				}

				return GetMemoryHead(pBuff);
			}

		}//当前的链表的剩余空间不足了，之间换用下一个链表
		else {
			pCurrentMemoryList = pCurrentMemoryList->m_pMemoryNext;
		}
	}

	//现有内存不存在可用了，也不是初次使用
	pBuff = malloc(szBuffSize + MAX_MEMORY_SIZE);
	if (pBuff == nullptr)
		return nullptr;

	_MemoryList* pMemoryList = (_MemoryList*)malloc(sizeof(_MemoryList));

	if (pMemoryList == nullptr) {
		free(pBuff);
		return nullptr;
	}
	pMemoryList->Init();
	_MemoryBlock* pMemoryUsed = (_MemoryBlock*)malloc(sizeof(_MemoryBlock));
	if (pMemoryUsed == nullptr) {
		free(pBuff);
		return nullptr;
	}
	pMemoryUsed->Init();

	pMemoryUsed->m_pBrick = pBuff;

	pMemoryList->m_iSize = (int)szBuffSize;
	pMemoryList->m_pMemoryUsed = pMemoryUsed;
	pMemoryList->m_pMemoryUsedLast = pMemoryUsed;

	m_pMemoryListLast->m_pMemoryNext = pMemoryList;
	m_pMemoryListLast = pMemoryList;

	return SetMemoryHead(pBuff, pMemoryList, pMemoryUsed);

}

bool CMemoryPools::DelBuff(size_t szBuffSize, void* pBuff) {

	CAutoLock autoLock(&m_ThreadLock);

	_MemoryList* pCurrentMemoryList = m_pMemoryList;
	while (pCurrentMemoryList != nullptr) {

		if (pCurrentMemoryList->m_iSize == szBuffSize) {

			_MemoryBlock* pMemoryUsed = pCurrentMemoryList->m_pMemoryUsed;

			//标记列表第一个
			bool blFirst = true;
			while (pMemoryUsed != nullptr) {
				if (pBuff == pMemoryUsed->m_pBrick) {
					if (pMemoryUsed != nullptr) {
						if (blFirst == true) {
							if (pMemoryUsed->m_pNext != nullptr) {
								pMemoryUsed->m_pNext->m_pPrev = pMemoryUsed->m_pPrev;
							}
							pCurrentMemoryList->m_pMemoryUsed = pMemoryUsed->m_pNext;
							//使用了一个块的时候，直接把当前的列表
							if (pMemoryUsed == pCurrentMemoryList->m_pMemoryUsedLast) {
								pCurrentMemoryList->m_pMemoryUsedLast = pCurrentMemoryList->m_pMemoryUsedLast->m_pPrev;
							}
							blFirst = false;
						}
						else {

							if (pMemoryUsed->m_pNext != nullptr) {
								pMemoryUsed->m_pNext->m_pPrev = pMemoryUsed->m_pPrev;
							}

							if (pMemoryUsed == pCurrentMemoryList->m_pMemoryUsedLast) {
								pCurrentMemoryList->m_pMemoryUsedLast = pCurrentMemoryList->m_pMemoryUsedLast->m_pPrev;
							}
							else {
								pMemoryUsed->m_pPrev->m_pNext = pMemoryUsed->m_pNext;
							}
						}

						//没有内存块的时候
						if (pCurrentMemoryList->m_pMemoryFree == nullptr) {

							pMemoryUsed->m_pPrev = nullptr;
							pMemoryUsed->m_pNext = nullptr;
							pCurrentMemoryList->m_pMemoryFree = pMemoryUsed;
							pCurrentMemoryList->m_pMemoryFreeLast = pMemoryUsed;
						}
						else {
							pMemoryUsed->m_pPrev = pCurrentMemoryList->m_pMemoryFreeLast;
							pMemoryUsed->m_pNext = nullptr;
							pCurrentMemoryList->m_pMemoryFreeLast->m_pNext = pMemoryUsed;
							pCurrentMemoryList->m_pMemoryFreeLast = pMemoryUsed;
						}
						return true;
					}
					else {
						return false;
					}
				}
				pMemoryUsed = pMemoryUsed->m_pNext;
				blFirst = false;
			}	
		}

		pCurrentMemoryList = pCurrentMemoryList->m_pMemoryNext;
	}
	return false;
}


bool CMemoryPools::DelBuff(void* pBuff) {

	CAutoLock autolock(&m_ThreadLock);

	_MemoryBlock* pMemoryUsed = nullptr;
	_MemoryList* pCurrentMemoryList = nullptr;

	if (GetHeadMemoryBlock(pBuff, pCurrentMemoryList, pMemoryUsed))
		return false;

	//链表不空，并且有被使用的块空间
	if (pMemoryUsed != nullptr && pCurrentMemoryList != nullptr) {

		//当前是列表的第一个
		if (pCurrentMemoryList->m_pMemoryUsed == pMemoryUsed) {
			pCurrentMemoryList->m_pMemoryUsed = pMemoryUsed->m_pNext;
		}
		else {
			pMemoryUsed->m_pPrev->m_pNext = pMemoryUsed->m_pNext;
		}

		if (pMemoryUsed->m_pNext != nullptr) {
			pMemoryUsed->m_pNext->m_pPrev = pMemoryUsed->m_pPrev;
		}
		//当前使用是链表的尾部
		if (pMemoryUsed == pCurrentMemoryList->m_pMemoryUsedLast) {
			pCurrentMemoryList->m_pMemoryUsedLast = pCurrentMemoryList->m_pMemoryUsedLast->m_pPrev;
		}

		//当前无空余了
		if (pCurrentMemoryList->m_pMemoryFree == nullptr) {
			//把当前的这个变成空余的块
			pMemoryUsed->m_pPrev = nullptr;
			pMemoryUsed->m_pNext = nullptr;
			pCurrentMemoryList->m_pMemoryFree = pMemoryUsed;
			pCurrentMemoryList->m_pMemoryFreeLast = pMemoryUsed;
		}
		else
		{
			pMemoryUsed->m_pPrev = pCurrentMemoryList->m_pMemoryFreeLast;
			pMemoryUsed->m_pNext = nullptr;
			pCurrentMemoryList->m_pMemoryFreeLast->m_pNext = pMemoryUsed;
			pCurrentMemoryList->m_pMemoryFreeLast = pMemoryUsed;
		}
		return true;
	}
	else {
		return false;
	}
	return false;
}


void CMemoryPools::DisplayMemoryList() {

	int iUsedCount = 0;
	int iFreeCount = 0;

	_MemoryList* pCurrentMemoryList = m_pMemoryList;
	while (pCurrentMemoryList != nullptr)
	{
		_MemoryBlock* pMemoryUsed = pCurrentMemoryList->m_pMemoryUsed;
		_MemoryBlock* pMemoryFree = pCurrentMemoryList->m_pMemoryFree;

		iUsedCount = 0;
		iFreeCount = 0;
		while (pMemoryUsed != nullptr)
		{
			iUsedCount++;
			pMemoryUsed = pMemoryUsed->m_pNext;
		}
		printf("[CMemoryPools::DisplayMemoryList] pMemoryUsed nUsedCount = %d, Size = %d.\n", iUsedCount, pCurrentMemoryList->m_iSize * iUsedCount);
		while (pMemoryFree != nullptr)
		{
			iFreeCount++;
			pMemoryFree = pMemoryFree->m_pNext;
		}

		printf("[CMemoryPools::DisplayMemoryList] pMemoryFree nFreeCount = %d, Size = %d.\n", iFreeCount, pCurrentMemoryList->m_iSize * iFreeCount);
		pCurrentMemoryList = pCurrentMemoryList->m_pMemoryNext;
	}
}

inline void* operator new(size_t szBuff) {

	void* pBuff = CMemoryPools::Instance().GetBuff(szBuff);
	return pBuff;
}

inline void operator delete(void* p) {

	if (false == CMemoryPools::Instance().DelBuff(p))
	{
		CMemoryPools::Instance().DisplayMemoryList();
	}


}

inline void operator delete[](void* p) {

	if (false == CMemoryPools::Instance().DelBuff(p))
		CMemoryPools::Instance().DisplayMemoryList();
}