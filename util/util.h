#ifndef H_MY_UTIL___
#define H_MY_UTIL___

#pragma once
// DEBUG用
//#define DEBUG___
// RELEASE用
//#define RELEASE___

// ディスプレイが16bitなら宣言
//#define DISPLAY_16BIT
//#define FULLSCREEN

#define NOTEMPLATE	1

#if	(DIRECTINPUT_VERSION != 0x0800)
#undef	DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION	0x0800
#endif

#include <windows.h>
#include <TCHAR.h>
//#include <strsafe.h>
#include <d3d9.h>
#include <d3dx9.h>
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif

#include "math.h"
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include < locale.h >
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <string>

/* パケットエンド??カ? */
#define PEND 0xC8D1

//当たり判定
//int HitJuge(D3DXVECTOR3 Pos1, D3DXVECTOR3 Pos2, int HitLenge);
#define HITLENGEFRUITH			3
#define HITLENGEHAND			10

#define		D_PI						3.14159265358979323846
#define		DegreeToRadian( degree ) ((degree) * (D_PI / 180.0))
#define		RadianToDegree( radian ) ((radian) * (180.0 / D_PI))
// 汎用?作
// FLOATをDWORD引数に詰め込むヘルパ???
inline DWORD FtoDW( FLOAT f )		{ return *((DWORD*)&f);	}
inline float DWtoF( DWORD n )		{ return *(float*)&n;			}
inline float MaxFloat()					{ DWtoF(0x7f7fffff);			}
#define GET_RAD(x)	((D3DX_PI/180)*(x))

