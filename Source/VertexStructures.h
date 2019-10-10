#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

struct SkinnedVertexStruct {
	DirectX::XMFLOAT3					pos;
	DirectX::XMFLOAT3					normal;
	DirectX::XMFLOAT3					tangent;
	DirectX::PackedVector::XMCOLOR		matDiffuse;
	DirectX::PackedVector::XMCOLOR		matSpecular;
	DirectX::XMFLOAT2					texCoord;

	DirectX::XMFLOAT4					boneWeights;
	DirectX::XMINT4						boneIndices;

	DirectX::XMFLOAT4					boneWeights2;
	DirectX::XMINT4						boneIndices2;
	DirectX::XMFLOAT4					boneWeights3;
	DirectX::XMINT4						boneIndices3;
};

// Vertex input descriptor based on skinnedVertexStruct
static const D3D11_INPUT_ELEMENT_DESC skinVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SPECULAR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "INDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 68, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTSX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 84, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "INDICESX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 100, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTSY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 116, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "INDICESY", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 132, D3D11_INPUT_PER_VERTEX_DATA, 0 }


};


struct FlareVertexStruct {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 posL;
	DirectX::PackedVector::XMCOLOR		colour;
};



// Vertex input descriptor based on Flare VertexStruct
static const D3D11_INPUT_ELEMENT_DESC flareVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct BasicVertexStruct {
	DirectX::XMFLOAT3					pos;
	DirectX::PackedVector::XMCOLOR		colour;
};

// Add code here (Create an input element description)
// Vertex input descriptor based on BasicVertexStruct

static const D3D11_INPUT_ELEMENT_DESC basicVertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


struct ExtendedVertexStruct {
	DirectX::XMFLOAT3					pos;
	DirectX::XMFLOAT3					normal;
	DirectX::XMFLOAT3					tangent;
	DirectX::PackedVector::XMCOLOR		matDiffuse;
	DirectX::PackedVector::XMCOLOR		matSpecular;
	DirectX::XMFLOAT2					texCoord;
};

// Vertex input descriptor based on ExtendedVertexStruct
static const D3D11_INPUT_ELEMENT_DESC extVertexDesc[] = {
	//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "DIFFUSE", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "SPECULAR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }


	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SPECULAR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct ParticleVertexStruct {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 posL;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 data;//;[age,?,?]
};

// Vertex input descriptor based on ParticleVertexStruct
static const D3D11_INPUT_ELEMENT_DESC particleVertexDesc[] = {
	// Add Code Here (setup particle vertex description)
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "DATA", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
