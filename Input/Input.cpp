#include "Input.h"

// -------------------------------------------------------------------
// �C���v�b�g�C�N���X
// -------------------------------------------------------------------
CInput::CInput()
{
	pDInput = NULL;
	pDIJoyDevice = NULL;
	pDIKeyDevice = NULL;
	bCreated = FALSE;
	dwInType = eITNone;

	ZeroMemory(&diJoyState, sizeof(DIJOYSTATE2));
	ZeroMemory(&diDObjData, sizeof(DIDEVICEOBJECTDATA));
	
}

CInput::~CInput()
{
	Release();
}

/*static*/BOOL CInput::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	return (((CInput*)pContext)->EnumJoysticks(pdidInstance, pContext));
}

/*static*/BOOL CInput::EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	return (((CInput*)pvRef)->EnumAxes(lpddoi, pvRef));
}

BOOL CInput::Create(HINSTANCE hInst, HWND hwndApp, BOOL bJoyPad, BOOL bKeybd, int nContOn)
{
	HRESULT hr;

	if (bCreated) return FALSE;
	nContOnMin = nContOn;
	// *****************************************
	// DirectInput�̍쐬

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&pDInput, NULL ) ) )
	{
		DXTRACE_ERR(_T("DirectInput8�I�u�W�F�N�g�̍쐬�Ɏ��s"), hr);
		return FALSE;
	}

	// �eInput�쐬
	BOOL bRes = FALSE;
	if (bJoyPad)
	{
		bRes = CreateJoystick(hwndApp);
		if (bRes)
		{
			//���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
			hr = pDIJoyDevice->SetCooperativeLevel(hwndApp, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�t�H�A�O���E���h����r�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �R�[���o�b�N�֐����g���Ċe���̃��[�h��ݒ�
			hr = pDIJoyDevice->EnumObjects(EnumAxesCallback, this, DIDFT_AXIS);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �����[�h��ݒ�i��Βl���[�h�ɐݒ�B�f�t�H���g�Ȃ̂ŕK�������ݒ�͕K�v�Ȃ��j
			DIPROPDWORD diprop;
			diprop.diph.dwSize	= sizeof(diprop); 
			diprop.diph.dwHeaderSize	= sizeof(diprop.diph); 
			diprop.diph.dwObj	= 0;
			diprop.diph.dwHow	= DIPH_DEVICE;
			diprop.dwData		= DIPROPAXISMODE_ABS;
//	diprop.dwData		= DIPROPAXISMODE_REL;	// ���Βl���[�h�̏ꍇ
			hr = pDIJoyDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �o�b�t�@�����O�E�f�[�^���擾���邽�߁A�o�b�t�@�E�T�C�Y��ݒ�
			diprop.dwData = DIDEVICE_BUFFERSIZE;
			hr = pDIJoyDevice->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�o�b�t�@�E�T�C�Y�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// ���͐���J�n
			hr = pDIJoyDevice->Acquire();
			while (hr==DIERR_INPUTLOST)
				hr = pDIJoyDevice->Acquire();

			dwInType |= eITJoystick;
		}
	}
	if (bKeybd)
	{
		bRes = CreateKeyboard(hwndApp);
		if (bRes)
		{
			//���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
			hr = pDIKeyDevice->SetCooperativeLevel(hwndApp, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�t�H�A�O���E���h����r�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �R�[���o�b�N�֐����g���Ċe���̃��[�h��ݒ�
			hr = pDIKeyDevice->EnumObjects(EnumAxesCallback, this, DIDFT_AXIS);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �����[�h��ݒ�i��Βl���[�h�ɐݒ�B�f�t�H���g�Ȃ̂ŕK�������ݒ�͕K�v�Ȃ��j
			DIPROPDWORD diprop;
			diprop.diph.dwSize	= sizeof(diprop); 
			diprop.diph.dwHeaderSize	= sizeof(diprop.diph); 
			diprop.diph.dwObj	= 0;
			diprop.diph.dwHow	= DIPH_DEVICE;
			diprop.dwData		= DIPROPAXISMODE_ABS;
//	diprop.dwData		= DIPROPAXISMODE_REL;	// ���Βl���[�h�̏ꍇ
			hr = pDIKeyDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�����[�h�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// �o�b�t�@�����O�E�f�[�^���擾���邽�߁A�o�b�t�@�E�T�C�Y��ݒ�
			diprop.dwData = DIDEVICE_BUFFERSIZE;
			hr = pDIKeyDevice->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph);
			if (FAILED(hr))
			{
//		DXTRACE_ERR("�o�b�t�@�E�T�C�Y�̐ݒ�Ɏ��s", hr);
				return false;
			}

			// ���͐���J�n
			hr = pDIKeyDevice->Acquire();
			while (hr==DIERR_INPUTLOST)
				hr = pDIKeyDevice->Acquire();
			dwInType |= eITKeyboard;
		}
	}
	
	if (!dwInType)
		return FALSE;

	bCreated = bRes;
	return bRes;
}

// -CreateJoystick- //
BOOL CInput::CreateJoystick(HWND hwndApp)
{
	HRESULT hr;
	// *****************************************
	// �W���C�X�e�B�b�N�̍쐬

	// �f�o�C�X��񋓂��č쐬
	hr = pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
							this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr) || pDIJoyDevice==NULL)
	{
//		DXTRACE_ERR("DirectInputDevice8�I�u�W�F�N�g�̍쐬�Ɏ��s", hr);
		return false;
	}

	// �f�[�^�`����ݒ�
	hr = pDIJoyDevice->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr))
	{
//		DXTRACE_ERR("c_dfDIJoystick2�`���̐ݒ�Ɏ��s", hr);
		return false;
	}


	return TRUE;
}

// -CreateKeyboard- //
BOOL CInput::CreateKeyboard(HWND hwndApp)
{
	HRESULT hr;
	// *****************************************
	// �L�[�{�[�h�̍쐬

	// �f�o�C�X�E�I�u�W�F�N�g���쐬
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIKeyDevice, NULL); 
	if (FAILED(hr)) {
//		DXTRACE_ERR("DirectInputDevice8�I�u�W�F�N�g�̍쐬�Ɏ��s", hr);
	    return false;
	}

	// �f�[�^�`����ݒ�
	hr = pDIKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
//		DXTRACE_ERR("c_dfDIMouse2�`���̐ݒ�Ɏ��s", hr);
		return false;
	}

	return TRUE;
}

// �W���C�X�e�B�b�N��񋓂���֐�
BOOL CInput::EnumJoysticks(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	HRESULT hr;
	CInput*	pInput = (CInput*)pContext;
	LPDIRECTINPUT8 pDirectInput = pInput->GetDirectInput();
	// �񋓂��ꂽ�W���C�X�e�B�b�N�ւ̃C���^�[�t�F�C�X���擾����B
	hr = pDirectInput->CreateDevice(pdidInstance->guidInstance, &pDIJoyDevice, NULL);
	if (FAILED(hr)) 
		return DIENUM_CONTINUE;

	// �W���C�X�e�B�b�N�̔\�͂𒲂ׂ�
	diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hr = pDIJoyDevice->GetCapabilities(&diDevCaps);
	if (FAILED(hr))
	{
		// �W���C�X�e�B�b�N�\�͂̎擾�Ɏ��s
		SafeRelease(pDIJoyDevice);
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}
// �W���C�X�e�B�b�N�̎���񋓂���֐�
BOOL CInput::EnumAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	HRESULT hr;

	// ���̒l�͈̔͂�ݒ�i-1000�`1000�j
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize	= sizeof(diprg); 
	diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
	diprg.diph.dwObj	= lpddoi->dwType;
	diprg.diph.dwHow	= DIPH_BYID;
	diprg.lMin	= -1000;
	diprg.lMax	= +1000;
	hr = pDIJoyDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
		return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

// �j��
BOOL CInput::Release()
{
	// DirectInput�̃f�o�C�X�����
	if (pDIKeyDevice)
		pDIKeyDevice->Unacquire(); 
	SafeRelease(pDIKeyDevice);
	if (pDIJoyDevice)
		pDIJoyDevice->Unacquire(); 
	SafeRelease(pDIJoyDevice);
	SafeRelease(pDInput);

	ZeroMemory(&diJoyState, sizeof(DIJOYSTATE2));
	ZeroMemory(&diDObjData, sizeof(DIDEVICEOBJECTDATA));

	dwInType = eITNone;
	bCreated = FALSE;
	return TRUE;
}

// ��ԍX�V
BOOL CInput::UpdateState()
{
	if (!bCreated) return FALSE;

	BOOL bRes = FALSE;

	if (dwInType & eITJoystick)
		bRes |= UpdateJoyState();
	if (dwInType & eITKeyboard)
		bRes |= UpdateKeyState();

	return bRes;
}
// �W���C�X�e�B�b�N��ԍX�V
BOOL CInput::UpdateJoyState()
{
	if (pDIJoyDevice!=NULL)
	{
		HRESULT hr;

		// �f�o�C�X�̒��ڃf�[�^���擾����
		hr = pDIJoyDevice->Poll();
		if (FAILED(hr))
		{
			hr = pDIJoyDevice->Acquire();
			while (bActive && hr==DIERR_INPUTLOST)
				hr = pDIJoyDevice->Acquire();
		}

		hr = pDIJoyDevice->GetDeviceState(sizeof(DIJOYSTATE2), &diJoyState);
		return SUCCEEDED(hr);

	}

	return FALSE;
}

BOOL CInput::UpdateKeyState()
{
	if (pDIKeyDevice!=NULL)
	{
		HRESULT hr;

		// �f�o�C�X�̒��ڃf�[�^���擾����
		hr = pDIKeyDevice->Poll();
		if (FAILED(hr))
		{
			hr = pDIKeyDevice->Acquire();
			while (bActive && hr==DIERR_INPUTLOST)
				hr = pDIKeyDevice->Acquire();
		}

		hr = pDIKeyDevice->GetDeviceState(256, diKeyState);

		return SUCCEEDED(hr);
	}
	return FALSE;
}

DWORD CInput::UpdateObjData(LPDIRECTINPUTDEVICE8 pDIDev)
{
	HRESULT hr = E_FAIL;
	DWORD dwItems = 1;
	if (!pDIDev) return 0;
	hr = pDIDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &diDObjData, &dwItems, 0);
	if (hr==DIERR_INPUTLOST)
		pDIDev->Acquire();
	else if (FAILED(hr) || dwItems == 0)
		return 0;

	return dwItems;
}


// -------------------------------------------------------------------
// �Q�[���R���g���[���E�N���X
// -------------------------------------------------------------------
CGameControl::CGameControl():CInput()
{
	ZeroMemory(&tInState, sizeof(TInputState));
	ZeroMemory(&tPastState, sizeof(TInputState));
	
	ZeroMemory(&trKeyCode, sizeof(TRelKeyCode));
	ZeroMemory(&trJoystick, sizeof(TRelJoystick));

	for(int i=0; i<MAX_BUTTON; i++)
		trJoystick.bytBtnIndex[i] = i;

	bChanged = FALSE;
}

CGameControl::~CGameControl()
{

}

BOOL CGameControl::Release()
{
	BOOL bRes = CInput::Release();
	
	ZeroMemory(&tInState, sizeof(TInputState));
	ZeroMemory(&tPastState, sizeof(TInputState));
	ZeroMemory(&trKeyCode, sizeof(TRelKeyCode));
	ZeroMemory(&trJoystick, sizeof(TRelJoystick));

	for(int i=0; i<MAX_BUTTON; i++)
		trJoystick.bytBtnIndex[i] = i;

	bChanged = FALSE;
	bCreated = FALSE;
	return bRes;
}

BOOL CGameControl::UpdateState()
{
	bChanged = FALSE;
	BOOL bRes = CInput::UpdateState();

	if (!bRes)
		return FALSE;
	tPastState = tInState;

	int i;

	ZeroMemory(&tInState, sizeof(TInputState));

	if (dwInType & eITJoystick)
	{
		if (diJoyState.lX>=nContOnMin)
			tInState.lX = nContOnMin;
		else if (diJoyState.lX<=-nContOnMin)
			tInState.lX = -nContOnMin;
		if (diJoyState.lY>=nContOnMin)
			tInState.lY = nContOnMin;
		else if (diJoyState.lY<=-nContOnMin)
			tInState.lY = -nContOnMin;
		if (diJoyState.lZ>=nContOnMin)
			tInState.lZ = nContOnMin;
		else if (diJoyState.lZ<=-nContOnMin)
			tInState.lZ = -nContOnMin;

		for(i=0; i<MAX_BUTTON; i++)
			tInState.bytButtons[i] = (diJoyState.rgbButtons[trJoystick.bytBtnIndex[i]] & 0x80);
	}

	if (dwInType & eITKeyboard)
	{
		for(i=0; i<MAX_KEY_STT; i++)
		{
			if (diKeyState[i] & 0x80)
			{
				// �\��
				if (i == trKeyCode.up)
					tInState.lY = -nContOnMin;
				else if(i == trKeyCode.down)
					tInState.lY = nContOnMin;
				else if(i == trKeyCode.right)
					tInState.lX = nContOnMin;
				else if(i == trKeyCode.left)
					tInState.lX = -nContOnMin;
				else
				{
					for (int j=0; j<MAX_BUTTON; j++)
					{
						if (trKeyCode.buttons[j] == i)
							tInState.bytButtons[j] |= KEY_BTN_ON;
						//break;
					}
				}
			}
		}
	}

	bChanged = IsChange(&tInState, &tPastState);
	return TRUE;
}

BOOL CGameControl::IsChange(TInputState* now, TInputState* past)
{
	BOOL result = FALSE;
	if (now->lX != past->lX)
		result = TRUE;
	if (now->lY != past->lY)
		result = TRUE;
	if (now->lZ != past->lZ)
		result = TRUE;
	for (int i=0;i<MAX_BUTTON;i++)
		if (now->bytButtons[i] != (past->bytButtons[i]&KEY_BTN_ON))
		{
			now->bytButtons[i] |= KEY_BTN_CHANGE;
			result |= TRUE;
		}
	return result;
}

void CGameControl::SetKeyCode(TRelKeyCode trk)
{
	trKeyCode = trk;
}


void CGameControl::SetJoystick(TRelJoystick trj)
{
	trJoystick = trj;
}

void CGameControl::SaveFileW(WCHAR* fn)
{
	WCHAR kbuf[MAX_INI_BUF];
	WCHAR jbuf[MAX_INI_BUF];
	WCHAR fpath[_MAX_PATH*2+1];
	WCHAR vbuf[32];

	GetModuleDirW(fpath, MAX_PATH*2+1);
		
	if (!fn)
		wcsncat_s(fpath, _MAX_PATH*2+1, KEY_INI_W, _TRUNCATE);
	else
		wcsncat_s(fpath, _MAX_PATH*2+1, fn, _TRUNCATE);

	int i;
	for (i=0;i<MAX_BUTTON;i++)
	{
		SafePrintfW(kbuf, MAX_INI_BUF, L"%s%d", KEY_HEAD_KEY_W, i);
		SafePrintfW(jbuf, MAX_INI_BUF, L"%s%d", JOY_HEAD_KEY_W, i);
		SafePrintfW(vbuf, 32, L"%d", (int)trKeyCode.buttons[i]);
		WritePrivateProfileStringW(KEY_SECTION_W, kbuf, vbuf, fpath);
		SafePrintfW(vbuf, 32, L"%d", (int)trJoystick.bytBtnIndex[i]);
		WritePrivateProfileStringW(JOY_SECTION_W, jbuf, vbuf, fpath);
	}
	SafePrintfW(kbuf, MAX_INI_BUF, L"up");
	SafePrintfW(vbuf, 32, L"%d", (int)trKeyCode.up);
	WritePrivateProfileStringW(KEY_SECTION_W, kbuf, vbuf, fpath);
//	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"down");
	SafePrintfW(vbuf, 32, L"%d", (int)trKeyCode.down);
	WritePrivateProfileStringW(KEY_SECTION_W, kbuf, vbuf, fpath);
//	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"left");
	SafePrintfW(vbuf, 32, L"%d", (int)trKeyCode.left);
	WritePrivateProfileStringW(KEY_SECTION_W, kbuf, vbuf, fpath);
//	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"right");
	SafePrintfW(vbuf, 32, L"%d", (int)trKeyCode.right);
	WritePrivateProfileStringW(KEY_SECTION_W, kbuf, vbuf, fpath);

	SafePrintfW(vbuf, 32, L"%d", (int)nContOnMin);
	WritePrivateProfileStringW(JOY_SECTION_W, L"ON", vbuf, fpath);
	return;
}

void CGameControl::SaveFileA(char* fn)
{
	char kbuf[MAX_INI_BUF];
	char jbuf[MAX_INI_BUF];
	char fpath[_MAX_PATH*2+1];
	char vbuf[32];

	GetModuleDirA(fpath, MAX_PATH*2+1);
		
	if (!fn)
		strncat_s(fpath, MAX_PATH*2+1, KEY_INI_A, _TRUNCATE);
	else
		strncat_s(fpath, MAX_PATH*2+1, fn, _TRUNCATE);

	int i;
	for (i=0;i<MAX_BUTTON;i++)
	{
		SafePrintfA(kbuf, MAX_INI_BUF, "%s%d", KEY_HEAD_KEY_A, i);
		SafePrintfA(jbuf, MAX_INI_BUF, "%s%d", JOY_HEAD_KEY_A, i);
		SafePrintfA(vbuf, 32, "%d", (int)trKeyCode.buttons[i]);
		WritePrivateProfileStringA(KEY_SECTION_A, kbuf, vbuf, fpath);
		SafePrintfA(vbuf, 32, "%d", (int)trJoystick.bytBtnIndex[i]);
		WritePrivateProfileStringA(JOY_SECTION_A, jbuf, vbuf, fpath);
	}
	SafePrintfA(kbuf, MAX_INI_BUF, "up");
	SafePrintfA(vbuf, 32, "%d", (int)trKeyCode.up);
	WritePrivateProfileStringA(KEY_SECTION_A, kbuf, vbuf, fpath);
//	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "down");
	SafePrintfA(vbuf, 32, "%d", (int)trKeyCode.down);
	WritePrivateProfileStringA(KEY_SECTION_A, kbuf, vbuf, fpath);
//	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "left");
	SafePrintfA(vbuf, 32, "%d", (int)trKeyCode.left);
	WritePrivateProfileStringA(KEY_SECTION_A, kbuf, vbuf, fpath);
//	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "right");
	SafePrintfA(vbuf, 32, "%d", (int)trKeyCode.right);
	WritePrivateProfileStringA(KEY_SECTION_A, kbuf, vbuf, fpath);

	SafePrintfA(vbuf, 32, "%d", (int)nContOnMin);
	WritePrivateProfileStringA(JOY_SECTION_A, "ON", vbuf, fpath);
}

BOOL CGameControl::LoadFileW(WCHAR* fn)
{
	WCHAR kbuf[MAX_INI_BUF];
	WCHAR jbuf[MAX_INI_BUF];
	WCHAR fpath[MAX_PATH*2+1];
	GetModuleDirW(fpath, MAX_PATH*2+1);
		
	if (!fn)
		wcsncat_s(fpath, _MAX_PATH*2+1, KEY_INI_W, _TRUNCATE);
	else
		wcsncat_s(fpath, _MAX_PATH*2+1, fn, _TRUNCATE);

	if (!IsFilePath(fpath))
		return FALSE;

	int i;
	for (i=0;i<MAX_BUTTON;i++)
	{
		SafePrintfW(kbuf, MAX_INI_BUF, L"%s%d", KEY_HEAD_KEY_W, i);
		SafePrintfW(jbuf, MAX_INI_BUF, L"%s%d", JOY_HEAD_KEY_W, i);
		trKeyCode.buttons[i] = (BYTE)GetPrivateProfileIntW(KEY_SECTION_W, kbuf, 0, fpath);
		trJoystick.bytBtnIndex[i] = GetPrivateProfileIntW(JOY_SECTION_W, jbuf, MAX_BUTTON, fpath);
	}
	SafePrintfW(kbuf, MAX_INI_BUF, L"up");
	trKeyCode.up = (BYTE)GetPrivateProfileIntW(KEY_SECTION_W, kbuf, 0, fpath);
	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"down");
	trKeyCode.down = (BYTE)GetPrivateProfileIntW(KEY_SECTION_W, kbuf, 0, fpath);
	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"left");
	trKeyCode.left = (BYTE)GetPrivateProfileIntW(KEY_SECTION_W, kbuf, 0, fpath);
	i++;
	SafePrintfW(kbuf, MAX_INI_BUF, L"right");
	trKeyCode.right = (BYTE)GetPrivateProfileIntW(KEY_SECTION_W, kbuf, 0, fpath);

	return TRUE;
}

BOOL CGameControl::LoadFileA(char* fn)
{
	char kbuf[MAX_INI_BUF];
	char jbuf[MAX_INI_BUF];
	char fpath[MAX_PATH*2+1];

	GetModuleDirA(fpath, MAX_PATH*2+1);
		
	if (!fn)
		strncat_s(fpath, MAX_PATH*2+1, KEY_INI_A, _TRUNCATE);
	else
		strncat_s(fpath, MAX_PATH*2+1, fn, _TRUNCATE);

	errno_t err;
	FILE *pf;
	err = fopen_s(&pf, &fpath[0], "r");
	if (err != 0)	return FALSE;
	fclose(pf);

	int i;
	for (i=0;i<MAX_BUTTON;i++)
	{
		SafePrintfA(kbuf, MAX_INI_BUF, "%s%d", KEY_HEAD_KEY_A, i);
		SafePrintfA(jbuf, MAX_INI_BUF, "%s%d", JOY_HEAD_KEY_A, i);
		trKeyCode.buttons[i] = (BYTE)GetPrivateProfileIntA(KEY_SECTION_A, kbuf, 0, fpath);
		trJoystick.bytBtnIndex[i] = GetPrivateProfileIntA(JOY_SECTION_A, jbuf, MAX_BUTTON, fpath);
	}
	SafePrintfA(kbuf, MAX_INI_BUF, "up");
	trKeyCode.up = (BYTE)GetPrivateProfileIntA(KEY_SECTION_A, kbuf, 0, fpath);
	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "down");
	trKeyCode.down = (BYTE)GetPrivateProfileIntA(KEY_SECTION_A, kbuf, 0, fpath);
	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "left");
	trKeyCode.left = (BYTE)GetPrivateProfileIntA(KEY_SECTION_A, kbuf, 0, fpath);
	i++;
	SafePrintfA(kbuf, MAX_INI_BUF, "right");
	trKeyCode.right = (BYTE)GetPrivateProfileIntA(KEY_SECTION_A, kbuf, 0, fpath);

	return TRUE;
}


void CGameControl::OnLost()
{
	bCreated = FALSE;
	if (pDIJoyDevice)
		pDIJoyDevice->Unacquire();
	if (pDIKeyDevice)
		pDIKeyDevice->Unacquire();
}

void CGameControl::OnReset()
{
	HRESULT hr;
	if (pDIJoyDevice)
	{
		hr = pDIJoyDevice->Acquire();
		while (hr==DIERR_INPUTLOST)
			hr = pDIJoyDevice->Acquire();
		// Update the dialog text 
		if( hr == DIERR_OTHERAPPHASPRIO || 
			hr == DIERR_NOTACQUIRED ) 
			OutputDebugStr(TEXT("Unacquired") );
	}
	if (pDIKeyDevice)
	{
		hr = pDIKeyDevice->Acquire();
		while ( hr==DIERR_INPUTLOST)
			hr = pDIKeyDevice->Acquire();
		// Update the dialog text 
		if( hr == DIERR_OTHERAPPHASPRIO || 
			hr == DIERR_NOTACQUIRED ) 
			OutputDebugStr(TEXT("Unacquired") );
	}
	bCreated = TRUE;
}
