#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/core/ErrorWarningAssert.hpp"

RenderBuffer::RenderBuffer(RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memHint)
{
	//initialize here to guarantee order
	m_owner = owner;
	m_usage = usage;
	m_memHint = memHint;

	m_handle = nullptr;
	m_bufferByteSize = 0U;
	m_elementByteSize = 0U;
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

bool RenderBuffer::IsCompatible(size_t dataByteSize, size_t elementByteSize)
{
	if (m_handle == nullptr)
	{
		return false;
	}
	if (m_elementByteSize != elementByteSize)
	{
		return false;
	}
	if (m_memHint == MEMORY_HINT_DYNAMIC)
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else
	{
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::ClearUp()
{
	DX_SAFE_RELEASE(m_handle);
	m_bufferByteSize = 0;
	m_elementByteSize = 0;
}

D3D11_USAGE ToDXMemoryUsage(eRenderMemoryHint hint)//TODO:move to d3d11common
{
	switch (hint)
	{
	case MEMORY_HINT_GPU: return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC: return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING: return D3D11_USAGE_STAGING;
	default: ERROR_AND_DIE("Unknown hint"); }
}

UINT ToDXUsage(eRenderBufferUsage usage)
{
	UINT ret = 0;
	if (usage & VERTEX_BUFFER_BIT)
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (usage & INDEX_BUFFER_BIT)
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if (usage & UNIFORM_BUFFER_BIT)
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}
	return ret;
}

bool RenderBuffer::Create(size_t dataByteSize, size_t elementByteSize)
{
	m_bufferByteSize = dataByteSize;
	m_elementByteSize = elementByteSize;
	ID3D11Device* device = m_owner->m_device;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = ToDXMemoryUsage(m_memHint);
	desc.BindFlags = ToDXUsage(m_usage);
	desc.CPUAccessFlags = 0;
	if (m_memHint == MEMORY_HINT_DYNAMIC)
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (m_memHint == MEMORY_HINT_STAGING)
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	desc.MiscFlags = 0;//used in special cases
	desc.StructureByteStride = (UINT)elementByteSize;

	device->CreateBuffer(&desc, nullptr, &m_handle);
	return (m_handle != nullptr);
}

bool RenderBuffer::Update(void const* data, size_t dataByteSize, size_t elementByteSize)
{
	if (!IsCompatible(dataByteSize, elementByteSize))
	{
		ClearUp();//destroy the handle, reset things
	}
	//1 if not compatible - destory the old buffer;
	//our elementSize matches the passed in
	//if we are GPU
		//bufferSizes MUST match
	//if we're dynamic
		//passed in buffer size is less than our bufferSize

	//2 if no buffer, create one that is compatible
	if (!m_handle)
	{
		Create(dataByteSize, elementByteSize);
	}
	//3 updating the buffer
	ID3D11DeviceContext* ctx = m_owner->m_context;
	if (m_memHint == MEMORY_HINT_DYNAMIC)
	{
		//Mapping - Only available to dynamic buffer
		
		D3D11_MAPPED_SUBRESOURCE mapped;
		//CPU->GPU memory copy
		HRESULT result = ctx->Map(m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(result))
		{
			memcpy(mapped.pData, data, dataByteSize);
			ctx->Unmap(m_handle, 0);
		}
		else
		{
			return false;
		}
	}

	else
	{
		//if this is MEMORY_HINT_GPU
		ctx->UpdateSubresource(m_handle, 0, nullptr, data, 0, 0);
	}
	
	//But, don't have reallocate if going smaller.

	//CopySubresource (direct copy)
	//This is only avaliable to GPU buffer that have exactly
	//The same size, and element size
	return true;
}

VertexBuffer::VertexBuffer(RenderContext* ctx, eRenderMemoryHint memHint)
	: RenderBuffer(ctx, VERTEX_BUFFER_BIT, memHint)
{
	m_layout = nullptr;
}

IndexBuffer::IndexBuffer(RenderContext* ctx, eRenderMemoryHint memHint)
	: RenderBuffer(ctx, INDEX_BUFFER_BIT, memHint)
{}