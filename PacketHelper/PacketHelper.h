#ifndef _CLASS_PACKET_HELPER_H___
#define _CLASS_PACKET_HELPER_H___

#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE	0x200
#endif

#include <assert.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <TCHAR.h>
#include "util.h"
#include <string>

namespace PacketHelper
{
	static WORD VarEndMarker = 0xC8D1;
	static WORD	VarPacketTopData = 2;
};
const WORD c_wMaxPacketWrap = MAX_PACKET_SIZE-4;
const WORD c_wPacketIndex = 2;

class PacketWrapper
{
public:
	PacketWrapper()
	{
		Clear();
	};
	virtual ~PacketWrapper(){};

	// パラメータを全クリア
	virtual inline void Clear()
	{
		ZeroMemory(m_bytPacket, sizeof(BYTE)*MAX_PACKET_SIZE);
		m_wSize = 0;
	}

	// 各種パラメータの追加
	virtual PacketWrapper & Bool(const bool value);								// 1Byte
	virtual PacketWrapper & Char(const char value);								// 1Byte
	virtual PacketWrapper & Byte(const BYTE value);							// 1Byte
	virtual PacketWrapper & Short(const short value);							// 2Byte
	virtual PacketWrapper & Word(const WORD value);							// 2Byte
	virtual PacketWrapper & Int(const int value);									// 4/8Byte
	virtual PacketWrapper & Long(const long value);								// 4Byte
	virtual PacketWrapper & DWord(const DWORD value);						// 4Byte
	virtual PacketWrapper & Float(const FLOAT value);							// 4Byte
	virtual PacketWrapper & CString(const char *value);							// size(1) + char*(1..255)
	virtual PacketWrapper & WString(const WCHAR *value);					// size(1) + wchar_t*(1..255)
	virtual PacketWrapper & MultiByte1(const BYTE *value, int size);		// size(1) + value(1..255)
	virtual PacketWrapper & MultiByte2(const BYTE *value, int size);		// size(2) + value(1..65536)
	virtual PacketWrapper & Copy(const void *value, int size);					// value(size)
	template <class Type>
	PacketWrapper &  TypeCopy(Type& value)
	{
		return Copy(&value, sizeof(Type));
	};

	virtual inline void SeekIndex(WORD offset, int origin)
	{
		switch (origin)
		{
		case SEEK_CUR:	m_wSize = max(0,min(MAX_PACKET_SIZE, ((int)m_wSize+(int)offset)));	break;
		case SEEK_SET:	m_wSize = max(0,min(MAX_PACKET_SIZE, offset));									break;
		case SEEK_END:	m_wSize = max(0,min(MAX_PACKET_SIZE, m_wSize+offset));						break;
		}
	}
	virtual inline int GetSize() { return m_wSize;	};
	// EndMarkerを最後に貼り付け、先頭2バイトに現在のサイズを入れる。
	// Pack後もパラメータ追加出来るようにindex値は動かさない
	virtual const BYTE* GetPacket(){ return m_bytPacket;	};
protected:
	BYTE m_bytPacket[MAX_PACKET_SIZE];
	WORD	m_wSize;
};

class VarPacketWrapper : public PacketWrapper
{
public:
	VarPacketWrapper(){	Clear();	};
	virtual ~VarPacketWrapper() {};

	// パラメータを全クリア
	virtual inline void Clear()
	{
		ZeroMemory(m_bytPacket, sizeof(BYTE)*MAX_PACKET_SIZE);
		m_wSize = c_wPacketIndex;
	}

	// EndMarkerを最後に貼り付け、先頭2バイトに現在のサイズを入れる。
	// Pack後もパラメータ追加出来るようにindex値は動かさない
	VarPacketWrapper & Pack(WORD* pOutPacketSize);
	// EndMarkerを最後に貼り付け、サイズを返す
	WORD Pack();
protected:
private:
};

