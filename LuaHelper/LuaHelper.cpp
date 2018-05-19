#ifdef _WIN32
#include <windows.h>
#endif
#include <string.h>
#include "LuaHelper.h"

//
//  class LuaFuncParamItem : パラメータ１つを表すクラス
//

// バッファ解放
void LuaFuncParamItem::ReleaseValue() {
	if (m_type == LUA_TSTRING) {
		if (m_str) delete [] m_str;
		m_str = NULL;
	}
}

// 数値をセット
void LuaFuncParamItem::SetNumber(double number) {
	ReleaseValue();
	m_type = LUA_TNUMBER;
	m_number = number;
}
// 文字列値をセット
void LuaFuncParamItem::SetString(const char * str) {
	ReleaseValue();
	m_type = LUA_TSTRING;
	size_t len = strlen(str);
	m_str = new char[len+1];
	if (m_str) {
		strncpy_s(m_str, len+1, str, len+1);
//		strncpy(m_str, str, len);
		m_str[len] = '\0';
	}
}
// nil値をセット
void LuaFuncParamItem::SetNil() {
	ReleaseValue();
	m_type = LUA_TNIL;
}
// bool値をセット
void LuaFuncParamItem::SetBool(bool value) {
	ReleaseValue();
	m_type = LUA_TBOOLEAN;
	m_bool = value;
}

// 格納している値をLuaスタックに積む
void LuaFuncParamItem::PushToStack(lua_State *L) {
	switch(m_type) {
	case LUA_TNUMBER: lua_pushnumber(L, m_number); break;
	case LUA_TSTRING: lua_pushstring(L, m_str); break;
	case LUA_TNIL: lua_pushnil(L); break;
	case LUA_TBOOLEAN: lua_pushboolean(L, m_bool); break;
	}
}

//
//  class LuaFuncParam：複数のパラメータや返り値を表すクラス
//

// パラメータを全クリア
void LuaFuncParam::Clear() {
	for(int i=0 ; i<m_params_count ; i++) {
		m_params[i].ReleaseValue();
	}
	m_params_count = 0;
}

// 数値パラメータの追加
LuaFuncParam & LuaFuncParam::Number(double number) {
	m_params[m_params_count].SetNumber(number);
	m_params_count++;
	return *this;
}
// 文字列パラメータの追加
LuaFuncParam & LuaFuncParam::String(const char *str) {
	m_params[m_params_count].SetString(str);
	m_params_count++;
	return *this;
}
// nilパラメータの追加
LuaFuncParam & LuaFuncParam::Nil() {
	m_params[m_params_count].SetNil();
	m_params_count++;
	return *this;
}
// ブール値パラメータの追加
LuaFuncParam & LuaFuncParam::Bool(bool value) {
	m_params[m_params_count].SetBool(value);
	m_params_count++;
	return *this;
}

// 指定インデックスのパラメータのNULLチェック
// (インデックスは０ベース）
bool LuaFuncParam::IsNil(int index) {
	if (index < 0 || index >= m_params_count ) return true;
	return m_params[index].IsNil();
}
// 指定インデックスのパラメータ数値取得
// (インデックスは０ベース）
double LuaFuncParam::GetNumber(int index) {
	if (index < 0 || index >= m_params_count ) return 0;
	if (m_params[index].GetType() != LUA_TNUMBER) {
		return 0;
	}
	return m_params[index].GetNumber();
}
// 指定インデックスのパラメータ文字列取得
// (インデックスは０ベース）
const char * LuaFuncParam::GetString(int index) {
	if (index < 0 || index >= m_params_count ) return NULL;
	if (m_params[index].GetType() != LUA_TSTRING) {
		return NULL;
	}
	return m_params[index].GetString();
}
// 指定インデックスのパラメータ文字列取得
// (インデックスは０ベース）
const WCHAR * LuaFuncParam::GetWString(int index, WCHAR** wstr, int *count)
{
	if (index < 0 || index >= m_params_count ) return NULL;
	if (m_params[index].GetType() != LUA_TSTRING) {
		return NULL;
	}
	return m_params[index].GetWString(wstr, count);
}
// 指定インデックスのブール値取得
// (インデックスは０ベース）
const bool LuaFuncParam::GetBool(int index) {
	if (index < 0 || index >= m_params_count ) return NULL;
	if (m_params[index].GetType() != LUA_TBOOLEAN) {
		return NULL;
	}
	return m_params[index].GetBool();
}

// Luaスタックに引数をプッシュして、プッシュした引数の数を返す
int LuaFuncParam::PushToStack(lua_State *L) {
	for (int i=0 ; i<m_params_count ; i++) {
		m_params[i].PushToStack(L);
	}
	return m_params_count;
}

