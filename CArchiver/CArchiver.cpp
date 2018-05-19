#include "CArchiver.h"

CArchiver* CArchiver::m_pInstance = 0;

CArchiver* CArchiver::Instance()
{
	return m_pInstance;
}

void CArchiver::Create()
{
	_ASSERT( !m_pInstance && "already created." );
	m_pInstance = new CArchiver();
}

void CArchiver::Destroy()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

CArchiver::CArchiver()
{
	InitializeCriticalSection(&m_csLoad);
	InitializeCriticalSection(&m_csLoading);
	m_bReqThreadEnd = FALSE;
	m_hMyEvent = NULL;
	GetModuleDir(m_wsBaseDirectory, _MAX_PATH+1);

	unsigned dummy;
	m_hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, Loading, this, 0, &dummy ) );
	SetThreadPriority( m_hThread, THREAD_PRIORITY_BELOW_NORMAL ); //優先度ちょっと下
}

CArchiver::~CArchiver()
{
	m_bReqThreadEnd = TRUE;
	while ( 1 )
	{
		SetEvent(m_hMyEvent);
		if ( WAIT_OBJECT_0 == WaitForSingleObject( m_hThread, 1 ) )
		{
			CloseHandle( m_hThread );
			m_hThread = 0;
			break;
		}
	}

	ClearLoadedBuffer();
	for (std::vector<FILE*>::iterator it = m_vecFilePtr.begin();
		it != m_vecFilePtr.end();
		it++)
	{
		fclose((*it));
	}
	DeleteCriticalSection(&m_pInstance->m_csLoad);
	DeleteCriticalSection(&m_pInstance->m_csLoading);
}

void CArchiver::Open(const WCHAR* path)
{
	EnterLoadCriticalSection();
	std::wstring ws = m_wsBaseDirectory;
	FILE* rf;
	ws += path;
	if (c_eLoadPrimary == eLoadPrimaryArc)
	{
		if (_wfopen_s(&rf, ws.c_str(), L"rb") == 0)
			ListArcInfo(ws.c_str(), rf);
		ListFileInfo(ws.c_str());
	}
	else
	{
		ListFileInfo(ws.c_str());
		if (_wfopen_s(&rf, ws.c_str(), L"rb") == 0)
			ListArcInfo(ws.c_str(), rf);
	}
	LeaveLoadCriticalSection();
}

BOOL CArchiver::Load(const WCHAR* path)
{
	if (!m_hMyEvent) return FALSE;
	CArchiver::EnterLoadCriticalSection();
	m_qReqLoad.push(path);
	CArchiver::LeaveLoadCriticalSection();
	return SetEvent(m_hMyEvent);
}

BOOL CArchiver::ListArcInfo(const WCHAR* path, FILE* pFile)
{
	DWORD dwSize = _filelength(_fileno(pFile));

	fseek(pFile, -2, SEEK_END);
	WORD wFiles = 0;
	fread(&wFiles, 1, sizeof(WORD), pFile);
	fseek(pFile, -2-(wFiles*sizeof(TArcInfo)), SEEK_END);
	std::wstring ws;
	TArcInfo tai;
	TFileInfo tfi;
	for (int i=0;i<wFiles;i++)
	{
		if ( fread(&tai, 1, sizeof(TArcInfo), pFile) != sizeof(TArcInfo))
		{
			fclose(pFile);
			return FALSE;
		}
		tfi.index = tai.index;
		tfi.size = tai.size;
		ws = tai.path;
		tfi.buffer = NULL;
		tfi.f = pFile;
		m_mapFileInfo.insert(std::map<std::wstring, TFileInfo>::value_type(ws, tfi));
	}
	m_vecFilePtr.push_back(pFile);
	return TRUE;
}

