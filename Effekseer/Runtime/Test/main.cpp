
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include<assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#define __DicrectX9	1
#define __OpenGL	0

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if __DicrectX9

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3dx9.lib" )

#include "../Effekseer/Effekseer.h"
#include "../EffekseerRendererDX9/EffekseerRendererDX9.h"

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Release.lib" )
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if __OpenGL

#include <windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib" )

/*
#define GLFW_DLL
#include <GL/glfw.h>
#pragma comment(lib, "GLFWDLL.lib")
*/

#define __EFFEKSEER_GL_USE_GLEW__ 1
#include "../Effekseer/Effekseer.h"
#include "../EffekseerRendererGL/EffekseerRendererGL.h"

#include <png.h>
#pragma comment(lib, "libpng15.lib" )

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererGL.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererGL.Release.lib" )
#endif

#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::Vector3D			g_focus;

#if __DicrectX9
static LPDIRECT3D9			g_d3d;
static LPDIRECT3DDEVICE9	g_d3d_device;
#endif

#if __OpenGL
static HDC					g_hDC;
static HGLRC				g_hGLRC;


struct PngDataStream {
	const void* data;
	uint32_t offset;
	uint32_t size;
};
// libpngのリードコールバック関数
static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PngDataStream* stm = (PngDataStream*)png_get_io_ptr(png_ptr);
	memcpy(data, (uint8_t*)stm->data + stm->offset, length);
	stm->offset += length;
}

// PNGデータをRAWデータにデコード
static void* PngDecodeData(const void* source, uint32_t size, uint32_t& width, uint32_t& height)
{
	png_structp png;
	png_infop png_info;
	PngDataStream stm;
	int32_t color_type, interlace_type, compression_type, filter_type;
	int32_t i, bit_depth, pitch, count;
	uint8_t *ptr, *image;

	// pngアクセス構造体を作成
	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	// リードコールバック関数指定
	stm.data = source;
	stm.size = size;
	stm.offset = 0;
	png_set_read_fn(png, &stm, &PngReadData);
	
	// png画像情報構造体を作成
	png_info = png_create_info_struct(png);

	// IHDRチャンク情報を取得
	png_read_info(png, png_info);
	png_get_IHDR(png, png_info, &width, &height, &bit_depth, 
		&color_type, &interlace_type, &compression_type, &filter_type);
	
	// 色々やってRGBA8888フォーマットに変換する
	if (bit_depth < 8) {
		png_set_packing(png);
	} else if (bit_depth == 16) {
		png_set_strip_16(png);
	}
	switch (color_type) {
	case PNG_COLOR_TYPE_PALETTE:
		png_set_palette_to_rgb(png);
		break;
	case PNG_COLOR_TYPE_GRAY:
		png_set_expand_gray_1_2_4_to_8(png);
		break;
	case PNG_COLOR_TYPE_RGB:
		png_set_filler(png, 0xff, PNG_FILLER_AFTER);
		break;
	}

	image = (uint8_t*)malloc(width * height * 4);
	ptr = image;
	pitch = (int32_t)width * 4;
	count = (int32_t)height;

	// イメージデータを読み込む
	for (i = 0; i < count; i++) {
		png_read_row(png, ptr, NULL);
		ptr += pitch;
	}
	
	png_destroy_read_struct(&png, &png_info, NULL);
	return image;
}

class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:

public:
	TextureLoader() {}
	virtual ~TextureLoader() {}

public:
	void* Load( const EFK_CHAR* path );

	void Unload( void* data );
};

