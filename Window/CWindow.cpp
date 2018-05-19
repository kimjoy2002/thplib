//#include "Inc.h"
#include "CWindow.h"

/* =================================== */
/* =====		�v���O����		 ===== */
/* =================================== */
// -------------------------------------------------------------------
// �R���X�g���N�^
// input:	HINSTANCE = �C���X�^���X�n���h��
//			SIZE      = �E�B���h�E�̃T�C�Y
//			TCHAR*    = �N���X��
//			TCHAR*    = �L���v�V����������
//			DWORD     = �E�B���h�E�X�^�C��(�ȗ��\)
//			DWORD     = ���\�[�X�A�C�R���w��(�ȗ��\)
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

	wc.lpszClassName = m_pszClassName;						// �N���X��
	wc.lpfnWndProc	 = BeginWndProc;						// �E�B���h�E�v���V�[�W��
	wc.style		 = CS_VREDRAW | CS_HREDRAW;				// ��L�w��
	wc.hInstance	 = hInst;								// �C���X�^���X
	
	wc.hIcon		 = dwIcon
	                 ? LoadIcon(hInst, MAKEINTRESOURCE(dwIcon))
					 : LoadIcon(NULL, IDI_APPLICATION);		// �f�t�H���g�A�C�R��
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);			// �J�[�\���`��
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// �w�i�F
	wc.lpszMenuName  = NULL;								// ���j���[����
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;

	if (m_Atom = RegisterClass(&wc)){						// �N���X�̓o�^������������

		RECT rcWindow = { 0, 0, size.cx, size.cy };
		AdjustWindowRectEx(&rcWindow, dwStyle, false, NULL);
		m_hWnd = CreateWindowEx(							// �E�B���h�E�̍쐬
			0,
			m_pszClassName,									// �E�B���h�E��
			strCaption,										// �L���v�V����
			dwStyle,										// �E�B���h�E�X�^�C��
			CW_USEDEFAULT, CW_USEDEFAULT,					// �D���Ȉʒu�ɕ\��
			m_nXsize = rcWindow.right  - rcWindow.left,		// �T�C�Y
			m_nYsize = rcWindow.bottom - rcWindow.top,
			NULL, NULL, hInst, NULL							// �C���X�^���X�w��
		);
		SetProp(m_hWnd, PROP_WINPROC, this);				// �I�u�W�F�N�g�̃|�C���^��o�^����
		if (bShow) Update();								// ������悤�ɂ���
		Sleep(100);
	}
}

// -------------------------------------------------------------------
// �f�X�g���N�^
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
// [�ÓI�����o�֐�] �U�蕪���p�̃E�B���h�E�v���V�[�W��
// -------------------------------------------------------------------
LRESULT CALLBACK CWindow::BeginWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_DESTROY:										// �K����Еt������
		CWindow* win = (CWindow*)GetProp(hWnd, PROP_WINPROC);
		win->WndProc(hWnd, msg, wParam, lParam);
		RemoveProp(hWnd, PROP_WINPROC);						// �֘A�t���𖕏�����
		PostQuitMessage(0);
		return 0;
	}

	// �I�u�W�F�N�g���̃v���V�[�W�����Ăяo��
	CWindow* win = (CWindow*)GetProp(hWnd, PROP_WINPROC);	// �֘A�t����ꂽ�|�C���^�����o��
	if (!win) return DefWindowProc(hWnd, msg, wParam, lParam);	// ���s������f�t�H���g�������ďI������
	return win->WndProc(hWnd, msg, wParam, lParam)			// �I�u�W�F�N�g���̃v���V�[�W����������
	       ? DefWindowProc(hWnd, msg, wParam, lParam)		// �K�v�Ȃ�f�t�H���g����������
		   : 0;												// �s�v�Ȃ炻�̂܂܏I������
}

// -------------------------------------------------------------------
// �I�u�W�F�N�g���̃E�B���h�E�v���V�[�W��
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
	case WM_KEYDOWN:										// �����L�[��������Ă���?
		if (m_pFuncUserKeyDown) m_pFuncUserKeyDown(hWnd, wParam);
#if 0	// ESC�ŏI��
		else
			switch(wParam){
			case VK_ESCAPE:									// [ESC]�{�^����������Ă�����
				DestroyWindow(hWnd);						// �Q�[�����I������
				break;
			}
#endif
		break;
	case WM_KEYUP:										// �����L�[��������Ă���?
		if (m_pFuncUserKeyUp) m_pFuncUserKeyUp(hWnd, wParam);
#if 0	// ESC�ŏI��
		else
			switch(wParam){
			case VK_ESCAPE:									// [ESC]�{�^����������Ă�����
				DestroyWindow(hWnd);						// �Q�[�����I������
				break;
			}
#endif
		break;
	case WM_PAINT:											// �ĕ`��
		{/*
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);				// �ĕ`��J�n��ʒm
			if (m_pFuncUserRender) m_pFuncUserRender(hWnd);	// �O���`��֐�
			EndPaint(hWnd, &ps);							// �ĕ`�抮����ʒm����
			*/
			break;
		}

	case WM_ACTIVATE:		// �E�C���h�E�̃A�N�e�B�u��Ԃ��ω�
		if (m_pFuncUserActivate)
			m_pFuncUserActivate(hWnd, (LOWORD(wParam) != WA_INACTIVE));
		break;
	case WM_DESTROY:										// �E�B���h�E���j�����ꂽ��
		m_hWnd = NULL;
		return 0;
	}

	return 1;
}

// -------------------------------------------------------------------
// �ĕ`����w�肷��
// -------------------------------------------------------------------
void CWindow::Update()
{
	if (IsEnable()){
		SetWindowPos(m_hWnd, NULL, 0, 0, m_nXsize, m_nYsize, SWP_NOMOVE | SWP_SHOWWINDOW);
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);					// ������悤�ɂ���
		UpdateWindow(m_hWnd);
	}
}

// -------------------------------------------------------------------
// ���b�Z�[�W����������
// out:	BOOL = TRUE : ��������, FALSE : WM_QUIT
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