inline int GetRectWidth( const RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int GetRectHeight( const RECT &rc ) { return ( (rc).bottom - (rc).top ); }

inline int Roundup(double d)
{
	return (int)(d+0.5-(d<0));
};

/*ベクトルの角度を計算*/
inline int GetAngle(double x,double y)
{
	double s;
	int deg;

	s= acos(x/sqrt(x*x+y*y)); /*角度θを求める*/
	s=(s/D_PI)*180.0; /* ラジアンを度に変換 */
	if (y<0) /* θ＞πの時 */
	  s=360-s;

	deg=(int)floor(s+0.5);/*小数?を四捨五入*/
	return deg; /*角度θを返す*/
};

/*ベクトルの角度を計算*/
inline float GetAnglef(float x,float y)
{
	float s;
//	float deg = 0;

	s= acosf(x/sqrtf(x*x+y*y)); /*角度θを求める*/
	s=(s/D3DX_PI)*180.0f; /* ラジアンを度に変換 */
	if (y<0.0f) /* θ＞πの時 */
	  s=360.0f-s;

//	deg=floorf((float)(s+0.5));/*小数?を四捨五入*/
//	return deg; /*角度θを返す*/
	return s; /*角度θを返す*/
};

/*ベクトルの角度を計算*/
inline double GetAngled(double x,double y)
{
	double s;
//	double deg = 0;

	s= acos(x/sqrt(x*x+y*y)); /*角度θを求める*/
	s=(s/D_PI)*180.0; /* ラジアンを度に変換 */
	if (y<0) /* θ＞πの時 */
	  s=360-s;

//	deg=floor(s+0.5);/*小数?を四捨五入*/
//	return deg; /*角度θを返す*/
	return s; /*角度θを返す*/
};

inline double GetRadian(double x, double y)
{
	return D3DXToRadian(GetAngle(x,y));
}

inline float GetRadianf(float x, float y)
{
	return D3DXToRadian(GetAnglef(x,y));
}

inline double GetRadiand(double x, double y)
{
	return DegreeToRadian(GetAngled(x,y));
}

inline bool isSpecial( float a){
	unsigned* p = reinterpret_cast< unsigned* >( &a );
	return ( ( *p & 0x7e000000) == 0x7e000000 );
}

inline void SetUnitVecf(D3DXVECTOR2* pOutVec, float fRad)
{
	float fCos = cosf(fRad);
	pOutVec->x = isSpecial(fCos)?0.0f:fCos;
	float fSin = sinf(fRad);
	pOutVec->y = isSpecial(fSin)?0.0f:fSin;
};

inline void SetUnitVec(D3DXVECTOR2* pOutVec, double dRad)
{
	double dCos = cos(dRad);
	pOutVec->x = (float)(dCos);
	double dSin = sin(dRad);
	pOutVec->y = (float)(dSin);
};

/* UNICODEと分けたりする*/
#ifdef _UNICODE
#define LPTSTRToPCHAR LPTSTRToPCHARW
#define SafePrintfAndGetLen	SafePrintfAndGetLenW
#define SafePrintf					SafePrintfW
#define HexStrToInt	HexStrToIntW
#define ChangeWordCode ChangeWordCodeW
#define GetWordNum GetWordNumW
#define IsFilePath	IsFilePathW
#define IsDirPath	IsDirPathW
#else
#define LPTSTRToPCHAR LPTSTRToPCHARA
#define SafePrintfAndGetLen	SafePrintfAndGetLenA
#define SafePrintf					SafePrintfA
#define HexStrToInt	HexStrToIntA
#define ChangeWordCode ChangeWordCodeA
#define GetWordNum GetWordNumA
#define IsFilePath	IsFilePathA
#define IsDirPath	IsDirPathA
#endif	// _UNICODE


#ifdef _DEBUG
#ifdef OUTPUT_DEBUG_NEW_MSG
#define OUTPUT_DEBUG_NEW_MSG(s)	DXTRACE_MSG( s );
#else
#define OUTPUT_DEBUG_NEW_MSG(s)	0L;
#endif
#ifdef OUTPUT_DEBUG_DELETE_MSG
#define OUTPUT_DEBUG_DELETE_MSG(s)	DXTRACE_MSG( s );
#else
#define OUTPUT_DEBUG_DELETE_MSG(s)	0L;
#endif

#endif

#ifdef _DEBUG
#include "dxerr.h"
#pragma comment(lib, "dxerr.lib")
// np = new a;	OutputString << msg
#define DEBUG_NEW(np, a, msg)	{ADD_DEBUG_NEW(np, a,msg);}	//DXTRACE_MSG( L"@call new (" ## _T( # a) ## L") : " ## msg);
// delete d;	OutputString << msg
#define DEBUG_DELETE(d, msg)	{ADD_DEBUG_DELETE(d,msg);}//if (a)	{delete a; a = NULL; } DXTRACE_MSG( L"@call delete (" ## _T( # a) ## L") : " ## msg);
// delete [] d;	OutputString << msg
#define DEBUG_DELETE_ARRAY(d, msg)		{ADD_DEBUG_DELETE_ARRAY(d,msg);}//	if (a)	{delete [] a; a = NULL; } DXTRACE_MSG( L"@call delete [] (" ## _T( # a) ## L") : " ## msg);
#define CHECK_DEBUG_MEMORY_LEAK	CHECK_DEBUG_MEMORY_LEAK_ON
#else
#define DEBUG_NEW(np, a, msg) np = new a;
#define DEBUG_DELETE(a, msg)	if (a)	{delete a; a = NULL; } 
#define DEBUG_DELETE_ARRAY(a, msg)		if (a)	{delete [] a; a = NULL; }
#define CHECK_DEBUG_MEMORY_LEAK	0L;
#endif

#ifdef _DEBUG
// <pointer, msg>
static std::map<INT, std::wstring>	MAP_DebugNewData;
typedef std::pair < INT, std::wstring > debug_data_pair;
#ifdef DEBUG___
#define ADD_DEBUG_NEW(np, n, msg)	np = new n; { WCHAR ln[8]; _itow_s(__LINE__, ln, 5, 10 ); WCHAR wstr[256]; swprintf_s(wstr, 255, L"%s(%s)_%s = %s; << %s" ,_T(__FILE__) ,ln ,_T(# np) ,_T(# n), msg);MAP_DebugNewData.insert(debug_data_pair( (INT)np,  wstr) );}	\
DXTRACE_MSG(msg);
#else
#define ADD_DEBUG_NEW(np, n, msg)	np = new n; { WCHAR ln[8]; _itow_s(__LINE__, ln, 5, 10 ); WCHAR wstr[256]; swprintf_s(wstr, 255, L"%s(%s)_%s = %s; << %s" ,_T(__FILE__) ,ln ,_T(# np) ,_T(# n), msg);MAP_DebugNewData.insert(debug_data_pair( (INT)np,  wstr) );}	
#endif

#ifdef DEBUG___
#define ADD_DEBUG_DELETE(d,msg)	if (d)	{	\
delete d;														\
MAP_DebugNewData.erase((INT)d);				\
d = NULL;}													\
DXTRACE_MSG(msg);
#else
#define ADD_DEBUG_DELETE(d,msg)	if (d)	{	\
delete d;														\
MAP_DebugNewData.erase((INT)d);				\
d = NULL;}
#endif

