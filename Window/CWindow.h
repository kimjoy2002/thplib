#ifndef H_CLASS_WINDOW___
#define H_CLASS_WINDOW___

#include <windows.h>

#include "util.h"
/* =================================== */
/* =====		 �萔�錾		 ===== */
/* =================================== */

#define	STYLE_WINDOW	(WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER)
#define PROP_WINPROC	_T("PropClassWindowProc")		// �v���p�e�B�����ʂ��镶����

/* =================================== */
/* =====   �O���Q�Ɗ֐��̌^�錾  ===== */
/* =================================== */

typedef void (*P_RENDER)(HWND hWnd);
typedef void (*P_KEYDOWN)(HWND hWnd, WORD wKey);
typedef void (*P_KEYUP)(HWND hWnd, WORD wKey);
typedef void (*P_MOUSE_LBDOWN)(HWND hWnd);
typedef void (*P_MOUSE_RBDOWN)(HWND hWnd);
typedef void (*P_ACTIVATE)(HWND hWnd, BOOL active);
typedef void (*P_MOUSE_MOVE)(HWND hWnd, WPARAM wParam, LPARAM lParam);
/* =================================== */
/* =====		 �N���X��`		 ===== */
/* =================================== */

class CWindow {
private:

	HINSTANCE	m_hInstance;								// �C���X�^���X�n���h��
	HWND		m_hWnd;										// �E�B���h�E�n���h��
	ATOM		m_Atom;										// �E�B���h�E�N���X�o�^����ATOM
	TCHAR*		m_pszClassName;								// �N���X��

	LONG		m_nXsize, m_nYsize;							// �E�B���h�E�T�C�Y

	P_RENDER	m_pFuncUserRender;							// �N���X�O���̕`��֐��|�C���^
	P_KEYDOWN	m_pFuncUserKeyDown;							// �N���X�O���̃L�[���擾�֐��|�C���^
	P_KEYUP		m_pFuncUserKeyUp;
	P_MOUSE_LBDOWN m_pFuncUserMouseLBDown;
	P_MOUSE_RBDOWN m_pFuncUserMouseRBDown;
	P_ACTIVATE	m_pFuncUserActivate;
	P_MOUSE_MOVE m_pFuncUserMouseMove;

	// �I�u�W�F�N�g���̃v���V�[�W��
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// [�ÓI�����o] �I�u�W�F�N�g���ɏ�����U�蕪���邽�߂̐ÓI�v���V�[�W��
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
	HWND GetHWnd(){ return m_hWnd; }									// �E�B���h�E�n���h�����擾����

	void SetUserRender(P_RENDER pFunc){   m_pFuncUserRender  = pFunc; }	// WM_PAINT�̊O���֐�
	void SetUserKeyDown(P_KEYDOWN pFunc){ m_pFuncUserKeyDown = pFunc; }	// WM_KEUDOWN�̊O���֐�
	void SetUserKeyUp(P_KEYUP pFunc){ m_pFuncUserKeyUp = pFunc; }	// WM_KEUDOWN�̊O���֐�
	void SetUserMouseLBDown(P_MOUSE_LBDOWN pFunc){ m_pFuncUserMouseLBDown = pFunc; }
	void SetUserMouseRBDown(P_MOUSE_RBDOWN pFunc){ m_pFuncUserMouseRBDown = pFunc; }
	void SetUserActivate(P_ACTIVATE pFunc){ m_pFuncUserActivate = pFunc; }
	void SetUserMouseMove(P_MOUSE_MOVE pFunc){ m_pFuncUserMouseMove = pFunc; }
};

#endif