// Luaスタックから値を取得
// スタックトップからresult_count個の値を取得して格納
void LuaFuncParam::GetFromStack(lua_State *L, int result_count) {
	for ( int i=0 ; i<result_count ; i++) {
		int index = -result_count+i;
		int type = lua_type(L, index);
		switch(type) {
		case LUA_TNIL: this->Nil(); break;
		case LUA_TSTRING: this->String(lua_tostring(L, index)); break;
		case LUA_TNUMBER: this->Number(lua_tonumber(L, index)); break;
		case LUA_TBOOLEAN: this->Bool(lua_toboolean(L, index)? true:false); break;
		default: this->Nil(); break;
		}
	}
}

//
//  class LuaHelper：Lua関数呼び出しを簡単にするヘルパークラス
//

// Luaステートを閉じる
void LuaHelper::Close() {
	if (m_L) lua_close(m_L);
	m_L = NULL;
}

// Luaステートをセットする
// 同時に、debug.tracebackの実装へのポインタを得る
// （このため、Luaライブラリオープン後が望ましい）
void LuaHelper::SetLua(lua_State *L) {
	m_L = L;
	lua_getglobal(L, "debug");
	if (!lua_isnil(L, -1)) {
		lua_getfield(L, -1, "traceback");
		m_pGetStackTraceFunc = lua_tocfunction(L, -1);
	}
}

// print関数を設定
void LuaHelper::InitPrintFunc() {
	// Windows以外なら標準のもの(stdout出力）で良い
#ifdef _WIN32
	lua_register(m_L, "print", LuaHelper::LuaPrintWindows);
	lua_atpanic(m_L, LuaHelper::LuaPrintWindows);
#endif
}

#ifdef _WIN32
// print関数実装：VC++のメッセージとして出力
int LuaHelper::LuaPrintWindows(lua_State *L) {
	int count = lua_gettop(L);
	lua_getglobal(L, "tostring");
	for (int i=0 ; i<count ; i++) {
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i+1);
		lua_call(L, 1, 1);
		const char *str = lua_tostring(L,-1);
#ifdef _UNICODE
		int len = strlen(str);
		wchar_t *wstr = new wchar_t[len+1]();
		if (wstr)
		{
			if (MultiByteToWideChar(CP_ACP, 0, str, len, wstr, len) == len) 
			{
				OutputDebugString(wstr);
			}
			delete[] wstr;
		}
#else
		if (i != 0) OutputDebugString("\t");
#endif
		if (i != 0) OutputDebugString(TEXT("\t"));
		lua_pop(L, 1);
	}
	OutputDebugString(TEXT("\n"));
	return 0;
}
#endif

// リターン値とスタックの値からエラーメッセージを設定
void LuaHelper::AnalyzeError(int res_call, const char *location) {
	const char * reason = "";
	switch( res_call ) {
	case LUA_ERRRUN: reason = "SCRIPT RUNTIME ERROR"; break;
	case LUA_ERRSYNTAX: reason = "SCRIPT SYNTAX ERROR"; break;
	case LUA_ERRMEM: reason = "SCRIPT MEMORY ERROR"; break;
	case LUA_ERRFILE: reason = "SCRIPT FILE ERROR"; break;
	default: break;
	}
	// エラーメッセージ取得
	const char *mes = lua_tostring(m_L, -1);
	char err_mes[1000];
	sprintf_s(err_mes, 1000, "%s : %s", reason, mes);
	SetErr(location, 1000, err_mes);
}

// エラーメッセージをセットする
void LuaHelper::SetErr(const char *location, const int max_str, const char *message) {
	sprintf_s(m_err, max_str, "%s : %s", location, message);
}

// pcallおよびスタックまわりの処理を行うサブルーチン
int LuaHelper::CallSub(LuaFuncParam* result, int result_count, 
					   LuaFuncParam* params, int errfunc_index) {
	// paramsによって指定された可変個の引数をスタックに置く
	int params_count = 0;
	if (params) {
		params_count = params->PushToStack(m_L);
	}
	// ロードされたチャンクをコール
	int res_call = lua_pcall(m_L, params_count, result_count, errfunc_index);
	if (res_call != 0) {
		// エラー処理
		return res_call;
	}
	// 返り値解析
	if (result) result->GetFromStack(m_L, result_count);
	return res_call;
}