#ifdef DEBUG___
#define ADD_DEBUG_DELETE_ARRAY(d,msg)	if (d)	{	\
delete [] d;													\
MAP_DebugNewData.erase((INT)d);				\
d = NULL;}													\
DXTRACE_MSG(msg);
#else
#define ADD_DEBUG_DELETE_ARRAY(d,msg)	if (d)	{	\
delete [] d;													\
MAP_DebugNewData.erase((INT)d);				\
d = NULL;}
#endif

#define CHECK_DEBUG_MEMORY_LEAK_ON	OutputDebugString(L"-----Check MemoryLeak-----\n");	\
if (!MAP_DebugNewData.empty()) {	\
	for (std::map< INT,  std::wstring >::iterator itr = MAP_DebugNewData.begin(); itr != MAP_DebugNewData.end(); itr++) {OutputDebugString(itr->second.c_str()); OutputDebugString(L"\n");} }\
else	OutputDebugString(L"-----All Delete v(^o^)v-----\n");


#endif


// 分解
inline void ColorDec(D3DCOLOR clr,BYTE* a, BYTE* r,BYTE* g,BYTE* b)
{
	if (a)	*a = (BYTE)((clr & 0xFF000000) >> 24);
	if (r)	*r = (BYTE)((clr & 0x00FF0000) >> 16);
	if (g)	*g = (BYTE)((clr & 0x0000FF00) >> 8);
	if (b)	*b = (BYTE)((clr & 0x000000FF));
}
// 組立
inline D3DCOLOR ColorEnc(BYTE a,BYTE r,BYTE g,BYTE b)
{
	return (a << 24 | r << 16 | g << 8 | b);
}
// StrToInt
inline int StrToInt(LPTSTR str)
{
	int nTotal=0;
	int nketa = 1;
	for (int i= (int)_tcslen(str)-1;i>=0 && ((BYTE)str[i]-(BYTE)'0')<10;i--)
	{
		nTotal += (str[i]-'0') * nketa;
		nketa *= 10;
	}
	return nTotal;
}
// StrToInt
inline int StrToIntFromConst(LPCTSTR str)
{
	int nTotal=0;
	int nketa = 1;
	for (int i= (int)_tcslen(str)-1;i>=0 && ((BYTE)str[i]-(BYTE)'0')<10;i--)
	{
		nTotal += (str[i]-'0') * nketa;
		nketa *= 10;
	}
	return nTotal;
}
// new + strncpy
inline int new_tcscpy(LPTSTR &str_new, LPCTSTR str_old)
{
	int nl = (str_old) ? (int)_tcslen(str_old)+1 : 0;
	if (nl)
	{	
		str_new = new TCHAR[nl];
		_tcscpy_s(str_new, nl, str_old);
	}
	else
		str_new = NULL;
	return nl;
}

/**
	char*		ppszDestChar	:	out
	TCHAR*	pszSrcTchar	:	in
	ppszCharは未割り当てで引数わたし、呼び出し側で解放
*/
BOOL LPTSTRToPCHARW(char** ppszDestChar, TCHAR* pszSrcTchar);
/**
	char*		ppszDestChar	:	out
	char*	pszSrcTchar	:	in
	ppszCharは未割り当てで引数わたし、呼び出し側で解放
*/
BOOL LPTSTRToPCHARA(char** ppszDestChar, char* pszSrcTchar);
// -------------------------------------------------------------------
// [テンプレ?ト] 安全にオブジェクトを削除する
// in:	削除するオブジェクト?イン?(配列を除く)
// -------------------------------------------------------------------

INT ConvertUTF8toSJIS( BYTE* pSource, BYTE* pDist, int* pSize );
BOOL ConvertSJIStoUTF8( BYTE* pSource, BYTE* pDist, int* pSize );

inline void ErrMsg(LPTSTR msg, HWND hWnd = NULL)
{
	MessageBox(hWnd, msg, _T("Error"), MB_OK);
}
#define ERR_MSG_SIZE	64

// newで生成した物を消す
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	{	if (x) { delete (x); (x) = NULL;	}}
#endif
template <class Type>
inline void SafeDelete(Type& obj)
{
#ifdef DEBUG___
#ifdef _UNICODE
	DXTRACE_MSG(L"call safe delete");
#else
	DXTRACE_MSG("call safe delete");
#endif
#endif
	if (obj){
		delete obj;
		obj = NULL;
	}
}

