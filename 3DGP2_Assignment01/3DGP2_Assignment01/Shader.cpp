//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "Scene.h"

default_random_engine dre;
uniform_real_distribution<float> uid(1000.0, 4500.0);
uniform_real_distribution<float> uid2(2000.0, 4000.0);

CShader::CShader()
{
	//m_d3dSrvCPUDescriptorStartHandle.ptr = NULL;
	//m_d3dSrvGPUDescriptorStartHandle.ptr = NULL;
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}

	//if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char* pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE* pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE* pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE* pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char*)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE; // 투명한 물체에 대한 알파 블렌딩 설정 경우 FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE; //다중 렌더링 사용시
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE; //논리 연산 사용 경우
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; //픽셀 색상에 곱하는 값 (픽셀 셰이더 색상)
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; // Src(ONE)+Dst(ZERO)는 현재 픽셀 색상 그대로 (렌더 타겟 대상 색상)
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 더하는 연산
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);
}

//void CShader::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
//{
//	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
//	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
//	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	d3dDescriptorHeapDesc.NodeMask = 0;
//	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);
//
//	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
//	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
//	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
//
//	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
//	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
//}
//
//void CShader::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
//{
//	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
//	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
//	d3dCBVDesc.SizeInBytes = nStride;
//	for (int j = 0; j < nConstantBufferViews; j++)
//	{
//		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
//		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
//		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
//		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
//	}
//}
//
//void CShader::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
//{
//	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
//	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
//
//	int nTextures = pTexture->GetTextures();
//	UINT nTextureType = pTexture->GetTextureType();
//	for (int i = 0; i < nTextures; i++)
//	{
//		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
//		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
//		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
//		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
//		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
//		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
//	}
//	int nRootParameters = pTexture->GetRootParameters();
//	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
//}
//
//void CShader::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex)
//{
//	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
//	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
//	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
//	{
//		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
//		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
//		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
//
//		pTexture->SetGpuDescriptorHandle(nIndex, m_d3dSrvGPUDescriptorNextHandle);
//		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
//	}
//}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);

	//if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{

	OnPrepareRender(pd3dCommandList, nPipelineState);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CSkyBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CStandardShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

float Random(float fMin, float fMax)
{
	float fRandomValue = (float)rand();
	if (fRandomValue < fMin) fRandomValue = fMin;
	if (fRandomValue > fMax) fRandomValue = fMax;
	return(fRandomValue);
}

float Random()
{
	return  static_cast<float>(std::rand()) / RAND_MAX;

}
XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace)
{
	float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
	xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
	xmf3Position.y = xmf3Center.y - (nColumn * float(nColumnSpace) / 2.0f) + (nColumn * nColumnSpace) + Random();
	xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}
XMFLOAT3 RandomPositionInCircle(XMFLOAT3 xmf3Center, float fRadius)
{
	float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
	xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
	xmf3Position.y = xmf3Center.y;
	xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}


