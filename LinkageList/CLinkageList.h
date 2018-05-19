#ifndef H_LINKAGE_LIST_CLASS___
#define H_LINKAGE_LIST_CLASS___
// ===================================================================
// CLinkageList.h
//	�A�����X�g�N���X�E�w�b�_�[
//	
// ===================================================================

#include <windows.h>
#include <stdio.h>
#include <STDLIB.H>
#include "util.h"

#define LLIST_ALLCLEAR		(-1)

// �m�[�h��̃N���X
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
	
	// �^�[�Q�b�g,��Ɉړ�
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

	// �^�[�Q�b�g�A�O�Ɉړ�
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

// �A�����X�g�N���X
// ���ǉ������|�C���^�͎����ō폜����܂�
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

	// �O���p�̃g�b�v�i����m_pTopNode��Next�j
	CLinkageNode* GetTopNode();
	// �O���p�̃g�b�v�i����m_pLastNode��Prev�j
	CLinkageNode* GetLastNode();
	// �O���p�̃g�b�v�f�[�^�i����m_pTopNode��Next�j
	void* GetTopData()
	{
		CLinkageNode *pNode = NULL;
		if (pNode = GetTopNode())
			return pNode->GetData();
		return NULL;
	}
	// �O���p�̃g�b�v�f�[�^�i����m_pLastNode��Prev�j
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
	CLinkageNode*	m_pTargetNode;		// �����̃g�b�v
	CLinkageNode*	m_pTopNode;			// �����̃g�b�v
	CLinkageNode*	m_pLastNode;		// �����̃��X�g
	int				m_nCount;


	CLinkageNode* GetStartTop(int index);
	CLinkageNode* GetStartLast(int index);
private:

};

#endif
