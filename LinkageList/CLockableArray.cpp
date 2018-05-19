#include "CLockableArray.h"


CLockableArray::CLockableArray() :
CDynamicArray()
{
	m_bLock = FALSE;
	m_pArray = NULL;

}

void CLockableArray::Clear(int nLV)
{
	CDynamicArray::Clear(nLV);			// LinkageList削除
	SafeDeleteArray(m_pArray);		// ポインタ配列削除
	SortMyList();
	m_bLock = FALSE;
}

void* CLockableArray::GetData(int index)
{
	if (!m_bLock || index < 0 || index >= m_nCount || m_nCount == 0)
		return NULL;

	return m_pArray[index];
}

int CLockableArray::Remove(int index, int nLV)
{
#ifdef _DEBUG
	if (m_bLock)
		return DXTRACE_ERR( TEXT("CDynamicArray:GetLastNode!m_bLock=true"), -1 );
#else
	if (m_bLock)
		return -1;
#endif
	return CLinkageList::Remove(index, nLV);
}

BOOL CLockableArray::Insert(int index ,void* pNewData, int nLV)
{
#ifdef _DEBUG
	if (m_bLock)
		return DXTRACE_ERR( TEXT("CLockableArray:GetLastNode!m_bLock=true"), FALSE );
#else
	if (m_bLock)
		return FALSE;
#endif
	return CLinkageList::Insert(index, pNewData, nLV);
}

int	CLockableArray::RemoveTargetNode()
{
#ifdef _DEBUG
	if (m_bLock)
		return DXTRACE_ERR( TEXT("CLockableArray:GetLastNode!m_bLock=true"), FALSE );
#else
	if (m_bLock)
		return FALSE;
#endif
	return CDynamicArray::RemoveTargetNode();
}


void CLockableArray::Lock()
{
	if (m_bLock)	return;
	SafeDeleteArray(m_pArray);			// 一応
	
	m_pArray = new LPVOID[m_nCount];
	CLinkageNode* pNode = GetTopNode();

	for (int i=0;i<m_nCount;i++)
	{
		m_pArray[i] = pNode->GetData();
		pNode = pNode->GetNext();
	}

	m_bLock = TRUE;
}

void CLockableArray::Unlock()
{
	if (!m_bLock)	return;
	SafeDeleteArray(m_pArray);

	m_bLock = FALSE;
}

