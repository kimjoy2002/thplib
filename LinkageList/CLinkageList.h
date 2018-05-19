#ifndef H_LINKAGE_LIST_CLASS___
#define H_LINKAGE_LIST_CLASS___
// ===================================================================
// CLinkageList.h
//	連結リストクラス・ヘッダー
//	
// ===================================================================

#include <windows.h>
#include <stdio.h>
#include <STDLIB.H>
#include "util.h"

#define LLIST_ALLCLEAR		(-1)

// ノード一個のクラス
typedef class CLinkageNode
{
public:
	CLinkageNode(CLinkageNode* pPrev, void* pNewData, int nDataSize, CLinkageNode* pNext, int nLV=0)
	{
		m_nCommonLevel = nLV;
		m_pPrev = pPrev;
		m_pData = NULL;
		if (nDataSize && pNewData)
		{
			m_pData = malloc(nDataSize);
			memcpy(m_pData, pNewData, nDataSize);
		}
		m_pNext = pNext;
		if (m_pPrev)	m_pPrev->SetNext(this);
		if (m_pNext)	m_pNext->SetPrev(this);
	};
	~CLinkageNode()
	{
		if (m_pPrev)	m_pPrev->SetNext(m_pNext);
		if (m_pNext)	m_pNext->SetPrev(m_pPrev);
		SafeFree(m_pData);
	}

	inline int GetCommonLevel()
	{	return m_nCommonLevel;	};

	inline void* GetData(){ return m_pData;	};
	inline CLinkageNode* GetNext(){	return m_pNext;	};
	inline CLinkageNode* GetPrev(){	return m_pPrev;	};

	inline void SetNext(CLinkageNode* pNext){	m_pNext = pNext;	};
	inline void SetPrev(CLinkageNode* pPrev){	m_pPrev = pPrev;	};
	
	// ターゲット,後に移動
	BOOL MoveNodeToPushBack(CLinkageNode *pTarget)
	{
		CLinkageNode* pNode=NULL;
		if (!pTarget) return FALSE;
		if (m_pPrev)	m_pPrev->SetNext(m_pNext);
		if (m_pNext)	m_pNext->SetPrev(m_pPrev);
		pTarget->SetNext(this);
		if (pNode = pTarget->GetNext())
			pNode->SetPrev(this);
		return TRUE;
	}

	// ターゲット、前に移動
	BOOL MoveNodeToPushFront(CLinkageNode *pTarget)
	{
		CLinkageNode* pNode=NULL;
		if (!pTarget) return FALSE;
		if (m_pPrev)	m_pPrev->SetNext(m_pNext);
		if (m_pNext)	m_pNext->SetPrev(m_pPrev);
		pTarget->SetPrev(this);
		if (pNode = pTarget->GetPrev())
			pNode->SetNext(this);
		return TRUE;
	}


protected:
	int		m_nCommonLevel;
	void*	m_pData;
	CLinkageNode*	m_pNext;
	CLinkageNode*	m_pPrev;

private:


} *PLinkageNode, **PPLinkageNode;

// 連結リストクラス
// ※追加したポインタは自動で削除されます
class CLinkageList
{
public:
	CLinkageList();
	virtual ~CLinkageList()
	{
		AllClear();
		SafeDelete(m_pTopNode);
		SafeDelete(m_pLastNode);
	};

	inline virtual void AllClear()
	{	Clear(-1);	};
	virtual void Clear(int nLV=0);
	inline int GetCount(){	return m_nCount;	};

	virtual void* GetData(int index);

	const CLinkageNode* GetBegin()
	{ return m_pTopNode;	};
	const CLinkageNode* GetEnd()
	{ return m_pLastNode;	};

	// return: m_nCount
	virtual int Remove(int index, int nLV=LLIST_ALLCLEAR);
	virtual int RemoveNode(CLinkageNode* pNode)
	{
		if (pNode == m_pTargetNode)
			m_pTargetNode = m_pTargetNode->GetPrev();

		SafeDelete(pNode);
		m_nCount--;
		return m_nCount;
	};

	// 外部用のトップ（内部m_pTopNodeのNext）
	CLinkageNode* GetTopNode();
	// 外部用のトップ（内部m_pLastNodeのPrev）
	CLinkageNode* GetLastNode();
	// 外部用のトップデータ（内部m_pTopNodeのNext）
	void* GetTopData()
	{
		CLinkageNode *pNode = NULL;
		if (pNode = GetTopNode())
			return pNode->GetData();
		return NULL;
	}
	// 外部用のトップデータ（内部m_pLastNodeのPrev）
	void* GetLastData()
	{
		CLinkageNode *pNode = NULL;
		if (pNode = GetLastNode())
			return pNode->GetData();
		return NULL;
	}

	virtual void RewindTargetNode() { m_pTargetNode = m_pTopNode; };
	virtual int GetTargetNodeCommonLevel();

	virtual CLinkageNode* GetNextTarget()
	{
		m_pTargetNode = m_pTargetNode->GetNext();
		return m_pTargetNode;
	};
	BOOL GetNextTargetData(void** pTargetData);
	virtual int	 RemoveTargetNode();

	virtual BOOL Insert(int index ,void* pNewData, int nDataSize, int nLV=0);
	inline virtual BOOL AddLast(void* pNewData, int nDataSize, int nLV=0)
	{	return Insert(m_nCount, pNewData, nDataSize, nLV);	};
	
	inline virtual BOOL AddTop(void* pNewData, int nDataSize, int nLV=0)
	{	return Insert(0, pNewData, nDataSize, nLV);	};
	
	inline CLinkageNode* GetNode(int index)
	{
		if (index < (m_nCount/2))
			return GetStartTop(index);
		else
			return GetStartLast(index);
	};

	virtual BOOL Sort(int size=255);

protected:
	CLinkageNode*	m_pTargetNode;		// 内部のトップ
	CLinkageNode*	m_pTopNode;			// 内部のトップ
	CLinkageNode*	m_pLastNode;		// 内部のラスト
	int				m_nCount;


	CLinkageNode* GetStartTop(int index);
	CLinkageNode* GetStartLast(int index);
private:

};

#endif
