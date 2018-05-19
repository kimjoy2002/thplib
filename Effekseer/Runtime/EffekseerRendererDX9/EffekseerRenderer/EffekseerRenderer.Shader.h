
#ifndef	__EFFEKSEERRENDERER_SHADER_H__
#define	__EFFEKSEERRENDERER_SHADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader
	: public DeviceObject
{
private:
	
	ID3DXEffect* m_shader;

	Shader( RendererImplemented* renderer, ID3DXEffect* shader );
public:
	virtual ~Shader();

	static Shader* Create( RendererImplemented* renderer, const uint8_t effect[], int32_t size, const char* name );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	ID3DXEffect* GetInterface() const;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_SHADER_H__