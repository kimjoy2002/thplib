
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ShaderAttribInfo
{
	const char*	name;
	GLenum		type;
	uint16_t	count;
	uint16_t	offset;
	bool		normalized;
};

struct ShaderUniformInfo
{
	const char*	name;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader
	: public DeviceObject
{
private:
	
	GLuint m_program;

	Shader( RendererImplemented* renderer, GLuint program );
public:
	virtual ~Shader();

	static Shader* Create( RendererImplemented* renderer, 
		const char* vs_src, const char* fs_src, const char* name );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	GLuint GetInterface() const;
	
	GLint GetAttribId(const char* name) const;
	GLint GetUniformId(const char* name) const;
	void GetAttribIdList(int count, const ShaderAttribInfo* info, GLint* aid_list) const;
	void GetUniformIdList(int count, const ShaderUniformInfo* info, GLint* uid_list) const;

	void BeginScene();
	void EndScene();
	void EnableAttribs(int count, const GLint* aid_list);
	void DisableAttribs(int count, const GLint* aid_list);
	void SetVertex(int count, const ShaderAttribInfo* info, 
		const GLint* aid_list, const void* vertices, uint32_t stride);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_SHADER_H__