//#include "Inc.h"
#include "CWindow.h"

/* =================================== */
/* =====		プログラム		 ===== */
/* =================================== */
// -------------------------------------------------------------------
// コンストラクタ
// input:	HINSTANCE = インスタンスハンドル
//			SIZE      = ウィンドウのサイズ
//			TCHAR*    = クラス名
//			TCHAR*    = キャプション文字列
//			DWORD     = ウィンドウスタイル(省略可能)
//			DWORD     = リソースアイコン指定(省略可能)
// -------------------------------------------------------------------
CWindow::CWindow(HINSTANCE hInst, SIZE size, TCHAR* strClass, TCHAR* strCaption, DWORD dwStyle, bool bShow, DWORD dwIcon)
{
	m_hWnd             = NULL;
	m_Atom             = NULL;
	m_pFuncUserRender  = NULL;
	m_pFuncUserKeyDown = NULL;
	m_pFuncUserKeyUp = NULL;
	m_pFuncUserActivate= NULL;
	m_pFuncUserMouseLBDown = NULL;
	m_pFuncUserMouseRBDown = NULL;
	m_pFuncUserMouseMove = NULL;
	m_pszClassName     = NULL;
	m_hInstance        = hInst;
	WNDCLASS wc;
	
	m_pszClassName = new TCHAR[_tcslen(strClass) + 8 + 1];
	if (!m_pszClassName) return;

	SafePrintf(m_pszClassName, sizeof(m_pszClassName), _T("%s%08X"), strClass, timeGetTime());
//	wsprintf(m_pszClassName, "%s%08X", strClass, timeGetTime());

	wc.lpszClassName = m_pszClassName;						// クラス名
	wc.lpfnWndProc	 = BeginWndProc;						// ウィンドウプロシージャ
	wc.style		 = CS_VREDRAW | CS_HREDRAW;				// 占有指定
	wc.hInstance	 = hInst;								// インスタンス
	
	wc.hIcon		 = dwIcon
	                 ? LoadIcon(hInst, MAKEINTRESOURCE(dwIcon))
					 : LoadIcon(NULL, IDI_APPLICATION);		// デフォルトアイコン
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);			// カーソル形状
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// 背景色
	wc.lpszMenuName  = NULL;								// メニュー無し
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;

	if (m_Atom = RegisterClass(&wc)){						// クラスの登録が成功したら

		RECT rcWindow = { 0, 0, size.cx, size.cy };
		AdjustWindowRectEx(&rcWindow, dwStyle, false, NULL);
		m_hWnd = CreateWindowEx(							// ウィンドウの作成
			0,
			m_pszClassName,									// ウィンドウ名
			strCaption,										// キャプション
			dwStyle,										// ウィンドウスタイル
			CW_USEDEFAULT, CW_USEDEFAULT,					// 好きな位置に表示
			m_nXsize = rcWindow.right  - rcWindow.left,		// サイズ
			m_nYsize = rcWindow.bottom - rcWindow.top,
			NULL, NULL, hInst, NULL							// インスタンス指定
		);
		SetProp(m_hWnd, PROP_WINPROC, this);				// オブジェクトのポインタを登録する
		if (bShow) Update();								// 見えるようにする
		Sleep(100);
	}
}

// -------------------------------------------------------------------
// デストラクタ
// -------------------------------------------------------------------
CWindow::~CWindow()
{
	if (m_hWnd) DestroyWindow(m_hWnd);
	if (m_Atom) UnregisterClass(m_pszClassName, m_hInstance);
	if (m_pszClassName){
		delete[] m_pszClassName;
		m_pszClassName = NULL;
	}
	InvalidateRect(NULL, NULL, false);
}

// -------------------------------------------------------------------
// [静的メンバ関数] 振り分け用のウィンドウプロシージャ
// -------------------------------------------------------------------
LRESULT CALLBACK CWindow::BeginWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_DESTROY:										// 必ず後片付けする
		CWindow* win = (CWindow*)GetProp(hWnd, PROP_WINPROC);
		win->WndProc(hWnd, msg, wParam, lParam);
		RemoveProp(hWnd, PROP_WINPROC);						// 関連付けを抹消する
		PostQuitMessage(0);
		return 0;
	}

	// オブジェクト毎のプロシージャを呼び出す
	CWindow* win = (CWindow*)GetProp(hWnd, PROP_WINPROC);	// 関連付けられたポインタを取り出す
	if (!win) return DefWindowProc(hWnd, msg, wParam, lParam);	// 失敗したらデフォルト処理して終了する
	return win->WndProc(hWnd, msg, wParam, lParam)			// オブジェクト毎のプロシージャ処理して
	       ? DefWindowProc(hWnd, msg, wParam, lParam)		// 必要ならデフォルト処理もする
		   : 0;												// 不要ならそのまま終了する
}

// -------------------------------------------------------------------
// オブジェクト毎のウィンドウプロシージャ
// -------------------------------------------------------------------
LRESULT CALLBACK CWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_MOUSEMOVE:
		if (m_pFuncUserMouseMove) m_pFuncUserMouseMove(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		if (m_pFuncUserMouseLBDown)	m_pFuncUserMouseLBDown(hWnd);
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONDOWN:
		if (m_pFuncUserMouseRBDown) m_pFuncUserMouseRBDown(hWnd);
		break;
	case WM_RBUTTONUP:
		break;
	case WM_KEYDOWN:										// 何かキーを押されている?
		if (m_pFuncUserKeyDown) m_pFuncUserKeyDown(hWnd, wParam);
#if 0	// ESCで終了
		else
			switch(wParam){
			case VK_ESCAPE:									// [ESC]ボタンが押されていたら
				DestroyWindow(hWnd);						// ゲームを終了する
				break;
			}
#endif
		break;
	case WM_KEYUP:										// 何かキーを押されている?
		if (m_pFuncUserKeyUp) m_pFuncUserKeyUp(hWnd, wParam);
#if 0	// ESCで終了
		else
			switch(wParam){
			case VK_ESCAPE:									// [ESC]ボタンが押されていたら
				DestroyWindow(hWnd);						// ゲームを終了する
				break;
			}
#endif
		break;
	case WM_PAINT:											// 再描画
		{/*
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);				// 再描画開始を通知
			if (m_pFuncUserRender) m_pFuncUserRender(hWnd);	// 外部描画関数
			EndPaint(hWnd, &ps);							// 再描画完了を通知する
			*/
			break;
		}

	case WM_ACTIVATE:		// ウインドウのアクティブ状態が変化
		if (m_pFuncUserActivate)
			m_pFuncUserActivate(hWnd, (LOWORD(wParam) != WA_INACTIVE));
		break;
	case WM_DESTROY:										// ウィンドウが破棄されたら
		m_hWnd = NULL;
		return 0;
	}

	return 1;
}

// -------------------------------------------------------------------
// 再描画を指定する
// -------------------------------------------------------------------
void CWindow::Update()
{
	if (IsEnable()){
		SetWindowPos(m_hWnd, NULL, 0, 0, m_nXsize, m_nYsize, SWP_NOMOVE | SWP_SHOWWINDOW);
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);					// 見えるようにする
		UpdateWindow(m_hWnd);
	}
}

// -------------------------------------------------------------------
// メッセージを処理する
// out:	BOOL = TRUE : 処理成功, FALSE : WM_QUIT
// -------------------------------------------------------------------
BOOL CWindow::Msg()
{
	MSG msg;
	if (!IsEnable()) return FALSE;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		if (msg.message == WM_QUIT) return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return TRUE;
}

