#pragma once

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <memory.h>
#include <stdlib.h>
#include <windows.h>
#include "alloc_list.h"

namespace asr
{
	class lock 
	{
	public:
		lock(bool threads):_threads(threads)
		{
			if(_threads)
			{
				InitializeCriticalSection(&cs);
				EnterCriticalSection(&cs);
			}
		}
		~lock()
		{
			if(_threads)
			LeaveCriticalSection(&cs);
		}

		bool _threads;
		CRITICAL_SECTION cs;
	};

	template <bool threads = 1, size_t BlockNum = 5, size_t BlockSize = 4096>
	class MemoryPool
	{
	private:

		CMemoryList* pFreeList;
		CMemoryList* pUsedList;

	public:

		static MemoryPool& getInstance()
		{
			static MemoryPool<> mp;
			return mp;
		}

		~MemoryPool()
		{
			if(pFreeList != NULL)
			{
				pFreeList->printList();
				delete pFreeList;
			}
			
			if(pUsedList != NULL)
			{
				pUsedList->printList();
				delete pUsedList;
			}
		}

		template<typename T>
		T* alloc(size_t n = 1)
		{
			int nBytes  = sizeof(T) * n;			
			cout<<"分配内存大小为 "<<nBytes<<" 个字节"<<endl;
			T* ret = NULL;
			lock lock_t(threads);
			if(pFreeList->getNodeNum() == 0)
				throw exception("NO free memory left");
			else
			{
				//找出第一个大于nBytes的node节点
				node* pNode = pFreeList->findNode(nBytes);
				cout<<"使用节点"<<pNode<<endl;
				if(pNode != NULL)
				{
					cout<<"将节点 "<<pNode<<" 加入UsedList"<<endl;
					pUsedList->appendNode(pNode);
					ret = (T*) pNode->pData;
				}
				else
				{
					//将第一个结点也即freelist中最大的节点更改大小拿来使用
					node* pNode = pFreeList->removeHead();
					if(pNode == NULL)
						throw exception("NO free memory left");
					cout<<"更改节点"<<pNode<<"的大小"<<endl;
					pFreeList->resizeNode(pNode, nBytes);
					cout<<"节点"<<pNode<<"的大小为"<<pNode->nBytes<<endl;
					pUsedList->appendNode(pNode);
					ret = (T*) pNode->pData;
				}
			}

			pFreeList->printList();
			pUsedList->printList();
			return ret;
		}

		template<typename T>
		void dealloc(T* p)
		{
			void* pd = (void*) p;

			pFreeList->printList();
			pUsedList->printList();

			lock lock_t(threads);

			node* pNode = pUsedList->removeNode(pd);

			pFreeList->printList();
			pUsedList->printList();

			cout<<"释放"<<pNode<<"加入到FreeList"<<endl;

			if(pNode == NULL)
				throw exception("Not find");

			pFreeList->printList();
			pUsedList->printList();

			pFreeList->insertNode(pNode);

			pFreeList->printList();
			pUsedList->printList();

			return;
		}

	private:
		// POD means plain old data
		//void fill_n_aux(pointer p, size_type n, const_reference val, __true_type);

		//void fill_n_aux(pointer p, size_type n, const_reference val, __false_type);

		MemoryPool()
		{
			pFreeList = new CMemoryList(BlockNum, BlockSize);
			pUsedList = new CMemoryList;
			pFreeList->printList();
			pUsedList->printList();
		}

	private:

		const MemoryPool& operator=(const MemoryPool& c);

		bool operator==(const MemoryPool& c) const;

		bool operator!=(const MemoryPool& c) const;

		MemoryPool(const MemoryPool& memoryPool) throw();

		//template <class U> MemoryPool(const MemoryPool<U>& memoryPool);
	};

}
#endif
