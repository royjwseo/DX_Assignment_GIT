//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"
#include "GameSound.h"
#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3
#define POINT_ENEMY_LIGHT   4

//const int OBJECTS_INDEX=0;
const int BUILDING_INDEX = 0;
const int WINDMILL_INDEX = 1;
const int ENEMYTANK_INDEX = 2;
const int CACTUS_AND_ROCKS_INDEX = 3;
const int TREE_INDEX = 4;
const int MULTI_SPRITE_INDEX = 5;

#define _WITH_RIPPLE_WATER

enum class SceneMode{
	Start,
	StartCameraChange,
	Playing,
	EndCameraChange,
	End

};

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

class CDescriptorHeap
{
public:
	CDescriptorHeap();
	~CDescriptorHeap();

	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorNextHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorNextHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }
};


class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void CheckObjectByBulletCollisions();
	void CheckBuilding_threeByTankCollisions();
	void CheckBuilding_fourByTankCollisions();
	void CheckBuilding_fiveByTankCollisions();
	void CheckWindMillByTankCollisions();
	void CheckCactusByTankCollisions();
	void CheckRockByTankCollisions(float fTimeElapsed);
	void CheckEnemyTankByBulletCollisions();

	void FindEnemyTankWithLight(float fTimeElapsed);
	bool TurnLights = false;
	float ElapsedTimeLightTurnOn = 0.f;
	float DurationLightTime = 2.5f;

	//void MoveObjectsInCircle(float fTimeElapsed);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void RenderTransparentAfterPlayer(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();

	//-----------
	//纠 包府
	
	SceneMode scene_Mode;
	void ChangeSceneMode(SceneMode Mode){
		scene_Mode = Mode;
	}
	void Start_Scene(float fTimeElapsed);
	
	SceneMode GetSceneMode() {
		return scene_Mode;
	}
	float StartSceneElapsedTime = 0.f;
	bool Start_Game = false;
	bool End_Game = false;
	void End_Scene(float fTimeElapsed);
	float EndSceneElapsedTime = 0.f;
	//------

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	CPlayer* m_pPlayer = NULL;

public:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//---------------
		/*int									m_nGameObjects = 0;
		CGameObject** m_ppGameObjects = NULL;*/
		//---------------
	//----------
	bool Win = false;
	bool Lose = false;
	float WinElapsedTime = 0.f;
	float WinRotationDuration = 0.6f;
	void Lose_Animation(float fTimeElapsed);
	void Win_Animation(float fTimeElapsed);
	//------
	//攀农 磷篮 俺荐 包府
	int n_deadTank = 0;
	//
	int									m_nShaders = 0;
	CShader** m_ppShaders = NULL;
	//---------------
	CSkyBox* m_pSkyBox = NULL;
	CSkyBox* m_pStartSkyBox = NULL;

	//---------------
	int m_nDotBillboard = 0;
	CBillboardObject** m_ppDotBillboard = NULL;
	int aiming_point_mode = 0;
	//---------------
	int m_nSpriteAnimation = 0;
	CMultiSpriteObject** m_ppSprite = NULL;
	//---------------
	CHeightMapTerrain* m_pTerrain = NULL;
	//---------------
	LIGHT* m_pLights = NULL;
	int									m_nLights = 0;
	//---------------

	float SpriteAnimationElapsedTime = 0.f;

	//--------water  animation
	CTerrainWater* m_pTerrainWater = NULL;
	CRippleWater* m_pRipplewater = NULL;
	XMFLOAT4X4					m_xmf4x4WaterAnimation;
	ID3D12Resource* m_pd3dcbWaterAnimation = NULL;
	XMFLOAT4X4* m_pcbMappedWaterAnimation = NULL;
	//---------------
	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

public:

	GameSound Game_Sound;
	GameSound* GetSound() { return &Game_Sound; }

public:
	static CDescriptorHeap* m_pDescriptorHeap;

	static void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle); }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle); }

};
