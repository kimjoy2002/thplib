#ifndef H_DYNAMIC_ARRAY_LIST_CLASS___
#define H_DYNAMIC_ARRAY_LIST_CLASS___
// ===================================================================
// CDynamicArray.h
//	動的配列クラス・ヘッダー
//	　動的な取得を早くする
//　　動的な追加/削除は遅くなる
//　　ポインタ配列を作るので実態を指す領域が倍必要になる
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

//	// 外部用のトップ（内部m_pTopNodeのNext）
//	CLinkageNode* GetTopNode();
//	// 外部用のトップ（内部m_pLastNodeのPrev）
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
	virtual void	SortMyList();		// 内部のポインタ整列

	void**			m_pArray;			// 内部のポインタ配列
//	CLinkageNode*	m_pTargetNode;		// 内部のトップ
//	CLinkageNode*	m_pTopNode;			// 内部のトップ
//	CLinkageNode*	m_pLastNode;		// 内部のラスト
//	int				m_nCount;



private:

};


#endif