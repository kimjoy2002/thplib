//#include "Inc.h"
#include "CDisplay.h"

/* =================================== */
/* =====        プログラム       ===== */
/* =================================== */
// -------------------------------------------------------------------
// CBitmap コンストラクタ
// input:	CDisplay* = ディスプレイクラスポインタ
//			char*     = BITMAPリソース(MAKEINTRESOURCE())
//			COLORREF  = 透明色(省略時は左上の色)
// -------------------------------------------------------------------
CBitmap::CBitmap(CDisplay* disp, char* pcBitmap, COLORREF color)
{
	m_bEnable = FALSE;

	BITMAP bm;
	m_hBitmap = LoadBitmap(disp->GetInstance(), pcBitmap);
	m_hMask   = disp->GetMaskHandle(m_hBitmap, color);
	GetObject(m_hBitmap, sizeof(bm), &bm);
	m_nWidth = bm.bmWidth;
	m_nHeight = bm.bmHeight;

	m_bEnable = TRUE;
}

// -------------------------------------------------------------------
// CBitmap デストラクタ
// -------------------------------------------------------------------
CBitmap::~CBitmap()
{
	SafeDeleteObject(m_hBitmap);
	SafeDeleteObject(m_hMask);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// CDisplay コンストラクタ
// input:	HINSTANCE = インスタンスハンドル
//			HWND      = ウィンドウハンドル
//			int, int  = スクリーン X,Y サイズ
// -------------------------------------------------------------------
CDisplay::CDisplay(HINSTANCE hInst, HWND hWnd, int nXsize, int nYsize)
{
	m_hInst   = hInst;
	m_hBitmap = NULL;
	m_hMemDC  = NULL;
	m_hGdiOld = NULL;
	Create(hWnd, nXsize, nYsize);
}

// -------------------------------------------------------------------
// CDisplay デストラクタ
// -------------------------------------------------------------------
CDisplay::~CDisplay()
{
	Delete();
}

// -------------------------------------------------------------------
// クライアント領域と同じ大きさでメモリDCを作成する
// input:	HWND    = ウィンドウハンドル
//			int,int = X,Y サイズ
// -------------------------------------------------------------------
bool CDisplay::Create(HWND hWnd, int nXsize, int nYsize)
{
	m_bEnable = FALSE;

	Delete();

	HDC hDC   = GetDC(hWnd);
	m_hMemDC  = CreateCompatibleDC(hDC);
	m_hBitmap = CreateCompatibleBitmap(hDC, nXsize, nYsize);
	m_hGdiOld = SelectObject(m_hMemDC, m_hBitmap);
	ReleaseDC(hWnd, hDC);

	m_nScrWidth = nXsize;
	m_nScrHeight = nYsize;

	m_bEnable = TRUE;
	return true;
}

// -------------------------------------------------------------------
// メモリDCを削除する
// -------------------------------------------------------------------
void CDisplay::Delete()
{
	if (m_hGdiOld) SelectObject(m_hMemDC, m_hGdiOld);	// GDIハンドルを元に戻す
	m_hGdiOld = NULL;
	DeleteObject(m_hBitmap);					// メモリビットマップの削除
	m_hBitmap = NULL;
	DeleteDC(m_hMemDC);							// メモリDCの削除
	m_hMemDC  = NULL;
}

// -------------------------------------------------------------------
// 指定のビットマップからマスク用のBITMAPを作成する
// input:	HBITMAP  = BITMAPハンドル
//			COLORREF = マスクの色(省略時は左上の色)
// output:	HBITMAP  = マスクBITMAPハンドル
// -------------------------------------------------------------------
HBITMAP CDisplay::GetMaskHandle(HBITMAP hSrcBitmap, COLORREF color)
{
	// 元データのDC作成
	HDC hSrcDC = CreateCompatibleDC(m_hMemDC);			// BITMAP作業用のDC作成
	HGDIOBJ hSrcOld = SelectObject(hSrcDC, hSrcBitmap);	// BITMAPを選択
	if (color == CLR_INVALID)							// もし、マスク色の指定が無ければ
		color = GetPixel(hSrcDC, 0, 0);					// 左上の座標の色をマスクとする

	// マスク用BITMAPの作成
	BITMAP bm;
	GetObject(hSrcBitmap, sizeof(bm), &bm);				// BITMAPの取得
	HBITMAP hMaskBitmap = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);	// モノクロBITMAPの作成

	// マスクデータのDC作成
	HDC hDestDC = CreateCompatibleDC(m_hMemDC);				// MASK作業用のDC作成
	HGDIOBJ hDestOld = SelectObject(hDestDC, hMaskBitmap);	// BITMAPを選択

	// マスクBITMAPの作成
	SetBkColor(hSrcDC, color);					// 背景色を設定(この色だけが残る)
	BitBlt(										// モノクロBITMAPにコピーする
		hDestDC,
		0, 0, bm.bmWidth, bm.bmHeight,
		hSrcDC,
		0, 0, SRCCOPY
	);

	// 不要なDCの破棄
	SelectObject(hSrcDC, hSrcOld);
	SelectObject(hDestDC, hDestOld);
	DeleteDC(hSrcDC);
	DeleteDC(hDestDC);

	return hMaskBitmap;
}