void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nObjects = 50;
	m_ppObjects = new CGameObject * [m_nObjects];
	m_pAngle = new float[m_nObjects];
	m_pX = new float[m_nObjects];
	m_pY = new float[m_nObjects];
	m_pRadius = new float[m_nObjects];
	m_pRotateSpeed = new float[m_nObjects];
	for (int i = 0; i < m_nObjects; i++) {
		m_pAngle[i] = 0.f;
		m_pRotateSpeed[i] = (float)((rand() % 50 + 40) / 100.0f);
		m_pRadius[i] = rand() % 40 + 20;
	}
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50); //SuperCobra(17), Gunship(2)

	CGameObject* pSuperCobraModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Cylinder001.bin", this);
	CGameObject* pGunshipModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Cylinder001.bin", this);

	int nColumnSpace = 5, nColumnSize = 30;
	int nFirstPassColumnSize = (m_nObjects % nColumnSize) > 0 ? (nColumnSize - 1) : nColumnSize;

	int nObjects = 0;
	for (int h = 0; h < nFirstPassColumnSize; h++)
	{
		for (int i = 0; i < floor(float(m_nObjects) / float(nColumnSize)); i++)
		{
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel);
				pSuperCobraModel->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel);
				pGunshipModel->AddRef();
			}
			m_ppObjects[nObjects]->SetPosition(RandomPositionInSphere(XMFLOAT3(3500, 200.0f, 2800), Random(20.0f, 100.0f), h - int(floor(nColumnSize / 2.0f)), nColumnSpace));
			m_ppObjects[nObjects]->Rotate(0.0f, 180.0f, 0.0f);
			m_ppObjects[nObjects]->SetOOBB(5, 5, 5);
			m_ppObjects[nObjects]->SetScale(2.0, 2.0, 2.0);
			m_pX[nObjects] = m_ppObjects[nObjects]->GetPosition().x;
			m_pY[nObjects] = m_ppObjects[nObjects]->GetPosition().y;
			m_ppObjects[nObjects++]->PrepareAnimate();

		}
	}

	if (nFirstPassColumnSize != nColumnSize)
	{
		for (int i = 0; i < m_nObjects - int(floor(float(m_nObjects) / float(nColumnSize)) * nFirstPassColumnSize); i++)
		{
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel);
				pSuperCobraModel->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel);
				pGunshipModel->AddRef();
			}
			m_ppObjects[nObjects]->SetPosition(RandomPositionInSphere(XMFLOAT3(3500, 250.0f, 2800), Random(20.0f, 100.0f), nColumnSize - int(floor(nColumnSize / 2.0f)), nColumnSpace));
			m_ppObjects[nObjects]->Rotate(0.0f, 180.0f, 0.0f);
			m_ppObjects[nObjects]->SetOOBB(5, 5, 5);
			m_ppObjects[nObjects]->SetScale(2.0, 2.0, 2.0);
			m_pX[nObjects] = m_ppObjects[nObjects]->GetPosition().x;
			m_pY[nObjects] = m_ppObjects[nObjects]->GetPosition().y;
			m_ppObjects[nObjects++]->PrepareAnimate();
		}
	}



	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;

	}
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{



	/*for (int j = 0; j < m_nObjects; j++) {


		if (m_ppObjects[j]->hitByBullet) {
			HitEffect(j, fTimeElapsed);

		}

		if (m_ppObjects[j]->die) {

			m_ppObjects[j]->MoveUp(-1.0f);

			if (m_ppObjects[j]->GetPosition().y < -300) {

				m_ppObjects[j]->SetActive(false);
				m_ppObjects[j]->UpdateBoundingBox();

			}
			else {
				ShakeEffect(j, fTimeElapsed);
			}
		}
	}*/

	/*for (int j = 0; j < m_nObjects; j++) {
		m_ppObjects[j]->Animate(fTimeElapsed);
	}*/
}

void CObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CObjectsShader::CollideEffect(int j)
{

	if (++tick % 2 == 0)
		m_ppObjects[j]->MoveStrafe(1.5f);
	else
		m_ppObjects[j]->MoveStrafe(-1.5f);
}
void CObjectsShader::ShakeEffect(int j, float fTimeElapsed)
{
	// 셰이킹 속도와 범위를 정의합니다.
	float shakeSpeed = 10.0f; // 예시로 10.0f로 설정
	float shakeRange = 0.3f; // 예시로 0.1f로 설정

	// 경과 시간에 따라 셰이킹합니다.
	static float direction = 1.0f;
	static float elapsedTime = 0.0f;

	elapsedTime += fTimeElapsed;

	if (elapsedTime >= 0.1f) // 셰이킹 간격 설정 (예시로 0.1초로 설정)
	{
		direction *= -1.0f; // 방향을 반대로 변경
		elapsedTime = 0.0f;
	}

	float shakeAmount = direction * shakeRange * sin(shakeSpeed * elapsedTime);

	m_ppObjects[j]->MoveStrafe(shakeAmount);
}

void CObjectsShader::HitEffect(int j, float fTimeElapsed)
{

	//HitEffectTimeElapsed += fTimeElapsed;
	//if (HitEffectTimeElapsed < HitRotationDuration) {
	//	//m_ppObjects[j]->isHit = 0.6;
	//	m_ppObjects[j]->Rotate(0.0f, 0.f, 120.0f * fTimeElapsed);
	//}
	//else if (HitEffectTimeElapsed < 3 * HitRotationDuration)
	//{
	//	// 두 번째 회전


	//	m_ppObjects[j]->Rotate(0.0f, 0.0f, -120.0f * fTimeElapsed);
	//}
	//else
	//{
	//	// 세 번째 회전
	//	m_ppObjects[j]->Rotate(0.0f, 0.0f, 120.0f * fTimeElapsed);
	//}

	//// 회전 시간이 경과하면 초기화
	//if (HitEffectTimeElapsed >= 4 * HitRotationDuration)
	//{
	//	HitEffectTimeElapsed = 0.0f;
	//	m_ppObjects[j]->hitByBullet = false;

	//}

}

//void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {
//	for (int j = 0; j < m_nObjects; j++) {
//		
//		m_ppObjects[j]->UpdateShaderVariables(pd3dCommandList);
//	}
//}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{	//m_ppObjects[j]->UpdateBoundingBox();
			//m_ppObjects[j]->Animate(0.16f);
			//m_ppObjects[j]->Animate(0.16f);



			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PlayerStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CTexturedShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CTexturedShader::ReleaseShaderVariables()
{
	//
}

