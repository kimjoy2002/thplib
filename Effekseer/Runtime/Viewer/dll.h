
#ifndef	__EFFEKSEER_DLL_H__
#define	__EFFEKSEER_DLL_H__

/**
	@file
	@brief	ÉcÅ[Éãå¸ÇØDLLèoóÕ
*/

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include "EffekseerRenderer/EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer/EffekseerRenderer.RendererImplemented.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"
#include "EffekseerTool/EffekseerTool.Sound.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ViewerParamater
{
public:
	int32_t	GuideWidth;
	int32_t	GuideHeight;
	float	RateOfMagnification;
	bool	IsPerspective;
	bool	IsOrthographic;
	float	FocusX;
	float	FocusY;
	float	FocusZ;
	float	AngleX;
	float	AngleY;
	float	Distance;
	bool	RendersGuide;

	ViewerParamater();
};

class Native
{
private:

	class TextureLoader
	: public ::Effekseer::TextureLoader
	{
	private:
		EffekseerRenderer::Renderer*	m_renderer;

	public:
		TextureLoader( EffekseerRenderer::Renderer* renderer );
		virtual ~TextureLoader();

	public:
		void* Load( const EFK_CHAR* path );

		void Unload( void* data );

		std::wstring RootPath;
	};

	class SoundLoader
	: public ::Effekseer::SoundLoader
	{
	private:
		::Effekseer::SoundLoader* m_loader;
	
	public:
		SoundLoader( Effekseer::SoundLoader* loader );
		virtual ~SoundLoader();

	public:
		void* Load( const EFK_CHAR* path );

		void Unload( void* data );
		
		std::wstring RootPath;
	};

public:
	Native();

	~Native();

	bool CreateWindow_Effekseer( void* handle, int width, int height );

	bool UpdateWindow();

	bool ResizeWindow( int width, int height );

	bool DestroyWindow();

	bool LoadEffect( void* data, int size, const wchar_t* path );

	bool RemoveEffect();

	bool PlayEffect();

	bool StopEffect();

	bool StepEffect( int frame );

	bool Rotate( float x, float y );

	bool Slide( float x, float y );

	bool Zoom( float zoom );

	bool SetRandomSeed( int seed );

	bool Record( const wchar_t* path, int32_t xCount, int32_t yCount, int32_t offsetFrame, int32_t frameSkip, bool isTranslucent );

	ViewerParamater GetViewerParamater();

	void SetViewerParamater( ViewerParamater& paramater );

	bool SetSoundMute( bool mute );

	bool SetSoundVolume( float volume );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DLL_H__