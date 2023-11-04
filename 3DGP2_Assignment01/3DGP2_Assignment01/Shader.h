//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once

#include "Object.h"
#include "Camera.h"
#include "Player.h"
class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int									m_nReferences = 0;
protected:
	bool							m_bActive = true;

protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int									m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

	//ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;

	//D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorStartHandle;
	//D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorStartHandle;
	//D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorStartHandle;
	//D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorStartHandle;

	//D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorNextHandle;
	//D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorNextHandle;

public:


	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetActive(bool bActive) { m_bActive = bActive; }
	bool GetActive() { return(m_bActive); }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	/*void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);*/

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	bool ReturnTerrainHeightDifference(float h1, float h2, void* pContext);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
};

class CTerrainShader : public CShader
{
public:
	CTerrainShader() {};
	virtual ~CTerrainShader() {};

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CObjectsShader : public CStandardShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	///	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();
	int tick = 0;
	void CollideEffect(int j);
	void ShakeEffect(int j, float fTimeElapsed);
	void HitEffect(int j, float fTimeElapsed);
	float HitEffectTimeElapsed = 0.f;
	float HitRotationDuration = 0.15f;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	CAirplanePlayer* m_pPlayer = NULL;
public:

	float* m_pAngle = NULL;
	float* m_pRotateSpeed = NULL;
	float* m_pRadius = NULL;
	float* m_pX = NULL;
	float* m_pY = NULL;


	CGameObject** m_ppObjects = 0;
	int lifeCount = 10;
	int								m_nObjects = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

#define BULLETS 10
class CBulletsShader : public CStandardShader
{
public:
	CBulletsShader();
	virtual ~CBulletsShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual void ReleaseUploadBuffers();

	LPVOID						m_pContextforAnimation = NULL;

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	//CAirplanePlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;
	CPlayer* m_pPlayer = NULL;
	CPlayerBulletObject** m_ppBullets = 0;
	int								m_nBullets = 0;
	bool All_nonActive = false;
};

//class CBulletShader : public CStandardShader
//{
//public:
//	CBulletShader();
//	virtual ~CBulletShader();
//
//	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
//	virtual void AnimateObjects(float fTimeElapsed);
//	virtual void ReleaseObjects();
//
//
//	virtual void ReleaseUploadBuffers();
//
//	LPVOID						m_pContextforAnimation = NULL;
//
//	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
//
//
//	CPlayerBulletObject* m_pBullet =NULL;
//	
//	bool All_nonActive = false;
//};



class CBillboardShader :public CStandardShader {
public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();


	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void ReleaseUploadBuffers();

#ifdef _WITH_BATCH_MATERIAL
	CMaterial* m_ppGrassMaterials[2] = { NULL, NULL };
	CMaterial* m_ppFlowerMaterials[2] = { NULL, NULL };
#endif

	CRawFormatImage* m_pRawFormatImage = NULL;

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

class CBuildingShader :public CStandardShader {
public:
	CBuildingShader();
	virtual ~CBuildingShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	virtual void ReleaseObjects();


	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
	int* m_ppBuildingsHeight = NULL;




	CGameObject** m_ppBuildings = NULL;
	int m_ntotal_Buildings = 0;
	int m_nBuildings_type_four = 0;
	int m_nBuildings_type_three = 0;
	int m_nBuildings_type_five = 0;
	/*
	 BuildingThree => 축이 살짝 건물 기준 위에 있다.
	 -> 가로길이 약 98 세로 길이 약 90 높이 약 60 인데.
	 -> 정면이 계단이 보이는 쪽이고 계단 보이는 기준 오른쪽이 -x ,왼쪽이 +x ,뒷쪽이 -z ,앞쪽(계단 보이는) +z
	-> 중심 원점 축 기준 왼쪽 계단 끝은 +x인데 +55정도 오른쪽 끝은 -43정도 / 축기준 앞쪽 문쪽이 +z인데 +25 뒤쪽이 -z축 -65이다.



	*/
};

class CWindMillShader :public CStandardShader {
public:
	CWindMillShader();
	virtual ~CWindMillShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	virtual void ReleaseObjects();


	virtual void ReleaseUploadBuffers();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
	int* m_ppWindMillsHeight = NULL;

	CGameObject** m_ppWindMills = NULL;
	int m_nWindMills = 0;

};

class CTerrainWaterShader : public CStandardShader
{
public:
	CTerrainWaterShader();
	virtual ~CTerrainWaterShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};


class CRippleWaterShader : public CStandardShader
{
public:
	CRippleWaterShader();
	virtual ~CRippleWaterShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

};

class CTankObjectsShader : public CStandardShader
{
public:
	CTankObjectsShader();
	virtual ~CTankObjectsShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	///	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	


	int m_nTanks = 0;
	CGameObject** m_ppTankObjects = NULL;

	//-------적 탱크 총 쏘는 시간 관리
	float FireBulletTimeElapsed = 0.f;
	float FireBulletTimeDuration = 1.5f;

};

class CTreeShader :public CStandardShader {
public:
	CTreeShader();
	virtual ~CTreeShader();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	CGameObject** m_ppMultipleTrees = NULL;
	int m_nMultipleTrees = 0;
	CGameObject** m_ppSingleTrees = NULL;
	int m_nSingleTrees = 0;

};


class CCactusAndRocksShader :public CStandardShader {
public:
	CCactusAndRocksShader();
	virtual ~CCactusAndRocksShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	virtual void ReleaseObjects();


	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);


	CGameObject** m_ppCactus = NULL;
	int m_nCactus = 0;
	CGameObject** m_ppRocks = NULL;
	int m_nRock = 0;
};


class CBillboardObjectsShader : public CStandardShader
{
public:
	CBillboardObjectsShader();
	virtual ~CBillboardObjectsShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void ReleaseUploadBuffers();


	CRawFormatImage* m_pRawFormatImage = NULL;
};

class CMultiSpriteObjectsShader : public CShader
{
public:
	CMultiSpriteObjectsShader();
	virtual ~CMultiSpriteObjectsShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();


	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	/*virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();


	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void ReleaseUploadBuffers();*/

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);


	//int m_nSpriteObjects = 0;
	//CGameObject** m_ppSpriteObjects = 0;

	//ID3D12Resource* m_pd3dcbMultiSpriteTextures = NULL;
	//CB_GAMEOBJECT_TEXTURE_INFO* m_pcbMappedMultiSpriteTextures = NULL;

	// int GetNumberOfObjects() { return(m_nSpriteObjects); }
};