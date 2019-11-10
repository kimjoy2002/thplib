#ifndef H_CLASS_INPUT___
#define H_CLASS_INPUT___
// ===================================================================
// CInput.h
//	DirextInput�Ǘ��N���X�E�w�b??
// ===================================================================

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <dinput.h>
#include <Dxerr.h>

#ifndef	DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION	0x0800
#endif

#define DIDEVICE_BUFFERSIZE	100						// �f�o�C�X�ɐݒ肷��o�b�t?�E�T�C�Y
#define MAX_BUTTON	(9)
#define MAX_KEY_STT	(256)
#define KEY_CONT_ON	(600)
#define KEY_BTN_ON	(0x80)
#define KEY_BTN_CHANGE	(0x40)

#define MAX_INI_BUF	(256*sizeof(TCHAR))

/////////// GAME_CONTROL /////////////
#define KEY_INI_W					L"key.ini"
#define KEY_INI_A					"key.ini"
#define KEY_SECTION_W		L"KEY"
#define KEY_SECTION_A		"KEY"
#define JOY_SECTION_W		L"JOY"
#define JOY_SECTION_A		"JOY"
#define KEY_HEAD_KEY_W		L"Key"
#define KEY_HEAD_KEY_A		"Key"
#define JOY_HEAD_KEY_W		L"Joy"
#define JOY_HEAD_KEY_A			"Joy"
//#define MAX_BUTTON		9
#define MAX_KEY			4

// ���b�Z?�W�֐��̒�?
//LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

enum EInputType {
	eITNone		= 0x00,
	eITJoystick = 0x01,
	eITKeyboard = 0x02,
};

// KeyCode�ƃR���g��?���̑Ή�
struct TRelKeyCode {
	BYTE	up;
	BYTE	down;
	BYTE	left;
	BYTE	right;
	BYTE	buttons[MAX_BUTTON];
};

// Joystick�ƃR���g��?���̑Ή�
struct TRelJoystick {
	LONG	lX;
	LONG	lY;
	LONG	lZ;
	BYTE	bytBtnIndex[MAX_BUTTON];
};

// �����ꂽ���
struct TInputState {
	LONG	lX;
	LONG	lY;
	LONG	lZ;
	BYTE	bytButtons[MAX_BUTTON];
};

// Event
typedef void (*P_ENUM_JOY_STICKS)(const DIDEVICEINSTANCE* pdidInstance);
typedef void (*P_KEYDOWN)(HWND hWnd, WORD wKey);

// -------------------------------------------------------------------
// �C���v�b�g�C�N���X
// -------------------------------------------------------------------
class CInput{
public:
	CInput();
	virtual ~CInput();

	virtual BOOL	Create(HINSTANCE hInst, HWND hwndApp, BOOL bJoyPad, BOOL bKeybd, int nContOn=KEY_CONT_ON);

	virtual BOOL	Release();

	virtual BOOL	UpdateState();

	virtual BOOL	UpdateJoyState();
	virtual BOOL	UpdateKeyState();

	virtual DWORD	UpdateObjData(LPDIRECTINPUTDEVICE8 pDIDev);
	BOOL	EnumJoysticks(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	BOOL	EnumAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

	LPDIRECTINPUT8 GetDirectInput() { return pDInput; };

	BOOL	bCreated;
	DWORD	dwInType;

	virtual void	OnActive() {
		if (pDIJoyDevice)
			pDIJoyDevice->Acquire();
		if (pDIJoyDevice)
			pDIKeyDevice->Acquire();
		bActive = TRUE;
	};
	virtual void	OnInactive() { bActive = FALSE;};

	DIJOYSTATE2				diJoyState;			// �W���C�X�e�B�b�N�̏��
	DIDEVICEOBJECTDATA		diDObjData;
	BYTE					diKeyState[MAX_KEY_STT];

protected:
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

	BOOL	CreateJoystick(HWND hwndApp);
	BOOL	CreateKeyboard(HWND hwndApp);

	// DirectInput�̕ϐ�
	LPDIRECTINPUT8			pDInput;			// DirectInput
	LPDIRECTINPUTDEVICE8	pDIJoyDevice;		// DirectInput�f�o�C�X
	LPDIRECTINPUTDEVICE8	pDIKeyDevice;		// DirectInput�f�o�C�X
	DIDEVCAPS				diDevCaps;			// �W���C�X�e�B�b�N��?��

	BOOL	bActive;
	int			nContOnMin;
};

// -------------------------------------------------------------------
// �Q??�R���g��?���E�N���X
// -------------------------------------------------------------------

class CGameControl : public CInput
{
public:
	CGameControl();
	virtual ~CGameControl();

	virtual void SetKeyCode(TRelKeyCode trk);
	virtual void SetJoystick(TRelJoystick trj);
	virtual void GetRelKeyCode(TRelKeyCode* trk)
	{
		_ASSERT(trk);
		memcpy(trk, &trKeyCode, sizeof(TRelKeyCode));
	};
	virtual void GetRelJoystick(TRelJoystick* trj)
	{
		_ASSERT(trj);
		memcpy(trj, &trJoystick, sizeof(TRelJoystick));
	};

	virtual void OnLost();
	virtual void OnReset();

	virtual	BOOL Release();
	virtual BOOL UpdateState();
#ifdef _UNICODE
#define LoadFile	LoadFileW
#else
#define LoadFile LoadFileA
#endif
	virtual BOOL LoadFileW(LPTSTR fn=NULL);
	virtual BOOL LoadFileA(char* fn=NULL);
#ifdef _UNICODE
#define SaveFile	SaveFileW
#else
#define SaveFile SaveFileA
#endif
	virtual void SaveFileW(LPTSTR fn=NULL);
	virtual void SaveFileA(char* fn=NULL);

	TInputState				tInState;
	TInputState				tPastState;

	BOOL					bChanged;
	inline BOOL					IsChangeX()
	{	return tInState.lX != tPastState.lX;	};
	inline BOOL					IsChangeY()
	{	return tInState.lY != tPastState.lY;	};
	inline BOOL					IsChangeZ()
	{	return tInState.lZ != tPastState.lZ;	};
	inline BOOL					IsChangeBtn(int index)
	{	return (tInState.bytButtons[index]&KEY_BTN_ON) != (tPastState.bytButtons[index]&&KEY_BTN_ON);	};

protected:
	BOOL					IsChange(TInputState* now, TInputState* past);

	TRelKeyCode				trKeyCode;
	TRelJoystick			trJoystick;

};

#endif