#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <stdio.h>
#include <d3dcompiler.h>

Shader::Shader(RenderContext* owner)
	: m_owner(owner)
{
	//CreateRasterState();
}

Shader::~Shader()
{
	//DX_SAFE_RELEASE(m_rasterState);
	DX_SAFE_RELEASE(m_inputLayout);
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout(buffer_attribute_t const* attribs)
{
	if (m_inputLayout != nullptr || attribs == m_inputLayoutAttribLast)
	{
		return m_inputLayout;
	}
	else
	{
		DX_SAFE_RELEASE(m_inputLayout);
	}
	D3D11_INPUT_ELEMENT_DESC vertexDescription[32];
	int attribIndex = 0;
	while (strcmp(attribs[attribIndex].name, "") != 0)
	{
		buffer_attribute_t attrib = attribs[attribIndex];
		const char* n = attrib.name;
		
		vertexDescription[attribIndex].SemanticIndex = 0;// array element
		vertexDescription[attribIndex].SemanticName = n;
		switch (attrib.type)
		{
		case BUFFER_FORMAT_VEC2:
			vertexDescription[attribIndex].Format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case BUFFER_FORMAT_R8G8B8A8_UNORM:
			vertexDescription[attribIndex].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case BUFFER_FORMAT_VEC3:
			vertexDescription[attribIndex].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case BUFFER_FORMAT_VEC4:
			vertexDescription[attribIndex].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			vertexDescription[attribIndex].Format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		}
		vertexDescription[attribIndex].InputSlot = 0;
		vertexDescription[attribIndex].AlignedByteOffset = attrib.offset;
		vertexDescription[attribIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexDescription[attribIndex].InstanceDataStepRate = 0;
		attribIndex++;
	}

	m_inputLayoutAttribLast = attribs;
	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout(
		vertexDescription, attribIndex,
		m_vertexStage.GetByteCode(), m_vertexStage.GetByteCodeLength(),
		&m_inputLayout);
	return m_inputLayout;
}

void* FileReadToNewBuffer(std::string const& filename, size_t* out_size)//move to somewhere like FileUtils class someday
{
	FILE* fp = nullptr; 
	fopen_s(&fp, filename.c_str(), "rb");
	if (fp == nullptr)
	{
		return nullptr;
	}
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);

	unsigned char* buffer = new unsigned char[file_size + 1];
	if (nullptr != buffer)
	{
		fseek(fp, 0, SEEK_SET);
		size_t bytes_read = fread(buffer, 1, (size_t)file_size, fp);
		buffer[bytes_read] = NULL;

		if (out_size != nullptr)
		{
			*out_size = (size_t)file_size;
		}
	}

	

	fclose(fp);

	return buffer;
}

static char const* GetDefaultEntryPointForStage(eShaderType type)
{
	switch (type)
	{
	case SHADER_TYPE_VERTEX: return "VertexFunction"; break;
	case SHADER_TYPE_FRAGMENT: return "FragmentFunction"; break;
	default: GUARANTEE_OR_DIE(false, "Unimplemented Stage"); return ""; break;
	}
}

static char const* GetShaderModelForStage(eShaderType type)
{
	switch (type)
	{
	case SHADER_TYPE_VERTEX: return "vs_5_0"; break;
	case SHADER_TYPE_FRAGMENT: return "ps_5_0"; break;
	default:GUARANTEE_OR_DIE(false, "Unimplemented Stage"); return ""; break;
	}
}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_byteCode);
	DX_SAFE_RELEASE(m_handle);
}

bool ShaderStage::Compile(RenderContext* ctx, std::string const& filename, void const* source, size_t const sourceByteLen, eShaderType stage)
{
	//HLSL

	//Device Assembly - This is device specific
	//Compile: HLSL -> bytecode
	//Link Bytecode->device assembly

	
	DWORD compileFlags = 0U;
	#if defined(DEBUG_SHADERS)
		compile_flags |= D3DCOMPILE_DEBUG;
		compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
	#else 
		// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
		compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
	#endif

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;
	char const* entrypoint = GetDefaultEntryPointForStage(stage);
	char const* shaderModel = GetShaderModelForStage(stage);
	HRESULT hr = ::D3DCompile(source,
		sourceByteLen,						// plain text source code
		filename.c_str(),                   // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		shaderModel,                        // Compile Target (MSDN - "Specifying Compiler Targets")
		compileFlags,						// Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&byteCode,							// [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors);							// [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr)) {
		if (errors != nullptr) {
			char* errorString = (char*)errors->GetBufferPointer();
			DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
				filename.c_str(),
				errorString);
			//DEBUGBREAK();
		}
	}
	else {
		ID3D11Device* device = ctx->m_device;
		void const* byteCodePtr = byteCode->GetBufferPointer();
		size_t byteCodeSize = byteCode->GetBufferSize();

		switch (stage)
		{
		case SHADER_TYPE_VERTEX: 
		{
			hr = device->CreateVertexShader(byteCodePtr, byteCodeSize, nullptr, &m_vs);
			GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to link shader stage"); 
		} break;
		case SHADER_TYPE_FRAGMENT:
		{
			hr = device->CreatePixelShader(byteCodePtr, byteCodeSize, nullptr, &m_fs);
			GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to link shader stage");
		} break;
		default: GUARANTEE_OR_DIE(false, "Unimplemented Stage"); break;
		}
	}

	DX_SAFE_RELEASE(errors);
	//bytecode
	if (stage == SHADER_TYPE_VERTEX)
	{
		m_byteCode = byteCode;
	}
	else
	{
		DX_SAFE_RELEASE(byteCode);
		m_byteCode = nullptr;
	}

	m_type = stage;

	return IsValid();
}

bool Shader::CreateFromFile(std::string const& filename)
{
	size_t filesize = 0;
	//src = new char[g_DefaultShaderCode.size()];
	//memcpy(src, &g_DefaultShaderCode[0], g_DefaultShaderCode.size());
	void* src = FileReadToNewBuffer(filename, &filesize);
	if (src == nullptr)
	{
		return false;
	}

	if (!m_vertexStage.Compile(m_owner, filename, src, filesize, SHADER_TYPE_VERTEX)
		|| !m_fragmentStage.Compile(m_owner, filename, src, filesize, SHADER_TYPE_FRAGMENT))
	{
		delete[] src;
		src = new char[g_DefaultShaderCode.size()];
		memcpy(src, &g_DefaultShaderCode[0], g_DefaultShaderCode.size());
		m_vertexStage.Compile(m_owner, "defaultShader", src, g_DefaultShaderCode.size(), SHADER_TYPE_VERTEX);
		m_fragmentStage.Compile(m_owner, "defaultShader", src, g_DefaultShaderCode.size(), SHADER_TYPE_FRAGMENT);
	}	

	delete[] src;
	return false;
}

void Shader::CreateRasterState()
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = D3D11_FILL_SOLID;	//Full triangle
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE; // the only reason we're doing this; 
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	//device->CreateRasterizerState(&desc, &m_rasterState);
}

void const* ShaderStage::GetByteCode() const
{
	return m_byteCode->GetBufferPointer();
}
size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}