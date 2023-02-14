#pragma once
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<pthread.h>

//针对的线程锁方式
#define LOCK_MUTEXT pthread_mutex_t

class CThreadLock {

public :
	CThreadLock(void) {
		Init();
	};

	~CThreadLock() {
		Close();
	};

	void Init() {
		pthread_mutex_init(&m_lock, nullptr);
	}

	void Close() {
		pthread_mutex_destroy(&m_lock);
	}

	void Lock() {

		pthread_mutex_lock(&m_lock);
	}

	void unLock() {
		pthread_mutex_unlock(&m_lock);
	}

private:
	LOCK_MUTEXT m_lock;
	
};

//类的自动锁
class CAutoLock {

public :
	CAutoLock(CThreadLock *pthreadLock) {
		m_pthreadLock = pthreadLock;
		if (m_pthreadLock != nullptr)
			m_pthreadLock->Lock();
	}

	~CAutoLock() {
		if (m_pthreadLock != nullptr)
			m_pthreadLock->unLock();
	}

private:
	CThreadLock *m_pthreadLock;
};



/*
	内存池的整体结构其实就是一个双向链表，链表内部是表项，表象中有自己的序列和内存块
	链表也有很多个，之间可以互相指向
	
*/

#define MAX_MEMORY_SIZE 12
#define MAGIC_CODE 0x123456

//双向链表结构
struct _MemoryBlock {

	_MemoryBlock *m_pNext;
	_MemoryBlock* m_pPrev;
	//真正的块内存
	void* m_pBrick;

	void Init() {
		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pBrick = nullptr;
	}

	_MemoryBlock() {
		Init();
	}
};

//内存的本体管理列表
struct _MemoryList{
	_MemoryList* m_pMemoryNext;
	_MemoryBlock* m_pMemoryFree;
	_MemoryBlock* m_pMemoryFreeLast;
	_MemoryBlock* m_pMemoryUsed;
	_MemoryBlock* m_pMemoryUsedLast;

	int m_iSize;

	void Init() {
		m_pMemoryNext = nullptr;
		m_pMemoryFree = nullptr;
		m_pMemoryFreeLast = nullptr;
		m_pMemoryUsed = nullptr;
		m_pMemoryUsedLast = nullptr;
	}

	_MemoryList() {
		Init();
	}

};

class CMemoryPools {

public:
	static CMemoryPools& Instance() {

		if (m_pMemoryPools == nullptr) {
			m_pMemoryPools = (CMemoryPools*)malloc(sizeof(CMemoryPools));
			m_pMemoryPools->Init();
		}
		return *m_pMemoryPools;
	}

public:
	~CMemoryPools(void);

	void* GetBuff(size_t szBuffSize);
	bool DelBuff(size_t szBuffSize, void* pBuff);
	bool DelBuff(void* pBuff);
	void DisplayMemoryList();

private:
	CMemoryPools(void);
	void Close();
	void Init();
	void *SetMemoryHead(void* pBuff, _MemoryList* pList, _MemoryBlock* pBlock);
	void *GetMemoryHead(void* pBuff);
	bool GetHeadMemoryBlock(void* pBuff, _MemoryList*& pList, _MemoryBlock*& pBlock);

private:
	static CMemoryPools* m_pMemoryPools;
	_MemoryList* m_pMemoryList;
	_MemoryList* m_pMemoryListLast;
	CThreadLock m_ThreadLock;
};
