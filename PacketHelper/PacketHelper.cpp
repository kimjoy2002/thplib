#include "PacketHelper.h"

// boolの追加
PacketWrapper & PacketWrapper::Bool(const bool value)
{
	_ASSERT( m_wSize+1 < c_wMaxPacketWrap && "PacketSize over!" );
	m_bytPacket[m_wSize] = value;
	m_wSize++;
	return *this;
}

// charの追加
PacketWrapper & PacketWrapper::Char(const char value)
{
	_ASSERT( m_wSize+1 < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(char));
	m_wSize++;
	return *this;
}

// BYTEの追加
PacketWrapper & PacketWrapper::Byte(const BYTE value)
{
	_ASSERT( m_wSize+1 < c_wMaxPacketWrap && "PacketSize over!" );
	m_bytPacket[m_wSize] = value;
	m_wSize++;
	return *this;
}

// shortの追加
PacketWrapper & PacketWrapper::Short(const short value)
{
	_ASSERT( m_wSize+sizeof(short) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(short));
	m_wSize+=sizeof(short);
	return *this;
}

// WORDの追加
PacketWrapper & PacketWrapper::Word(const WORD value)
{
	_ASSERT( m_wSize+sizeof(WORD) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(WORD));
	m_wSize+=sizeof(WORD);
	return *this;
}

// intの追加
PacketWrapper & PacketWrapper::Int(const int value)
{
	_ASSERT( m_wSize+sizeof(int) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(int));
	m_wSize+=sizeof(int);
	return *this;
}

// longの追加
PacketWrapper & PacketWrapper::Long(const long value)
{
	_ASSERT( m_wSize+sizeof(long) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(long));
	m_wSize+=sizeof(long);
	return *this;
}

// DWORDの追加
PacketWrapper & PacketWrapper::DWord(const DWORD value)
{
	_ASSERT( m_wSize+sizeof(DWORD) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(DWORD));
	m_wSize+=sizeof(DWORD);
	return *this;
}

// FLOATの追加
PacketWrapper & PacketWrapper::Float(const FLOAT value)
{
	_ASSERT( m_wSize+sizeof(FLOAT) < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], &value, sizeof(FLOAT));
	m_wSize+=sizeof(FLOAT);
	return *this;
}

// 文字列の追加
PacketWrapper & PacketWrapper::CString(const char* value)
{
	int size = strlen(value);
	_ASSERT( size <= 0xFF && "PacketSize over!" );
	_ASSERT( m_wSize+size+1 < c_wMaxPacketWrap && "PacketSize over!" );
	m_bytPacket[m_wSize] = size;
	m_wSize++;
	memcpy(&m_bytPacket[m_wSize], value, size);
	m_wSize+=size;
	return *this;
}

// 文字列の追加
PacketWrapper & PacketWrapper::WString(const WCHAR* value)
{
	int size = wcslen(value)*sizeof(WCHAR);
	_ASSERT( size <= 0xFF && "PacketSize over!" );
	_ASSERT( m_wSize+size+1 < c_wMaxPacketWrap && "PacketSize over!" );
	m_bytPacket[m_wSize] = size;
	m_wSize++;
	memcpy(&m_bytPacket[m_wSize], value, size);
	m_wSize+=size;
	return *this;
}

// マルチバイト(255バイトまで)の追加
PacketWrapper & PacketWrapper::MultiByte1(const BYTE *value, int size)
{
	_ASSERT( m_wSize+size+1 < c_wMaxPacketWrap && "PacketSize over!" );
	m_bytPacket[m_wSize] = (BYTE)size;
	m_wSize++;
	memcpy(&m_bytPacket[m_wSize], value, (BYTE)size);
	m_wSize+=size;
	return *this;
}

// マルチバイト(65536バイトまで)の追加
PacketWrapper & PacketWrapper::MultiByte2(const BYTE *value, int size)
{
	_ASSERT( m_wSize+size+2 < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], (WORD*)&size, sizeof(WORD));
	m_wSize+=sizeof(WORD);
	memcpy(&m_bytPacket[m_wSize], value, (WORD)size);
	m_wSize+=size;
	return *this;
}

// value(size)
PacketWrapper & PacketWrapper::Copy(const void *value, int size)
{
	_ASSERT( m_wSize+size < c_wMaxPacketWrap && "PacketSize over!" );
	memcpy(&m_bytPacket[m_wSize], value, (BYTE)size);
	m_wSize+=size;
	return *this;
}

