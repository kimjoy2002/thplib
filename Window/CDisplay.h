#ifndef H_CLASS_DISPLAY___
#define H_CLASS_DISPLAY___

#include "windows.h"

/* =================================== */
/* =====        クラス定義       ===== */
/* =================================== */
// -------------------------------------------------------------------
// ディスプレイクラス
// -------------------------------------------------------------------
class CBitmap;
class CDisplay{
friend class CBitmap;
private:

	HINSTANCE	m_hInst;	// インスタンスハンドル
	HDC			m_hMemDC;	// メモリコンテキスト
	HBITMAP		m_hBitmap;	// メモリコンテキスト用のBITMAPハンドル
	HGDIOBJ		m_hGdiOld;	// 元のGDIハンドル
	BOOL		m_bEnable;	// 描画可能か?

	int m_nScrWidth;
	int m_nScrHeight;

	HBITMAP GetMaskHandle(HBITMAP hSrcBitmap, COLORREF color = CLR_INVALID);
	bool Create(HWND hWnd, int nXsize, int nYsize);		// メモリコンテキストの取得
	void Delete();										// メモリコンテキストの解放

	HINSTANCE GetInstance(){ return m_hInst; };

public:

	CDisplay(HINSTANCE hInst, HWND hWnd, int nXsize, int nYsize);
	virtual ~CDisplay();
	void Clear();
	bool Picture(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc = NULL);
	bool Sprite(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc = NULL, BOOL bAddTrans = FALSE);
	BOOL IsEnable(){ return m_bEnable; }
	void Flush(HWND hWnd);
	
	void SetPxl(HWND hWnd, int nPosX, int nPosY, int r, int g, int b);
};

// -------------------------------------------------------------------
// ビットマップ管理クラス
// -------------------------------------------------------------------
class CBitmap{
friend class CDisplay;
private:

	HBITMAP	m_hBitmap;				// 描画用BITMAPハンドル
	HBITMAP	m_hMask;				// マスク用のBITMAPハンドル
	int		m_nWidth, m_nHeight;	// X,Y スプライトサイズ
	BOOL	m_bEnable;				// 描画可能か?
	template <class Type> void SafeDeleteObject(Type& obj){
		if (obj){
			DeleteObject(obj);
			obj = NULL;
		}
	}

public:

	CBitmap(CDisplay* disp, char* pcBitmap, COLORREF color = CLR_INVALID);
	virtual ~CBitmap();
	BOOL IsEnable(){ return m_bEnable; }
};

#if 0
/* =================================== */
/* =====           遍歴          ===== */
/* =================================== */

2001/01/21		初版完成
2001/02/01		念のため、SelectObject()の戻り値をきちんと返すように変更した。
2002/07/20		最新記述法に応じて修正した。

#endif
#endif
