//#include "Inc.h"
#include "CDisplay.h"

/* =================================== */
/* =====        �v���O����       ===== */
/* =================================== */
// -------------------------------------------------------------------
// CBitmap �R���X�g���N�^
// input:	CDisplay* = �f�B�X�v���C�N���X�|�C���^
//			char*     = BITMAP���\�[�X(MAKEINTRESOURCE())
//			COLORREF  = �����F(�ȗ����͍���̐F)
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
// CBitmap �f�X�g���N�^
// -------------------------------------------------------------------
CBitmap::~CBitmap()
{
	SafeDeleteObject(m_hBitmap);
	SafeDeleteObject(m_hMask);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// CDisplay �R���X�g���N�^
// input:	HINSTANCE = �C���X�^���X�n���h��
//			HWND      = �E�B���h�E�n���h��
//			int, int  = �X�N���[�� X,Y �T�C�Y
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
// CDisplay �f�X�g���N�^
// -------------------------------------------------------------------
CDisplay::~CDisplay()
{
	Delete();
}

// -------------------------------------------------------------------
// �N���C�A���g�̈�Ɠ����傫���Ń�����DC���쐬����
// input:	HWND    = �E�B���h�E�n���h��
//			int,int = X,Y �T�C�Y
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
// ������DC���폜����
// -------------------------------------------------------------------
void CDisplay::Delete()
{
	if (m_hGdiOld) SelectObject(m_hMemDC, m_hGdiOld);	// GDI�n���h�������ɖ߂�
	m_hGdiOld = NULL;
	DeleteObject(m_hBitmap);					// �������r�b�g�}�b�v�̍폜
	m_hBitmap = NULL;
	DeleteDC(m_hMemDC);							// ������DC�̍폜
	m_hMemDC  = NULL;
}

// -------------------------------------------------------------------
// �w��̃r�b�g�}�b�v����}�X�N�p��BITMAP���쐬����
// input:	HBITMAP  = BITMAP�n���h��
//			COLORREF = �}�X�N�̐F(�ȗ����͍���̐F)
// output:	HBITMAP  = �}�X�NBITMAP�n���h��
// -------------------------------------------------------------------
HBITMAP CDisplay::GetMaskHandle(HBITMAP hSrcBitmap, COLORREF color)
{
	// ���f�[�^��DC�쐬
	HDC hSrcDC = CreateCompatibleDC(m_hMemDC);			// BITMAP��Ɨp��DC�쐬
	HGDIOBJ hSrcOld = SelectObject(hSrcDC, hSrcBitmap);	// BITMAP��I��
	if (color == CLR_INVALID)							// �����A�}�X�N�F�̎w�肪�������
		color = GetPixel(hSrcDC, 0, 0);					// ����̍��W�̐F���}�X�N�Ƃ���

	// �}�X�N�pBITMAP�̍쐬
	BITMAP bm;
	GetObject(hSrcBitmap, sizeof(bm), &bm);				// BITMAP�̎擾
	HBITMAP hMaskBitmap = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);	// ���m�N��BITMAP�̍쐬

	// �}�X�N�f�[�^��DC�쐬
	HDC hDestDC = CreateCompatibleDC(m_hMemDC);				// MASK��Ɨp��DC�쐬
	HGDIOBJ hDestOld = SelectObject(hDestDC, hMaskBitmap);	// BITMAP��I��

	// �}�X�NBITMAP�̍쐬
	SetBkColor(hSrcDC, color);					// �w�i�F��ݒ�(���̐F�������c��)
	BitBlt(										// ���m�N��BITMAP�ɃR�s�[����
		hDestDC,
		0, 0, bm.bmWidth, bm.bmHeight,
		hSrcDC,
		0, 0, SRCCOPY
	);

	// �s�v��DC�̔j��
	SelectObject(hSrcDC, hSrcOld);
	SelectObject(hDestDC, hDestOld);
	DeleteDC(hSrcDC);
	DeleteDC(hDestDC);

	return hMaskBitmap;
}

