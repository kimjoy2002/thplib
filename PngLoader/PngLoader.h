#ifndef H_PNG_LOADER___
#define H_PNG_LOADER___

#include "windows.h"
#include "TCHAR.h"
#include <stdio.h>
#include <stdlib.h>

#include <png.h>

#include <vector>
#include "util.h"
#include <d3d9types.h>

using namespace std;

#include "CFiler.h"

#ifdef _DEBUG
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "Filer_d.lib")
#else
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "Filer.lib")
#endif

typedef struct TPNG_BUFFER
{
    unsigned char *data;
    unsigned long data_len;
    unsigned long data_offset;
} TPngBuffer;

typedef struct TPNG_IMAGE
{
	DWORD*	data;
	DWORD**	lines;
	int				width;
	int				height;
} TPngImage, PPngImage;

class PngLoader
{
public:
	// メモリからオ?プン
	// dest_data			: out image data
	// dest_data_lines	: out image data lines pointer
	// src_buff				: buffer
	// size					: buffer_size
	// width					: out png_width
	// height				: out png_height
	static BOOL PngLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, BYTE* src_buff, UINT size, int* width, int* height);

	// メモリからオ?プン
	// dest_data			: out image data
	// dest_data_lines	: out image data lines pointer
	// pfiler					: filer
	// width					: out png_width
	// height				: out png_height
	static BOOL PngLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, WCHAR* src_path, CFiler *pfiler, int* width, int* height);

	// メモリからオ?プン
	// dest_image		: out image data
	// pfiler					: filer
	// width					: out png_width
	// height				: out png_height
	static BOOL PngLoadFromFileInMemory(TPngImage* dest_image, WCHAR* src_path, CFiler *pfiler);

		
	// PngLoaderを使ってメモリからテクス?ャを作成
	// pDev					: device
	// src_buff				: filer
	// size					: バッフ?サイズ
	// width					: out png_width
	// height				: out png_height
	// ppCreatedTexture: out image data
	static BOOL CreateTextureFromFileInMemory(LPDIRECT3DDEVICE9 pDev, BYTE* src_buff, UINT size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool=D3DPOOL_DEFAULT);

	static BOOL CopyToTexture(LPDIRECT3DDEVICE9 pDev, RECT* pPasteRect, int nCopyX, int nCopyY, const TPngImage* pCopyImage, LPDIRECT3DTEXTURE9 pPasteTexture);
	static BOOL PasteFromTexture(LPDIRECT3DDEVICE9 pDev, RECT* pCopyRect, int nPasteX, int nPasteY, const TPngImage* pPasteImage, LPDIRECT3DTEXTURE9 pCopyTexture);
	static BOOL PasteFromImage(RECT* pCopyRect, int nPasteX, int nPasteY, const TPngImage* pPasteImage,const TPngImage* pCopyImage);

private:
	static void png_memread_func(png_structp png_ptr, png_bytep buf, png_size_t size);
	static void png_data_read(png_structp png_ptr, TPngBuffer *png_buff);

};



#endif
