#pragma once
#include "Engine/Math/Vec2.hpp"
#include <string>

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct buffer_attribute_t;
enum eShaderType
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT
};

class ShaderStage
{
	public:
		~ShaderStage();
		bool Compile(RenderContext* ctx, std::string const& filename, void const* source, size_t const sourceByteLen, eShaderType stage);
		bool IsValid() const { return m_handle; }
		void const* GetByteCode() const;
		size_t GetByteCodeLength() const;
	public:
		eShaderType m_type = SHADER_TYPE_FRAGMENT;
		ID3D10Blob* m_byteCode = nullptr;
		union {
			ID3D11Resource* m_handle = nullptr;
			ID3D11VertexShader* m_vs;
			ID3D11PixelShader* m_fs;
		};

};
class Shader
{
public:
	Shader(RenderContext* owner);
	~Shader();
	void CreateRasterState();
	bool CreateFromFile(std::string const& filename);

	ID3D11InputLayout* GetOrCreateInputLayout(buffer_attribute_t const* attribs);
public:
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;
	RenderContext* m_owner;

	//ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
	const buffer_attribute_t* m_inputLayoutAttribLast = nullptr;
};