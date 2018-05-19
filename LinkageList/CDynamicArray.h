#ifndef H_DYNAMIC_ARRAY_LIST_CLASS___
#define H_DYNAMIC_ARRAY_LIST_CLASS___
// ===================================================================
// CDynamicArray.h
//	���I�z��N���X�E�w�b�_�[
//	�@���I�Ȏ擾�𑁂�����
//�@�@���I�Ȓǉ�/�폜�͒x���Ȃ�
//�@�@�|�C���^�z������̂Ŏ��Ԃ��w���̈悪�{�K�v�ɂȂ�
// ===================================================================

#include <windows.h>
#include <stdio.h>
#include <STDLIB.H>
#include "util.h"
#include "CLinkageList.h"

class CDynamicArray : public CLinkageList
{
public:
	CDynamicArray();
	virtual ~CDynamicArray()
	{
		AllClear();
//		SafeDelete(m_pTopNode);
//		SafeDelete(m_pLastNode);
	};

	// Remove
	virtual void Clear(int nLV=0);
	inline virtual void AllClear()
	{	Clear(-1);	};

	inline int	GetCount()
	{	return m_nCount;	};

	inline void* GetDataFromLinkageList(int index)
	{	return CLinkageList::GetData(index);	};

	virtual void* GetData(int index);
	void* operator[]( int index )
	{ return GetData(index);	};

	inline virtual BOOL AddLast(void* pNewData, int nLV=0)
	{	return (Insert(m_nCount, pNewData, nLV));	};	
	inline virtual BOOL AddTop(void* pNewData, int nLV=0)
	{	return (Insert(0, pNewData, nLV));	};
	virtual int	Insert(int index, void* pNewData, int nLV=0);
	virtual int	Remove(int index, int nLV=LLIST_ALLCLEAR);
	virtual int RemoveNode(CLinkageNode* pNode);

//	// �O���p�̃g�b�v�i����m_pTopNode��Next�j
//	CLinkageNode* GetTopNode();
//	// �O���p�̃g�b�v�i����m_pLastNode��Prev�j
//	CLinkageNode* GetLastNode();

	virtual void RewindTargetNode() { m_pTargetNode = m_pTopNode; };
//	BOOL GetNextTargetData(void** pTargetData);
//	virtual int	RemoveTargetNode();
	virtual int	RemoveTargetNode()
	{
		int res = RemoveNode(m_pTargetNode);
		SortMyList();
		return res;
	};

	static int DA_StrBinarySearch(CDynamicArray *array, char* str, int size=255);

protected:
	virtual void	SortMyList();		// �����̃|�C���^����

	void**			m_pArray;			// �����̃|�C���^�z��
//	CLinkageNode*	m_pTargetNode;		// �����̃g�b�v
//	CLinkageNode*	m_pTopNode;			// �����̃g�b�v
//	CLinkageNode*	m_pLastNode;		// �����̃��X�g
//	int				m_nCount;



private:

};


#endif