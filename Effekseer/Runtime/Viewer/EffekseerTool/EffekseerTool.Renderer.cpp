
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Renderer.h"
#include "EffekseerTool.Grid.h"
#include "EffekseerTool.Guide.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer::Renderer( int32_t squareMaxCount )
	: m_handle		( NULL )
	, m_width		( 0 )
	, m_height		( 0 )
	, m_d3d			( NULL )
	, m_d3d_device	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_projection	( PROJECTION_TYPE_PERSPECTIVE )
	, m_renderer	( NULL )

	, RateOfMagnification	( 1.0f )

	, m_grid	( NULL )
	, m_guide	( NULL )

	, GuideWidth	( 100 )
	, GuideHeight	( 100 )
	, RendersGuide	( false )

	, m_recording		( false )
	, m_recordingTarget	( NULL )
	, m_recordingTargetTexture	( NULL )
	, m_recordingDepth	( NULL )
	, m_recordingTempTarget	( NULL )
	, m_recordingTempDepth	( NULL )

	, IsBackgroundTranslucent	( false )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer::~Renderer()
{
	assert( !m_recording );

	ES_SAFE_DELETE( m_guide );
	ES_SAFE_DELETE( m_grid );

	if( m_renderer != NULL )
	{
		m_renderer->Destory();
		m_renderer = NULL;
	}

	ES_SAFE_RELEASE( m_d3d_device );
	ES_SAFE_RELEASE( m_d3d );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Initialize( HWND handle, int width, int height )
{
	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = width;
	d3dp.BackBufferHeight = height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if( m_d3d == NULL ) return false;

	hr = m_d3d->CreateDevice( 
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		handle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dp,
		&m_d3d_device );

	if( FAILED( hr ) )
	{
		ES_SAFE_RELEASE( m_d3d_device );
		ES_SAFE_RELEASE( m_d3d );
		return false;
	}

	m_handle = handle;
	m_width = width;
	m_height = height;

	m_renderer = (::EffekseerRenderer::RendererImplemented*)::EffekseerRenderer::Renderer::Create( m_d3d_device, m_squareMaxCount );
	
	// グリッド生成
	m_grid = ::EffekseerRenderer::Grid::Create( m_renderer );

	// ガイド作成
	m_guide = ::EffekseerRenderer::Guide::Create( m_renderer );

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 Renderer::GetDevice()
{
	return m_d3d_device;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Present()
{
	HRESULT hr;

	hr = m_d3d_device->Present( NULL, NULL, NULL, NULL );

	switch ( hr )
	{
		// ドライバ内部の意味不明なエラー
	case D3DERR_DRIVERINTERNALERROR:
		return false;

		// デバイスロスト
	case D3DERR_DEVICELOST:
		while ( FAILED( hr = m_d3d_device->TestCooperativeLevel() ) )
		{
			switch ( hr )
			{
				// デバイスロスト
			case D3DERR_DEVICELOST:
				::SleepEx( 1000, true );
				break;
				// デバイスロスト：リセット可
			case D3DERR_DEVICENOTRESET:
				ResetDevice();
				break;
			}
		}
		break;
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::ResetDevice()
{
	m_renderer->OnLostDevice();

	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = m_width;
	d3dp.BackBufferHeight = m_height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = m_handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	hr = m_d3d_device->Reset( &d3dp );

	if( FAILED( hr ) )
	{
		assert(0);
		return;
	}

	m_renderer->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eProjectionType Renderer::GetProjectionType()
{
	return m_projection;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetProjectionType( eProjectionType type )
{
	m_projection = type;

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( m_width, m_height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( m_width, m_height );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetPerspectiveFov( int width, int height )
{
	::Effekseer::Matrix44 proj;

	// 右手
	proj.PerspectiveFovRH( 60.0f / 180.0f * 3.141592f, (float)width / (float)height, 1.0f, 300.0f );
	// 左手
	// m_proj.PerspectiveFovLH( 60.0f / 180.0f * 3.141592f, (float)width / (float)height, 1.0f, 300.0f );

	proj.Values[0][0] *= RateOfMagnification;
	proj.Values[1][1] *= RateOfMagnification;

	m_renderer->SetProjectionMatrix( proj );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetOrthographic( int width, int height )
{
	::Effekseer::Matrix44 proj;

	// 右手
	proj.OrthographicRH( (float)width / 16.0f / RateOfMagnification, (float)height / 16.0f / RateOfMagnification, 1.0f, 300.0f );

	// 左手
	// m_proj.OrthographicLH( (float)width / 16.0f, (float)height / 16.0f, 1.0f, 300.0f );

	m_renderer->SetProjectionMatrix( proj );
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Resize( int width, int height )
{
	m_width = width;
	m_height = height;

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	ResetDevice();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::BeginRendering()
{
	assert( m_d3d_device != NULL );

	HRESULT hr;

	if( m_recording && IsBackgroundTranslucent )
	{
		m_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0 );
	}
	else
	{
		m_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	}

	m_renderer->BeginRendering();

	// ガイド部分が描画されるように拡大
	if( m_recording )
	{
		::Effekseer::Matrix44 mat;
		mat.Values[0][0] = (float)m_width / (float)GuideWidth;
		mat.Values[1][1] = (float)m_height / (float)GuideHeight;
		::Effekseer::Matrix44::Mul( m_renderer->GetCameraProjectionMatrix(), m_renderer->GetCameraProjectionMatrix(), mat );
	}

	hr = m_d3d_device->BeginScene();

	if( FAILED( hr ) ) return false;

	if( !m_recording )
	{
		m_grid->Rendering();
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::EndRendering()
{
	assert( m_d3d != NULL );
	assert( m_d3d_device != NULL );

	m_renderer->EndRendering();

	if( RendersGuide && !m_recording )
	{
		m_guide->Rendering( m_width, m_height, GuideWidth, GuideHeight );
	}

	m_d3d_device->EndScene();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::BeginRecord( int32_t width, int32_t height )
{
	assert( !m_recording );
	assert( m_recordingTempTarget == NULL );
	assert( m_recordingTempDepth == NULL );

	HRESULT hr;

	hr = GetDevice()->CreateTexture( m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_recordingTargetTexture, NULL );
	if( FAILED( hr ) ) return false;

	m_recordingTargetTexture->GetSurfaceLevel( 0, &m_recordingTarget );

	GetDevice()->CreateDepthStencilSurface( m_width, m_height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_recordingDepth, NULL );

	if( m_recordingTarget == NULL || m_recordingDepth == NULL )
	{
		ES_SAFE_RELEASE( m_recordingTarget );
		ES_SAFE_RELEASE( m_recordingTargetTexture );
		ES_SAFE_RELEASE( m_recordingDepth );
		return false;
	}

	GetDevice()->GetRenderTarget( 0, &m_recordingTempTarget );
	GetDevice()->GetDepthStencilSurface( &m_recordingTempDepth );

	GetDevice()->SetRenderTarget( 0, m_recordingTarget );
	GetDevice()->SetDepthStencilSurface( m_recordingDepth );
	
	m_recording = true;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetRecordRect( int32_t x, int32_t y )
{
	assert( m_recording );

	D3DVIEWPORT9 vp;
	vp.X = x;
	vp.Y = y;
	vp.Width = GuideWidth;
	vp.Height = GuideHeight;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
		
	GetDevice()->SetViewport( &vp );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::EndRecord( const wchar_t* outputPath )
{
	assert( m_recording );

	GetDevice()->SetRenderTarget( 0, m_recordingTempTarget );
	GetDevice()->SetDepthStencilSurface( m_recordingTempDepth );
	ES_SAFE_RELEASE( m_recordingTempTarget );
	ES_SAFE_RELEASE( m_recordingTempDepth );

	D3DXSaveSurfaceToFileW( outputPath, D3DXIFF_PNG, m_recordingTarget, NULL, NULL );

	ES_SAFE_RELEASE( m_recordingTarget );
	ES_SAFE_RELEASE( m_recordingTargetTexture );
	ES_SAFE_RELEASE( m_recordingDepth );

	m_recording = false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
