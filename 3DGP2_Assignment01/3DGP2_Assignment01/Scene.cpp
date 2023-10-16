//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
	m_xmf4x4WaterAnimation = XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = false;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	
	m_pLights[1].m_bEnable = false;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 1000.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 15.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(25.0f));
	
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	
	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.0f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.0f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.0f, 0.0f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 50.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(45.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(20.0f));



}
#define _WITH_TERRAIN_PARTITION
void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildDefaultLightsAndMaterials();

	//m_pBillboard = new CBillboardObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//빌보드 위치 초기화
	m_nSpriteAnimation = 3;
	m_ppSprite = new CMultiSpriteObject * [m_nSpriteAnimation];
	m_ppSprite[0] = new CMultiSpriteObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/MyExplosive_01.dds", 8, 8); //원형 폭발 제일 뒤 그려짐
	m_ppSprite[1] = new CMultiSpriteObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/MyExplosive_02.dds",8,8); 
	m_ppSprite[2] = new CMultiSpriteObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/Explosion_6x6.dds", 6, 6); //크기와 타이밍때문에 제일 뒤에 보임

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	XMFLOAT3 xmf3Scale(25.0f, 3.0f, 25.0f);
	XMFLOAT3 xmf3Normal(0.0f, 1.0f, 0.0f);
#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/HeightMap2.raw"), 257, 257, 257, 257, xmf3Scale, xmf3Normal);

	//m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/ImageHeightMap.raw"), 512, 512, 512, 512, xmf3Scale, xmf4Color);
	//	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif
	m_pTerrain->SetPosition(0.0, 0.0, 0.0);
#ifdef _WITH_RIPPLE_WATER
	m_pRipplewater = new CRippleWater(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 257, 257, 257, 257, xmf3Scale, xmf3Normal, m_pTerrain);
	m_pRipplewater->SetPosition(+(257 * 0.5f), 180.0f, +(257 * 0.5f));
#else
	m_pTerrainWater = new CTerrainWater(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 257 * xmf3Scale.x, 257 * xmf3Scale.z);
	m_pTerrainWater->SetPosition(+(257 * xmf3Scale.x * 0.5f), 200.0f, +(257 * xmf3Scale.z * 0.5f));
#endif
	m_nShaders = 5;
	m_ppShaders = new CShader * [m_nShaders];

	//CObjectsShader* pObjectsShader = new CObjectsShader();
	//pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);

	//m_ppShaders[0] = pObjectsShader;

	CBuildingShader* pBuildingShader = new CBuildingShader();
	pBuildingShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBuildingShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[BUILDING_INDEX] = pBuildingShader;

	CWindMillShader* pWindMillShader = new CWindMillShader();
	pWindMillShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pWindMillShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[WINDMILL_INDEX] = pWindMillShader;

	CTankObjectsShader* pEnemyTankShader = new CTankObjectsShader();
	pEnemyTankShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pEnemyTankShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[ENEMYTANK_INDEX] = pEnemyTankShader;

	CCactusAndRocksShader* pCactusAndRocksShader = new CCactusAndRocksShader();
	pCactusAndRocksShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pCactusAndRocksShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[CACTUS_AND_ROCKS_INDEX] = pCactusAndRocksShader;

	CTreeShader* pTreeShader = new CTreeShader();
	pTreeShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pTreeShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[TREE_INDEX] = pTreeShader;


	//CMultiSpriteObjectsShader* pMultiSpriteObjectShader = new CMultiSpriteObjectsShader();

	//pMultiSpriteObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//pMultiSpriteObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
	//pMultiSpriteObjectShader->SetActive(false);
	////int nMultiSpriteObjects = pMultiSpriteObjectShader->GetNumberOfObjects();
	//m_ppShaders[MULTI_SPRITE_INDEX] = pMultiSpriteObjectShader;
	//


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pSkyBox) delete m_pSkyBox;
	if (m_pTerrain) delete m_pTerrain;
	if (m_pBillboard) delete m_pBillboard;
	if (m_ppSprite) { 
		for (int i = 0; i < m_nSpriteAnimation; i++) {
			m_ppSprite[i]->Release();
		}
		delete m_ppSprite; }
	/*if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}*/
	if (m_pTerrainWater) delete m_pTerrainWater;
	if (m_pRipplewater) delete m_pRipplewater;
	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[11];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtDetailAlbedoTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtDetailNormalTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 5;
	pd3dDescriptorRanges[8].BaseShaderRegister = 14; //t14: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//pd3dDescriptorRanges[9].NumDescriptors = 1;
	//pd3dDescriptorRanges[9].BaseShaderRegister = 19; // t19 billboard texture
	//pd3dDescriptorRanges[9].RegisterSpace = 0;
	//pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 3;
	pd3dDescriptorRanges[9].BaseShaderRegister = 19; //t19: gtxtWaterBaseTexture, t20: gtxtWaterDetailTexture, t21: gtxtWaterDetailAlphaTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 1;
	pd3dDescriptorRanges[10].BaseShaderRegister = 22; 
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER pd3dRootParameters[17]; //비용이 64이상 x 지금 70

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //비용 2
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS; //비용 33
	pd3dRootParameters[1].Constants.Num32BitValues = 33; // 32비트 원래 32개까지 사용권장 왜 되는진 모르겠음.
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //비용 2
	pd3dRootParameters[2].Descriptor.ShaderRegister = 5; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;  //비용 1
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[8];
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;  //비용 2
	pd3dRootParameters[12].Constants.Num32BitValues = 2; //Time, ElapsedTime
	pd3dRootParameters[12].Constants.ShaderRegister = 3; //Time
	pd3dRootParameters[12].Constants.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //비용 2 루트 상수에서 CBV로 바꿈.
	pd3dRootParameters[13].Constants.ShaderRegister = 4; //WaterTextureAnimation 행렬
	pd3dRootParameters[13].Constants.RegisterSpace = 0;
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[9]; //Water 관련 텍스쳐 3개
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //비용 2 루트 상수에서 CBV로 바꿈.
	pd3dRootParameters[15].Constants.ShaderRegister = 6; //TextureAnimationSpriteImage 행렬
	pd3dRootParameters[15].Constants.RegisterSpace = 0;
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //비용 1
	pd3dRootParameters[16].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[16].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[10];
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	/*pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[15].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[15].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[10];
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;*/