// new xxx[num]で生成した物を消す
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	{	if (x) { delete [] (x); (x) = NULL;	}}
#endif
template <class Type>
inline void SafeDeleteArray(Type& obj)
{
#ifdef DEBUG___
#ifdef _UNICODE
	DXTRACE_MSG(L"call safe delete array");
#else
	DXTRACE_MSG("call safe delete array");
#endif
#endif
	if (obj){
		delete [] obj;
		obj = NULL;
	}
}

// free
#define SAFE_FREE(x)	{ if(x) { free(x);     (x)=NULL; } }
template <class Type>
inline void SafeFree(Type& obj)
{
#ifdef DEBUG___
#ifdef _UNICODE
	DXTRACE_MSG(L"call safe free");
#else
	DXTRACE_MSG("call safe free");
#endif
#endif
	if (obj){
		free(obj);
		obj = NULL;
	}
}

// DXのRelease
template <class Type>
inline void SafeRelease(Type& obj)
{
#ifdef DEBUG___
#ifdef _UNICODE
	DXTRACE_MSG(L"call safe release");
#else
	DXTRACE_MSG("call safe release");
#endif
#endif
	if (obj){
		obj->Release();
		obj = NULL;
	}
}

// newで生成した物を消す
template <class Type, class RetType>
inline RetType SafeDeleteAndReturn(Type& obj, RetType& ret)
{
	if (obj){
		delete obj;
		obj = NULL;
	}
	return ret;
}