BOOL CArchiver::ListFileInfo(const WCHAR* path)
{
	WCHAR pDrive[_MAX_DRIVE+1];
	WCHAR pDir[_MAX_DIR+1];
	WCHAR pFileName[_MAX_FNAME+1];
	_wsplitpath_s(path, pDrive, _MAX_DRIVE+1, pDir, _MAX_DIR+1, pFileName, _MAX_FNAME, NULL, NULL);
	WCHAR pBaseDir[_MAX_PATH+1];
	WCHAR pSearchDir[_MAX_PATH+1];
	_wmakepath_s(pBaseDir, _MAX_PATH+1, pDrive, pDir, L"\\", NULL);
	_wmakepath_s(pSearchDir, _MAX_PATH+1, pDrive, pDir, pFileName, NULL);
	
	BOOL ret = ListDir(pSearchDir, wcslen(pBaseDir)-1);

	return ret;
}

BOOL CArchiver::ListDir(const WCHAR* path, int nBaseLen)
{
	TFileInfo tfi;
	std::wstring tmp;
	std::wstring search = path;
	if (search.at(search.length()-1) == L'\\')
		search += L"*";
	else
		search += L"\\*";

	std::wstring ws(search.c_str(), search.length()-2);

	_wfinddata_t	fd;
	long handle = _wfindfirst(search.c_str(), &fd);
    if (handle == -1) return FALSE;
    do {
		if (wcscmp(fd.name, L"..") == 0) continue;
        if (wcscmp(fd.name, L".") == 0) continue;
		tmp = path;
		tmp += L"\\";
		tmp += fd.name;
		if (fd.attrib & _A_SUBDIR)
		{
			if (!ListDir(tmp.c_str(), nBaseLen))
				return FALSE;
		}
        else
		{
			tfi.index = 0;
			tfi.size = fd.size;
			tfi.buffer = NULL;
			tfi.f = NULL;
			if (IsFilePath(tmp.c_str()))
			{
				tmp.erase(0, nBaseLen);
				m_mapFileInfo.insert(std::map<std::wstring, TFileInfo>::value_type(tmp, tfi));
			}
		}
	}
	while(_wfindnext(handle, &fd) == 0);	
	
	return TRUE;
}

#define QUEUE_MUTEX	(L"Q_MUTEX")
void CArchiver::Loading()
{
	m_hMyEvent = CreateEvent(NULL, TRUE, FALSE, QUEUE_MUTEX);
	while(!m_bReqThreadEnd)
	{
		do
		{
			WaitForSingleObject(m_hMyEvent, INFINITE);
			if (m_bReqThreadEnd)	break;
		} while(!UpdateIsLoading());
		ResetEvent(m_hMyEvent);

		CArchiver::EnterLoadCriticalSection();
		while (UpdateIsLoading())
		{
			if (m_bReqThreadEnd)
				break;
			const WCHAR* path = m_qReqLoad.front();
			LoadBuffer(path);
			m_qReqLoad.pop();
		}
		CArchiver::LeaveLoadCriticalSection();
	}
	CloseHandle(m_hMyEvent);
}

void CArchiver::LoadBuffer(const WCHAR* path)
{
	std::map< std::wstring, TFileInfo >::iterator itfind = m_mapFileInfo.find(path);
	if (itfind == m_mapFileInfo.end())
		return;
	
	TFileInfo* pfi =&(*itfind).second;
	SAFE_DELETE_ARRAY( pfi->buffer );
	(*itfind).second.buffer = new unsigned char[pfi->size];
	_ASSERT( pfi->buffer && L"new unsigned char[pfi->size]" );

	if (c_eLoadPrimary == eLoadPrimaryArc)
	{
		if (!LoadBufferFromArc(pfi))
			LoadBufferFromFile(path, pfi);
	}
	else
	{
		if (!LoadBufferFromFile(path, pfi))
			LoadBufferFromArc(pfi);
	}
	return;
}

BOOL CArchiver::LoadBufferFromArc(TFileInfo* pfi)
{
	if (!pfi->f) return FALSE;
	fseek(pfi->f, pfi->index, SEEK_SET);
#ifdef _DEBUG
	_ASSERT(fread(pfi->buffer, 1, pfi->size, pfi->f) == pfi->size && L"LoadBufferFromArc");
#else
	fread(pfi->buffer, 1, pfi->size, pfi->f);
#endif
	fflush(pfi->f);
	return TRUE;
}

