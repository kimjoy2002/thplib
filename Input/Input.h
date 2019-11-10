#ifndef H_CLASS_INPUT___
#define H_CLASS_INPUT___
// ===================================================================
// CInput.h
//	DirextInput管理クラス・ヘッ??
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

#define DIDEVICE_BUFFERSIZE	100						// デバイスに設定するバッフ?・サイズ
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

// メッセ?ジ関数の定?
//LRESULT CALLBACK MainWndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);

enum EInputType {
	eITNone		= 0x00,
	eITJoystick = 0x01,
	eITKeyboard = 0x02,
};

// KeyCodeとコントロ?ルの対応
struct TRelKeyCode {
	BYTE	up;
	BYTE	down;
	BYTE	left;
	BYTE	right;
	BYTE	buttons[MAX_BUTTON];
};

// Joystickとコントロ?ルの対応
struct TRelJoystick {
	LONG	lX;
	LONG	lY;
	LONG	lZ;
	BYTE	bytBtnIndex[MAX_BUTTON];
};

// 押された状態
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
// インプットイクラス
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

	DIJOYSTATE2				diJoyState;			// ジョイスティックの状態
	DIDEVICEOBJECTDATA		diDObjData;
	BYTE					diKeyState[MAX_KEY_STT];

protected:
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

	BOOL	CreateJoystick(HWND hwndApp);
	BOOL	CreateKeyboard(HWND hwndApp);

	// DirectInputの変数
	LPDIRECTINPUT8			pDInput;			// DirectInput
	LPDIRECTINPUTDEVICE8	pDIJoyDevice;		// DirectInputデバイス
	LPDIRECTINPUTDEVICE8	pDIKeyDevice;		// DirectInputデバイス
	DIDEVCAPS				diDevCaps;			// ジョイスティックの?力

	BOOL	bActive;
	int			nContOnMin;
};

// -------------------------------------------------------------------
// ゲ??コントロ?ル・クラス
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