void* TextureLoader::Load( const EFK_CHAR* path )
{
	FILE* fp_texture = _wfopen( (const wchar_t *)path, L"rb" );

	if( fp_texture != NULL )
	{
		fseek( fp_texture, 0, SEEK_END );
		size_t size_texture = ftell( fp_texture );
		char* data_texture = new char[size_texture];
		fseek( fp_texture, 0, SEEK_SET );
		fread( data_texture, 1, size_texture, fp_texture );
		fclose( fp_texture );

		
		void* image;
		uint32_t width, height;

		// 対応フォーマットはPNGオンリー
		image = PngDecodeData(data_texture, size_texture, width, height);
		if (image == NULL) {
			return NULL;
		}

		GLuint m_texture = 0;
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glBindTexture(GL_TEXTURE_2D, 0);

		free(image);

		delete [] data_texture;

		return (void*)m_texture;
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	GLuint m_texture = (GLuint)data;
	glDeleteTextures(1, &m_texture);
}

#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitWindow()
{
	WNDCLASS wndClass;
	wchar_t szClassNme[]      =  L"Effekseer";
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = WndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandle(0);
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = szClassNme;
	RegisterClass(&wndClass);
	g_window_handle = CreateWindow(
		szClassNme,
		L"Effekseer",
		WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		g_window_width,
		g_window_height,
		NULL,
		NULL,
		GetModuleHandle(0),
		NULL);
	ShowWindow( g_window_handle, true );
	UpdateWindow( g_window_handle );

#if __DicrectX9
	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = g_window_width;
	d3dp.BackBufferHeight = g_window_height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = g_window_handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	
	g_d3d->CreateDevice( 
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		g_window_handle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dp,
		&g_d3d_device );
#endif

#if __OpenGL
	g_hDC = GetDC( g_window_handle );

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
		0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pxfm = ChoosePixelFormat( g_hDC, &pfd );

	SetPixelFormat( g_hDC, pxfm, &pfd );

	g_hGLRC = wglCreateContext( g_hDC );

	wglMakeCurrent( g_hDC, g_hGLRC );
	GLenum glewIniterr = glewInit();
    assert ( glewIniterr == GLEW_OK );

	wglSwapIntervalEXT(1);

	wglMakeCurrent( 0, 0);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void MainLoop()
{
	for(;;)
	{ 
		MSG msg;
		if (PeekMessage (&msg,NULL,0,0,PM_NOREMOVE)) 
		{
			if( msg.message == WM_QUIT )
			{
				return ;
			}
			GetMessage (&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static int count = 0;
			count++;
			if( count > 20 )
			{
				//g_manager->SetPaused( g_handle, true );
			}

			g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );
			g_manager->Update();
			
#if __DicrectX9
			g_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
			g_d3d_device->BeginScene();

			g_renderer->BeginRendering();
			g_manager->Draw();
			g_renderer->EndRendering();

			g_d3d_device->EndScene();
			g_d3d_device->Present( NULL, NULL, NULL, NULL );
#endif

#if __OpenGL
			wglMakeCurrent( g_hDC, g_hGLRC );
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g_renderer->BeginRendering();
			g_manager->Draw();
			g_renderer->EndRendering();

			glFlush();
			SwapBuffers( g_hDC );
			wglMakeCurrent( 0, 0);
#endif
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	InitWindow();
	
#if __DicrectX9
	g_renderer = ::EffekseerRenderer::Renderer::Create( g_d3d_device, 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );
#endif

#if __OpenGL
	wglMakeCurrent( g_hDC, g_hGLRC );
	g_renderer = ::EffekseerRenderer::Renderer::Create( 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );
#endif

	g_manager = ::Effekseer::Manager::Create( 2000 );
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );

	g_manager->SetCoordinateSystem( ::Effekseer::COORDINATE_SYSTEM_RH );

#if __DicrectX9
	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
#endif

#if __OpenGL
	g_manager->SetTextureLoader( new TextureLoader() );
#endif

	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f );
	g_focus = ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f );

	g_renderer->SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( g_position, g_focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"test.efk" );

	g_handle = g_manager->Play( g_effect, 0, 0, 0 );
	g_manager->SetLocation( g_handle, -5.0f, 0.0f, -20.0f );
#if __OpenGL
	wglMakeCurrent( 0, 0);
#endif

	MainLoop();
	
	g_manager->StopEffect( g_handle );

	g_manager->Destroy();
	g_renderer->Destory();

	ES_SAFE_RELEASE( g_effect );

	

#if __DicrectX9
	ES_SAFE_RELEASE( g_d3d_device );
	ES_SAFE_RELEASE( g_d3d );
#endif

#if __OpenGL
	wglDeleteContext( g_hGLRC );
#endif
	return 0;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------