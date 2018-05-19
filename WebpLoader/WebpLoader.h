#ifndef H_WEBP_LOADER___
#define H_WEBP_LOADER___

#include "windows.h"
#include "TCHAR.h"
#include <stdio.h>
#include <stdlib.h>

#include <webp/decode.h>
#include <webp/types.h>

#include <vector>
#include "util.h"
#include <d3d9types.h>

using namespace std;

#include "CFiler.h"

#ifdef _DEBUG
#pragma comment(lib, "libwebp_a_debug.lib")
#else
#pragma comment(lib, "libwebp_a.lib")
#endif

class WebpLoader
{
public:
	// メモリからオープン
	// dest_data			: out image data
	// dest_data_lines	: out image data lines pointer
	// src_buff				: buffer
	// size					: buffer_size
	// width					: out png_width
	// height				: out png_height
//	static BOOL WebpLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, BYTE* src_buff, UINT size, int* width, int* height);

	// メモリからオープン
	// dest_data			: out image data
	// dest_data_lines	: out image data lines pointer
	// pfiler					: filer
	// width					: out png_width
	// height				: out png_height
//	static BOOL WebpLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, WCHAR* src_path, CFiler *pfiler, int* width, int* height);

	// メモリからオープン
	// dest_image		: out image data
	// pfiler					: filer
	// width					: out png_width
	// height				: out png_height
//	static BOOL WebpLoadFromFileInMemory(TPngImage* dest_image, WCHAR* src_path, CFiler *pfiler);

		
	// PngLoaderを使ってメモリからテクスチャを作成
	// pDev					: device
	// src_buff				: filer
	// size					: バッファサイズ
	// width					: out png_width
	// height				: out png_height
	// ppCreatedTexture: out image data
	static BOOL CreateTextureFromFileInMemory(LPDIRECT3DDEVICE9 pDev, BYTE* src_buff, UINT size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool=D3DPOOL_DEFAULT);

private:

};



#endif
