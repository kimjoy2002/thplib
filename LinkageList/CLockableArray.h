#ifndef H_LOCKABLE_ARRAY_LIST_CLASS___
#define H_LOCKABLE_ARRAY_LIST_CLASS___
// ===================================================================
// CLoockableArray.h
//	���I�z��N���X�E�w�b�_�[
//	�@���b�N��������Get�p�̔z��������P�[�W���X�g�N���X����J���č��
//	�@���I�Ȏ擾�𑁂�����
//�@�@�ς��Ƀ��b�N/���b�N�������K�v�ɂȂ�B���I�Ȓǉ�/�폜�͒x���Ȃ�
//�@�@�@���b�N�F�|�C���^�z������ꂽ��ԁA�ǉ�/�폜�ł��Ȃ�
//�@�@�@���b�N�����F�����P�[�W���X�g�Ŕz��ҏW��
//�@�@�|�C���^�z������̂Ŏ��Ԃ��w���̈悪�{�K�v�ɂȂ�
// ===================================================================

#include <windows.h>
#include <stdio.h>
#include <STDLIB.H>
#include "util.h"
#include "CLinkageList.h"
#include "CDynamicArray.h"



class CLockableArray : public CDynamicArray
{
public:
	CLockableArray();
	virtual ~CLockableArray()
	{
		AllClear();
//		SafeDelete(m_pTopNode);
//		SafeDelete(m_pLastNode);
	};

	// Remove & Unlock
	virtual void Clear(int nLV=0);
	inline virtual void AllClear()
	{	Clear(-1);	};

	inline int	GetCount()
	{	return m_nCount;	};

	inline BOOL	IsLock()
	{	return m_bLock;	};

	virtual void Lock();
	virtual void Unlock();

	inline void* GetDataFromLinkageList(int index)
	{	return CDynamicArray::GetData(index);	};
	// Lock��������
	virtual void* GetData(int index);

	// Unlock��������

	inline virtual BOOL AddLast(void* pNewData, int nLV=0)
	{	return (!m_bLock && Insert(m_nCount, pNewData, nLV));	};	
	inline virtual BOOL AddTop(void* pNewData, int nLV=0)
	{	return (!m_bLock && Insert(0, pNewData, nLV));	};
	virtual int	Insert(int index, void* pNewData, int nLV=0);
	virtual int	Remove(int index, int nLV=LLIST_ALLCLEAR);

//	// �O���p�̃g�b�v�i����m_pTopNode��Next�j
//	CLinkageNode* GetTopNode();
//	// �O���p�̃g�b�v�i����m_pLastNode��Prev�j
//	CLinkageNode* GetLastNode();

	virtual void RewindTargetNode() { m_pTargetNode = m_pTopNode; };
//	BOOL GetNextTargetData(void** pTargetData);
	virtual int	RemoveTargetNode();

	// util
	// CLockableArray���當����̂���index��T��
	// ���\�[�g����Ă��邱��(LL_StrSort)
	static int LA_StrBinarySearch(CLockableArray *array, char* str, int size=255)
	{
		if (!array->IsLock()) return -1;
		return CDynamicArray::DA_StrBinarySearch(array, str, size);
	}

protected:
	void**			m_pArray;			// �����̃|�C���^�z��
//	CLinkageNode*	m_pTargetNode;		// �����̃g�b�v
//	CLinkageNode*	m_pTopNode;			// �����̃g�b�v
//	CLinkageNode*	m_pLastNode;		// �����̃��X�g
//	int				m_nCount;



private:
	BOOL			m_bLock;

};


#endif