#include <iostream>
#include <exception>
using std::exception;
using std::cout;
using std::endl;

typedef struct NODE{
	void* pData;
	size_t nBytes;
	NODE* pNext;
}node;

class CMemoryList
{
	node* _pHead;
	node* _pTail;
	size_t _nodeNum;

public:
	CMemoryList():_pHead(NULL), _pTail(NULL), _nodeNum(0)
	{
		
	}

	CMemoryList(size_t blockNum, size_t blockSize)
	{
		if(blockNum <= 0 || blockSize <= 0)
			throw exception("Error");

		_nodeNum = blockNum;
		_pTail = createNode(NULL, blockSize);

		node* pCur = _pTail;
		node* pNext = NULL;
		for(int i = 1; i < blockNum; ++i)
		{
			pCur = createNode(pCur, blockSize);
		}

		_pHead = pCur;
	};

	const node* getHead() const
	{
		return _pHead;
	}

	const node* getTail() const
	{
		return _pHead;
	}

	void setHead(node* pNode)
	{
		_pHead = pNode;
	}

	void setTail(node* pNode)
	{
		_pTail = pNode;
	}

	size_t getNodeNum() const
	{
		return _nodeNum;
	}

	node* createNode(node* pNext, size_t blockSize)
	{
		node* pNode = new node;

		cout<<"alloc node 节点"<<pNode<<endl;
		pNode->nBytes = blockSize;
		pNode->pNext = pNext;
		pNode->pData = malloc(blockSize);
		if(pNode->pData == NULL)
			throw exception("oh");
		return pNode;
	}

	void appendNode(node* pNode)
	{
		if(pNode == NULL)
			throw exception("error");

		++_nodeNum;
		pNode->pNext = NULL;
		if(_pHead == NULL)
		{
			_pTail = pNode;
			_pHead = pNode;
		}
		else
		{
			_pTail->pNext = pNode;
			_pTail = pNode;
		}

		return;
	}

	void insertNode(node* pNode)
	{
		if(pNode == NULL)
			throw exception("error!");

		pNode->pNext = NULL;
		++ _nodeNum;

		//Free List 为空
		if(_pHead == NULL)
		{
			_pHead = pNode;
			_pTail = pNode;
			return;
		}

		//插入节点为Free List 头结点
		if(pNode->nBytes > _pHead->nBytes)
		{
			pNode->pNext = _pHead;
			_pHead = pNode;
			return;
		}

		//插入节点为Free List 尾结点
		if(pNode->nBytes <= _pTail->nBytes)
		{
			_pTail->pNext = pNode;
			_pTail = pNode;
			return;
		}

		node *pPrev, *pCur, *pNext;
		pPrev = NULL;
		pCur = _pHead;
		pNext = pCur->pNext;

		while(pCur != NULL)
		{
			if(pCur->nBytes >= pNode->nBytes && pNext->nBytes < pNode->nBytes)
			{
				pCur->pNext = pNode;
				pNode->pNext = pNext;
				break;
			}

			pPrev = pCur;
			pCur = pNext;
			pNext = pCur->pNext;
		}

		return;
	}

	node* removeNode(const void* pData)
	{
		if(pData == NULL)
			return NULL;

		node* pRet = NULL;
		node* pCur = _pHead;
		node* pPrev = NULL;

		if(pCur == NULL)
			return pRet;

		while(pCur != NULL)
		{
			if(pCur->pData == pData)
			{
				--_nodeNum;
				pRet = pCur;
				break;
			}

			pPrev = pCur;
			pCur = pCur->pNext;
		}

		if(pCur == NULL)
			pRet = NULL;
		else if(pPrev != NULL && pCur->pNext != NULL)
			pPrev->pNext = pCur->pNext;
		else if(pCur == _pHead)
			_pHead = pCur->pNext;
		else if(pCur == _pTail)
		{
			_pTail = pPrev;
			pPrev->pNext = NULL;
		}

		return pRet;
	}

	node* findNode(size_t nBytes)
	{		
		node* pRet = NULL;

		if(_pHead == NULL)
			return pRet;

		node* pPrev = NULL;
		node* pCur = _pHead;
		node* pNext = pCur->pNext;

		//头结点
		if(pCur->nBytes < nBytes)
			return pRet;

		while(pCur != NULL)
		{
			//尾节点
			if(pCur->nBytes >= nBytes && pNext == NULL)
			{
				--_nodeNum;
				pPrev->pNext = NULL;
				_pTail = pPrev;
				pRet = pCur;
				break;
			}

			//找到插入点
			if(pCur->nBytes >= nBytes && pNext->nBytes < nBytes)
			{
				--_nodeNum;
				pRet = pCur;
				if(pCur == _pHead)
					_pHead = pNext;
				else
					pPrev->pNext = pNext;

				if(pNext == NULL)
					_pTail = NULL;
				break;
			}

			pPrev = pCur;
			pCur = pNext;
			pNext = pCur->pNext;
		}

		return pRet;
	}

	node* removeHead()
	{
		node* pHead = _pHead;
		if(_pHead != NULL)
		{
			_pHead = _pHead->pNext;
			--_nodeNum;
		}

		if(pHead->pNext == NULL)
			_pHead = NULL;
		return pHead;
	}

	void printList()
	{
		cout<<"********************"<<endl;
		cout<< " 输出Memory pool"<<endl;
		if(_pHead == NULL)
		{
			cout<<"memory pool is NULL"<<endl;
			return;
		}

		cout<<"head is "<<_pHead<<endl;
		cout<<"tail is "<<_pTail<<endl;
		cout<<"node num is "<<_nodeNum<<endl;

		const node* pNode = _pHead;
		while(pNode != NULL)
		{
			cout<<pNode<<"->";
			pNode = pNode->pNext;
		}

		cout<<"NULL"<<endl;
		cout<<"********************"<<endl<<endl;
		return;
	}

	void sortList()
	{
		node* pCur = _pHead;
		while(pCur != _pTail)
		{
			node* pTemp;
			pTemp = findMaxSlot(pCur->pNext);
			if(pTemp == NULL)
				break;

			if(pCur->nBytes < pTemp->nBytes)
			{
				void* pd = pCur->pData;
				pCur->pData = pTemp->pData;
				pTemp->pData = pd;
			}
			pCur = pCur->pNext;
		}

		return;
	}

	node* findMaxSlot(node* pHead)
	{
		node* pRet = pHead;
		node* pTmp = pHead;
		size_t maxBytes = pHead->nBytes;	
		while(pTmp != NULL)
		{
			if(pTmp->nBytes > maxBytes)
			{
				maxBytes = pTmp->nBytes;
				pRet = pTmp;
			}
		}

		return pRet;
	}

	void resizeNode(node* pNode, int new_size)
	{
		if(pNode == NULL)
			return;

		void* pTemp = malloc(new_size);
		if(pTemp == NULL)
			throw exception("bad");

		if(pNode->pData != NULL)
			free(pNode->pData);

		pNode->pData = pTemp;
		pNode->nBytes = new_size;
		return;
	}

	~CMemoryList()
	{
		if(_pHead == NULL)
			return;
		node* pNode = _pHead;
		node* pTmp = NULL;
		while(pNode != NULL)
		{
			if(pNode->pData != NULL)
				free(pNode->pData);

			pTmp = pNode->pNext;
			free(pNode);
			pNode = pTmp;
		}
	}
};