void CTexturedShader::ReleaseUploadBuffers()
{


}

void CTexturedShader::ReleaseObjects()
{

}

void CTexturedShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);


}

void CTexturedShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


CBulletsShader::CBulletsShader()
{
}

CBulletsShader::~CBulletsShader()
{
}

void CBulletsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 4);

	CGameObject* pBulletMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Missile.bin", this);

	m_ppBullets = new CBulletObject * [BULLETS];
	for (int i = 0; i < BULLETS; i++) {


		//pBulletMesh->Rotate(0.f, 60.f, 0.f);
		m_ppBullets[i] = new CBulletObject(200);
		m_ppBullets[i]->SetChild(pBulletMesh, true);
		pBulletMesh->AddRef();
		//pBulletMesh->SetScale(2., 2., 2.);
	//	m_ppBullets[i]->SetScale(0.1, 0.1, 0.1);
		m_ppBullets[i]->SetOOBB(6, 6, 16);
		m_ppBullets[i]->SetMovingSpeed(50.0f);
		m_ppBullets[i]->UpdateTransform(NULL);
		m_ppBullets[i]->SetActive(false);
		m_ppBullets[i]->m_pPlayer = m_pPlayer;


	}
	m_pContextforAnimation = pContext;
}


void CBulletsShader::AnimateObjects(float fTimeElapsed)
{
	if (static_cast<CTankPlayer*>(m_pPlayer)->machine_mode) {
		for (int i = 0; i < BULLETS; i++) {
			m_ppBullets[i]->SetMovingSpeed(100.f);
		}
	}
	else {
		for (int i = 0; i < BULLETS; i++) {
			m_ppBullets[i]->SetMovingSpeed(50.0f);
		}
	}

	XMFLOAT3 Axis(0.f, 0.f, 1.f);
	
	All_nonActive = true;
	/*if (All_nonActive) {
		static_cast<CTankPlayer*>(m_pPlayer)->Fired_Bullet = false;
	}*/
	for (int i = 0; i < BULLETS; i++)
	{
		
		if (m_ppBullets[i]->m_bActive) {
			All_nonActive = false;
			m_ppBullets[i]->Rotate(&Axis, 360.0f * fTimeElapsed);
			m_ppBullets[i]->Animate(fTimeElapsed, m_pContextforAnimation);

		}

	}

}

void CBulletsShader::ReleaseObjects()
{
	if (m_ppBullets)
	{
		for (int j = 0; j < BULLETS; j++) if (m_ppBullets[j]) m_ppBullets[j]->Release();
		delete[] m_ppBullets;
	}

}

void CBulletsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < BULLETS; j++) if (m_ppBullets[j]) m_ppBullets[j]->ReleaseUploadBuffers();
}



void CBulletsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int i = 0; i < BULLETS; i++) {
		m_ppBullets[i]->UpdateTransform(NULL);
		
		if (m_ppBullets[i]->m_bActive && !m_ppBullets[i]->Collided) {

			m_ppBullets[i]->Render(pd3dCommandList, pCamera);
		}
	}
}

D3D12_SHADER_BYTECODE CBulletsShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBulletsShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSBullet", "ps_5_1", &m_pd3dPixelShaderBlob));
}

CBillboardShader::CBillboardShader()
{
}

CBillboardShader::~CBillboardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
{

	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSBillboard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSBillboard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_RASTERIZER_DESC CBillboardShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CBillboardShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CBillboardShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
}

void CBillboardShader::ReleaseObjects()
{
}

void CBillboardShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void CBillboardShader::ReleaseUploadBuffers()
{
}

void CBillboardShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

bool CShader::ReturnTerrainHeightDifference(float h1, float h2, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	if (pTerrain->GetHeight(h1, h2) < 190.0f) return false;
	if (abs(pTerrain->GetHeight(h1, h2) - pTerrain->GetHeight(h1 + 10, h2)) > 3.0f || abs(pTerrain->GetHeight(h1, h2) - pTerrain->GetHeight(h1 - 10, h2)) > 3.0f
		|| abs(pTerrain->GetHeight(h1, h2) - pTerrain->GetHeight(h1, h2 + 10)) > 3.0f || abs(pTerrain->GetHeight(h1, h2) - pTerrain->GetHeight(h1, h2 - 10)) > 3.0f) {
		return false;
	}
	else {
		return true;
	}


}

CBuildingShader::CBuildingShader()
{
}

CBuildingShader::~CBuildingShader()
{
}

void CBuildingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 9);
	//CGameObject* pBulletMesh= CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/dagger.bin", this);
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	CGameObject* pBuilding_threeMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/DB_three.bin", this);
	CGameObject* pBuilding_fiveMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/DB_five.bin", this);
	CGameObject* pBuilding_fourMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/DB_four.bin", this);


	m_nBuildings_type_five = 5;
	m_nBuildings_type_four = 5;
	m_nBuildings_type_three = 5;

	m_ntotal_Buildings = m_nBuildings_type_three + m_nBuildings_type_four + m_nBuildings_type_five;
	m_ppBuildings = new CGameObject * [m_ntotal_Buildings];
	m_ppBuildingsHeight = new int[m_ntotal_Buildings];
	for (int i = 0; i < 5; i++) {
		XMFLOAT3 xmf3RandomPosition{ uid(dre),0,uid(dre) };

		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		int z = (int)(xmf3RandomPosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		float fHeight = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		while (!ReturnTerrainHeightDifference(xmf3RandomPosition.x, xmf3RandomPosition.z, pContext)) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppBuildingsHeight[i] = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		m_ppBuildings[i] = new CGameObject(0, 0);
		m_ppBuildings[i]->SetChild(pBuilding_threeMesh);
		pBuilding_threeMesh->AddRef();
		m_ppBuildings[i]->SetPosition(xmf3RandomPosition.x, m_ppBuildingsHeight[i], xmf3RandomPosition.z);
		//m_ppBuildings[i]->Rotate(0.f, 90.0f * i, 0.f);
	//	m_ppBuildings[i]->SetOOBB(130.0f, 80.5f, 120.0f);
		m_ppBuildings[i]->SetOOBB(196.0f, 120.f, 180.0f);

	}
	for (int i = 5; i < 10; i++) {
		//XMFLOAT3 BuildingPos = RandomPositionInCircle(XMFLOAT3(3000, 330, 3000), 3000);
		//terrain 가로 세로 6425 가운데는 3212 3212
		XMFLOAT3 xmf3RandomPosition{ uid(dre),0,uid(dre) };
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		int z = (int)(xmf3RandomPosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		float fHeight = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		while (!ReturnTerrainHeightDifference(xmf3RandomPosition.x, xmf3RandomPosition.z, pContext)) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppBuildingsHeight[i] = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		m_ppBuildings[i] = new CGameObject(0, 0);
		m_ppBuildings[i]->SetChild(pBuilding_fiveMesh);
		pBuilding_fiveMesh->AddRef();
		m_ppBuildings[i]->SetPosition(xmf3RandomPosition.x, m_ppBuildingsHeight[i], xmf3RandomPosition.z);
		//m_ppBuildings[i]->Rotate(0.f, 90.0f * i, 0.f);
		m_ppBuildings[i]->SetOOBB(168.0f, 60.5f, 146.0f);
	}
	for (int i = 10; i < 15; i++) {
		//XMFLOAT3 BuildingPos = RandomPositionInCircle(XMFLOAT3(3000, 330, 3000), 3000);
		//terrain 가로 세로 6425 가운데는 3212 3212
		XMFLOAT3 xmf3RandomPosition{ uid(dre),0,uid(dre) };
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		int z = (int)(xmf3RandomPosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		float fHeight = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		while (!ReturnTerrainHeightDifference(xmf3RandomPosition.x, xmf3RandomPosition.z, pContext)) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppBuildingsHeight[i] = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		m_ppBuildings[i] = new CGameObject(0, 0);
		m_ppBuildings[i]->SetChild(pBuilding_fourMesh);
		pBuilding_fourMesh->AddRef();
		m_ppBuildings[i]->SetPosition(xmf3RandomPosition.x, m_ppBuildingsHeight[i], xmf3RandomPosition.z);
		m_ppBuildings[i]->SetOOBB(168.0f, 60.5f, 196.0f); //실제 바운딩박스는 두배적게 안만들어짐 
	}
}

void CBuildingShader::ReleaseObjects()
{
	for (int i = 0; i < m_ntotal_Buildings; i++) {
		if (m_ppBuildings[i]) m_ppBuildings[i]->Release();

	}
	if (m_ppBuildings)delete m_ppBuildings;
}


void CBuildingShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_ntotal_Buildings; i++) {
		if (m_ppBuildings[i]) m_ppBuildings[i]->ReleaseUploadBuffers();

	}

}

void CBuildingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int i = 0; i < m_ntotal_Buildings; i++) {

		if (m_ppBuildings[i]) {
			m_ppBuildings[i]->UpdateTransform(NULL);
			m_ppBuildings[i]->Render(pd3dCommandList, pCamera);
		}
	}


}

CWindMillShader::CWindMillShader()
{
}

CWindMillShader::~CWindMillShader()
{
}

void CWindMillShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 9);
	//CGameObject* pBulletMesh= CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/dagger.bin", this);
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;


	m_nWindMills = 5;

	CGameObject* pWindMillMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Village_WindMill.bin", this);


	m_ppWindMills = new CGameObject * [m_nWindMills];
	m_ppWindMillsHeight = new int[m_nWindMills];
	for (int i = 0; i < m_nWindMills; i++) {
		//XMFLOAT3 BuildingPos = RandomPositionInCircle(XMFLOAT3(3000, 330, 3000), 3000);
		//terrain 가로 세로 6425 가운데는 3212 3212
		XMFLOAT3 xmf3RandomPosition{ uid(dre),0,uid(dre) };
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		int z = (int)(xmf3RandomPosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		float fHeight = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		while (!ReturnTerrainHeightDifference(xmf3RandomPosition.x, xmf3RandomPosition.z, pContext)) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppWindMillsHeight[i] = pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z, bReverseQuad);
		m_ppWindMills[i] = new CWindMillObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppWindMills[i]->SetChild(pWindMillMesh);
		pWindMillMesh->AddRef();
		m_ppWindMills[i]->SetPosition(xmf3RandomPosition.x, m_ppWindMillsHeight[i], xmf3RandomPosition.z);
		m_ppWindMills[i]->Rotate(0.f, rand() % 270, 0.f);
		(m_ppWindMills[i])->SetOOBB(168.0f, 120.f, 168.0f);
		m_ppWindMills[i]->PrepareAnimate();

	}

}

void CWindMillShader::ReleaseObjects()
{
	for (int i = 0; i < m_nWindMills; i++) {
		if (m_ppWindMills[i]) m_ppWindMills[i]->Release();

	}
	if (m_ppWindMills)delete m_ppWindMills;
}



void CWindMillShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nWindMills; i++) {
		if (m_ppWindMills[i]) m_ppWindMills[i]->ReleaseUploadBuffers();

	}
}

void CWindMillShader::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nWindMills; i++) {
		if (m_ppWindMills[i])m_ppWindMills[i]->Animate(fTimeElapsed);


	}
}

void CWindMillShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int i = 0; i < m_nWindMills; i++) {
		if (m_ppWindMills[i]) {
			m_ppWindMills[i]->UpdateTransform(NULL);
			m_ppWindMills[i]->Render(pd3dCommandList, pCamera);
		}
	}

}

//----------------------------------------
CTerrainWaterShader::CTerrainWaterShader()
{
}

CTerrainWaterShader::~CTerrainWaterShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainWaterShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainWaterShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrainWater", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainWaterShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrainWater", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CTerrainWaterShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//-------------------
CRippleWaterShader::CRippleWaterShader()
{
}

CRippleWaterShader::~CRippleWaterShader()
{
}

D3D12_INPUT_LAYOUT_DESC CRippleWaterShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}



D3D12_SHADER_BYTECODE CRippleWaterShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSRippleWater", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CRippleWaterShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSRippleWater", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void  CRippleWaterShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC CRippleWaterShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CRippleWaterShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
CTankObjectsShader::CTankObjectsShader()
{
}

CTankObjectsShader::~CTankObjectsShader()
{
}

void CTankObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nTanks = 3;
	m_ppTankObjects = new CGameObject * [m_nTanks];

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 5);  //탱크 텍스쳐 5개

	CGameObject* pTankObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/EnemyTank.bin", this);

	for (int i = 0; i < m_nTanks; i++) {
		m_ppTankObjects[i] = new CTankObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext);
		m_ppTankObjects[i]->SetChild(pTankObject);
		pTankObject->AddRef();
		m_ppTankObjects[i]->SetOOBB(9.0, 6.0f, 19.0);
		CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
		m_ppTankObjects[i]->SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f + 100 * i, 260.0f, pTerrain->GetLength() * 0.5f));
		static_cast<CTankObject*>(m_ppTankObjects[i])->SetMovingDuration(5.0f * (i + 1));
		static_cast<CTankObject*>(m_ppTankObjects[i])->SetMovingSpeed(5.0f * (i + 1));
		static_cast<CTankObject*>(m_ppTankObjects[i])->SetRotationSpeed(-0.5f * (i + 1));

		/*	m_pPlayerShader = new CBulletsShader();
			((CBulletsShader*)m_pPlayerShader)->m_pPlayer = this;
			((CBulletsShader*)m_pPlayerShader)->m_pCamera = m_pCamera;
			m_pPlayerShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			m_pPlayerShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);*/
		m_ppTankObjects[i]->PrepareAnimate();
	}


}

void CTankObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nTanks; i++) {
		if (m_ppTankObjects[i])m_ppTankObjects[i]->Animate(fTimeElapsed);
	}
}

