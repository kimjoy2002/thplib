#include "CDynamicArray.h"

CDynamicArray::CDynamicArray() :
CLinkageList()
{
	m_pArray = NULL;
}

void CDynamicArray::Clear(int nLV)
{
	CLinkageList::Clear(nLV);			// LinkageListíœ
//	SafeDeleteArray(m_pArray);		// ƒ|ƒCƒ“ƒ^”z—ñíœ
	SortMyList();
}

void* CDynamicArray::GetData(int index)
{
	if (index < 0 || index >= m_nCount || m_nCount == 0)
		return NULL;

	return m_pArray[index];
}

int	CDynamicArray::Remove(int index, int nLV)
{
	int res = CLinkageList::Remove(index, nLV);
	if (res)	SortMyList();

	return res;
}


int	CDynamicArray::RemoveNode(CLinkageNode* pNode)
{
	int res = CLinkageList::RemoveNode(pNode);
	if (res)	SortMyList();

	return res;
}

BOOL CDynamicArray::Insert(int index ,void* pNewData, int nLV)
{
	BOOL res = CLinkageList::Insert(index, pNewData, nLV);
	if (res)	SortMyList();

	return res;
}

void CDynamicArray::SortMyList()
{
	SafeDeleteArray(m_pArray);			// ˆê‰
	
	m_pArray = new LPVOID[m_nCount];
	CLinkageNode* pNode = GetTopNode();

	for (int i=0;i<m_nCount;i++)
	{
		m_pArray[i] = pNode->GetData();
		pNode = pNode->GetNext();
	}
}


enum EBINARY_SEARCH_ANSWER
{
	EBSAnsMatchTopLast = 0,				// top = last			:ˆê’v
	EBSAnsMatchMiddleTop = 1,			// middle = top			:ˆê’v
	EBSAnsMatchMiddleLast,				// middle = last		:ˆê’v
	EBSAnsMismatchMiddleTop,			// middle = top			:•sˆê’v
	EBSAnsMismatchMiddleLast,			// middle = last		:•sˆê’v
	EBSAnsMismatchOverLast,				// middle > last		:•sˆê’v
	EBSAnsMismatchOverTop,				// middle < top			:•sˆê’v

};

// util
// CDynamicArray‚©‚ç•¶š—ñ‚Ì‚ ‚éindex‚ğ’T‚·
// ¦ƒ\[ƒg‚³‚ê‚Ä‚¢‚é‚±‚Æ(LL_StrSort)
int CDynamicArray::DA_StrBinarySearch(CDynamicArray *array, char* str, int size)
{
	if (array->GetCount() == 0) return -1;

	int tidx = 0;
	int lidx = array->GetCount()-1;
	int midx = (int)(lidx/2);

	char *tdata = (char*)array->GetData(tidx);
	char *ldata = (char*)array->GetData(lidx);
	char *mdata = (char*)array->GetData(midx);
	int d = 0;	// ƒf[ƒ^“à‚Ì‰½Byte–Ú‚Ìˆê’v‚©

	BOOL flg=1;

	while (str[d])
	{
		if (mdata[d] == str[d])
		{
			d++;
			continue;
		}
		else if (tdata[d] == str[d])
		{
			d++;
			continue;
		}
		else if (ldata[d] == str[d])
		{
			d++;
			continue;
		}

		if (mdata[d] > str[d])				// t<s<m<l
		{
			lidx = midx;					// t<s<m=l
			ldata = (char*)array->GetData(lidx);
			midx = (int)(lidx/2)+tidx;		// t<s?m<l
		}
		else if (mdata[d] < str[d])			// t<m<s<l
		{
			tidx = midx;					// t=m<s<l
			tdata = (char*)array->GetData(tidx);
			midx = (int)(lidx/2)+tidx;		// t<m?s<l
		}
	}
	if (strncmp(str, mdata, size) == 0)
		return midx;
	if (strncmp(str, tdata, size) == 0)
		return tidx;
	if (strncmp(str, ldata, size) == 0)
		return lidx;
	return -1;
}

