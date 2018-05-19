#include "CLinkageList.h"


CLinkageList::CLinkageList()
{
	m_pTopNode = new CLinkageNode(NULL, NULL, NULL, NULL);
	m_pLastNode = new CLinkageNode(m_pTopNode, NULL, NULL, NULL);
	m_pTopNode->SetNext(m_pLastNode);
	m_nCount = 0;
	
}

void CLinkageList::Clear(int nLV)
{
	if (!m_pTopNode || !m_pLastNode)
		return;
	CLinkageNode *pNode = m_pTopNode->GetNext();
	CLinkageNode *pTemp = NULL;

	while (pNode != m_pLastNode)
	{
		pTemp = pNode->GetNext();
		int nCLv = pTemp->GetCommonLevel();
		if (nCLv <= nLV || nCLv == -1)
		{
			SafeDelete(pNode);
			m_nCount--;
		}
		pNode = pTemp;
	}
}

void* CLinkageList::GetData(int index)
{
	if (index < 0 || index >= m_nCount || m_nCount == 0)
		return NULL;

	return (GetNode(index))->GetData();

}

// 外部用のトップ（内部m_pTopNodeのNext）
CLinkageNode* CLinkageList::GetTopNode()
{
	CLinkageNode *pNode = m_pTopNode->GetNext();
//	if (pNode == m_pLastNode)
//		return NULL;
	return pNode;
}


// 外部用のラスト（内部m_pLastNodeのPrev）
CLinkageNode* CLinkageList::GetLastNode()
{
	CLinkageNode *pNode = m_pLastNode->GetPrev();
//	if (pNode == m_pTopNode)
//		return NULL;
	return pNode;
}

int CLinkageList::Remove(int index, int nLV)
{
	if (index < 0 || index >= m_nCount || m_nCount == 0)
		return NULL;

	CLinkageNode *pNode = GetNode(index);
	if (pNode->GetCommonLevel() < nLV)
		return m_nCount;
	
//	SafeDelete(pNode);

//	m_nCount--;
	return RemoveNode(pNode);
}

BOOL CLinkageList::Insert(int index ,void* pNewData, int nDataSize, int nLV)
{
//	if (!pNewData)	return FALSE;	NULLでも通す
	
	if (index < 0 || index > m_nCount)
		return FALSE;
	
	CLinkageNode *pNext = NULL;
	CLinkageNode *pPrev = NULL;
	if (index == m_nCount)
		pNext = m_pLastNode;
	else
		pNext = GetNode(index);
	if (!pNext)	return FALSE;

	pPrev = pNext->GetPrev();
	if (!pPrev)	return FALSE;

	CLinkageNode *pNewNode = new CLinkageNode(pPrev, pNewData, nDataSize, pNext, nLV);
	if (!pNewNode) return FALSE;

	m_nCount++;
	return TRUE;
}

int CLinkageList::GetTargetNodeCommonLevel()
{
	if (m_pTargetNode == m_pLastNode) return FALSE;

	return m_pTargetNode->GetCommonLevel();
}
/*
BOOL CLinkageList::GetNextTargetData(void** pTargetData)
{
	if (m_pTargetNode == m_pLastNode) return FALSE;

	m_pTargetNode = m_pTargetNode->GetNext();
	if (m_pTargetNode == m_pLastNode) return FALSE;

	*pTargetData = m_pTargetNode->GetData();
	return TRUE;
}
*/
int	CLinkageList::RemoveTargetNode()
{
	if (m_pTargetNode == m_pTopNode) return m_nCount;
	if (m_pTargetNode == m_pLastNode) return m_nCount;
	
//	CLinkageNode *pPrevNode = m_pTargetNode->GetPrev();
//	SafeDelete(m_pTargetNode);
//	m_nCount--;
//	m_pTargetNode = pPrevNode;
	return RemoveNode(m_pTargetNode);
}

CLinkageNode* CLinkageList::GetStartTop(int index)
{
	CLinkageNode *pNode = m_pTopNode->GetNext();
	for (int i=0;i<index;i++)
		pNode = pNode->GetNext();
	return pNode;
}

CLinkageNode* CLinkageList::GetStartLast(int index)
{
	CLinkageNode *pNode = m_pLastNode->GetPrev();
	for (int i=(m_nCount-1);i>index;i--)
		pNode = pNode->GetPrev();
	return pNode;
}


// ソート
// ロック解除状態のときのみ
BOOL CLinkageList::Sort(int size)
{
	if (m_nCount == 0) return FALSE;
	
	CLinkageNode *pNode1 = m_pTopNode->GetNext();
	CLinkageNode *pNode2,*pNodeT;
	char *data1,*data2;
	int ret;

	for (int i=0;i<m_nCount;i++)
	{
		data1 = (char*)pNode1->GetData();
		pNode2 = pNode1->GetNext();
		for (int j=0;j<m_nCount;j++)
		{
			data2 = (char*)pNode2->GetData();
			ret = strncmp(data1, data2, size);
			pNodeT = pNode2->GetNext();
			if (0 < ret)
				pNode2->MoveNodeToPushFront(pNode1);
			else if (0 > ret)
				pNode2->MoveNodeToPushBack(pNode1);
			pNode2 = pNodeT;
		}
		pNode1 = pNode1->GetNext();
	}
	return TRUE;
}