class FixPacketWrapper : public PacketWrapper
{
public:
	// 各種パラメータの追加
	virtual inline PacketWrapper & Bool(const bool value)	{ PacketWrapper::Bool(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Char(const char value)			{ PacketWrapper::Char(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Byte(const BYTE value)			{ PacketWrapper::Byte(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Short(const short value)		{ PacketWrapper::Short(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Word(const WORD value)		{ PacketWrapper::Word(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Int(const int value)	{ PacketWrapper::Int(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Long(const long value)	{ PacketWrapper::Long(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & DWord(const DWORD value)	{ PacketWrapper::DWord(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Float(const FLOAT value)		{ PacketWrapper::Float(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & CString(const char *value)	{ PacketWrapper::CString(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & WString(const WCHAR *value){ PacketWrapper::WString(value);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & MultiByte1(const BYTE *value, int size){ PacketWrapper::MultiByte1(value, size);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & MultiByte2(const BYTE *value, int size){ PacketWrapper::MultiByte2(value, size);	CheckSizeOver();	return *this;}
	virtual inline PacketWrapper & Copy(const void *value, int size){ PacketWrapper::Copy(value, size);	CheckSizeOver();	return *this;}
	template <class Type>
	inline PacketWrapper &  TypeCopy(Type& value)
	{
		PacketWrapper::TypeCopy(value);	CheckSizeOver();	return *this;
	};

	virtual inline void SeekIndex(WORD offset, int origin)
	{
		PacketWrapper::SeekIndex(offset, origin);	CheckSizeOver();
	}
	virtual inline int GetSize() { return m_wPacketSize;	};
protected:
	void CheckSizeOver()	{	_ASSERT(m_wSize<=m_wPacketSize);	};
	WORD m_wPacketSize;
};

class PacketPicker
{
public:
	// bufcopy : TRUEの時はパケットのバッファを内部にコピーして扱う
	PacketPicker(const BYTE* pkt)
	{
		_ASSERT( pkt );
		m_wIndex = 0;
		m_pPacket = pkt;
	};
	virtual ~PacketPicker()
	{	Clear();	};

	// 各種パラメータの取得
	bool GetBool();
	inline PacketPicker & Bool(bool* value) { *value = GetBool();	return *this;	};

	char GetChar();
	inline PacketPicker & Char(BYTE* value) { *value = GetChar();	return *this;	};

	BYTE GetByte();
	inline PacketPicker & Byte(BYTE* value) { *value = GetByte();	return *this;	};
	
	short GetShort();
	inline PacketPicker & Short(short* value) { *value = GetShort();	return *this;	};

	WORD GetWord();
	inline PacketPicker & Word(WORD* value) { *value = GetWord();	return *this;	};

	int GetInt();
	inline PacketPicker & Int(int* value) { *value = GetInt();	return *this;	};

	long GetLong();
	inline PacketPicker & Long(long* value) { *value = GetLong();	return *this;	};

	DWORD GetDWord();
	inline PacketPicker & DWord(DWORD* value) { *value = GetDWord();	return *this;	};

	FLOAT GetFloat();
	inline PacketPicker & Float(FLOAT* value) { *value = GetFloat();	return *this;	};

	// *size==0のときvalueのバッファオーバーを気にせずsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	// 最後にNULLも入れる
	PacketPicker & CString(char *value, int* size);
	// *size==0のときvalueのbuffer_sizeかsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	// 最後にNULLも入れる
	PacketPicker & CString(char *value, int* size, int buffer_size);
	// *size==0のときvalueのバッファオーバーを気にせずsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	// buffse_sizeはNULLも含んだ数値
	// 最後にNULLも入れる
	PacketPicker & WString(WCHAR *value, int* size);
	// *size==0のときvalueのbuffer_sizeかsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	// 最後にNULLも入れる
	PacketPicker & WString(WCHAR *value, int* size, int buffer_size);

	// *size==0のときvalueのバッファオーバーを気にせずsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	PacketPicker & MultiByte1(BYTE *value, int* size);
	// *size==0のときvalueのバッファオーバーを気にせずsize分コピーする
	// value==NULLのときsizeに値を入れて返す。(※このとき内部のインデックスは進めない)
	PacketPicker & MultiByte2(BYTE *value, int* size);
	// size分べたコピー
	PacketPicker & Copy(BYTE *value, int size);

	template <class Type>
	inline PacketPicker &  TypeCopy(Type& value)
	{
		return Copy((BYTE*)&value, sizeof(Type));
	};

	inline void SeekIndex(WORD offset, int origin)
	{
		switch (origin)
		{
		case SEEK_CUR:	m_wIndex = max(0,min(m_wSize, ((int)m_wIndex+(int)offset)));	break;
		case SEEK_SET:	m_wIndex = max(0,min(m_wSize, offset));									break;
		case SEEK_END:	m_wIndex = max(0,min(m_wSize, m_wSize+offset));						break;
		}
	}

	WORD GetIndex(){ return m_wIndex;	};
	WORD GetSize(){ return m_wSize;	};

protected:
	virtual inline void Clear()
	{	m_wIndex = 0;	m_wSize = 0;	m_pPacket = NULL;};
	void CheckPacketFormat();
	const BYTE* m_pPacket;
	WORD	m_wSize;
	WORD	m_wIndex;
};

class VarPacketPicker : public PacketPicker
{
public:
	// bufcopy : TRUEの時はパケットのバッファを内部にコピーして扱う
	VarPacketPicker(const BYTE* pkt, BOOL bufcopy=FALSE):PacketPicker(pkt)
	{
		m_wIndex = c_wPacketIndex;
		m_pBuffer = NULL;
		CheckPacketFormat();
		if (m_bCheckPacket && bufcopy)
		{
			m_pBuffer = new BYTE[m_wSize];
			_ASSERT(m_pBuffer);
			memcpy(m_pBuffer, pkt, m_wSize);
			m_pPacket = m_pBuffer;
		}
	};
	virtual ~VarPacketPicker()
	{	Clear();	};

	// サイズ情報がMAX_PACKET_SIZEを越えていないか、
	// エンドマーカーが合っているか
	BOOL CheckPacket() { return m_bCheckPacket;	};
private:
	virtual inline void Clear()
	{	m_wIndex = 0;	m_wSize = 0;	m_pPacket = NULL;	SAFE_DELETE_ARRAY( m_pBuffer );};
	void CheckPacketFormat();
	BOOL	m_bCheckPacket;
	BYTE*	m_pBuffer;
};

#endif