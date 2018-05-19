#ifndef H_WEBP_LOADER___
#define H_WEBP_LOADER___

#include "windows.h"
#include "TCHAR.h"
#include <stdio.h>
#include <stdlib.h>

# include "jpegxr.h"

#include "util.h"
#include <d3d9types.h>

using namespace std;

class JxrLoader
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
	// fd					: file handle
	// size					: バッファサイズ
	// width					: out png_width
	// height				: out png_height
	// ppCreatedTexture: out image data
	static BOOL CreateTextureFromFileHandle(LPDIRECT3DDEVICE9 pDev, FILE* fd, UINT size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool=D3DPOOL_DEFAULT);

private:

};



#endif
