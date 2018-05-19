#ifndef __LUA_HELPER_H__
#define __LUA_HELPER_H__

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// パラメータの１つの値を格納するクラス
class LuaFuncParamItem
{
public:
	unsigned int m_type; //パラメータタイプ：LUA_TNIL/LUA_TNUMBER/LUA_TSTRING/LUA_TBOOLEAN
	union {
		double m_number; // 数値の場合の値
		char *m_str; // 文字列の場合の値
		bool m_bool; // ブールの場合の値
	};

public:
	LuaFuncParamItem() : m_type(LUA_TNIL), m_number(0) {}
	~LuaFuncParamItem() { ReleaseValue(); }

	// 型取得関数
	unsigned int GetType() const { return m_type; }

	// 型チェック関数
	bool IsNil()    const { return m_type == LUA_TNIL; }
	bool IsNumber() const { return m_type == LUA_TNUMBER; }
	bool IsString() const { return m_type == LUA_TSTRING; }
	bool IsBool()   const { return m_type == LUA_TBOOLEAN; }

	// 値取得関数
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
	// バッファ解放
	void ReleaseValue();

	// 数値をセット
	void SetNumber(double number);
	// 文字列値をセット
	void SetString(const char * str);
	// nil値をセット
	void SetNil();
	// bool値をセット
	void SetBool(bool value);

	// 格納している値をLuaスタックに積む
	void PushToStack(lua_State *L);
};

// ユーザーから直接使う関数パラメータクラス
// LuaFuncParam param;
// param.Number(10).String("hello").Nil().Number(50);
// のように使える。
class LuaFuncParam
{
private:
	enum {
		PARAMS_MAX = 20, // 最大パラメータ数
	};
	LuaFuncParamItem m_params[PARAMS_MAX];
	int m_params_count;

public:
	LuaFuncParam() : m_params_count(0) {}
	~LuaFuncParam() { Clear(); }

	// パラメータを全クリア
	void Clear();
	// パラメータ数を返す
	int GetCount() { return m_params_count; }

	// 各種パラメータの追加
	LuaFuncParam & Number(double number);
	LuaFuncParam & String(const char *str);
	LuaFuncParam & Nil();
	LuaFuncParam & Bool(bool value);

	// 指定インデックスのパラメータ値取得
	// (インデックスは０ベース）
	bool IsNil(int index);
	double GetNumber(int index);
	const char * GetString(int index);
	const bool GetBool(int index);
	const WCHAR * LuaFuncParam::GetWString(int index, WCHAR** wstr, int *count);

	// Luaスタックに引数をプッシュして、プッシュした引数の数を返す
	int PushToStack(lua_State *L);
	// Luaスタックから値を取得
	// スタックトップからresult_count個の値を取得して格納
	void GetFromStack(lua_State *L, int result_count);
};

// Luaを使いやすくするためのクラス
class LuaHelper
{
private:
	lua_State *m_L;		// Luaステート
	char m_err[1000];	// エラー文字列
	lua_CFunction m_pGetStackTraceFunc;	// debug.tracebackの実装へのポインタ

public:
	LuaHelper() : m_L(NULL), m_pGetStackTraceFunc(NULL) {}
	~LuaHelper() { Close(); }

	// Luaステートを閉じる
	void Close();

	// Luaステートをセットする
	// 同時に、debug.tracebackの実装へのポインタを得る
	// （このため、Luaライブラリオープン後が望ましい）
	void SetLua(lua_State *L) ;
	// Luaステートの取得
	lua_State *GetLua() { return m_L; }

	// print関数を設定
	void InitPrintFunc();

#ifdef _WIN32
	// print関数実装：VC++のメッセージとして出力
	static int LuaPrintWindows(lua_State *L);
#endif

	// 直前のCallFuncについてのエラーメッセージを取得する
	const char * GetErr() { return m_err; }
	// 関数コール
	// result,paramsはNULLでも良い
	bool CallFunc(const char *funcname, LuaFuncParam* result = NULL, int result_count = 0, LuaFuncParam* params = NULL);
	// ファイル実行
	// result,paramsはNULLでも良い
	bool DoFile(const char *path, LuaFuncParam* result = NULL, int result_count = 0, LuaFuncParam* params = NULL);

	static void PrintStack(lua_State *L);
protected:
	static void PrintStackItem(lua_State *L, int idx);
	// エラーメッセージをセットする
	void SetErr(const char *location, const int max_str, const char *message);
	// リターン値とスタックの値からエラーメッセージを設定
	void AnalyzeError(int res_call, const char *location);
	// pcallおよびスタックまわりの処理を行うサブルーチン
	int CallSub(LuaFuncParam* result, int result_count, LuaFuncParam* params, int errfunc_index);
};

#endif //__LUA_HELPER_H__