VarPacketWrapper & VarPacketWrapper::Pack(WORD* pOutPacketSize)
{
	memcpy(&m_bytPacket[m_wSize], &PacketHelper::VarEndMarker, sizeof(WORD));
	WORD size = m_wSize+sizeof(WORD);
	memcpy(m_bytPacket, &size, sizeof(WORD));
	if (pOutPacketSize)
		*pOutPacketSize = size;
	return *this;
}

WORD VarPacketWrapper::Pack()
{
	memcpy(&m_bytPacket[m_wSize], &PacketHelper::VarEndMarker, sizeof(WORD));
	WORD size = m_wSize+sizeof(WORD);
	memcpy(m_bytPacket, &size, sizeof(WORD));
	return size;
}

///////////////////////////////////////////////////////////////////////////////
// 各種パラメータの設定
bool PacketPicker::GetBool()
{
	_ASSERT( m_wSize>m_wIndex+1 && "PacketSize over!" );
	bool ret = m_pPacket[m_wIndex]?true:false;
	m_wIndex++;
	return ret;
}

char PacketPicker::GetChar()
{
	_ASSERT( m_wSize>m_wIndex+1 && "PacketSize over!" );
	char ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(char));
	m_wIndex++;
	return ret;
}

BYTE PacketPicker::GetByte()
{
	_ASSERT( m_wSize>m_wIndex+1 && "PacketSize over!" );
	BYTE ret = m_pPacket[m_wIndex];
	m_wIndex++;
	return ret;
}

short PacketPicker::GetShort()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(short) && "PacketSize over!" );
	short ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(short));
	m_wIndex+=sizeof(short);
	return ret;
}

WORD PacketPicker::GetWord()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(WORD) && "PacketSize over!" );
	WORD ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(WORD));
	m_wIndex+=sizeof(WORD);
	return ret;
}

int PacketPicker::GetInt()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(int) && "PacketSize over!" );
	int ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(int));
	m_wIndex+=sizeof(int);
	return ret;
}

long PacketPicker::GetLong()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(long) && "PacketSize over!" );
	long ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(long));
	m_wIndex+=sizeof(long);
	return ret;
}

DWORD PacketPicker::GetDWord()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(DWORD) && "PacketSize over!" );
	DWORD ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(DWORD));
	m_wIndex+=sizeof(DWORD);
	return ret;
}

FLOAT PacketPicker::GetFloat()
{
	_ASSERT( m_wSize>m_wIndex+sizeof(FLOAT) && "PacketSize over!" );
	FLOAT ret;
	memcpy(&ret, &m_pPacket[m_wIndex], sizeof(FLOAT));
	m_wIndex+=sizeof(FLOAT);
	return ret;
}

PacketPicker & PacketPicker::CString(char *value, int* size)
{
	value[0] = NULL;
	if (!(*size))
	{
		if (int nSize = m_pPacket[m_wIndex])
		{
			m_wIndex++;
			_ASSERT( m_wSize>m_wIndex+nSize && "PacketSize over!" );
			memcpy(value, &m_pPacket[m_wIndex], nSize);
			value[nSize] = NULL;
			*size = nSize;
			m_wIndex += nSize;
		}
	}
	else if (!value)
	{
		*size = m_pPacket[m_wIndex];		
	}
	else
	{
		m_wIndex++;
		memcpy(value, &m_pPacket[m_wIndex], *size);
		value[*size] = NULL;
		m_wIndex += (*size);
	}
	return *this;
}

PacketPicker & PacketPicker::CString(char *value, int* size, int buffer_size)
{
	_ASSERT(size);
	if (!value)
	{
		*size = m_pPacket[m_wIndex];		
	}
	else if (!(*size))
	{
		value[0] = NULL;
		int nLen = min(buffer_size-1, m_pPacket[m_wIndex]);
		m_wIndex++;
		_ASSERT( m_wSize>m_wIndex+nLen && "PacketSize over!" );
		memcpy(value, &m_pPacket[m_wIndex], nLen);
		value[nLen] = NULL;
		*size = nLen;
		m_wIndex += nLen;
	}
	else
	{
		value[0] = NULL;
		int nLen = min(buffer_size-1, *size);
		m_wIndex++;
		memcpy(value, &m_pPacket[m_wIndex], nLen);
		value[nLen] = NULL;
		*size = nLen;
		m_wIndex += (*size);
	}
	return *this;
}

