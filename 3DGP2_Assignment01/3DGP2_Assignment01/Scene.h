//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

//const int OBJECTS_INDEX=0;
const int BUILDING_INDEX = 0;
const int WINDMILL_INDEX = 1;
const int ENEMYTANK_INDEX = 2;
const int CACTUS_AND_ROCKS_INDEX = 3;
const int TREE_INDEX = 4;
const int MULTI_SPRITE_INDEX = 5;

#define _WITH_RIPPLE_WATER

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
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	CPlayer* m_pPlayer = NULL;

public:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	int									m_nGameObjects = 0;
	CGameObject** m_ppGameObjects = NULL;

	int									m_nShaders = 0;
	CShader** m_ppShaders = NULL;

	CSkyBox* m_pSkyBox = NULL;
	CBillboardObject* m_pBillboard = NULL;
	CBillboardObject* m_pBillboard2 = NULL;

	int m_nSpriteAnimation = 0;
	CMultiSpriteObject** m_ppSprite = NULL;

	CHeightMapTerrain* m_pTerrain = NULL;
	LIGHT* m_pLights = NULL;
	int									m_nLights = 0;


	float SpriteAnimationElapsedTime = 0.f;

	//--------water  animation
	CTerrainWater* m_pTerrainWater = NULL;
	CRippleWater* m_pRipplewater = NULL;
	XMFLOAT4X4					m_xmf4x4WaterAnimation;
	ID3D12Resource* m_pd3dcbWaterAnimation = NULL;
	XMFLOAT4X4* m_pcbMappedWaterAnimation = NULL;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
};
