#ifndef H_CLASS_WINDOW___
#define H_CLASS_WINDOW___

#include <windows.h>

#include "util.h"
/* =================================== */
/* =====		 定数宣言		 ===== */
/* =================================== */

#define	STYLE_WINDOW	(WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER)
#define PROP_WINPROC	_T("PropClassWindowProc")		// プロパティを識別する文字列

/* =================================== */
/* =====   外部参照関数の型宣言  ===== */
/* =================================== */

typedef void (*P_RENDER)(HWND hWnd);
typedef void (*P_KEYDOWN)(HWND hWnd, WORD wKey);
typedef void (*P_KEYUP)(HWND hWnd, WORD wKey);
typedef void (*P_MOUSE_LBDOWN)(HWND hWnd);
typedef void (*P_MOUSE_RBDOWN)(HWND hWnd);
typedef void (*P_ACTIVATE)(HWND hWnd, BOOL active);
typedef void (*P_MOUSE_MOVE)(HWND hWnd, WPARAM wParam, LPARAM lParam);
/* =================================== */
/* =====		 クラス定義		 ===== */
/* =================================== */

class CWindow {
private:

	HINSTANCE	m_hInstance;								// インスタンスハンドル
	HWND		m_hWnd;										// ウィンドウハンドル
	ATOM		m_Atom;										// ウィンドウクラス登録時のATOM
	TCHAR*		m_pszClassName;								// クラス名

	LONG		m_nXsize, m_nYsize;							// ウィンドウサイズ

	P_RENDER	m_pFuncUserRender;							// クラス外部の描画関数ポインタ
	P_KEYDOWN	m_pFuncUserKeyDown;							// クラス外部のキー情報取得関数ポインタ
	P_KEYUP		m_pFuncUserKeyUp;
	P_MOUSE_LBDOWN m_pFuncUserMouseLBDown;
	P_MOUSE_RBDOWN m_pFuncUserMouseRBDown;
	P_ACTIVATE	m_pFuncUserActivate;
	P_MOUSE_MOVE m_pFuncUserMouseMove;

	// オブジェクト毎のプロシージャ
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// [静的メンバ] オブジェクト毎に処理を振り分けるための静的プロシージャ
	static LRESULT CALLBACK BeginWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	CWindow(
		HINSTANCE hInst,
		SIZE size,
		TCHAR* strClass,
		TCHAR* strCaption = NULL,
		DWORD dwStyle = STYLE_WINDOW,
		bool bShow = true,
		DWORD dwIcon = NULL
	);
	virtual ~CWindow();

	void Update();
	BOOL Msg();

	BOOL IsActive(){ return IsEnable(); }
	BOOL IsEnable(){ return (BOOL)m_hWnd; }
	HWND GetHWnd(){ return m_hWnd; }									// ウィンドウハンドルを取得する

	void SetUserRender(P_RENDER pFunc){   m_pFuncUserRender  = pFunc; }	// WM_PAINTの外部関数
	void SetUserKeyDown(P_KEYDOWN pFunc){ m_pFuncUserKeyDown = pFunc; }	// WM_KEUDOWNの外部関数
	void SetUserKeyUp(P_KEYUP pFunc){ m_pFuncUserKeyUp = pFunc; }	// WM_KEUDOWNの外部関数
	void SetUserMouseLBDown(P_MOUSE_LBDOWN pFunc){ m_pFuncUserMouseLBDown = pFunc; }
	void SetUserMouseRBDown(P_MOUSE_RBDOWN pFunc){ m_pFuncUserMouseRBDown = pFunc; }
	void SetUserActivate(P_ACTIVATE pFunc){ m_pFuncUserActivate = pFunc; }
	void SetUserMouseMove(P_MOUSE_MOVE pFunc){ m_pFuncUserMouseMove = pFunc; }
};

#endif