BOOL CArchiver::LoadBufferFromFile(const WCHAR* path, TFileInfo* pfi)
{
	if (pfi->f) return FALSE;
	FILE* rf;
	std::wstring ws = m_wsBaseDirectory;
	ws += path;
	if (!IsFilePath(ws.c_str()))
		return FALSE;
#ifdef _DEBUG
	_ASSERT( _wfopen_s(&rf, ws.c_str(), L"rb") == 0 );
	_ASSERT( fread(pfi->buffer, 1, pfi->size, rf) == pfi->size );
#else
	_wfopen_s(&rf, ws.c_str(), L"rb");
	fread(pfi->buffer, 1, pfi->size, rf);
#endif
	fclose(rf);
	return TRUE;
}

BOOL CArchiver::LoadOnFile(const WCHAR* path, TFileInfo* pfi)
{
	FILE* rf;
	if (!IsFilePath(path))
		return FALSE;
#ifdef _DEBUG
	_ASSERT( _wfopen_s(&rf, path, L"rb") == 0 );
	fseek(rf, 0, SEEK_END);
	pfi->size = ftell(rf);
	fseek(rf, 0, SEEK_SET);
	pfi->buffer = new BYTE[pfi->size];
	_ASSERT( fread(pfi->buffer, 1, pfi->size, rf) == pfi->size );
#else
	_wfopen_s(&rf, path, L"rb");
	fseek(rf, 0, SEEK_END);
	pfi->size = ftell(rf);
	fseek(rf, 0, SEEK_SET);
	pfi->buffer = new BYTE[pfi->size];
	fread(pfi->buffer, 1, pfi->size, rf);
#endif
	fclose(rf);
	return TRUE;
}

BOOL CArchiver::Unload(const WCHAR* path)
{
	EnterLoadCriticalSection();
	std::map< std::wstring, TFileInfo >::iterator itfind = m_mapFileInfo.find(path);
	if (itfind == m_mapFileInfo.end())
	{
		LeaveLoadCriticalSection();
		return FALSE;
	}
	TFileInfo* pfi = &(*itfind).second;
	SAFE_DELETE_ARRAY( pfi->buffer );
	LeaveLoadCriticalSection();
	return TRUE;
}

const TFileInfo* CArchiver::GetFileInfo(const WCHAR* path)
{
	EnterLoadCriticalSection();
	std::map< std::wstring, TFileInfo >::iterator itfind = m_mapFileInfo.find(path);
	if (itfind == m_mapFileInfo.end())
	{
		LeaveLoadCriticalSection();
		return NULL;
	}
	LeaveLoadCriticalSection();
	return &(*itfind).second;
}

void CArchiver::ClearLoadedBuffer()
{
	EnterLoadCriticalSection();
	for (std::map< std::wstring, TFileInfo >::iterator it = m_mapFileInfo.begin();
		it != m_mapFileInfo.end();
		it++)
	{
		SAFE_DELETE_ARRAY ( (*it).second.buffer );
	}
	LeaveLoadCriticalSection();
}

// 直ちにファイルをロード
BOOL CArchiver::LoadOn(const WCHAR* path, TFileInfo* pFileInfo)
{
	std::map< std::wstring, TFileInfo >::iterator itfind = m_mapFileInfo.find(path);
	_ASSERT( itfind != m_mapFileInfo.end() && path);
	
	TFileInfo* pfi =&(*itfind).second;
	memcpy(pFileInfo, pfi, sizeof(TFileInfo));
	// ロード済み
	if (pfi->buffer)
		return TRUE;

	pFileInfo->buffer = new unsigned char[pFileInfo->size];
	_ASSERT( pFileInfo->buffer && L"new unsigned char[pfi->size]" );
	BOOL bLoad = FALSE;
	if (c_eLoadPrimary == eLoadPrimaryArc)
	{
		if (! (bLoad = LoadBufferFromArc(pFileInfo)))
			bLoad = LoadBufferFromFile(path, pFileInfo);
	}
	else
	{
		if (!(bLoad = LoadBufferFromFile(path, pFileInfo)))
			bLoad = LoadBufferFromArc(pFileInfo);
	}

	if (!bLoad)
		SAFE_DELETE_ARRAY(pFileInfo);

	return bLoad;
}