// SafePrintfAndGetLen - _vsnwprintf_s簡略 戻り値 wcslen
inline int SafePrintfAndGetLenW(WCHAR* pszStr, size_t MaxBuf, const WCHAR* pszFormat, ...)
{
    HRESULT hr;
	UINT nLength = 0;
    va_list argList;
    va_start(argList, pszFormat);
	hr = _vsnwprintf_s(pszStr, MaxBuf, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if ( hr == S_OK)
		nLength = (int)wcslen( pszStr);
	return (int)nLength;
};

// SafePrintfAndGetLen - _vsnprintf_s簡略 戻り値 strlen
inline int SafePrintfAndGetLenA(char* pszStr, size_t MaxBuf, const char* pszFormat, ...)
{
    HRESULT hr;
	int nLength = 0;
    va_list argList;
    va_start(argList, pszFormat);
	hr = _vsnprintf_s(pszStr, (size_t)MaxBuf, _TRUNCATE, pszFormat, argList);
	va_end(argList);
	if ( hr == S_OK)
		nLength = (int)strlen( pszStr);
	return nLength;
};

// SafePrintf - _vsnwprintf_s簡略
inline HRESULT SafePrintfW(WCHAR* pszStr, size_t MaxBuf, const WCHAR* pszFormat, ...)
{
//	int res = -1;
    HRESULT hr = E_FAIL;
    va_list argList;
    va_start(argList, pszFormat);
//	res = swprintf_s(pszStr, MaxBuf, pszFormat, argList);
	hr = _vsnwprintf_s(pszStr, MaxBuf, _TRUNCATE, pszFormat, argList);
	va_end(argList);

#ifdef _DEBUG
	_ASSERT(pszStr != NULL && pszFormat);
#endif
//	_ASSERT(hr != -1 && pszFormat);
//	if (res == -1)		return E_FAIL;
//	return S_OK;
	return hr;
};

// SafePrintf - _vsnprintf_s簡略
inline HRESULT SafePrintfA(char* pszStr, size_t MaxBuf, const char* pszFormat, ...)
{
//	int res = -1;
    HRESULT hr = E_FAIL;
    va_list argList;
    va_start(argList, pszFormat);
//	res = sprintf_s(pszStr, MaxBuf, pszFormat, argList);
	hr = _vsnprintf_s(pszStr, MaxBuf, _TRUNCATE, pszFormat, argList);
	va_end(argList);
//	if (res == -1)	return E_FAIL;
//	return S_OK;
	return hr;
};

int NewSafePrintf(LPTSTR &str_new, size_t MaxBuf, LPCTSTR pszFormat, ...);
// memcpy
inline void* SafeMemCopy(void* dest, const void* src, size_t size, size_t maxsize)
{
	if (maxsize < size)
	{
		memcpy(dest, src, maxsize);
		return NULL;
	}
	return memcpy(dest, src, size);
	
}
// SetData(1Byte)
inline INT SetByteData(BYTE* data, BYTE b)
{
	data[0] = b;
	return 1;
};

// SetData(2Byte)
inline INT SetWordData(BYTE* data, WORD w)
{
	memcpy(data, &w, sizeof(WORD));
	return 2;
};

// SetData(4Byte)
inline INT SetDwordData(BYTE* data, DWORD dw)
{
	memcpy(data, &dw, sizeof(DWORD));
	return 4;
};
// SetData(wstring size <= 255)
inline INT SetWStringData(BYTE* data, std::wstring& wstr)
{
	size_t size = min(255, (BYTE)(wstr.size() * sizeof(WCHAR)));
	data[0] = (BYTE)size;
	memcpy(&data[1], wstr.c_str(), size);
	return size+1;
}

// SetData(2Byte)
inline INT SetEndMarker(BYTE* data)
{
	const WORD cEnd = PEND;
	memcpy(data, &cEnd, sizeof(WORD));
	return sizeof(WORD);
};

// SetData(NByte)
inline INT SetMultiByteData(BYTE* data,const BYTE* mb, INT size, INT max)
{
	SafeMemCopy(data, mb, size, max);
	return size;
};
// data:NULL無しで、頭にサイズ情報のあるデ??から文字列をコピ?する(NULL終?付加あり)
// 戻り値：文字数(BYTE)
// maxstr: ?送先Maxバッフ?(BYTE)
// maxsrc: ?送元Maxバッフ?(BYTE)
int DecodeSizeStrFromRaw(LPWSTR dest_str, BYTE* src_data, int maxstr, int maxsrc);

// ディレクトリ有無
BOOL IsDirPathW(const WCHAR* path);
BOOL IsDirPathA(const char* path);
// フ?イル有無
BOOL IsFilePathW(const WCHAR* path);
BOOL IsFilePathA(const char* path);

// N進数に変換
// ret				:各桁を格?して返す
// carry_num		:retの配列数
// value			:値
// base				:進数
// return			:桁あふれしてないか(ル?プしてあまった数値)
inline int DigitConvert(int *ret, int carry_num, int value, int base)
{
	do
	{	*ret++ =  value % base;	}
	while ((value /= base) && (--carry_num));
	return value;
}

BOOL HexStrToIntW(LPTSTR src, int len, int *dest);
BOOL HexStrToIntA(char* src, int len, int *dest);

/*
	文字系関数
*/
int ChangeWordCodeW(LPCTSTR wd, UINT* code);
int ChangeWordCodeA(const char* wd, UINT* code);

int GetWordNumW(LPCTSTR wd);
int GetWordNumA(const char* wd);

// char* -> WCHAR*  count = 文字数(否バイト数)
int CStringToWString(WCHAR* dest_wstr, char* src_str, int dest_size=0);

/* =================================== */
/* =====        クラス定?       ===== */
/* =================================== */

// ==XYZ?== //
class XYZ
{
public:
	SHORT x, y, z;
public:
	XYZ(){x=0;y=0;z=0;};

	XYZ(SHORT ax, SHORT ay = 0, SHORT az = 0)
	{ x = ax; y = ay; z = az;};

	void Set(SHORT ax = 0, SHORT ay = 0,SHORT az = 0)
	{ x = ax; y = ay; z = az;};

	// 加算
	XYZ operator+(XYZ& av)
	{ return XYZ(x+av.x, y+av.y, z+av.z); };
	// 加算
	XYZ& operator+=(XYZ& av) {	
		x += av.x; y += av.y; z += av.z;
		return *this;
	};

	// 減算
	XYZ operator-(XYZ& av)
	{ return XYZ(x-av.x,y-av.y,z-av.z); };

	// 減算
	XYZ& operator-=(XYZ& av) {
		x -= av.x; y -= av.y; z -= av.z;
		return *this;
	};

	// 乗算
	XYZ operator*(SHORT v){
		SHORT rx = x * v;
		SHORT ry = y * v;
		SHORT rz = z * z;
		return XYZ(rx,ry,rz);
	};
	// 乗算
	XYZ& operator*=(SHORT v){
		x *= v;	y *= v;	z *= v;
		return *this;
	};

	// 乗算
	XYZ operator/(SHORT v){
		SHORT rx = (SHORT)(x / v);
		SHORT ry = (SHORT)(y / v);
		SHORT rz = (SHORT)(z / v);
		return XYZ(rx,ry,rz);
	};
	// 除算
	XYZ& operator/=(SHORT v){
		x /= v; y /= v; z /= v;
		return *this;
	};
};

//==============================================================
// CTextFileクラス：テキストフ?イルを文字列を読み出す
#define MAX_CHAR	(1024)

class CTextFile {
protected:
	FILE	*f;
	int		fh;
	int		fsize;
	LPTSTR pszFName;
public:
	CTextFile(LPTSTR fn, LPTSTR rmode);
	~CTextFile();

	BOOL IsOpened()	{return (f != NULL); };
	// "\n\r"を見つけると\0に変えてpに入れる
	BOOL ReadLine(LPTSTR p);
	BOOL ReadLineA(char* p);
	// "\n\r"を最後に付けて書き込む
	BOOL WriteLine(LPTSTR p);
	BOOL Write(LPTSTR p, int size);

	DWORD GetSize() {
		if (f) return fsize;
		return 0;
	};
	DWORD GetPos() {
		if (f) return ftell(f);
		return 0;
	};
	
	DWORD Seek(DWORD ofs, int org=SEEK_SET){
		if (f) return fseek(f, ofs, org);
		return 0;
	};

	void SeekToBegin()
	{	if (f) fseek(f, 0, SEEK_SET); };
};

//==============================================================
// CTextFileクラス：暗号化??を入り。テキストフ?イルを文字列を読み出す
// Code
class CCodeTextFile : public CTextFile
{
protected:
	void CodeXOR(LPTSTR &p);
public:
	CCodeTextFile(LPTSTR fn, LPTSTR rmode) : CTextFile(fn,rmode) {};
	~CCodeTextFile() {};

	BOOL ReadLine(LPTSTR p, BOOL dec);
	BOOL WriteLine(LPTSTR p, BOOL enc);

};


template <class T> 
class AutoArray { 
	public: 
		AutoArray(size_t n) 
		{ 
#ifdef _DEBUG 
			// バッフ?オ?バ?ランの?ェックのため、 ?計にひとつアロケ?ト 
			m_ptr = new T[n + 1]; 
#else 
			m_ptr = new T[n]; 
#endif 
			if (m_ptr) { 
				m_size = n; 
#ifdef _DEBUG 
			// 番兵を置く  
			FillMemory(&m_ptr[n], sizeof m_ptr[n], SENTINEL); 
#endif 
		} 
		else { 
			m_size = 0; 
			} 
	
		} 
			~AutoArray() 
		{ 
#ifdef _DEBUG 
		if (m_ptr) { 
			PBYTE pb = reinterpret_cast<PBYTE>(&m_ptr[m_size]); 
			ASSERT(pb); 
			// バッフ?オ?バ?ランの?ェック 
			for (int i = 0; i < sizeof m_ptr[m_size]; ++i) { 
				ASSERT(pb[i] == SENTINEL); 
			} 
		} 
#endif 
			delete[] m_ptr; 
		} 
		
		bool OK() 
		{ 
			return m_ptr != NULL; 
		} 

		operator T*() 
		{ 
			return m_ptr; 
		} 

		T& operator[](size_t n) 
		{ 
			ASSERT(n < m_size); 
			return m_ptr[n]; 
		} 

	protected: 
		// to prohibit copy operations 
		AutoArray(const AutoArray<T>&); 
		AutoArray<T> operator=(const AutoArray<T>&); 

		enum { SENTINEL = 0xcc }; 

	protected: 
		T* m_ptr; 
		size_t m_size; 
}; 

class COneTex
{
public:
	COneTex()
	{	m_pTex = NULL;	};
	~COneTex()
	{	Release();	};

	inline void Release()
	{	SafeRelease(m_pTex);	};

	inline LPDIRECT3DTEXTURE9 Get()
	{	return m_pTex;	};

	inline void Set(LPDIRECT3DTEXTURE9 *ptex)
	{	if (ptex)
			m_pTex = *ptex;
		else
			SafeDelete(m_pTex);
	};


private:
	LPDIRECT3DTEXTURE9	m_pTex;	// Texture

};

// テクス?ャ配列クラス
class CSpTexs
{
public:
	CSpTexs()
	{	pTexs = NULL;
		nCount = 0;
	};
	~CSpTexs()
	{	Clear();	};
	inline BOOL Create(int count)
	{
		Clear();
		if (!count) return FALSE;
		pTexs = new LPDIRECT3DTEXTURE9[count];	//DEBUG_NEW( LPDIRECT3DTEXTURE9[count], _T("CSpTexs::Create") );
		ZeroMemory(&pTexs[0], sizeof(LPDIRECT3DTEXTURE9)*count);
		nCount= count;
		return (pTexs != NULL);
	};
	inline void Clear()
	{
		for (int i=0;i<nCount;i++)
			SafeRelease(pTexs[i]);
		SafeDeleteArray(pTexs);
		nCount = 0;
	};
	inline void Set(LPDIRECT3DTEXTURE9 val, int index)
	{	if (index < nCount && index>=0)
		pTexs[index] = val;
	};
	inline LPDIRECT3DTEXTURE9* Get(int index)
	{	if (index < nCount && index>=0)
			return &pTexs[index];
		return NULL;
	};
	// index0のD3DSURFACE_DESC情報を取得
	inline void GetDesc(D3DSURFACE_DESC *desc)
	{
		if (!pTexs)	return;
		pTexs[0]->GetLevelDesc(0, desc);
	};
	inline int GetCount()
	{	return nCount;	};
private:
	LPDIRECT3DTEXTURE9	*pTexs;		// Texture
	int					nCount;		// 数
};

inline void D3DXMatrixTranslationFromD3DXVECTOR3(D3DXMATRIX *mat, D3DXVECTOR3 *vec)
{
	D3DXMatrixTranslation(mat, vec->x, vec->y, vec->z);
};

BOOL WCHARToMB(char** ppDest, int* pLen, WCHAR* src);
#ifdef _UNICODE
#define GetModuleDir	GetModuleDirW
#else
#define GetModuleDir	GetModuleDirA
#endif
void GetModuleDirA(char* path, int nSize);
void GetModuleDirW(WCHAR* path, int nSize);

#define CALC_RECT_WIDTH(x)		(x->right - x->left)
#define CALC_RECT_HEIGHT(x)		(x->bottom - x->top)

#ifdef _UNICODE
#define round	roundA
#else
#define round	roundW
#endif
inline double roundA( double val, int precision )
{
    /* ***** 内部変数定? ***** */
    double    ret;
    char    buf[64] = {'\0'};
    char    *p;
    /* ***** 丸めを行い文字列に変換 ***** */
	_sprintf_p(buf, 64, "%.*f", precision, val );
    /* ***** 文字列から数値に再変換 ***** */
    ret = strtod( buf, &p );
    return ret;
}

inline double roundW( double val, int precision )
{
    /* ***** 内部変数定? ***** */
    double    ret;
    WCHAR    buf[64] = {L'\0'};
    WCHAR    *p;
    /* ***** 丸めを行い文字列に変換 ***** */
	_swprintf_p(buf, 64, L"%.*f", precision, val );
    /* ***** 文字列から数値に再変換 ***** */
	ret = wcstod( buf, &p );
    return ret;
}

inline void BytesFromHexString(unsigned char *data, const char *src) {
//    printf("string:%s\n", string);
    int len = static_cast<int>(strlen(src));
    for (int i=0; i<len; i+=2) {
        unsigned int x;
        sscanf_s((char *)(src + i), "%02X", &x);
        data[i/2] = x;
    }
}

//簡易delegate
template<typename rv_t, typename arg_t> class Delegator{
    typedef Delegator<rv_t, arg_t> this_t;
    void *obj, *method;

    template<typename obj_t> static rv_t __f(arg_t a, this_t *this_obj){
        typedef rv_t(obj_t::*method_t)(arg_t);
        method_t method = *(method_t*)this_obj->method;
        obj_t *obj = (obj_t*)this_obj->obj;
        return (obj->*method)(a);
    }

    rv_t(*_f)(arg_t, this_t*);

public:
    rv_t f(arg_t a){ return _f(a, this); }

    template<typename obj_t> void assign(obj_t *o, rv_t(obj_t::*m)(arg_t)){
        obj = (void*)o;
        delete method;
        typedef rv_t(obj_t::*method_t)(arg_t);
        method = (void*)new (method_t*)();
        *(method_t*)method = m;
        _f = __f<obj_t>;
    }

    Delegator() : method(0){}
    ~Delegator(){ delete method; }
};


#endif