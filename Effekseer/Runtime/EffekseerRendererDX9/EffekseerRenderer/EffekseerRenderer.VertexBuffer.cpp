
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRenderer.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::VertexBuffer( RendererImplemented* renderer, IDirect3DVertexBuffer9* buffer, int size, bool isDynamic )
	: DeviceObject		( renderer )
	, VertexBufferBase	( size, isDynamic )
	, m_buffer			( buffer )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	ES_SAFE_RELEASE( m_buffer );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create( RendererImplemented* renderer, int size, bool isDynamic )
{
	// フラグ
	D3DPOOL pool = D3DPOOL_MANAGED;
	int usage = D3DUSAGE_WRITEONLY;

	if ( isDynamic )
	{
		pool = D3DPOOL_DEFAULT;
		usage |= D3DUSAGE_DYNAMIC;
	}

	IDirect3DVertexBuffer9* vb = NULL;
	HRESULT hr = renderer->GetDevice()->CreateVertexBuffer( size, usage, 0, pool, &vb, NULL );

	if( FAILED( hr ) )
	{
		// DirectX9ExではD3DPOOL_MANAGED使用不可
		pool = D3DPOOL_DEFAULT;
		hr = renderer->GetDevice()->CreateVertexBuffer( size, usage, 0, pool, &vb, NULL );
	}

	if( FAILED( hr ) ) return NULL;

	return new VertexBuffer( renderer, vb, size, isDynamic );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
	if ( m_isDynamic )
	{
		ES_SAFE_RELEASE( m_buffer );
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
	// バッファ生成
	if ( m_buffer == NULL )
	{
		D3DPOOL pool = D3DPOOL_MANAGED;
		int usage = D3DUSAGE_WRITEONLY;

		if ( m_isDynamic )
		{
			pool = D3DPOOL_DEFAULT;
			usage |= D3DUSAGE_DYNAMIC;
		}

		HRESULT hr = GetRenderer()->GetDevice()->CreateVertexBuffer( m_size, usage, 0, pool, &m_buffer, NULL );
		
		if( FAILED( hr ) )
		{
			// DirectX9ExではD3DPOOL_MANAGED使用不可
			pool = D3DPOOL_DEFAULT;
			hr = GetRenderer()->GetDevice()->CreateVertexBuffer( m_size, usage, 0, pool, &m_buffer, NULL );
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert( !m_isLock );

	m_isLock = true;
	m_resource = NULL;
	m_offset = 0;

	m_buffer->Lock( 0, 0, (void**)&m_resource, m_isDynamic ? D3DLOCK_DISCARD : 0 );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert( m_isLock );
	
	m_resource = NULL;
	m_buffer->Unlock();

	m_isLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------