// -------------------------------------------------------------------
// 仮想スクリーンを初期化する
// -------------------------------------------------------------------
void CDisplay::Clear()
{
	if (!IsEnable()) return;
	RECT rcFullScreen = { 0, 0, m_nScrWidth, m_nScrHeight };
	FillRect(m_hMemDC, &rcFullScreen, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

// -------------------------------------------------------------------
// メモリDCにBITMAPを描画する
// input:	CBitmap* = ビットマップ情報格納クラスポインタ
//			int, int = 描画先X,Y座標
//			LPRECT   = 描画元範囲
// --------------------------------------------------------------------
bool CDisplay::Picture(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc)
{
	int nWidth  = prcSrc ? prcSrc->right  : cbm->m_nWidth;
	int nHeight = prcSrc ? prcSrc->bottom : cbm->m_nHeight;
	HDC hDC = CreateCompatibleDC(m_hMemDC);		// 作業用のDC作成
	HGDIOBJ hOld = SelectObject(hDC, cbm->m_hBitmap);	// ビットマップを指定する
	BitBlt(m_hMemDC,							// 描画先DC
		nPosX,  nPosY,							// 表示位置
		nWidth, nHeight,						// 描画サイズ
		hDC,									// 描画元DC
		prcSrc ? prcSrc->left : 0,				// 描画元位置
		prcSrc ? prcSrc->top  : 0,
		SRCCOPY									// 単純コピー
	);
	SelectObject(hDC, hOld);					// 元々選択されているオブジェクトに戻す
	DeleteObject(hDC);							// 作業用DCを破棄する
	return true;
}

// -------------------------------------------------------------------
// メモリDCにBITMAPを描画する
// input:	CBitmap* = ビットマップ情報格納クラスポインタ
//			int, int = 描画先X,Y座標
//			LPRECT   = 描画元範囲(省略時全て)
//			BOOL     = TRUE : なんちゃって加算半透明 ON (省略時 OFF)
// --------------------------------------------------------------------
bool CDisplay::Sprite(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc, BOOL bAddTrans)
{
	int nWidth    = prcSrc ? prcSrc->right  : cbm->m_nWidth;
	int nHeight   = prcSrc ? prcSrc->bottom : cbm->m_nHeight;
	int nSrcPosX  = prcSrc ? prcSrc->left   : 0;
	int nSrcPosY  = prcSrc ? prcSrc->top    : 0;

	HDC hDC = CreateCompatibleDC(m_hMemDC);	// 作業用のDC作成
	HGDIOBJ hOld;

	// 背景を抜く
	if (!bAddTrans){
		hOld = SelectObject(hDC, cbm->m_hMask);	// マスク用のBITMAPを指定する
		BitBlt(m_hMemDC,						// 描画先DC
			nPosX,  nPosY,						// 表示X,Y座標
			nWidth, nHeight,					// 表示X,Yサイズ
			hDC,								// 描画元DC
			nSrcPosX, nSrcPosY,					// 描画元X,Y座標
			SRCAND								// 論理積(AND)描画指定
		);
		SelectObject(hDC, hOld);
	}

	// 重ね合わせ描画する
	SelectObject(hDC, hOld);
	hOld = SelectObject(hDC, cbm->m_hBitmap);	// 表示用のBITMAPを指定する
	BitBlt(m_hMemDC,							// 描画先DC
		nPosX,  nPosY,							// 表示X,Y座標
		nWidth, nHeight,						// 表示X,Yサイズ
		hDC,									// 描画元DC
		nSrcPosX, nSrcPosY,						// 描画元X,Y座標
		SRCPAINT								// 論理和(OR)描画指定
	);

	SelectObject(hDC, hOld);					// 元々選択されているオブジェクトに戻す
	DeleteObject(hDC);							// 作業用DCを破棄する
	return true;
}

// -------------------------------------------------------------------
// 仮想スクリーンをクライアント領域に転送する
// -------------------------------------------------------------------
void CDisplay::Flush(HWND hWnd)
{
	if (!IsEnable()) return;
	HDC hDC = GetDC(hWnd);
	BitBlt(
		hDC,									// 描画先DC
		0, 0,									// 描画位置
		m_nScrWidth, m_nScrHeight,				// 描画サイズ
		m_hMemDC, 0, 0, SRCCOPY					// 描画元情報
	);
	ReleaseDC(hWnd, hDC);
}

void CDisplay::SetPxl(HWND hWnd, int nPosX, int nPosY, int r, int g, int b)
{
	if (!IsEnable()) return;
	HDC hDC = GetDC(hWnd);
	SetPixel(hDC,
			nPosX,
			nPosY,
			RGB(r,g,b));
	ReleaseDC(hWnd, hDC);

}