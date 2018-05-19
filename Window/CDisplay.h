#ifndef H_CLASS_DISPLAY___
#define H_CLASS_DISPLAY___

#include "windows.h"

/* =================================== */
/* =====        �N���X��`       ===== */
/* =================================== */
// -------------------------------------------------------------------
// �f�B�X�v���C�N���X
// -------------------------------------------------------------------
class CBitmap;
class CDisplay{
friend class CBitmap;
private:

	HINSTANCE	m_hInst;	// �C���X�^���X�n���h��
	HDC			m_hMemDC;	// �������R���e�L�X�g
	HBITMAP		m_hBitmap;	// �������R���e�L�X�g�p��BITMAP�n���h��
	HGDIOBJ		m_hGdiOld;	// ����GDI�n���h��
	BOOL		m_bEnable;	// �`��\��?

	int m_nScrWidth;
	int m_nScrHeight;

	HBITMAP GetMaskHandle(HBITMAP hSrcBitmap, COLORREF color = CLR_INVALID);
	bool Create(HWND hWnd, int nXsize, int nYsize);		// �������R���e�L�X�g�̎擾
	void Delete();										// �������R���e�L�X�g�̉��

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
// �r�b�g�}�b�v�Ǘ��N���X
// -------------------------------------------------------------------
class CBitmap{
friend class CDisplay;
private:

	HBITMAP	m_hBitmap;				// �`��pBITMAP�n���h��
	HBITMAP	m_hMask;				// �}�X�N�p��BITMAP�n���h��
	int		m_nWidth, m_nHeight;	// X,Y �X�v���C�g�T�C�Y
	BOOL	m_bEnable;				// �`��\��?
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
/* =====           �՗�          ===== */
/* =================================== */

2001/01/21		���Ŋ���
2001/02/01		�O�̂��߁ASelectObject()�̖߂�l��������ƕԂ��悤�ɕύX�����B
2002/07/20		�ŐV�L�q�@�ɉ����ďC�������B

#endif
#endif