void CTankObjectsShader::ReleaseObjects()
{
	for (int i = 0; i < m_nTanks; i++) {
		if (m_ppTankObjects[i])m_ppTankObjects[i]->Release();
	}
	if (m_ppTankObjects)delete[] m_ppTankObjects;
}

void CTankObjectsShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTanks; i++) {
		if (m_ppTankObjects[i])m_ppTankObjects[i]->ReleaseUploadBuffers();
	}

}

void CTankObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int i = 0; i < m_nTanks; i++) {
		//m_ppWindMills[i]->Animate(0.16f);
		if (m_ppTankObjects[i]) {
			m_ppTankObjects[i]->UpdateTransform(NULL);
			m_ppTankObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}

}

CCactusAndRocksShader::CCactusAndRocksShader()
{
}

CCactusAndRocksShader::~CCactusAndRocksShader()
{
}

void CCactusAndRocksShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nCactus = 40;
	m_ppCactus = new CGameObject * [m_nCactus];
	m_nRock = 20;
	m_ppRocks = new CGameObject * [m_nRock];
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 2); //각각 1개 Albedo만.
	CGameObject* pRockObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock.bin", this);
	CGameObject* pCactusObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/cactus3.bin", this);

	//바위는 x,z 10~11
	for (int i = 0; i < m_nRock; i++) {
		m_ppRocks[i] = new CGameObject();
		m_ppRocks[i]->SetChild(pRockObject);
		pRockObject->AddRef();
		m_ppRocks[i]->SetOOBB(26.0, 20.0f, 22.0);
		CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
		XMFLOAT3 xmf3RandomPosition{ uid(dre),0,uid(dre) };
		while (pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z) < 190.0f) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppRocks[i]->SetPosition(xmf3RandomPosition.x, pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z), xmf3RandomPosition.z);



	}
	for (int i = 0; i < m_nCactus; i++) {
		m_ppCactus[i] = new CGameObject();
		m_ppCactus[i]->SetChild(pCactusObject);
		pCactusObject->AddRef();
		m_ppCactus[i]->SetOOBB(5.0, 10.0f, 5.0);
		CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
		XMFLOAT3 xmf3RandomPosition{ uid2(dre),0,uid2(dre) };
		while (pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z) < 190.0f) {
			xmf3RandomPosition.x = uid(dre);
			xmf3RandomPosition.z = uid(dre);
		}
		m_ppCactus[i]->SetPosition(xmf3RandomPosition.x, pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z), xmf3RandomPosition.z);
		//m_ppCactus[i]->SetScale(1.f, 0.1f, 1.f);


	}
	//for (int i = 2; i < 3; i++) {
	//	m_ppCactus[i] = new CGameObject();
	//	CGameObject* pCactusObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/trees.bin", this);
	//	m_ppCactus[i]->SetChild(pCactusObject);
	//	//m_ppCactus[i]->SetOOBB(9.0, 6.0f, 19.0);
	//	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	//	m_ppCactus[i]->SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f + 200, 260.0f, pTerrain->GetLength() * 0.5f));



	//}

}

void CCactusAndRocksShader::ReleaseObjects()
{
	for (int i = 0; i < m_nRock; i++) {
		if (m_ppRocks[i]) m_ppRocks[i]->Release();
	}

	for (int i = 0; i < m_nCactus; i++) {
		if (m_ppCactus[i]) m_ppCactus[i]->Release();

	}
	if (m_ppCactus)delete[] m_ppCactus;
	if (m_ppRocks)delete[] m_ppRocks;
}

void CCactusAndRocksShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nRock; i++) {
		if (m_ppRocks[i]) m_ppRocks[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_nCactus; i++) {
		if (m_ppCactus[i]) m_ppCactus[i]->ReleaseUploadBuffers();

	}
}

void CCactusAndRocksShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int i = 0; i < m_nRock; i++) {

		if (m_ppRocks[i]) {
			m_ppRocks[i]->UpdateTransform(NULL);
			m_ppRocks[i]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < m_nCactus; i++) {
		if (m_ppCactus[i]) {
			m_ppCactus[i]->UpdateTransform(NULL);
			m_ppCactus[i]->Render(pd3dCommandList, pCamera);
		}
	}
}


CTreeShader::CTreeShader()
{
}

CTreeShader::~CTreeShader()
{
}


void CTreeShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nMultipleTrees = 10;
	m_ppMultipleTrees = new CGameObject * [m_nMultipleTrees];
	m_nSingleTrees = 30;
	m_ppSingleTrees = new CGameObject * [m_nSingleTrees];
	//CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 4);
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	CGameObject* pMultipleTreeModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Trees.bin", this);
	CGameObject* pSingleTreeModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/OneTree.bin", this);


	for (int i = 0; i < m_nMultipleTrees; i++) {
		m_ppMultipleTrees[i] = new TreesObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppMultipleTrees[i]->SetChild(pMultipleTreeModel);
		pMultipleTreeModel->AddRef();
		XMFLOAT3 xmf3RandomPosition{ uid2(dre),0,uid2(dre) };
		while (pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z) < 190.0f) {
			xmf3RandomPosition.x = uid2(dre);
			xmf3RandomPosition.z = uid2(dre);
		}
		m_ppMultipleTrees[i]->SetPosition(xmf3RandomPosition.x, pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z), xmf3RandomPosition.z);

	}
	for (int i = 0; i < m_nSingleTrees; i++) {
		m_ppSingleTrees[i] = new TreesObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppSingleTrees[i]->SetChild(pSingleTreeModel);
		pSingleTreeModel->AddRef();
		XMFLOAT3 xmf3RandomPosition{ uid2(dre),0,uid2(dre) };
		while (pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z) < 190.0f) {
			xmf3RandomPosition.x = uid2(dre);
			xmf3RandomPosition.z = uid2(dre);
		}
		m_ppSingleTrees[i]->SetPosition(xmf3RandomPosition.x, pTerrain->GetHeight(xmf3RandomPosition.x, xmf3RandomPosition.z), xmf3RandomPosition.z);

	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTreeShader::ReleaseObjects()
{
	if (m_ppMultipleTrees)
	{
		for (int j = 0; j < m_nMultipleTrees; j++) if (m_ppMultipleTrees[j]) m_ppMultipleTrees[j]->Release();
		delete[] m_ppMultipleTrees;
	}
	if (m_ppSingleTrees)
	{
		for (int j = 0; j < m_nSingleTrees; j++) if (m_ppSingleTrees[j]) m_ppSingleTrees[j]->Release();
		delete[] m_ppSingleTrees;
	}
}

D3D12_SHADER_BYTECODE CTreeShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTree", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_BLEND_DESC CTreeShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CTreeShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_RASTERIZER_DESC CTreeShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 1.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

void CTreeShader::AnimateObjects(float fTimeElapsed)
{
	/*for (int j = 0; j < m_nMultipleTrees; j++) {
		if (m_ppMultipleTrees[j]) m_ppMultipleTrees[j]->Animate(fTimeElapsed);

	}
	for (int j = 0; j < m_nSingleTrees; j++) {
		if (m_ppSingleTrees[j]) m_ppSingleTrees[j]->Animate(fTimeElapsed);

	}*/
}

void CTreeShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nMultipleTrees; j++) if (m_ppMultipleTrees[j]) m_ppMultipleTrees[j]->ReleaseUploadBuffers();
	for (int j = 0; j < m_nSingleTrees; j++) if (m_ppSingleTrees[j]) m_ppSingleTrees[j]->ReleaseUploadBuffers();
}

void CTreeShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int j = 0; j < m_nMultipleTrees; j++)
	{
		if (m_ppMultipleTrees[j])
		{
			m_ppMultipleTrees[j]->UpdateTransform(NULL); //이걸 해주고 Render해야 각 중첩 메쉬 오브젝트 그려짐 같은 메쉬사용하므로
			m_ppMultipleTrees[j]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int j = 0; j < m_nSingleTrees; j++)
	{
		if (m_ppSingleTrees[j])
		{
			m_ppSingleTrees[j]->UpdateTransform(NULL); //이걸 해주고 Render해야 각 중첩 메쉬 오브젝트 그려짐 같은 메쉬사용하므로
			m_ppSingleTrees[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

CMultiSpriteObjectsShader::CMultiSpriteObjectsShader()
{
	//m_nSpriteObjects = 2;
}

CMultiSpriteObjectsShader::~CMultiSpriteObjectsShader()
{
}

D3D12_RASTERIZER_DESC CMultiSpriteObjectsShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CMultiSpriteObjectsShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


D3D12_INPUT_LAYOUT_DESC CMultiSpriteObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CMultiSpriteObjectsShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSpriteAnimation", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CMultiSpriteObjectsShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSpriteAnimation", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//void CMultiSpriteObjectsShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	
//}
//
//void CMultiSpriteObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	
//}

//void CMultiSpriteObjectsShader::ReleaseShaderVariables()
//{
//	for (int j = 0; j < m_nSpriteObjects; j++) {
//		if (m_ppSpriteObjects[j]) m_ppSpriteObjects[j]->ReleaseShaderVariables();
//		
//
//	}
//
//}

void CMultiSpriteObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//void CMultiSpriteObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
//{
//	m_nSpriteObjects = 2;
//
//	CTexturedRectMeshWithOneVertex* pSpriteMesh = new CTexturedRectMeshWithOneVertex(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f);
//
//
//	m_ppSpriteObjects = new CMultiSpriteObject * [m_nSpriteObjects];
//
//	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 2);
//
//
//	CTexture* ppSpriteTextures[2];
//	ppSpriteTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 8, 8);
//	ppSpriteTextures[0]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/ceiling.dds", RESOURCE_TEXTURE2D, 0);
//
//
//	ppSpriteTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 6, 6);
//	ppSpriteTextures[1]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Explosion_6x6.dds", RESOURCE_TEXTURE2D, 0);
//
//	CreateShaderResourceViews(pd3dDevice, ppSpriteTextures[0], 0, 3);
//	CreateShaderResourceViews(pd3dDevice, ppSpriteTextures[1], 0, 3);
//
//	CMaterial* ppSpriteMaterials[2];
//
//	ppSpriteMaterials[0] = new CMaterial();
//	ppSpriteMaterials[0]->SetTexture(ppSpriteTextures[0]);
//	ppSpriteMaterials[0]->SetShader(this);
//
//	ppSpriteMaterials[1] = new CMaterial();
//	ppSpriteMaterials[1]->SetTexture(ppSpriteTextures[1]);
//	ppSpriteMaterials[1]->SetShader(this);
//
//	XMFLOAT3 xmf3Position = XMFLOAT3(3212, 255, 3212);
//	//CMultiSpriteObject* pSpriteObject = NULL;
//	for (int j = 0; j < m_nSpriteObjects; j++)
//	{
//		m_ppSpriteObjects[j] = new CMultiSpriteObject();
//		//m_ppSpriteObjects[j]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
//		m_ppSpriteObjects[j]->SetMesh(pSpriteMesh);
//		m_ppSpriteObjects[j]->SetMaterial(0, ppSpriteMaterials[j]);
//		m_ppSpriteObjects[j]->SetPosition(XMFLOAT3(xmf3Position.x, xmf3Position.y, xmf3Position.z));
//						 
//		m_ppSpriteObjects[j]->m_fSpeed = 3.0f / (ppSpriteTextures[j]->m_nRows * ppSpriteTextures[j]->m_nCols);
//		//m_ppSpriteObjects[j] = pSpriteObject;
//	}
//
//}

//void CMultiSpriteObjectsShader::ReleaseUploadBuffers()
//{
//	for (int j = 0; j < m_nSpriteObjects; j++) if (m_ppSpriteObjects[j]) m_ppSpriteObjects[j]->ReleaseUploadBuffers();
//	
//}
//
//void CMultiSpriteObjectsShader::ReleaseObjects()
//{
//	for (int j = 0; j < m_nSpriteObjects; j++) {
//	
//		if (m_ppSpriteObjects[j]) m_ppSpriteObjects[j]->Release();
//		
//
//	}
//}
//
//void CMultiSpriteObjectsShader::AnimateObjects(float fTimeElapsed)
//{
//	
//		for (int j = 0; j < m_nSpriteObjects; j++)
//		{
//			m_ppSpriteObjects[j]->Animate(fTimeElapsed);
//		}
//}

//void CMultiSpriteObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
//{
//	
//	if (m_bActive)
//	{
//	
//		XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
//		CPlayer* pPlayer = pCamera->GetPlayer();
//		XMFLOAT3 xmf3PlayerPosition = pPlayer->GetPosition();
//		XMFLOAT3 xmf3PlayerLook = pPlayer->GetLookVector();
//		xmf3PlayerPosition.y += 5.0f;
//		XMFLOAT3 xmf3Position = Vector3::Add(xmf3PlayerPosition, Vector3::ScalarProduct(xmf3PlayerLook, 50.0f, false));
//		for (int j = 0; j < m_nSpriteObjects; j++)
//		{
//			if (m_ppSpriteObjects[j])
//			{
//				m_ppSpriteObjects[j]->SetPosition(xmf3Position);
//				m_ppSpriteObjects[j]->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
//			}
//		}
//		CShader::Render(pd3dCommandList, pCamera);
//		for (int j = 0; j < m_nSpriteObjects; j++)
//		{
//			if (m_ppSpriteObjects[j])
//			{
//				m_ppSpriteObjects[j]->UpdateTransform(NULL); //이걸 해주고 Render해야 각 중첩 메쉬 오브젝트 그려짐 같은 메쉬사용하므로
//				m_ppSpriteObjects[j]->Render(pd3dCommandList, pCamera);
//			}
//		}
//		
//	
//	}
//}