// 関数コール
// result,paramsはNULLでも良い
bool LuaHelper::CallFunc(const char *funcname, LuaFuncParam* result, 
						 int result_count, LuaFuncParam* params) {
	int old_top = lua_gettop(m_L);
	// 返り値はクリアしておく
	if (result) result->Clear();
	// ランタイムエラー処理関数を積む
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// 関数をみつける
	lua_getglobal(m_L, funcname);
	if (!lua_isfunction(m_L, -1)) {
		char location[300] = "";
		sprintf_s(location, 300, "calling function<%s>", funcname);
		SetErr(location, 300, "the function not found.");
		return false;
	}

	// pcallおよびスタック処理
	int res_call = CallSub(result, result_count, params, old_top+1);
	if (res_call != 0) {
		// エラーメッセージ生成
		char location[300] = "";
		sprintf_s(location, 300, "calling function<%s>", funcname);
		AnalyzeError(res_call, location);
	}
	lua_settop(m_L, old_top); // スタックを元に戻す
	return (res_call == 0);
}

// ファイル実行
// result,paramsはNULLでも良い
bool LuaHelper::DoFile(const char *path, LuaFuncParam* result, 
						int result_count, LuaFuncParam* params) {
	int old_top = lua_gettop(m_L);
	// 返り値はクリアしておく
	if (result) result->Clear();
	// ランタイムエラー処理関数を積む
	lua_pushcfunction(m_L, m_pGetStackTraceFunc);

	// ファイルをロード
	int res_load = luaL_loadfile(m_L, path);
	if (res_load != 0) {
		// エラーメッセージ生成
		char location[300] = "";
		sprintf_s(location, 300, "loading file<%s>", path);
		AnalyzeError(res_load, location);
		lua_settop(m_L, old_top); // スタックを元に戻す
		return false;
	}
	// pcallおよびスタック処理
	int res_call = CallSub(result, result_count, params, old_top+1);
	if (res_call != 0) {
		// エラーメッセージ生成
		char location[300] = "";
		sprintf_s(location, 300, "executing file<%s>", path);
		AnalyzeError(res_call, location);
	}
	lua_settop(m_L, old_top); // スタックを元に戻す
	return (res_call == 0);
}

	// スタックの指定インデックスのアイテムの内容を表示する
void LuaHelper::PrintStackItem(lua_State *L, int idx)
{
#ifdef _UNICODE
	WCHAR s[255];
#else
	char s[255];	
#endif
	int type = lua_type(L, idx);
	switch(type){
	case LUA_TSTRING:
		// 文字列アイテムの内容表示
#ifdef _UNICODE
		wsprintf(s,L"index %2d : type=%s : \"%s\"\n",
#else
		wsprintf(s,"index %2d : type=%s : \"%s\"\n",
#endif
		idx, lua_typename(L, type), lua_tostring(L, idx));
		OutputDebugString(s);
		break;
	case LUA_TNUMBER:
		// 数値アイテムの内容表示
#ifdef _UNICODE
		wsprintf(s,L"index %2d : type=%s : %f\n",
#else
		wsprintf(s,"index %2d : type=%s : %f\n",
#endif
		idx, lua_typename(L, type), lua_tonumber(L, idx));
		OutputDebugString(s);
		break;
	case LUA_TBOOLEAN:
		// ブール値アイテムの内容表示
#ifdef _UNICODE
		wsprintf(s,L"index %2d : type=%s : \"%s\"\n",
#else
		wsprintf(s,"index %2d : type=%s : \"%s\"\n",
#endif
		idx, lua_typename(L, type), lua_toboolean(L, idx)?"true":"false");
		OutputDebugString(s);
		break;
	default:
		// その他ならば型だけ表示
#ifdef _UNICODE
		wsprintf(s,L"index %2d : type=%s\n", idx, lua_typename(L, type));
#else
		wsprintf(s,"index %2d : type=%s\n", idx, lua_typename(L, type));
#endif
		OutputDebugString(s);
		break;
	}
}

// スタックのアイテムの内容を一覧で出力する
void LuaHelper::PrintStack(lua_State *L)
{
#ifdef _UNICODE
	OutputDebugString(L"----- stack -----\n");
#else
	OutputDebugString("----- stack -----\n");
#endif
	int top = lua_gettop(L);
	// 正のインデックスで指定
	for(int i = top; i>=1; i--){
		LuaHelper::PrintStackItem(L, i);
	}
#ifdef _UNICODE
	OutputDebugString(L"-----------------\n");
#else
	OutputDebugString("-----------------\n");
#endif
	for(int i = -1; i >= -top; i--){
		LuaHelper::PrintStackItem(L, i);
	}
#ifdef _UNICODE
	OutputDebugString(L"-----------------\n");
#else
	OutputDebugString("-----------------\n");
#endif
}
