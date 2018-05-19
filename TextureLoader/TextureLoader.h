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

	// メモリ内のファイルからロード
	static BOOL LoadTextureFromFileInMemory(LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPCVOID pSrc, UINT size, int *width, int *height, RECT* pClipSrcRec = NULL, UINT mipleve = 0, DWORD usage=D3DUSAGE_RENDERTARGET, D3DPOOL pool = D3DPOOL_DEFAULT);

	// メモリ内のファイルの情報取得
	static void GetImageInfo(LPCVOID pSrc, UINT size, int *width, int *height);

private:
	// フ ァイルのロード
//	static BOOL LoadTexture(char *fn,LPDIRECT3DTEXTURE9 *p,int *width, int *height);

	/*
		テクスチャを分割
		pSrc:分割元 / pSplits:分割したテクスチャリスト
		w:Width / h:Height / in:分割数
		iw,ih:分割サイズ
		in:分割数
		sw,sh:元画像のサイズ
	*/
	static BOOL SplitTexture(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc,int iw, int ih, int in, int sw, int sh);


	// 範囲を指定してテクスチャ(のサーフェース)をコピー
	static BOOL Split(LPDIRECT3DTEXTURE9 pSrc, LPDIRECT3DTEXTURE9 pDest, RECT *pSrcRec=NULL, RECT *pDestRec=NULL);


	// テキストからテクスチャ作成から挿入
	static BOOL CreateSpTexsFromText(LPDIRECT3DDEVICE9 pDev, CSpTexs** ppNewTexs, const TCHAR* wd, DWORD color, LOGFONT *plf);

};

#endif