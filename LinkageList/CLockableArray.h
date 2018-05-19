#ifndef H_LOCKABLE_ARRAY_LIST_CLASS___
#define H_LOCKABLE_ARRAY_LIST_CLASS___
// ===================================================================
// CLoockableArray.h
//	動的配列クラス・ヘッダー
//	　ロックした時にGet用の配列をリンケージリストクラスを手繰って作り
//	　動的な取得を早くする
//　　変わりにロック/ロック解除が必要になる。動的な追加/削除は遅くなる
//　　　ロック：ポインタ配列を作られた状態、追加/削除できない
//　　　ロック解除：リンケージリストで配列編集中
//　　ポインタ配列を作るので実態を指す領域が倍必要になる
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
	// Lock中だけ可
	virtual void* GetData(int index);

	// Unlock中だけ可

	inline virtual BOOL AddLast(void* pNewData, int nLV=0)
	{	return (!m_bLock && Insert(m_nCount, pNewData, nLV));	};	
	inline virtual BOOL AddTop(void* pNewData, int nLV=0)
	{	return (!m_bLock && Insert(0, pNewData, nLV));	};
	virtual int	Insert(int index, void* pNewData, int nLV=0);
	virtual int	Remove(int index, int nLV=LLIST_ALLCLEAR);

//	// 外部用のトップ（内部m_pTopNodeのNext）
//	CLinkageNode* GetTopNode();
//	// 外部用のトップ（内部m_pLastNodeのPrev）
//	CLinkageNode* GetLastNode();

	virtual void RewindTargetNode() { m_pTargetNode = m_pTopNode; };
//	BOOL GetNextTargetData(void** pTargetData);
	virtual int	RemoveTargetNode();

	// util
	// CLockableArrayから文字列のあるindexを探す
	// ※ソートされていること(LL_StrSort)
	static int LA_StrBinarySearch(CLockableArray *array, char* str, int size=255)
	{
		if (!array->IsLock()) return -1;
		return CDynamicArray::DA_StrBinarySearch(array, str, size);
	}

protected:
	void**			m_pArray;			// 内部のポインタ配列
//	CLinkageNode*	m_pTargetNode;		// 内部のトップ
//	CLinkageNode*	m_pTopNode;			// 内部のトップ
//	CLinkageNode*	m_pLastNode;		// 内部のラスト
//	int				m_nCount;



private:
	BOOL			m_bLock;

};


#endif