PacketPicker & PacketPicker::WString(WCHAR *value, int* size)
{
	_ASSERT(size);
	if (!value)
	{
		*size = m_pPacket[m_wIndex];		
	}
	else if (!(*size))
	{
		int nSize = m_pPacket[m_wIndex];
		if (nSize >= 2)
		{
			value[0] = NULL;
			m_wIndex++;
			_ASSERT( m_wSize>m_wIndex+nSize && "PacketSize over!" );
			memcpy(value, &m_pPacket[m_wIndex], nSize);
			value[nSize/sizeof(WCHAR)] = NULL;
			*size = nSize;
			m_wIndex += nSize;
		}
	}
	else
	{
		value[0] = NULL;
		m_wIndex++;
		memcpy(value, &m_pPacket[m_wIndex], *size);
		value[(*size)/sizeof(WCHAR)] = NULL;
		m_wIndex += (*size);
	}
	return *this;
}

PacketPicker & PacketPicker::WString(WCHAR *value, int* size, int buffer_size)
{
	_ASSERT(size);
	if (!value)
	{
		*size = m_pPacket[m_wIndex];		
	}
	else if (!(*size))
	{
		value[0] = NULL;
		int nSize = min(((buffer_size-1)*sizeof(WCHAR)), m_pPacket[m_wIndex]);
		if (nSize >= 2)
		{
			m_wIndex++;
			_ASSERT( m_wSize>m_wIndex+nSize && "PacketSize over!" );
			memcpy(value, &m_pPacket[m_wIndex], nSize);
			value[nSize/sizeof(WCHAR)] = NULL;
			*size = nSize;
			m_wIndex += nSize;
		}
	}
	else
	{
		value[0] = NULL;
		int nSize = min(buffer_size-2, *size);
		if (nSize >= 2)
		{
			m_wIndex++;
			memcpy(value, &m_pPacket[m_wIndex], nSize);
			value[nSize/sizeof(WCHAR)] = NULL;
			m_wIndex += nSize;
		}
	}
	return *this;
}

PacketPicker & PacketPicker::MultiByte1(BYTE *value, int* size)
{
	_ASSERT(size);
	if (!value)
	{
		*size = m_pPacket[m_wIndex];		
	}
	else if (!(*size))
	{
		int nLen = m_pPacket[m_wIndex];
		m_wIndex++;
		_ASSERT( m_wSize>m_wIndex+nLen && "PacketSize over!" );
		memcpy(value, &m_pPacket[m_wIndex], nLen);
		*size = nLen;
		m_wIndex += nLen;
	}
	else
	{
		m_wIndex++;
		memcpy(value, &m_pPacket[m_wIndex], *size);
		m_wIndex += (*size);
	}
	return *this;
}

PacketPicker & PacketPicker::MultiByte2(BYTE *value, int* size)
{
	_ASSERT(size);
	if (!value)
	{
		memcpy(size, &m_pPacket[m_wIndex], sizeof(WORD));
	}
	else if (!(*size))
	{
		WORD wLen=0;
		memcpy(&wLen, &m_pPacket[m_wIndex], sizeof(WORD));
		m_wIndex+=sizeof(WORD);
		_ASSERT( m_wSize>m_wIndex+wLen && "PacketSize over!" );
		memcpy(value, &m_pPacket[m_wIndex], wLen);
		*size = wLen;
		m_wIndex += wLen;
	}
	else
	{
		m_wIndex+=sizeof(WORD);
		memcpy(value, &m_pPacket[m_wIndex], *size);
		m_wIndex += (*size);
	}
	return *this;
}

PacketPicker & PacketPicker::Copy(BYTE *value, int size)
{
	_ASSERT( m_wSize>m_wIndex+size && "PacketSize over!" );
	memcpy(value, &m_pPacket[m_wIndex], size);
	m_wIndex+=size;
	return *this;
}

void VarPacketPicker::CheckPacketFormat()
{
	m_bCheckPacket = FALSE;
	memcpy(&m_wSize, m_pPacket, sizeof(WORD));
	if (m_wSize > MAX_PACKET_SIZE)
		return;
	WORD emk = 0;
	memcpy(&emk, &m_pPacket[m_wSize-sizeof(WORD)], sizeof(WORD));
	if (emk == PacketHelper::VarEndMarker)
		m_bCheckPacket = TRUE;
}