#pragma once
struct ID3D11Buffer;
class RenderContext;
//A GPU Allocator(like malloc on the GPU, new byte[count])
typedef unsigned int uint;
#define BIT_FLAG(b)		(1<<(b))

// The "WHAT" are we use it for
enum eRenderBufferUsageBit : uint
{
	VERTEX_BUFFER_BIT	= BIT_FLAG(0),//A02 - VBO	
	INDEX_BUFFER_BIT	= BIT_FLAG(1),//A05 - IBO
	UNIFORM_BUFFER_BIT	= BIT_FLAG(2),//A03 - UBO
};
typedef uint eRenderBufferUsage;

// The "HOW" are we going to access it
enum eRenderMemoryHint : uint
{
	//MEMORY_HINT_STATIC
	MEMORY_HINT_GPU,		//GPU can read/write, cpu can't touch it. If we change it, it changes rarely from CPU
	MEMORY_HINT_DYNAMIC,	//GPU memory (read/write), that changes OFTEN from the CPU - it allows to 'Map' the memory
	MEMORY_HINT_STAGING,	//CPU read/write, and can copy from cpu-gpu
};

class RenderBuffer
{
public:
	RenderBuffer(RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memHint);
	~RenderBuffer();
	bool Update(void const* data, size_t dataByteSize, size_t elementByteSize);
	bool IsCompatible(size_t dataByteSize, size_t elementByteSize);
	void ClearUp();
	ID3D11Buffer* GetHandle() { return m_handle; }
private:
	bool Create(size_t dataByteSize, size_t elementByteSize);
public:
	RenderContext* m_owner;
	ID3D11Buffer* m_handle;

	eRenderBufferUsage m_usage;
	eRenderMemoryHint m_memHint;

	size_t m_bufferByteSize;
	size_t m_elementByteSize;
};
struct buffer_attribute_t;
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer(RenderContext* ctx, eRenderMemoryHint memHint);
public:
	const buffer_attribute_t* m_layout = nullptr;
};

class IndexBuffer : public RenderBuffer
{
public:
	IndexBuffer(RenderContext* ctx, eRenderMemoryHint memHint);
};