#else
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 7;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtStandardTextures[7] //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#endif

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	//pd3dSamplerDescs[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT hr = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	if (FAILED(hr))
	{
		if (pd3dErrorBlob)
		{
			OutputDebugStringA((char*)pd3dErrorBlob->GetBufferPointer());
			pd3dErrorBlob->Release();
		}
		// 오류 처리
	}
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	UINT ncbElementBytesMatrix((sizeof(XMFLOAT4X4) + 255) & ~255);
	m_pd3dcbWaterAnimation = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytesMatrix, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbWaterAnimation->Map(0, NULL, (void**)&m_pcbMappedWaterAnimation);

}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));



	XMStoreFloat4x4(m_pcbMappedWaterAnimation, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4WaterAnimation)));


}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbWaterAnimation) {
		m_pd3dcbWaterAnimation->Unmap(0, NULL);
		m_pd3dcbWaterAnimation->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pBillboard)m_pBillboard->ReleaseUploadBuffers();
	if (m_ppSprite) {
		for (int i = 0; i < m_nSpriteAnimation; i++) {
			m_ppSprite[i]->ReleaseUploadBuffers();
		}
	}
		
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	//	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
	if (m_pTerrainWater) m_pTerrainWater->ReleaseUploadBuffers();
	if (m_pRipplewater) m_pRipplewater->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:

			static_cast<CTankPlayer*>(m_pPlayer)->is_RotateWheel = true;
			static_cast<CTankPlayer*>(m_pPlayer)->RotateWheel_Forward = true;
			if (!static_cast<CTankPlayer*>(m_pPlayer)->bullet_camera_mode)
				static_cast<CTankPlayer*>(m_pPlayer)->Rotate(0.f, 1.5f, 0.f);
			break;
		case VK_LEFT:

			static_cast<CTankPlayer*>(m_pPlayer)->is_RotateWheel = true;
			static_cast<CTankPlayer*>(m_pPlayer)->RotateWheel_Forward = true;
			if (!static_cast<CTankPlayer*>(m_pPlayer)->bullet_camera_mode)
				static_cast<CTankPlayer*>(m_pPlayer)->Rotate(0.f, -1.5f, 0.f);
			break;
		case VK_UP:

			static_cast<CTankPlayer*>(m_pPlayer)->is_Going = true;
			static_cast<CTankPlayer*>(m_pPlayer)->RotateWheel_Forward = true;
			break;
		case VK_DOWN:

			static_cast<CTankPlayer*>(m_pPlayer)->is_Going = true;
			static_cast<CTankPlayer*>(m_pPlayer)->RotateWheel_Forward = false;
			break;
		case 'A':

			static_cast<CTankPlayer*>(m_pPlayer)->turret_rotate_value = -0.25f;

			break;
		case 'D':
			static_cast<CTankPlayer*>(m_pPlayer)->turret_rotate_value = 0.25f;
			break;
		case 'W':

			static_cast<CTankPlayer*>(m_pPlayer)->poshin_rotate_value = -0.15f;

			break;

		case 'S':

			static_cast<CTankPlayer*>(m_pPlayer)->poshin_rotate_value = 0.15f;

			break;
		case 'E':
			if (static_cast<CBulletsShader*>(static_cast<CTankPlayer*>(m_pPlayer)->m_pPlayerShader)->All_nonActive)
				static_cast<CTankPlayer*>(m_pPlayer)->machine_mode = true;
			break;
		case 'Q':
			if (static_cast<CBulletsShader*>(static_cast<CTankPlayer*>(m_pPlayer)->m_pPlayerShader)->All_nonActive)
				static_cast<CTankPlayer*>(m_pPlayer)->machine_mode = false;
			break;
		case 'F':
			//for (int i = 0; i < m_nSpriteAnimation; i++)m_ppSprite[i]->SetActive(!m_ppSprite[i]->m_bActive);
			break;
		default:
			break;
		}
		break;
	case  WM_KEYUP:
		switch (wParam)
		{
		case VK_RIGHT:
		case VK_LEFT:
			static_cast<CTankPlayer*>(m_pPlayer)->is_RotateWheel = false;
			break;
		case VK_UP:
		case VK_DOWN:
			static_cast<CTankPlayer*>(m_pPlayer)->is_Going = false;
			break;
		case 'A':
		case 'D':
			static_cast<CTankPlayer*>(m_pPlayer)->turret_rotate_value = 0.0f;
			break;
		case 'W':
		case 'S':

			static_cast<CTankPlayer*>(m_pPlayer)->poshin_rotate_value = 0.f;

			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	//for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	//for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->UpdateTransform(NULL);

	//CObjectsShader* pShader = (CObjectsShader*)m_ppShaders[0];
	CBuildingShader* pBuildingShader = static_cast<CBuildingShader*>(m_ppShaders[BUILDING_INDEX]);
	CWindMillShader* pWindMillShader = static_cast<CWindMillShader*>(m_ppShaders[WINDMILL_INDEX]);
	CTankObjectsShader* pEnemyTankShader = static_cast<CTankObjectsShader*>(m_ppShaders[ENEMYTANK_INDEX]);
	CCactusAndRocksShader* pCactusAndRocksShader = static_cast<CCactusAndRocksShader*>(m_ppShaders[CACTUS_AND_ROCKS_INDEX]);
	CTreeShader* pTreeShader = static_cast<CTreeShader*>(m_ppShaders[TREE_INDEX]);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	//for (int i = 0; i < pShader->m_nObjects; i++) {
	//	
	//	pShader->m_ppObjects[i]->UpdateBoundingBox();
	//	
	//}
	for (int i = 0; i < pEnemyTankShader->m_nTanks; i++) {
		pEnemyTankShader->m_ppTankObjects[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pBuildingShader->m_ntotal_Buildings; i++) {
		pBuildingShader->m_ppBuildings[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pWindMillShader->m_nWindMills; i++) {
		pWindMillShader->m_ppWindMills[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pCactusAndRocksShader->m_nCactus; i++) {
		pCactusAndRocksShader->m_ppCactus[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pCactusAndRocksShader->m_nRock; i++) {
		pCactusAndRocksShader->m_ppRocks[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pTreeShader->m_nMultipleTrees; i++) {
		pTreeShader->m_ppMultipleTrees[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < pTreeShader->m_nSingleTrees; i++) {
		pTreeShader->m_ppSingleTrees[i]->UpdateBoundingBox();
	}
	if (m_pBillboard)m_pBillboard->Animate(fTimeElapsed, m_pPlayer->GetCamera(), m_pPlayer->GetPosition());
	if (m_ppSprite) {
		for (int i = 0; i < m_nSpriteAnimation; i++) {
			if (m_ppSprite[i]->m_bActive) {
				SpriteAnimationElapsedTime += fTimeElapsed;
				if (SpriteAnimationElapsedTime < 2.5f) {
					m_ppSprite[i]->Animate(fTimeElapsed);
				}
				else {
					SpriteAnimationElapsedTime = 0.f;
					CTankObjectsShader* pEnemyShader = (CTankObjectsShader*)m_ppShaders[ENEMYTANK_INDEX];
					for (int j = 0; j < pEnemyShader->m_nTanks; j++) static_cast<CTankObject*>(pEnemyShader->m_ppTankObjects[j])->hitByBullet = false;
					m_ppSprite[i]->m_bActive = false;
				
				}
			}
		}
	}
	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
		m_pLights[3].m_xmf3Position = static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetPosition();
		m_pLights[3].m_xmf3Direction = XMFLOAT3(static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook().x, static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook().y+0.15f, static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook().z);

	}

	m_xmf4x4WaterAnimation._31 += fTimeElapsed * 0.00125f;
	m_xmf4x4WaterAnimation._32 += fTimeElapsed * 0.00125f;

	CheckBuilding_threeByTankCollisions();
	CheckBuilding_fiveByTankCollisions();
	CheckWindMillByTankCollisions();
	CheckBuilding_fourByTankCollisions();
	CheckCactusByTankCollisions();
	CheckRockByTankCollisions(fTimeElapsed);
	CheckEnemyTankByBulletCollisions();
	//CheckObjectByBulletCollisions();
//	MoveObjectsInCircle(fTimeElapsed);
}

void CScene::CheckObjectByBulletCollisions() {

	//CBulletObject** ppBullets = ((CBulletsShader*)((CTankPlayer*)m_pPlayer)->m_pPlayerShader)->m_ppBullets;
	//CObjectsShader* pShader = (CObjectsShader*)m_ppShaders[0];
	//for (int i = 0; i < pShader->m_nObjects; i++) {
	//	for (int j = 0; j < BULLETS; j++) {
	//		if (ppBullets[j]->m_bActive && pShader->m_ppObjects[i]->m_xmCollision.Intersects(ppBullets[j]->m_xmCollision))
	//		{
	//			//ppBullets[j]->Collided = true;
	//			if(!pShader->m_ppObjects[i]->hitByBullet)
	//			pShader->m_ppObjects[i]->hitByBullet = true;
	//			
	//		//	ppBullets[j]->Reset();
	//			//((CObjectsShader*)m_pPlayer->m_pShader)->m_ppObjects[i]->setPositionCollisionEffect();
	//			pShader->m_ppObjects[i]->lifeCount-=1;//render과 lifecount-.0이면 렌더 x
	//			if (pShader->m_ppObjects[i]->lifeCount < 0) {
	//				pShader->m_ppObjects[i]->die=(true);
	//				pShader->m_ppObjects[i]->lifeCount = 5;
	//			}
	//		}
	//	}
	//}
}
void CScene::CheckBuilding_threeByTankCollisions()
{

	XMFLOAT3 pos = m_pPlayer->GetPosition();
	CBuildingShader* pShader = static_cast<CBuildingShader*>(m_ppShaders[BUILDING_INDEX]);
	for (int i = 0; i < 5; i++) {
		if (pShader->m_ppBuildings[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {
			//탱크 가로 절반 대략 4.0f 세로 9.5f  이 건물 가로 98 세로 90 축으로 부터 z양쪽 25 z음쪽 65 x양쪽 55 음쪽 43
			if (m_pPlayer->GetPosition().z - 4.f < pShader->m_ppBuildings[i]->GetPosition().z + 25.f && m_pPlayer->GetPosition().z + 4.0f > pShader->m_ppBuildings[i]->GetPosition().z - 65.f)
			{
				if (m_pPlayer->GetPosition().x - 10.f < pShader->m_ppBuildings[i]->GetPosition().x + 55.f && m_pPlayer->GetPosition().x + 10.f > pShader->m_ppBuildings[i]->GetPosition().x - 43.f) {
					//0.5f차이로 충돌여부 체크 하는것 9.5+ 0.5(이부분으로) 
					if (m_pPlayer->GetPosition().x > pShader->m_ppBuildings[i]->GetPosition().x) {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x + 64.5f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x - 52.5f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().x - 4.0f < pShader->m_ppBuildings[i]->GetPosition().x + 55.f && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppBuildings[i]->GetPosition().x - 43.f)
			{
				if (m_pPlayer->GetPosition().z - 10.0f < pShader->m_ppBuildings[i]->GetPosition().z + 25.f && m_pPlayer->GetPosition().z + 10.f > pShader->m_ppBuildings[i]->GetPosition().z - 65.f) {
					if (m_pPlayer->GetPosition().z > pShader->m_ppBuildings[i]->GetPosition().z) {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z + 34.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z - 74.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}

			//pShader->m_ppBuildings[i]->collapse = true;
		}
	}
}
void CScene::CheckBuilding_fourByTankCollisions()
{

	XMFLOAT3 pos = m_pPlayer->GetPosition();
	CBuildingShader* pShader = static_cast<CBuildingShader*>(m_ppShaders[BUILDING_INDEX]);
	for (int i = 10; i < 15; i++) {
		if (pShader->m_ppBuildings[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {

			//탱크 가로 절반 대략 4.0f 세로 9.5f  이 건물 가로 84 세로 98 축으로 부터 z양쪽 25 z음쪽 73 x양쪽 42 음쪽 42
			if (m_pPlayer->GetPosition().z - 4.f < pShader->m_ppBuildings[i]->GetPosition().z + 25.f && m_pPlayer->GetPosition().z + 4.0f > pShader->m_ppBuildings[i]->GetPosition().z - 25.f)
			{
				if (m_pPlayer->GetPosition().x - 10.f < pShader->m_ppBuildings[i]->GetPosition().x + 43.f && m_pPlayer->GetPosition().x + 10.f > pShader->m_ppBuildings[i]->GetPosition().x - 43.f) {
					//0.5f차이로 충돌여부 체크 하는것 9.5+ 0.5(이부분으로) 
					if (m_pPlayer->GetPosition().x > pShader->m_ppBuildings[i]->GetPosition().x) {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x + 51.5f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x - 51.5f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().x + 4.0f < pShader->m_ppBuildings[i]->GetPosition().x && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppBuildings[i]->GetPosition().x - 43.f)
			{
				if (m_pPlayer->GetPosition().z - 10.0f < pShader->m_ppBuildings[i]->GetPosition().z + 25.f && m_pPlayer->GetPosition().z + 10.f > pShader->m_ppBuildings[i]->GetPosition().z - 25.f) {

					if (m_pPlayer->GetPosition().z > pShader->m_ppBuildings[i]->GetPosition().z) {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z + 34.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z - 34.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().z + 4.0f < pShader->m_ppBuildings[i]->GetPosition().z - 25.0f && m_pPlayer->GetPosition().z + 4.0f > pShader->m_ppBuildings[i]->GetPosition().z - 75.f) {
				if (m_pPlayer->GetPosition().x - 10.f < pShader->m_ppBuildings[i]->GetPosition().x + 43.f && m_pPlayer->GetPosition().x + 10.f > pShader->m_ppBuildings[i]->GetPosition().x) {
					//0.5f차이로 충돌여부 체크 하는것 9.5+ 0.5(이부분으로) 
					if (m_pPlayer->GetPosition().x > pShader->m_ppBuildings[i]->GetPosition().x) {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x + 51.5f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x - 4.f;  //왜 4로 해야 되는지 가로 세로 축관련 조사해보기 !!
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().x - 4.0f < pShader->m_ppBuildings[i]->GetPosition().x + 43.f && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppBuildings[i]->GetPosition().x)
			{
				if (m_pPlayer->GetPosition().z - 10.0f < pShader->m_ppBuildings[i]->GetPosition().z + 25.f && m_pPlayer->GetPosition().z + 10.f > pShader->m_ppBuildings[i]->GetPosition().z - 73.f) {
					if (m_pPlayer->GetPosition().z > pShader->m_ppBuildings[i]->GetPosition().z) {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z + 34.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z - 82.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}



			//pShader->m_ppBuildings[i]->collapse = true;
		}
	}
}

void CScene::CheckBuilding_fiveByTankCollisions()
{

	XMFLOAT3 pos = m_pPlayer->GetPosition();
	CBuildingShader* pShader = static_cast<CBuildingShader*>(m_ppShaders[BUILDING_INDEX]);
	for (int i = 5; i < 10; i++) {
		if (pShader->m_ppBuildings[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {


			//탱크 가로 절반 대략 4.0f 세로 9.5f  이 건물 가로 98 세로 90 축으로 부터 z양쪽 25 z음쪽 65 x양쪽 55 음쪽 43

			if (m_pPlayer->GetPosition().x - 4.0f < pShader->m_ppBuildings[i]->GetPosition().x + 42.f && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppBuildings[i]->GetPosition().x - 40.f)
			{
				if (m_pPlayer->GetPosition().z - 10.0f < pShader->m_ppBuildings[i]->GetPosition().z + 47.f && m_pPlayer->GetPosition().z + 10.f > pShader->m_ppBuildings[i]->GetPosition().z - 25.f) {
					if (m_pPlayer->GetPosition().z > pShader->m_ppBuildings[i]->GetPosition().z) {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z + 56.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.z = pShader->m_ppBuildings[i]->GetPosition().z - 34.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().z - 4.f < pShader->m_ppBuildings[i]->GetPosition().z + 47.f && m_pPlayer->GetPosition().z + 4.0f > pShader->m_ppBuildings[i]->GetPosition().z - 25.f)
			{
				if (m_pPlayer->GetPosition().x - 10.f < pShader->m_ppBuildings[i]->GetPosition().x + 42.f && m_pPlayer->GetPosition().x + 10.f > pShader->m_ppBuildings[i]->GetPosition().x - 40.f) {
					//0.5f차이로 충돌여부 체크 하는것 9.5+ 0.5(이부분으로) 
					if (m_pPlayer->GetPosition().x > pShader->m_ppBuildings[i]->GetPosition().x) {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x + 51.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.x = pShader->m_ppBuildings[i]->GetPosition().x - 49.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}


			//pShader->m_ppBuildings[i]->collapse = true;
		}

	}
}

void CScene::CheckWindMillByTankCollisions()
{
	XMFLOAT3 pos = m_pPlayer->GetPosition();
	CWindMillShader* pShader = static_cast<CWindMillShader*>(m_ppShaders[WINDMILL_INDEX]);

	for (int i = 0; i < 5; i++) {
		if (pShader->m_ppWindMills[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {

			if (m_pPlayer->GetPosition().x - 4.f < pShader->m_ppWindMills[i]->GetPosition().x + 41.f && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppWindMills[i]->GetPosition().x - 41.f)
			{
				if (m_pPlayer->GetPosition().z - 10.f < pShader->m_ppWindMills[i]->GetPosition().z + 41.f && m_pPlayer->GetPosition().z + 10.f > pShader->m_ppWindMills[i]->GetPosition().z - 41.f) {
					if (m_pPlayer->GetPosition().z > pShader->m_ppWindMills[i]->GetPosition().z) {
						pos.z = pShader->m_ppWindMills[i]->GetPosition().z + 50.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.z = pShader->m_ppWindMills[i]->GetPosition().z - 50.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			else if (m_pPlayer->GetPosition().z - 4.f < pShader->m_ppWindMills[i]->GetPosition().z + 41.f && m_pPlayer->GetPosition().z + 4.f > pShader->m_ppWindMills[i]->GetPosition().z - 41.f)
			{
				if (m_pPlayer->GetPosition().x - 10.f < pShader->m_ppWindMills[i]->GetPosition().x + 50.f && m_pPlayer->GetPosition().x + 10.f > pShader->m_ppWindMills[i]->GetPosition().x - 50.f) {
					if (m_pPlayer->GetPosition().x > pShader->m_ppWindMills[i]->GetPosition().x) {
						pos.x = pShader->m_ppWindMills[i]->GetPosition().x + 50.f;
						m_pPlayer->SetPosition(pos);
					}
					else {
						pos.x = pShader->m_ppWindMills[i]->GetPosition().x - 50.f;
						m_pPlayer->SetPosition(pos);
					}
				}
			}
			//pShader->m_ppBuildings[i]->collapse = true;
		}
	}

}

void CScene::CheckCactusByTankCollisions()
{
	XMFLOAT3 pos = m_pPlayer->GetPosition();
	CCactusAndRocksShader* pShader = static_cast<CCactusAndRocksShader*>(m_ppShaders[CACTUS_AND_ROCKS_INDEX]);
	for (int i = 0; i < pShader->m_nCactus; i++) {
		if (pShader->m_ppCactus[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {
			if (!pShader->m_ppCactus[i]->Cactus_hit) {
				pShader->m_ppCactus[i]->SetScale(1.3f, 0.1f, 1.3f);
				pShader->m_ppCactus[i]->Cactus_hit = true;
				static_cast<CTankPlayer*>(m_pPlayer)->Shake = true;
			}
		}
	}
}

void CScene::CheckRockByTankCollisions(float fTimeElapsed)
{

	CCactusAndRocksShader* pShader = static_cast<CCactusAndRocksShader*>(m_ppShaders[CACTUS_AND_ROCKS_INDEX]);
	XMFLOAT3 pos = m_pPlayer->GetPosition();


	for (int i = 0; i < pShader->m_nRock; i++) {
		XMFLOAT3 Rockpos = pShader->m_ppRocks[i]->GetPosition();

		if (m_pPlayer->GetPosition().z - 4.f < pShader->m_ppRocks[i]->GetPosition().z + 11.f && m_pPlayer->GetPosition().z + 4.f > pShader->m_ppRocks[i]->GetPosition().z - 11.f)
		{
			if (m_pPlayer->GetPosition().x - 9.f < pShader->m_ppRocks[i]->GetPosition().x + 13.f && m_pPlayer->GetPosition().x + 9.f > pShader->m_ppRocks[i]->GetPosition().x - 13.f) {
				if (m_pPlayer->GetPosition().x > pShader->m_ppRocks[i]->GetPosition().x) {
					pos.x = pShader->m_ppRocks[i]->GetPosition().x + 22.f;
					m_pPlayer->SetPosition(pos);
					pShader->m_ppRocks[i]->SetPosition(Rockpos.x - 5.0f * fTimeElapsed, Rockpos.y, Rockpos.z);
					XMFLOAT3 imsipos = pShader->m_ppRocks[i]->GetPosition();
					imsipos.y = m_pTerrain->GetHeight(imsipos.x, imsipos.z);
					pShader->m_ppRocks[i]->SetPosition(imsipos.x, imsipos.y, imsipos.z);
				}
				else {
					pos.x = pShader->m_ppRocks[i]->GetPosition().x - 22.f;
					m_pPlayer->SetPosition(pos);
					pShader->m_ppRocks[i]->SetPosition(Rockpos.x + 5.0f * fTimeElapsed, Rockpos.y, Rockpos.z);
					XMFLOAT3 imsipos = pShader->m_ppRocks[i]->GetPosition();
					imsipos.y = m_pTerrain->GetHeight(imsipos.x, imsipos.z);
					pShader->m_ppRocks[i]->SetPosition(imsipos.x, imsipos.y, imsipos.z);
				}
			}
		}
		else if (pShader->m_ppRocks[i]->m_xmCollision.Intersects(m_pPlayer->m_xmCollision)) {
			if (m_pPlayer->GetPosition().x - 4.f < pShader->m_ppRocks[i]->GetPosition().x + 13.f && m_pPlayer->GetPosition().x + 4.f > pShader->m_ppRocks[i]->GetPosition().x - 13.f)
			{
				if (m_pPlayer->GetPosition().z - 9.f < pShader->m_ppRocks[i]->GetPosition().z + 11.f && m_pPlayer->GetPosition().z + 9.f > pShader->m_ppRocks[i]->GetPosition().z - 11.f) {
					if (m_pPlayer->GetPosition().z > pShader->m_ppRocks[i]->GetPosition().z) {
						pos.z = pShader->m_ppRocks[i]->GetPosition().z + 20.f;
						m_pPlayer->SetPosition(pos);
						pShader->m_ppRocks[i]->SetPosition(Rockpos.x, Rockpos.y, Rockpos.z - 5.0f * fTimeElapsed);
						XMFLOAT3 imsipos = pShader->m_ppRocks[i]->GetPosition();
						imsipos.y = m_pTerrain->GetHeight(imsipos.x, imsipos.z);
						pShader->m_ppRocks[i]->SetPosition(imsipos.x, imsipos.y, imsipos.z);
					}
					else {
						pos.z = pShader->m_ppRocks[i]->GetPosition().z - 20.f;
						m_pPlayer->SetPosition(pos);
						pShader->m_ppRocks[i]->SetPosition(Rockpos.x, Rockpos.y, Rockpos.z + 5.0f * fTimeElapsed);
						XMFLOAT3 imsipos = pShader->m_ppRocks[i]->GetPosition();
						imsipos.y = m_pTerrain->GetHeight(imsipos.x, imsipos.z);
						pShader->m_ppRocks[i]->SetPosition(imsipos.x, imsipos.y, imsipos.z);
					}
				}
			}

		}
	}
}

void CScene::CheckEnemyTankByBulletCollisions()
{
	CBulletObject** ppBullets = ((CBulletsShader*)((CTankPlayer*)m_pPlayer)->m_pPlayerShader)->m_ppBullets;
	CTankObjectsShader* pEnemyShader = (CTankObjectsShader*)m_ppShaders[ENEMYTANK_INDEX];
	for (int i = 0; i < pEnemyShader->m_nTanks; i++) {
		for (int j = 0; j < BULLETS; j++) {
			if (ppBullets[j]->m_bActive && pEnemyShader->m_ppTankObjects[i]->m_xmCollision.Intersects(ppBullets[j]->m_xmCollision))
			{
				//ppBullets[j]->Collided = true;
				if (!static_cast<CTankObject*>(pEnemyShader->m_ppTankObjects[i])->hitByBullet) {
					static_cast<CTankObject*>(pEnemyShader->m_ppTankObjects[i])->hitByBullet = true; //탱크 맞으면 - 탱크 관련 처리
					ppBullets[j]->Collided = true; //총알 맞으면 - 총알 관련 처리
					for (int k = 0; k < m_nSpriteAnimation; k++) {
						m_ppSprite[k]->m_bActive = true; // 탱크 맞으면 스프라이트 ON
					}
				}
				if (static_cast<CTankObject*>(pEnemyShader->m_ppTankObjects[i])->hitByBullet) {
					XMFLOAT3 xmf3CameraPosition = m_pPlayer->GetCamera()->GetPosition();
					CPlayer* pPlayer = m_pPlayer;
					XMFLOAT3 xmf3PlayerPosition = pPlayer->GetPosition();
					XMFLOAT3 xmf3PlayerLook = pPlayer->GetLookVector();
					XMFLOAT3 xmf3EnemyPosition = pEnemyShader->m_ppTankObjects[i]->GetPosition();
					XMFLOAT3 GetPosDifference = Vector3::Subtract(xmf3EnemyPosition, xmf3PlayerPosition, false);
					float GetLength = Vector3::Length(GetPosDifference);
					xmf3EnemyPosition.y += 5.0f;
					XMFLOAT3 xmf3Position = Vector3::Add(xmf3PlayerPosition, Vector3::ScalarProduct(xmf3PlayerLook, GetLength, false));
					for (int k = 0; k < m_nSpriteAnimation; k++) {
						m_ppSprite[k]->SetPosition(xmf3EnemyPosition);
						m_ppSprite[k]->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
					}
				}
			
			}
		}
	}

}




//void CScene::MoveObjectsInCircle(float fTimeElapsed)
//{
//	CObjectsShader* pShader = (CObjectsShader*)m_ppShaders[0];
//	for (int i = 0; i < pShader->m_nObjects; i++) {
//		if (pShader->m_ppObjects[i]->m_bActive && !pShader->m_ppObjects[i]->die && !pShader->m_ppObjects[i]->hitByBullet) {
//			int changer = (i % 2) ? -1 : 1;
//			pShader->m_pAngle[i] += pShader->m_pRotateSpeed[i] * fTimeElapsed * changer;
//			XMFLOAT3 cur_pos = pShader->m_ppObjects[i]->GetPosition();
//			cur_pos.x = pShader->m_pX[i] + pShader->m_pRadius[i] * cos(pShader->m_pAngle[i]);
//			cur_pos.y = pShader->m_pY[i] + pShader->m_pRadius[i] * sin(pShader->m_pAngle[i]);
//			pShader->m_ppObjects[i]->SetPosition(cur_pos);
//		}
//	}
//}

void CScene::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) {

	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	//static std::chrono::steady_clock::time_point lastRenderTime = std::chrono::steady_clock::now();
	//static const int IntervalMilliseconds = 2000; 

	
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbWaterAnimationGpuVirtualAddress = m_pd3dcbWaterAnimation->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(13, d3dcbWaterAnimationGpuVirtualAddress); //WaterAnimationMatrix
	
	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	         //  if (m_pBillboard)m_pBillboard->Render(pd3dCommandList, pCamera);
	if (m_pTerrainWater) m_pTerrainWater->Render(pd3dCommandList, pCamera);
	if (m_pRipplewater) m_pRipplewater->Render(pd3dCommandList, pCamera);
//	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	if (m_ppSprite) {
		for (int i = 0; i < m_nSpriteAnimation; i++) {
			if (m_ppSprite[i]->m_bActive) {
				/*auto currentTime = std::chrono::steady_clock::now();
				auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRenderTime);*/

			
				m_ppSprite[i]->Render(pd3dCommandList, pCamera);
			}
		}
	}
}

