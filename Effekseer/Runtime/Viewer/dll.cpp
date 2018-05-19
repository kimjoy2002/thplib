
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "dll.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static float								g_RotX = 30.0f;
static float								g_RotY = 30.0f;
static float								g_Distance = 15.0f;
const float									PI = 3.14159265f;
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EFK_STDCALL print( Effekseer::Manager* manager, float* a1, float* r )
{
	printf("%f,%f,%f\n", a1[0], a1[1], a1[2] );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Combine( const wchar_t* rootPath, const wchar_t* treePath,  wchar_t* dst, int dst_length )
{
	int rootPathLength = 0;
	while( rootPath[ rootPathLength ] != 0 )
	{
		rootPathLength++;
	}

	int treePathLength = 0;
	while( treePath[ treePathLength ] != 0 )
	{
		treePathLength++;
	}

	// 両方ともなし
	if( rootPathLength == 0 && treePathLength == 0 )
	{
		return false;
	}

	// 片方なし
	if( rootPathLength == 0 )
	{
		if( treePathLength < dst_length )
		{
			memcpy( dst, treePath, sizeof(wchar_t) * (treePathLength + 1) );
			return true;
		}
		else
		{
			return false;
		}
	}

	if( treePathLength == 0 )
	{
		if( rootPathLength < dst_length )
		{
			memcpy( dst, rootPath, sizeof(wchar_t) * (rootPathLength + 1) );
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// 両方あり

	// ディレクトリパスまで戻す。
	int PathPosition = rootPathLength;
	while( PathPosition > 0 )
	{
		if( rootPath[ PathPosition - 1 ] == L'/' || rootPath[ PathPosition - 1 ] == L'\\' )
		{
			break;
		}
		PathPosition--;
	}

	// コピーする
	memcpy( dst, rootPath, sizeof(wchar_t) * PathPosition );
	dst[ PathPosition ] = 0;

	// 無理やり繋げる
	if( PathPosition + treePathLength > dst_length )
	{
		return false;
	}

	memcpy( &(dst[ PathPosition ]), treePath, sizeof(wchar_t) * treePathLength );
	PathPosition = PathPosition + treePathLength;
	dst[ PathPosition ] = 0;

	// ../ ..\ の処理
	for( int i = 0; i < PathPosition - 2; i++ )
	{
		if( dst[ i ] == L'.' && dst[ i + 1 ] == L'.' && ( dst[ i + 2 ] == L'/' || dst[ i + 2 ] == L'\\' ) )
		{
			int pos = 0;

			if( i > 1 && dst[ i - 2 ] == L'.' )
			{
				
			}
			else
			{
				for( pos = i - 2; pos > 0; pos-- )
				{
					if( dst[ pos - 1 ] == L'/' || dst[ pos - 1 ] == L'\\' )
					{
						break;
					}
				}

				for( int k = pos; k < PathPosition; k++ )
				{
					dst[ k ] = dst[ k + (i + 3) - pos ];
				}
				PathPosition = PathPosition - ( i + 3 - pos );
				i = pos - 1;
			}
		}
	}
	dst[ PathPosition ] = 0;
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerParamater::ViewerParamater()
	: GuideWidth		( 0 )
	, GuideHeight		( 0 )
	, IsPerspective		( false )
	, IsOrthographic	( false )
	, FocusX			( 0 )
	, FocusY			( 0 )
	, FocusZ			( 0 )
	, AngleX			( 0 )
	, AngleY			( 0 )
	, Distance			( 0 )
	, RendersGuide		( false )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerTool::Renderer*		g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::EffekseerTool::Sound*			g_sound = NULL;
static std::map<std::wstring,IDirect3DTexture9*> m_textures;

static ::Effekseer::Vector3D	g_focus_position;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::TextureLoader( EffekseerRenderer::Renderer* renderer )
	: m_renderer	( renderer )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Native::TextureLoader::Load( const EFK_CHAR* path )
{
	wchar_t dst[260];
	Combine( RootPath.c_str(), (const wchar_t *)path, dst, 260 );

	std::wstring key( dst );
	IDirect3DTexture9* pTexture = NULL;

	if( m_textures.count( key ) > 0 )
	{
		pTexture = m_textures[ key ];
	}
	else
	{
		FILE* fp_texture = _wfopen( dst, L"rb" );
		if( fp_texture != NULL )
		{
			fseek( fp_texture, 0, SEEK_END );
			size_t size_texture = ftell( fp_texture );
			char* data_texture = new char[size_texture];
			fseek( fp_texture, 0, SEEK_SET );
			fread( data_texture, 1, size_texture, fp_texture );
			fclose( fp_texture );

			D3DXCreateTextureFromFileInMemory( ((EffekseerRenderer::RendererImplemented*)m_renderer)->GetDevice(), data_texture, size_texture, &pTexture );

			delete [] data_texture;

			m_textures[ key ] = pTexture;
		}
	}

	return pTexture;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::TextureLoader::Unload( void* data )
{
	/*
	if( data != NULL )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
	*/
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::SoundLoader::SoundLoader( Effekseer::SoundLoader* loader )
	: m_loader( loader )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::SoundLoader::~SoundLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Native::SoundLoader::Load( const EFK_CHAR* path )
{
	EFK_CHAR dst[260];
	Combine( RootPath.c_str(), (const wchar_t *)path, (wchar_t *)dst, 260 );

	return m_loader->Load( dst );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SoundLoader::Unload( void* handle )
{
	m_loader->Unload( handle );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::Native()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Native::~Native()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::CreateWindow_Effekseer( void* pHandle, int width, int height )
{
	g_renderer = new ::EffekseerTool::Renderer( 20000 );
	if( g_renderer->Initialize( (HWND)pHandle, width, height ) )
	{
		// 関数追加
		//::Effekseer::ScriptRegister::SetExternalFunction(0, print);

		g_manager = ::Effekseer::Manager::Create( 20000 );

		{
			::Effekseer::SpriteRenderer* sprite_renderer = g_renderer->GetRenderer()->CreateSpriteRenderer();
			::Effekseer::RibbonRenderer* ribbon_renderer = g_renderer->GetRenderer()->CreateRibbonRenderer();
			::Effekseer::RingRenderer* ring_renderer = g_renderer->GetRenderer()->CreateRingRenderer();

			if( sprite_renderer == NULL )
			{
				printf("Failed create Renderer\n");
				g_manager->Destroy();
				g_manager = NULL;
				ES_SAFE_DELETE( g_renderer );
				return false;
			}
			g_manager->SetSpriteRenderer( sprite_renderer );
			g_manager->SetRibbonRenderer( ribbon_renderer );
			g_manager->SetRingRenderer( ring_renderer );
			g_manager->SetTextureLoader( new TextureLoader( g_renderer->GetRenderer() ) );
		}
	}
	else
	{
		ES_SAFE_DELETE( g_renderer );
		return false;
	}

	g_sound = new ::EffekseerTool::Sound();
	if( g_sound->Initialize( 16, 16 ) )
	{
		g_manager->SetSoundPlayer( g_sound->GetSound()->CreateSoundPlayer() );
		g_manager->SetSoundLoader( new SoundLoader( g_sound->GetSound()->CreateSoundLoader() ) );
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::UpdateWindow()
{
	assert( g_manager != NULL );
	assert( g_renderer != NULL );

	::Effekseer::Vector3D position( 0, 0, g_Distance );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );
	mat_rot_y.RotationY( g_RotY / 180.0f * PI );
	::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
	::Effekseer::Vector3D::Transform( position, position, mat );

	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	// 右手
	::Effekseer::Matrix44 cameraMat;
	g_renderer->GetRenderer()->SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	g_sound->GetSound()->SetListener( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) );
	g_renderer->BeginRendering();
	g_manager->Draw();
	g_renderer->EndRendering();
	g_renderer->Present();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::ResizeWindow( int width, int height )
{
	g_renderer->Resize( width, height );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::DestroyWindow()
{
	assert( g_renderer != NULL );
	assert( g_manager != NULL );

	g_manager->StopEffect( g_handle );

	std::map<std::wstring,IDirect3DTexture9*>::iterator it = m_textures.begin();
	std::map<std::wstring,IDirect3DTexture9*>::iterator it_end = m_textures.end();
	while( it != it_end )
	{
		ES_SAFE_RELEASE( (*it).second );
		++it;
	}

	ES_SAFE_RELEASE( g_effect );

	g_manager->Destroy();
	ES_SAFE_DELETE( g_renderer );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::LoadEffect( void* pData, int size, const wchar_t* Path )
{
	g_manager->StopEffect( g_handle );
	ES_SAFE_RELEASE( g_effect );
	g_handle = -1;
	((TextureLoader*)g_manager->GetTextureLoader())->RootPath = std::wstring( Path );
	((SoundLoader*)g_manager->GetSoundLoader())->RootPath = std::wstring( Path );
	g_effect = Effekseer::Effect::Create( g_manager, pData, size );
	assert( g_effect != NULL );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::RemoveEffect()
{
	g_manager->StopEffect( g_handle );
	ES_SAFE_RELEASE( g_effect );
	g_handle = -1;
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::PlayEffect()
{
	assert( g_effect != NULL );
	g_manager->StopEffect( g_handle );
	g_handle = g_manager->Play( g_effect, 0, 0, 0 );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StopEffect()
{
	if( g_handle < 0 ) return false;
	g_manager->StopEffect( g_handle );
	g_handle = -1;
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::StepEffect( int frame )
{
	if( frame <= 0 )
	{
	}
	else if( frame == 1 )
	{
		g_manager->Update();
	}
	else
	{
		bool mute = g_sound->GetMute();
		g_sound->SetMute(true);

		for( int i = 0; i < frame - 1; i++ )
		{
			g_manager->Update();
		}

		g_sound->SetMute(mute);
		g_manager->Update();
	}
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Rotate( float x, float y )
{
	g_RotY += x;
	g_RotX += y;

	while( g_RotY >= 180.0f )
	{
		g_RotY -= 180.0f * 2.0f;
	}

	while( g_RotY <= - 180.0f )
	{
		g_RotY += 180.0f * 2.0f;
	}

	if( g_RotX > 180.0f / 2.0f )
	{
		g_RotX = 180.0f / 2.0f;
	}

	if( g_RotX < - 180.0f / 2.0f )
	{
		g_RotX = - 180.0f / 2.0f;
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Slide( float x, float y )
{
	::Effekseer::Vector3D up( 0, 1, 0 );
	::Effekseer::Vector3D right( 1, 0, 0 );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );
	mat_rot_y.RotationY( g_RotY / 180.0f * PI );
	::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
	::Effekseer::Vector3D::Transform( up, up, mat );
	::Effekseer::Vector3D::Transform( right, right, mat );
	
	up.X = up.X * y;
	up.Y = up.Y * y;
	up.Z = up.Z * y;
	right.X = right.X * ( - x );
	right.Y = right.Y * ( - x );
	right.Z = right.Z * ( - x );

	::Effekseer::Vector3D v;
	v.X = up.X + right.X;
	v.Y = up.Y + right.Y;
	v.Z = up.Z + right.Z;
	g_focus_position.X += v.X;
	g_focus_position.Y += v.Y;
	g_focus_position.Z += v.Z;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Zoom( float zoom )
{
	g_Distance -= zoom;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetRandomSeed( int seed )
{
	srand( seed );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::Record( const wchar_t* path, int32_t xCount, int32_t yCount, int32_t offsetFrame, int32_t frameSkip, bool isTranslucent )
{
	if( g_effect == NULL ) return false;

	if( ! g_renderer->BeginRecord( g_renderer->GuideWidth * xCount, g_renderer->GuideHeight * yCount ) ) return false;
	g_renderer->IsBackgroundTranslucent = isTranslucent;

	::Effekseer::Vector3D position( 0, 0, g_Distance );
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX( - g_RotX / 180.0f * PI );
	mat_rot_y.RotationY( g_RotY / 180.0f * PI );
	::Effekseer::Matrix43::Multiple( mat, mat_rot_x, mat_rot_y );
	::Effekseer::Vector3D::Transform( position, position, mat );
	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	g_renderer->GetRenderer()->SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( position, g_focus_position, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	StopEffect();
	
	::Effekseer::Handle handle = g_manager->Play( g_effect, 0, 0, 0 );
	
	for( int i = 0; i < offsetFrame; i++ )
	{
		g_manager->Update();
	}

	for( int y = 0; y < yCount; y++ )
	{
		for( int x = 0; x < xCount; x++ )
		{
			g_renderer->SetRecordRect( g_renderer->GuideWidth * x, g_renderer->GuideHeight * y );

			g_renderer->BeginRendering();
			g_manager->Draw();
			g_renderer->EndRendering();

			for( int j = 0; j < frameSkip + 1; j++ )
			{
				g_manager->Update();
			}
		}
	}
	
	g_manager->StopEffect( handle );
	g_renderer->EndRecord( path );
	
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ViewerParamater Native::GetViewerParamater()
{
	assert( g_renderer != NULL );

	ViewerParamater paramater;

	paramater.GuideWidth = g_renderer->GuideWidth;
	paramater.GuideHeight = g_renderer->GuideHeight;
	paramater.IsPerspective = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE;
	paramater.IsOrthographic = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC;
	paramater.FocusX = g_focus_position.X;
	paramater.FocusY = g_focus_position.Y;
	paramater.FocusZ = g_focus_position.Z;
	paramater.AngleX = g_RotX;
	paramater.AngleY = g_RotY;
	paramater.Distance = g_Distance;
	paramater.RendersGuide = g_renderer->RendersGuide;
	paramater.RateOfMagnification = g_renderer->RateOfMagnification;
	return paramater;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Native::SetViewerParamater( ViewerParamater& paramater )
{
	assert( g_renderer != NULL );

	g_renderer->GuideWidth = paramater.GuideWidth;
	g_renderer->GuideHeight = paramater.GuideHeight;
	
	g_renderer->RateOfMagnification = paramater.RateOfMagnification;
	
	if( paramater.IsPerspective )
	{
		g_renderer->SetProjectionType( ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE );
	}

	if( paramater.IsOrthographic )
	{
		g_renderer->SetProjectionType( ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC );
	}

	g_focus_position.X = paramater.FocusX;
	g_focus_position.Y = paramater.FocusY;
	g_focus_position.Z = paramater.FocusZ;
	g_RotX = paramater.AngleX;
	g_RotY = paramater.AngleY;
	g_Distance = paramater.Distance;
	g_renderer->RendersGuide = paramater.RendersGuide;
	
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetSoundMute( bool mute )
{
	g_sound->SetMute( mute );
	return true;
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Native::SetSoundVolume( float volume )
{
	g_sound->SetVolume( volume );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------