// -------------------------------------------------------------------
// ���z�X�N���[��������������
// -------------------------------------------------------------------
void CDisplay::Clear()
{
	if (!IsEnable()) return;
	RECT rcFullScreen = { 0, 0, m_nScrWidth, m_nScrHeight };
	FillRect(m_hMemDC, &rcFullScreen, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

// -------------------------------------------------------------------
// ������DC��BITMAP��`�悷��
// input:	CBitmap* = �r�b�g�}�b�v���i�[�N���X�|�C���^
//			int, int = �`���X,Y���W
//			LPRECT   = �`�挳�͈�
// --------------------------------------------------------------------
bool CDisplay::Picture(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc)
{
	int nWidth  = prcSrc ? prcSrc->right  : cbm->m_nWidth;
	int nHeight = prcSrc ? prcSrc->bottom : cbm->m_nHeight;
	HDC hDC = CreateCompatibleDC(m_hMemDC);		// ��Ɨp��DC�쐬
	HGDIOBJ hOld = SelectObject(hDC, cbm->m_hBitmap);	// �r�b�g�}�b�v���w�肷��
	BitBlt(m_hMemDC,							// �`���DC
		nPosX,  nPosY,							// �\���ʒu
		nWidth, nHeight,						// �`��T�C�Y
		hDC,									// �`�挳DC
		prcSrc ? prcSrc->left : 0,				// �`�挳�ʒu
		prcSrc ? prcSrc->top  : 0,
		SRCCOPY									// �P���R�s�[
	);
	SelectObject(hDC, hOld);					// ���X�I������Ă���I�u�W�F�N�g�ɖ߂�
	DeleteObject(hDC);							// ��ƗpDC��j������
	return true;
}

// -------------------------------------------------------------------
// ������DC��BITMAP��`�悷��
// input:	CBitmap* = �r�b�g�}�b�v���i�[�N���X�|�C���^
//			int, int = �`���X,Y���W
//			LPRECT   = �`�挳�͈�(�ȗ����S��)
//			BOOL     = TRUE : �Ȃ񂿂���ĉ��Z������ ON (�ȗ��� OFF)
// --------------------------------------------------------------------
bool CDisplay::Sprite(CBitmap* cbm, int nPosX, int nPosY, LPRECT prcSrc, BOOL bAddTrans)
{
	int nWidth    = prcSrc ? prcSrc->right  : cbm->m_nWidth;
	int nHeight   = prcSrc ? prcSrc->bottom : cbm->m_nHeight;
	int nSrcPosX  = prcSrc ? prcSrc->left   : 0;
	int nSrcPosY  = prcSrc ? prcSrc->top    : 0;

	HDC hDC = CreateCompatibleDC(m_hMemDC);	// ��Ɨp��DC�쐬
	HGDIOBJ hOld;

	// �w�i�𔲂�
	if (!bAddTrans){
		hOld = SelectObject(hDC, cbm->m_hMask);	// �}�X�N�p��BITMAP���w�肷��
		BitBlt(m_hMemDC,						// �`���DC
			nPosX,  nPosY,						// �\��X,Y���W
			nWidth, nHeight,					// �\��X,Y�T�C�Y
			hDC,								// �`�挳DC
			nSrcPosX, nSrcPosY,					// �`�挳X,Y���W
			SRCAND								// �_����(AND)�`��w��
		);
		SelectObject(hDC, hOld);
	}

	// �d�ˍ��킹�`�悷��
	SelectObject(hDC, hOld);
	hOld = SelectObject(hDC, cbm->m_hBitmap);	// �\���p��BITMAP���w�肷��
	BitBlt(m_hMemDC,							// �`���DC
		nPosX,  nPosY,							// �\��X,Y���W
		nWidth, nHeight,						// �\��X,Y�T�C�Y
		hDC,									// �`�挳DC
		nSrcPosX, nSrcPosY,						// �`�挳X,Y���W
		SRCPAINT								// �_���a(OR)�`��w��
	);

	SelectObject(hDC, hOld);					// ���X�I������Ă���I�u�W�F�N�g�ɖ߂�
	DeleteObject(hDC);							// ��ƗpDC��j������
	return true;
}

// -------------------------------------------------------------------
// ���z�X�N���[�����N���C�A���g�̈�ɓ]������
// -------------------------------------------------------------------
void CDisplay::Flush(HWND hWnd)
{
	if (!IsEnable()) return;
	HDC hDC = GetDC(hWnd);
	BitBlt(
		hDC,									// �`���DC
		0, 0,									// �`��ʒu
		m_nScrWidth, m_nScrHeight,				// �`��T�C�Y
		m_hMemDC, 0, 0, SRCCOPY					// �`�挳���
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