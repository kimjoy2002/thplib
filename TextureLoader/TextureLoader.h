#ifndef H_TEXTURE_LOADER___
#define H_TEXTURE_LOADER___

#include "windows.h"
#include "TCHAR.h"
#include <vector>
#include "util.h"

#define TEXTURE_LOADER_COLOR_KEY	0x00FF00FF

using namespace std;

class TextureLoader
{
public:
	static BOOL SplitTextureFromFileInMemory(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPCVOID pSrc, UINT size, int width, int height, int splits, RECT* pClipSrcRec = NULL);
//	static BOOL CreateSpTexsFromFileEx(CSpTexs** ppNewTexs, char *fn, int width, int height, int splits);

	static BOOL ClipTexture(	LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc, RECT* pClipRec);

	// ���������̃t�@�C�����烍�[�h
	static BOOL LoadTextureFromFileInMemory(LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPCVOID pSrc, UINT size, int *width, int *height, RECT* pClipSrcRec = NULL, UINT mipleve = 0, DWORD usage=D3DUSAGE_RENDERTARGET, D3DPOOL pool = D3DPOOL_DEFAULT);

	// ���������̃t�@�C���̏��擾
	static void GetImageInfo(LPCVOID pSrc, UINT size, int *width, int *height);

private:
	// �t �@�C���̃��[�h
//	static BOOL LoadTexture(char *fn,LPDIRECT3DTEXTURE9 *p,int *width, int *height);

	/*
		�e�N�X�`���𕪊�
		pSrc:������ / pSplits:���������e�N�X�`�����X�g
		w:Width / h:Height / in:������
		iw,ih:�����T�C�Y
		in:������
		sw,sh:���摜�̃T�C�Y
	*/
	static BOOL SplitTexture(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc,int iw, int ih, int in, int sw, int sh);


	// �͈͂��w�肵�ăe�N�X�`��(�̃T�[�t�F�[�X)���R�s�[
	static BOOL Split(LPDIRECT3DTEXTURE9 pSrc, LPDIRECT3DTEXTURE9 pDest, RECT *pSrcRec=NULL, RECT *pDestRec=NULL);


	// �e�L�X�g����e�N�X�`���쐬����}��
	static BOOL CreateSpTexsFromText(LPDIRECT3DDEVICE9 pDev, CSpTexs** ppNewTexs, const TCHAR* wd, DWORD color, LOGFONT *plf);

};

#endif