
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRenderer.Shader.h"
#include "EffekseerRenderer.Renderer.h"

#define LOG_OUTPUT
//#define LOG_OUTPUT	OutputDebugStringA

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::Shader( RendererImplemented* renderer, GLuint program )
	: DeviceObject		( renderer )
	, m_program			( program )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	glDeleteProgram(m_program);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader* Shader::Create( RendererImplemented* renderer, 
	const char* vs_src, const char* fs_src, const char* name )
{
	const char* src_data[1];
	GLint src_size[1];

	GLuint program, vert_shader, frag_shader;
	GLint res_vs, res_fs, res_link;
	
	assert( renderer != NULL );

	// バーテックスシェーダをコンパイル
	src_data[0] = vs_src;
	src_size[0] = (GLint)strlen(vs_src);
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, src_data, src_size);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	// フラグメントシェーダをコンパイル
	src_data[0] = fs_src;
	src_size[0] = strlen(fs_src);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, src_data, src_size);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res_fs);
	
	// シェーダプログラムの作成
	program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	
	// シェーダオブジェクトの削除
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	// シェーダプログラムのリンク
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &res_link);
	
#ifndef NDEBUG
	{
		// エラー出力
		char log[512];
		int32_t log_size;
		glGetShaderInfoLog(vert_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG_OUTPUT("VS: ");
			LOG_OUTPUT(log);
		}
		glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG_OUTPUT("FS: ");
			LOG_OUTPUT(log);
		}
		glGetProgramInfoLog(program, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG_OUTPUT("Link: ");
			LOG_OUTPUT(log);
		}
	}
#endif

	if (res_link == GL_FALSE) {
		glDeleteProgram(program);
		printf("Failed to compile shader \"\".", name);
		return NULL;
	}
	
	return new Shader( renderer, program );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnLostDevice()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnResetDevice()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
GLuint Shader::GetInterface() const
{
	return m_program;
}

GLint Shader::GetAttribId(const char* name) const
{
	return glGetAttribLocation(m_program, name);
}

GLint Shader::GetUniformId(const char* name) const
{
	return glGetUniformLocation(m_program, name);
}

void Shader::GetAttribIdList(int count, const ShaderAttribInfo* info, GLint* aid_list) const
{
	for (int i = 0; i < count; i++) {
		aid_list[i] = glGetAttribLocation(m_program, info[i].name);
	}
}

void Shader::GetUniformIdList(int count, const ShaderUniformInfo* info, GLint* uid_list) const
{
	for (int i = 0; i < count; i++) {
		uid_list[i] = glGetUniformLocation(m_program, info[i].name);
	}
}

void Shader::BeginScene()
{
	glUseProgram(m_program);
}

void Shader::EndScene()
{
	glUseProgram(0);
}

void Shader::EnableAttribs(int count, const GLint* aid_list)
{
	for (int i = 0; i < count; i++) {
		if (aid_list[i] >= 0) {
			glEnableVertexAttribArray(aid_list[i]);
		}
	}
}

void Shader::DisableAttribs(int count, const GLint* aid_list)
{
	for (int i = 0; i < count; i++) {
		if (aid_list[i] >= 0) {
			glDisableVertexAttribArray(aid_list[i]);
		}
	}
}

void Shader::SetVertex(int count, const ShaderAttribInfo* info, 
		const GLint* aid_list, const void* vertices, uint32_t stride)
{
	for (int i = 0; i < count; i++) {
		if (aid_list[i] >= 0) {
			glVertexAttribPointer(aid_list[i], info[i].count, info[i].type, 
				info[i].normalized, stride, (uint8_t*)vertices + info[i].offset);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}