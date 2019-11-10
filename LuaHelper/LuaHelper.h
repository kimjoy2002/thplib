#ifndef __LUA_HELPER_H__
#define __LUA_HELPER_H__

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// �p����??�̂P�̒l���i?����N���X
class LuaFuncParamItem
{
public:
	unsigned int m_type; //�p����???�C�v�FLUA_TNIL/LUA_TNUMBER/LUA_TSTRING/LUA_TBOOLEAN
	union {
		double m_number; // ���l�̏ꍇ�̒l
		char *m_str; // ������̏ꍇ�̒l
		bool m_bool; // �u?���̏ꍇ�̒l
	};

public:
	LuaFuncParamItem() : m_type(LUA_TNIL), m_number(0) {}
	~LuaFuncParamItem() { ReleaseValue(); }

	// ?�擾�֐�
	unsigned int GetType() const { return m_type; }

	// ??�F�b�N�֐�
	bool IsNil()    const { return m_type == LUA_TNIL; }
	bool IsNumber() const { return m_type == LUA_TNUMBER; }
	bool IsString() const { return m_type == LUA_TSTRING; }
	bool IsBool()   const { return m_type == LUA_TBOOLEAN; }

	// �l�擾�֐�
	double GetNumber()        const { return m_number; }
	const char * GetString()  const { return m_str; }
	bool GetBool()            const { return m_bool; }

	const WCHAR *GetWString(WCHAR** wstr, int *count)
	{
		*count = strlen(m_str);
		*wstr = new wchar_t[*count +1]();
		if (*wstr)
		{
			MultiByteToWideChar(CP_THREAD_ACP, 0, m_str, *count, *wstr, *count);
			if (*count > 0) 
				return *wstr;
		}
		return NULL;
	};
	// �o�b�t?���
	void ReleaseValue();

	// ���l���Z�b�g
	void SetNumber(double number);
	// ������l���Z�b�g
	void SetString(const char * str);
	// nil�l���Z�b�g
	void SetNil();
	// bool�l���Z�b�g
	void SetBool(bool value);

	// �i?���Ă���l��Lua�X?�b�N�ɐς�
	void PushToStack(lua_State *L);
};

// ��?�U?���璼�ڎg���֐��p����??�N���X
// LuaFuncParam param;
// param.Number(10).String("hello").Nil().Number(50);
// �̂悤�Ɏg����B
class LuaFuncParam
{
private:
	enum {
		PARAMS_MAX = 20, // �ő�p����??��
	};
	LuaFuncParamItem m_params[PARAMS_MAX];
	int m_params_count;

public:
	LuaFuncParam() : m_params_count(0) {}
	~LuaFuncParam() { Clear(); }

	// �p����??��S�N���A
	void Clear();
	// �p����??����Ԃ�
	int GetCount() { return m_params_count; }

	// �e��p����??�̒ǉ�
	LuaFuncParam & Number(double number);
	LuaFuncParam & String(const char *str);
	LuaFuncParam & Nil();
	LuaFuncParam & Bool(bool value);

	// �w��C���f�b�N�X�̃p����??�l�擾
	// (�C���f�b�N�X�͂O�x?�X�j
	bool IsNil(int index);
	double GetNumber(int index);
	const char * GetString(int index);
	const bool GetBool(int index);
	const WCHAR * LuaFuncParam::GetWString(int index, WCHAR** wstr, int *count);

	// Lua�X?�b�N�Ɉ������v�b�V�����āA�v�b�V�����������̐���Ԃ�
	int PushToStack(lua_State *L);
	// Lua�X?�b�N����l���擾
	// �X?�b�N�g�b�v����result_count�̒l���擾���Ċi?
	void GetFromStack(lua_State *L, int result_count);
};

// Lua���g���₷�����邽�߂̃N���X
class LuaHelper
{
private:
	lua_State *m_L;		// Lua�X�e?�g
	char m_err[1000];	// �G��?������
	lua_CFunction m_pGetStackTraceFunc;	// debug.traceback�̎����ւ�?�C��?

public:
	LuaHelper() : m_L(NULL), m_pGetStackTraceFunc(NULL) {}
	~LuaHelper() { Close(); }

	// Lua�X�e?�g�����
	void Close();

	// Lua�X�e?�g���Z�b�g����
	// �����ɁAdebug.traceback�̎����ւ�?�C��?�𓾂�
	// �i���̂��߁ALua���C�u�����I?�v���オ?�܂����j
	void SetLua(lua_State *L) ;
	// Lua�X�e?�g�̎擾
	lua_State *GetLua() { return m_L; }

	// print�֐���ݒ�
	void InitPrintFunc();

#ifdef _WIN32
	// print�֐������FVC++�̃��b�Z?�W�Ƃ��ďo��
	static int LuaPrintWindows(lua_State *L);
#endif

	// ���O��CallFunc�ɂ��ẴG��?���b�Z?�W���擾����
	const char * GetErr() { return m_err; }
	// �֐��R?��
	// result,params��NULL�ł��ǂ�
	bool CallFunc(const char *funcname, LuaFuncParam* result = NULL, int result_count = 0, LuaFuncParam* params = NULL);
	// �t?�C�����s
	// result,params��NULL�ł��ǂ�
	bool DoFile(const char *path, LuaFuncParam* result = NULL, int result_count = 0, LuaFuncParam* params = NULL);

	static void PrintStack(lua_State *L);
protected:
	static void PrintStackItem(lua_State *L, int idx);
	// �G��?���b�Z?�W���Z�b�g����
	void SetErr(const char *location, const int max_str, const char *message);
	// ��??���l�ƃX?�b�N�̒l����G��?���b�Z?�W��ݒ�
	void AnalyzeError(int res_call, const char *location);
	// pcall����уX?�b�N�܂��̏������s���T�u��??��
	int CallSub(LuaFuncParam* result, int result_count, LuaFuncParam* params, int errfunc_index);
};

#endif //__LUA_HELPER_H__
