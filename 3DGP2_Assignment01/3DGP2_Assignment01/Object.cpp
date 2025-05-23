//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters, int nRows, int nCols)
{
	//new CTexture(7, RESOURCE_TEXTURE2D, 0, 7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = NULL;

		m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
		for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		m_pnBufferElements = new int[m_nTextures];
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];

	m_nRows = nRows;
	m_nCols = nCols;

	m_xmf4x4Texture = Matrix4x4::Identity();
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;

	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		if (m_pd3dSrvGpuDescriptorHandles[0].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

int CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, m_ppstrTextureNames[nIndex], 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', m_ppstrTextureNames[nIndex]);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex], RESOURCE_TEXTURE2D, nIndex);
			CScene::CreateShaderResourceView(pd3dDevice, this, nIndex);
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
			m_pnRootParameterIndices[nIndex] = PARAMETER_STANDARD_TEXTURE + nIndex;
#endif
		}
		else
		{
			if (pParent)
			{
				CGameObject* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->m_pParent) break;
					pRootGameObject = pRootGameObject->m_pParent;
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(m_ppstrTextureNames[nIndex], &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
					m_pnRootParameterIndices[nIndex] = nParameterIndex;
				}
			}
		}
	}
	return(bLoaded);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

void CTexture::AnimateRowColumn(float fTime)
{
	//	m_xmf4x4Texture = Matrix4x4::Identity();
	m_xmf4x4Texture._11 = 1.0f / float(m_nRows);
	m_xmf4x4Texture._22 = 1.0f / float(m_nCols);
	m_xmf4x4Texture._31 = float(m_nRow) / float(m_nRows);
	m_xmf4x4Texture._32 = float(m_nCol) / float(m_nCols);
	if (fTime == 0.0f)
	{
		if (++m_nCol == m_nCols) { m_nRow++; m_nCol = 0; }
		if (m_nRow == m_nRows) m_nRow = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
	//	if (m_pShader) m_pShader->ReleaseUploadBuffers();
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

//CGameObject::CGameObject(int nMaterials) 
//{
//	m_nMaterials = nMaterials;
//	if (m_nMaterials > 0)
//	{
//		m_ppMaterials = new CMaterial * [m_nMaterials];
//		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
//	}
//}
CGameObject::CGameObject(int nMeshes, int nMaterials)
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}

	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	//if (m_pMaterial) m_pMaterial->Release();
	if (m_ppMaterials) delete[] m_ppMaterials;
}

void CGameObject::SetOOBB(float fWidth, float fHeight, float fDepth)
{
	m_xmCollision = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fWidth * 0.5f, fHeight * 0.5f, fDepth * 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//if(m_pCollider)
	//	m_pCollider->multiplyScale(fWidth, fHeight, fDepth);
}
void CGameObject::UpdateBoundingBox()
{
	//m_xmCollision.Transform(m_xmCollision, XMLoadFloat4x4(&m_xmf4x4World));
	m_xmCollision.Center = { GetPosition() };
	XMStoreFloat4(&m_xmCollision.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmCollision.Orientation)));

}


void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pSibling) m_pSibling->Release();
	if (m_pChild) m_pChild->Release();

	if (--m_nReferences <= 0) delete this;
}

void CGameObject::SetChild(CGameObject* pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild)
	{
		pChild->m_pParent = this;
	}
}




void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}


void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader* pShader)
{
	if (!m_nMaterials)
	{
		m_nMaterials = 1;
		m_ppMaterials = new CMaterial * [m_nMaterials];
		m_ppMaterials[0] = new CMaterial();
	}
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader* pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}
void CGameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}


void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
	//UpdateBoundingBox();
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_bActive) return;



	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);


	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}

			if (m_pMesh) m_pMesh->Render(pd3dCommandList, i);
			if (m_ppMeshes)
			{
				for (int i = 0; i < m_nMeshes; i++)
				{
					if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, i);
				}
			}
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}


void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{

}


void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);



}


void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}
	//if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetNewUp(XMFLOAT3 newUp) {
	m_xmf4x4World._21 = newUp.x;
	m_xmf4x4World._22 = newUp.y;
	m_xmf4x4World._23 = newUp.z;
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

//#define _WITH_DEBUG_FRAME_HIERARCHY

int CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcsncmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName, _tcslen(pstrTextureName)))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial* pMaterial = NULL;
	CTexture* pTexture = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial();
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
#else
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 1); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
			pTexture->SetRootParameterIndex(0, PARAMETER_STANDARD_TEXTURE);
#endif
			pMaterial->SetTexture(pTexture);
			//			pMaterial->SetShader(pShader);
			SetMaterial(nMaterial, pMaterial);

			UINT nMeshType = GetMeshType();
			//if (nMeshType & VERTEXT_NORMAL_TEXTURE) pMaterial->SetStandardShader();
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject* pGameObject = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CGameObject* CGameObject::LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObject* pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pGameObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* pfilePath) : CGameObject(1, 1)
{
	CSkyBoxMesh* pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);
	SetMesh(0, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pfilePath , RESOURCE_TEXTURE_CUBE, 0);

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

//	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, PARAMETER_SKYBOX_CUBE_TEXTURE);

	CMaterial* pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);//변경 SetMaterial(0,pSkyBoxMaterial
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSuperCobraObject::CSuperCobraObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CSuperCobraObject::~CSuperCobraObject()
{
}

void CSuperCobraObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("MainRotor");
	m_pTailRotorFrame = FindFrame("TailRotor");
}

void CSuperCobraObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}
	/*if (die) {
		MoveUp(-1.0f);
		if (GetPosition().y < -300.0) {
			XMFLOAT3 xmf3Position = GetPosition();
			xmf3Position.y = 400;
			SetPosition(xmf3Position);
			die = false;
		}
	}

	UpdateBoundingBox();*/

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGunshipObject::CGunshipObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CGunshipObject::~CGunshipObject()
{
}

void CGunshipObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

void CGunshipObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}


	/*	if (die) {
			MoveUp(-1.0f);
			if (GetPosition().y < -300.0) {
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position.y = 400;
				SetPosition(xmf3Position);
				die = false;
			}
		}

	UpdateBoundingBox();*/
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMi24Object::CMi24Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CMi24Object::~CMi24Object()
{
}

void CMi24Object::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

void CMi24Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	/*if (die) {
			MoveUp(-1.0f);
			if (GetPosition().y < -300.0) {
				XMFLOAT3 xmf3Position = GetPosition();
				xmf3Position.y = 400;
				SetPosition(xmf3Position);
				die = false;
			}
		}

	UpdateBoundingBox();
	*/
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}


CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT3 xmf3Normal) : CGameObject(0, 1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh =
				new CHeightMapGridMesh(pd3dDevice, pd3dCommandList,
					xStart, zStart, nBlockWidth, nBlockLength,
					xmf3Scale, xmf3Normal, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	//UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CTexture* pTerrainTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0, 1);

	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Terrain_Ground.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Terrain_Mud.dds", RESOURCE_TEXTURE2D, 2);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detailed.dds", RESOURCE_TEXTURE2D, 3);
	//pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap-Alpha(Flipped).dds", RESOURCE_TEXTURE2D, 4);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap2(Flipped)Alpha.dds", RESOURCE_TEXTURE2D, 4); 	
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/WaveFoam.dds", RESOURCE_TEXTURE2D, 5);


	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//서술자 힙을 생성하고 -> 그를 채울 뷰들을 만들고 -> 루트시그니쳐에 바인딩
	//pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 5);
	// 스카이박스 실습에서는 게임오브젝트를 32비트 상수로 하여 자원을 한비트마다 세세하게 설정가능. 낭비 자원 체크 안해도됨. 
	// 터레인 실습에서는 게임오브젝트를 뷰로 하여 RangeType으로 나누어 또 세세하게 정해줌 ->공부ㅜ필요

	//여기서 우리가 셰이더에 건내줄 상수버퍼뷰를 만든다.
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, PARAMETER_TERRAIN_TEXTURES);
	

	CMaterial* pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetShader(pTerrainShader);
	pTerrainMaterial->SetTexture(pTerrainTexture);
	SetMaterial(0, pTerrainMaterial);


	//SetShader(pTerrainShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}



CPlayerBulletObject::CPlayerBulletObject(float fEffectiveRange) : CGameObject(0, 0)
{
	m_fBulletEffectiveRange = fEffectiveRange;

}

CPlayerBulletObject::~CPlayerBulletObject()
{

}

void CPlayerBulletObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CPlayerBulletObject::UpdateLooktoPoshin()
{
	XMFLOAT3 xmf3PoshinUp = static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetUp();
	XMFLOAT3 xmf3PoshinLook = static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook();
	XMFLOAT3 xmf3PoshinRight = Vector3::CrossProduct(xmf3PoshinUp, xmf3PoshinLook, true);
	
	m_xmf4x4Transform._11 = xmf3PoshinRight.x; m_xmf4x4Transform._12 = xmf3PoshinRight.y; m_xmf4x4Transform._13 = xmf3PoshinRight.z;
	m_xmf4x4Transform._21 = xmf3PoshinUp.x; m_xmf4x4Transform._22 = xmf3PoshinUp.y; m_xmf4x4Transform._23 = xmf3PoshinUp.z;
	m_xmf4x4Transform._31 = xmf3PoshinLook.x; m_xmf4x4Transform._32 = xmf3PoshinLook.y; m_xmf4x4Transform._33 = xmf3PoshinLook.z;

}

void CPlayerBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CPlayerBulletObject::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;
	CollideLockingTime = 0.0f;
	Collided = false;

	m_bActive = false;
	
}

void CPlayerBulletObject::Animate(float fElapsedTime, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	m_fElapsedTimeAfterFire += fElapsedTime;

	float fDistance = m_fMovingSpeed * fElapsedTime;
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);

	XMFLOAT3 xmf3Position = GetPosition();
	
	//XMFLOAT3 xmf3Target = Vector3::ScalarProduct(static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook(), fDistance * 2, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
	
	//SetLookAt(static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook());
	m_fMovingDistance += fDistance;
	if (xmf3Position.y < pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) && !Collided) {
		if (!m_pPlayer->machine_mode)
			m_pPlayer->bullet_camera_mode = false; //이건 껏다 켰다 하는 bool 값 
		Reset();
	}
	UpdateBoundingBox();
	if (Collided) {
		CollideLockingTime += fElapsedTime;
		fDistance = 0.f;//움직이지 못하게 하고, Collided발동시 렌더하지 않도록 함.
		if (CollideLockingTime > 5.0f) {
			if (!m_pPlayer->machine_mode) //제거 해야할수도
				m_pPlayer->bullet_camera_mode = false;
			Reset();
		}
	}
	if ((m_fMovingDistance > m_fBulletEffectiveRange) || (m_fElapsedTimeAfterFire > m_fLockingTime) && !Collided) {
		if (!m_pPlayer->machine_mode)
			m_pPlayer->bullet_camera_mode = false; //머신 모드가 아니고 bulletcameramode가 아닐때 쏘면 bulletcameramode로 바뀜.
		Reset();
	}
}




CTankObjectBullet::CTankObjectBullet(float fEffectiveRange) : CGameObject(0, 0)
{
	m_fBulletEffectiveRange = fEffectiveRange;

}

CTankObjectBullet::~CTankObjectBullet()
{

}

void CTankObjectBullet::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CTankObjectBullet::UpdateLooktoPoshin(XMFLOAT3 TankObjectPoshinUp,XMFLOAT3 TankObjectPoshinLook)
{
	XMFLOAT3 xmf3PoshinUp = TankObjectPoshinUp;
		XMFLOAT3 xmf3PoshinLook = TankObjectPoshinLook;
	XMFLOAT3 xmf3PoshinRight = Vector3::CrossProduct(xmf3PoshinUp, xmf3PoshinLook, true);

	m_xmf4x4Transform._11 = xmf3PoshinRight.x; m_xmf4x4Transform._12 = xmf3PoshinRight.y; m_xmf4x4Transform._13 = xmf3PoshinRight.z;
	m_xmf4x4Transform._21 = xmf3PoshinUp.x; m_xmf4x4Transform._22 = xmf3PoshinUp.y; m_xmf4x4Transform._23 = xmf3PoshinUp.z;
	m_xmf4x4Transform._31 = xmf3PoshinLook.x; m_xmf4x4Transform._32 = xmf3PoshinLook.y; m_xmf4x4Transform._33 = xmf3PoshinLook.z;

}

void CTankObjectBullet::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CTankObjectBullet::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;

	m_bActive = false;

}

void CTankObjectBullet::Animate(float fElapsedTime, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	m_fElapsedTimeAfterFire += fElapsedTime;

	float fDistance = m_fMovingSpeed * fElapsedTime;
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);

	XMFLOAT3 xmf3Position = GetPosition();

	//XMFLOAT3 xmf3Target = Vector3::ScalarProduct(static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook(), fDistance * 2, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);

	//SetLookAt(static_cast<CTankPlayer*>(m_pPlayer)->m_pPoshin->GetLook());
	m_fMovingDistance += fDistance;
	if (xmf3Position.y < pTerrain->GetHeight(xmf3Position.x, xmf3Position.z)) {
		Reset();
	}
	UpdateBoundingBox();
	
	if ((m_fMovingDistance > m_fBulletEffectiveRange) || (m_fElapsedTimeAfterFire > m_fLockingTime)) {
		Reset();
	}
}


void CGameObject::SetMovingDirection(const XMFLOAT3& xmf3MovingDirection)
{
	XMStoreFloat3(&m_xmf3MovingDirection, XMVector3Normalize(XMLoadFloat3(&xmf3MovingDirection)));
}

void CGameObject::SetLookAt(XMFLOAT3 xmf3Target, XMFLOAT3 xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4Transform._11 = mtxLookAt._11; m_xmf4x4Transform._12 = mtxLookAt._21; m_xmf4x4Transform._13 = mtxLookAt._31;
	m_xmf4x4Transform._21 = mtxLookAt._12; m_xmf4x4Transform._22 = mtxLookAt._22; m_xmf4x4Transform._23 = mtxLookAt._32;
	m_xmf4x4Transform._31 = mtxLookAt._13; m_xmf4x4Transform._32 = mtxLookAt._23; m_xmf4x4Transform._33 = mtxLookAt._33;



}

void CGameObject::SetLookAt_P(XMFLOAT3& xmf3Target)
{

	XMFLOAT3 xmf3Up = GetUp();
	XMFLOAT3 xmf3Position(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	m_xmf4x4Transform._11 = xmf3Right.x; m_xmf4x4Transform._12 = xmf3Right.y; m_xmf4x4Transform._13 = xmf3Right.z;
	m_xmf4x4Transform._21 = xmf3Up.x; m_xmf4x4Transform._22 = xmf3Up.y; m_xmf4x4Transform._23 = xmf3Up.z;
	m_xmf4x4Transform._31 = xmf3Look.x; m_xmf4x4Transform._32 = xmf3Look.y; m_xmf4x4Transform._33 = xmf3Look.z;

}


CBillboardObject::CBillboardObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* pfilePath, float width, float height) : CGameObject(0, 1)
{
	CTexturedRectMesh* pBillboardMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, width, height, 0.0f, 0.f, 0.f, 0.f);
	SetMesh(pBillboardMesh);



	CTexture* pBillboardTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pfilePath, RESOURCE_TEXTURE2D, 0);

	CBillboardShader* pBillboardShader = new CBillboardShader();
	pBillboardShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pBillboardShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//pBillboardShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	CScene::CreateShaderResourceViews(pd3dDevice, pBillboardTexture, 0, PARAMETER_2D_TEXTURE);

	CMaterial* pBillboardMaterial = new CMaterial();
	pBillboardMaterial->SetTexture(pBillboardTexture);
	pBillboardMaterial->SetShader(pBillboardShader);

	SetMaterial(0, pBillboardMaterial);//변경 SetMaterial(0,pSkyBoxMaterial);

}

CBillboardObject::~CBillboardObject()
{
}

void CBillboardObject::Animate(float fTimeElapsed, CCamera* pCamera, float distance)
{

	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	CPlayer* pPlayer = pCamera->GetPlayer();
	XMFLOAT3 xmf3PlayerPosition = static_cast<CTankPlayer*>(pPlayer)->m_pPoshin->GetPosition();//static_cast<CBulletsShader*>((static_cast<CTankPlayer*>(pPlayer)->m_pPlayerShader))->m_ppBullets[0]->GetPosition();
	XMFLOAT3 xmf3PlayerLook = static_cast<CTankPlayer*>(pPlayer)->m_pPoshin->GetLook();
	
	
	XMFLOAT3 xmf3Position = Vector3::Add(xmf3PlayerPosition, Vector3::ScalarProduct(xmf3PlayerLook, distance, false));

	SetPosition(xmf3Position);
	SetLookAt(xmf3CameraPosition);
}

void CBillboardObject::SetLookAt(XMFLOAT3& xmf3Target)
{

	XMFLOAT3 xmf3Up(0.f, 1.f, 0.f);
	XMFLOAT3 xmf3Position(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	m_xmf4x4Transform._11 = xmf3Right.x; m_xmf4x4Transform._12 = xmf3Right.y; m_xmf4x4Transform._13 = xmf3Right.z;
	m_xmf4x4Transform._21 = xmf3Up.x; m_xmf4x4Transform._22 = xmf3Up.y; m_xmf4x4Transform._23 = xmf3Up.z;
	m_xmf4x4Transform._31 = xmf3Look.x; m_xmf4x4Transform._32 = xmf3Look.y; m_xmf4x4Transform._33 = xmf3Look.z;

}

CWindMillObject::CWindMillObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CWindMillObject::~CWindMillObject()
{
}

void CWindMillObject::PrepareAnimate()
{
	m_pWindMill = FindFrame("GameObject");
	//m_pWindMill = FindFrame("Cube");
}

void CWindMillObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{

	if (m_pWindMill)
	{

		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(30.0f) * fTimeElapsed);
		m_pWindMill->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pWindMill->m_xmf4x4Transform);
	}
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

CTerrainWater::CTerrainWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fLength) : CGameObject(0, 1)
{
	CTexturedRectMeshWithOneVertex* pWaterMesh = new CTexturedRectMeshWithOneVertex(pd3dDevice, pd3dCommandList, fWidth, 0.f, fLength, 0.0f, 0.0f, 0.0f);
	SetMesh(pWaterMesh);

	//CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pWaterTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Base_Texture_0.dds", RESOURCE_TEXTURE2D, 0);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Detail_Texture_0.dds", RESOURCE_TEXTURE2D, 1);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Lava(Diffuse).dds", RESOURCE_TEXTURE2D, 2);
	//pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Texture_Alpha.dds", RESOURCE_TEXTURE2D, 2);
	CTerrainWaterShader* pWaterShader = new CTerrainWaterShader();
	pWaterShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pWaterShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//pWaterShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	CScene::CreateShaderResourceViews(pd3dDevice, pWaterTexture, 0, PARAMETER_WATER_TEXTURES);


	CMaterial* pWaterMaterial = new CMaterial();
	pWaterMaterial->SetTexture(pWaterTexture);
	pWaterMaterial->SetShader(pWaterShader);
	SetMaterial(0, pWaterMaterial);


}

CTerrainWater::~CTerrainWater()
{
}

CRippleWater::CRippleWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT3 xmf3Normal, void* pContext) : CGameObject(0, 1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CGridMesh* pGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pGridMesh = new CGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf3Normal, pContext);
			SetMesh(x + (z * cxBlocks), pGridMesh);
		}
	}



	CTexture* pWaterTexture = new CTexture(4, RESOURCE_TEXTURE2D, 0, 1);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/My_Water_Base.dds", RESOURCE_TEXTURE2D, 0);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Water_Pool.dds", RESOURCE_TEXTURE2D, 1);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detailed.dds", RESOURCE_TEXTURE2D, 2);
	pWaterTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/DarkLava.dds", RESOURCE_TEXTURE2D, 3);

	//UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CRippleWaterShader* pRippleWaterShader = new CRippleWaterShader();
	pRippleWaterShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pRippleWaterShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//pRippleWaterShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	CScene::CreateShaderResourceViews(pd3dDevice, pWaterTexture, 0, PARAMETER_WATER_TEXTURES);


	CMaterial* pWaterMaterial = new CMaterial();
	pWaterMaterial->SetTexture(pWaterTexture);
	pWaterMaterial->SetShader(pRippleWaterShader);
	SetMaterial(0, pWaterMaterial);

}

CRippleWater::~CRippleWater()
{
}

CTankObject::CTankObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, CShader* pShader) : CGameObject(0, 0)
{
	SetPlayerUpdatedContext(pContext);
	
	CGameObject* pBulletMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Missile.bin", pShader);
	
	m_pBullet = new CTankObjectBullet(200);
	m_pBullet->SetChild(pBulletMesh, true);
	pBulletMesh->AddRef();
	m_pBullet->SetOOBB(6, 6, 16);
	m_pBullet->SetMovingSpeed(75.0f);
	m_pBullet->SetActive(false);



}

CTankObject::~CTankObject() {

	if (m_pBullet)m_pBullet->Release();
	

}

void CTankObject::Update(float fTimeElapsed)
{
	//if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	if(!hitByBullet)
	MoveRandom(fTimeElapsed);

	RotateWheels(fTimeElapsed);
	if (m_pTankObjectUpdatedContext) {
		UpdateTimeElapsed += fTimeElapsed;
		if (UpdateTimeElapsed < UpdateDuration) {
			UpdateTankUpLookRight();
			UpdateTankPosition();
			//m_pTurret->SetLookAt(static_cast<CTankPlayer*>(m_pPlayer)->m_pTurret->GetPosition());
		}
		else {
			UpdateTimeElapsed = 0.0f;
		}
	}
	if (Float_in_Water)
		FloatEffect(fTimeElapsed);
	if (hitByBullet) {
		dieAnimationElapsedTime += fTimeElapsed;
		DieAnimation(fTimeElapsed);
		if(dieAnimationElapsedTime>2.5f){
			die = true;
		}
	}

}

void CTankObject::UpdateTankUpLookRight()
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pTankObjectUpdatedContext;
	XMFLOAT3 TankObjectPosition = GetPosition();
	XMFLOAT3 TankUp_FLOAT3 = GetUp();
	XMFLOAT3 TankLook_FLOAT3 = GetLook();
	XMFLOAT3 TankRight_FLOAT3 = GetRight();
	XMFLOAT3 TerrainNormal_FLOAT3;
	if (TankObjectPosition.y < 185.f) {
		TerrainNormal_FLOAT3 = XMFLOAT3(0.f, 1.f, 0.f);
	}
	else {
		TerrainNormal_FLOAT3 = pTerrain->GetNormal(TankObjectPosition.x, TankObjectPosition.z);
	}
	XMFLOAT3 UpdateAxis;
	XMVECTOR TankUp_VECTOR = XMLoadFloat3(&TankUp_FLOAT3);
	XMVECTOR TerrainNormal_VECTOR = XMLoadFloat3(&TerrainNormal_FLOAT3);

	XMMATRIX RotationMatrix_WORLD = XMMatrixIdentity();

	float Dot = XMVectorGetX(XMVector3Dot(TankUp_VECTOR, TerrainNormal_VECTOR));
	float TotalRotationAngle = acosf(Dot);
	float InterpolationFactor = 0.9f;

	TotalRotationAngle = (1.0f - InterpolationFactor) * TotalRotationAngle;

	if (TotalRotationAngle > 0.0f)
	{
		UpdateAxis = Vector3::CrossProduct(TankUp_FLOAT3, TerrainNormal_FLOAT3,true);

		RotationMatrix_WORLD = XMMatrixRotationAxis(XMLoadFloat3(&UpdateAxis), TotalRotationAngle);
	}

	

	TankLook_FLOAT3 = Vector3::TransformNormal(TankLook_FLOAT3, RotationMatrix_WORLD);
	TankUp_FLOAT3 = Vector3::TransformNormal(TankUp_FLOAT3, RotationMatrix_WORLD);
	TankRight_FLOAT3 = Vector3::TransformNormal(TankRight_FLOAT3, RotationMatrix_WORLD);

	m_xmf4x4Transform._11 = TankRight_FLOAT3.x; m_xmf4x4Transform._12 = TankRight_FLOAT3.y; m_xmf4x4Transform._13 = TankRight_FLOAT3.z;
	m_xmf4x4Transform._21 = TankUp_FLOAT3.x; m_xmf4x4Transform._22 = TankUp_FLOAT3.y; m_xmf4x4Transform._23 = TankUp_FLOAT3.z;
	m_xmf4x4Transform._31 = TankLook_FLOAT3.x; m_xmf4x4Transform._32 = TankLook_FLOAT3.y; m_xmf4x4Transform._33 = TankLook_FLOAT3.z;


}

void CTankObject::UpdateLookAtPlayer()
{
	

	
}

void CTankObject::LookAtDirection(XMFLOAT3& direction, CGameObject* Object)
{
	
}



void CTankObject::UpdateTankPosition()
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pTankObjectUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3TankPosition = GetPosition();
	int z = (int)(xmf3TankPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3TankPosition.x, xmf3TankPosition.z, bReverseQuad) + 6.0f;
	if (fHeight > 179.0f)MoveUp(-0.5f);
	if (xmf3TankPosition.y < fHeight)
	{

		if (fHeight < 180.0f) {

			Float_in_Water = true;
		}
		else {
			Float_in_Water = false;
			xmf3TankPosition.y = fHeight - 3.0f;
		}
		SetPosition(xmf3TankPosition);
	}

}



//std::random_device rd;
//std::mt19937 mt(rd());
//std::uniform_real_distribution<float> dist(-1.0f, 1.0f); // -1.0에서 1.0 사이의 랜덤 수 생성

void CTankObject::MoveRandom(float fTimeElapsed)
{
	float fDistance = 5.f;

	// 랜덤 방향으로 이동
	//float fRandX = dist(mt);//-1~1사이
	//float fRandZ = dist(mt);
	MoveStrafeTimeElapsed += fTimeElapsed;
	if (MoveStrafeTimeElapsed < MoveStrafeDuration) {
		if(GetPosition().x<6000.0f&&GetPosition().x>500.f&&GetPosition().z>500.f&&GetPosition().z<6000.f)
		MoveForward(MovingSpeed * fTimeElapsed);
	}
	else if (MoveStrafeTimeElapsed < 1.3 * MoveStrafeDuration)
	{
		Turning_Direction = true;
		Rotate(0.f, RotationSpeed, 0.f);

	}
	if (MoveStrafeTimeElapsed >= 1.3 * MoveStrafeDuration)
	{
		Turning_Direction = false;
		MoveStrafeTimeElapsed = 0.0f;
	}


	//if(!Turning_Direction)

}

void CTankObject::RotateWheels(float fTimeElapsed)
{
	for (int i = 0; i < m_nWheels; ++i) {
		if (!Turning_Direction) {
			XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(180.0f * 2.0f) * fTimeElapsed);
			m_pWheel[i]->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pWheel[i]->m_xmf4x4Transform);
		}
		else {
			XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(-240.0f * 2.0f) * fTimeElapsed);
			m_pWheel[i]->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pWheel[i]->m_xmf4x4Transform);
		}
	}
}

void CTankObject::FloatEffect(float fTimeElapsed)
{


	FloatEffectTimeElapsed += fTimeElapsed;
	if (FloatEffectTimeElapsed < FloatUpDuration) {
		MoveUp(0.1);
	}
	else if (FloatEffectTimeElapsed < 2 * FloatUpDuration)
	{
		MoveUp(-0.1);

	}
	if (FloatEffectTimeElapsed >= 2 * FloatUpDuration)
	{
		FloatEffectTimeElapsed = 0.0f;
	}

}
void CTankObject::DieAnimation(float fTimeElapsed) {
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

	MoveStrafe(shakeAmount);
}

void CTankObject::PrepareAnimate() {
	m_pWheel[0] = FindFrame("Cube.000");
	m_pWheel[1] = FindFrame("Cube.001");
	m_pWheel[2] = FindFrame("Cube.003");
	m_pWheel[3] = FindFrame("Cube.004");
	m_pWheel[4] = FindFrame("Cube.005");
	m_pWheel[5] = FindFrame("Cube.006");
	m_pWheel[6] = FindFrame("Cube.007");
	m_pWheel[7] = FindFrame("Cube.008");
	m_pWheel[8] = FindFrame("Cube.009");
	m_pWheel[9] = FindFrame("Cube.010");
	m_pWheel[10] = FindFrame("Cube.011");
	m_pWheel[11] = FindFrame("Cube.012");
	m_pWheel[12] = FindFrame("Cube.013");
	m_pWheel[13] = FindFrame("Cube.014");
	m_pWheel[14] = FindFrame("Cube.015");
	m_pWheel[15] = FindFrame("Cube.016");
	m_pTurret = FindFrame("Cube.017");
	m_pPoshin = FindFrame("Cube.018");
}

void CTankObject::FireBullet(CGameObject* pLockedObject)
{

	/*if (pLockedObject)
	{
		SetLookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		UpdateTransform();
	}*/


	CTankObjectBullet* pBulletObject = NULL;
	if (!(m_pBullet->m_bActive) && (m_pBullet->m_fMovingDistance == 0))
	{
		pBulletObject = m_pBullet;

	}


	XMFLOAT3 TankObjectLook = m_pPoshin->GetLook();


	if (pBulletObject)
	{



		XMFLOAT3 xmf3Position = m_pPoshin->GetPosition();
		XMFLOAT3 xmf3PoshinUp = m_pPoshin->GetUp();

		XMFLOAT3 xmf3Direction = TankObjectLook;
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(TankObjectLook, 10.0f, true));

		//xmf3Direction.y += 0.15f;
		pBulletObject->UpdateLooktoPoshin(xmf3PoshinUp, TankObjectLook);
		pBulletObject->SetMovingDirection(TankObjectLook);
		pBulletObject->SetFirePosition(xmf3FirePosition);
		//pBulletObject->SetScale(15.5, 15.5, 1.5);
		pBulletObject->SetActive(true);
		if (pLockedObject)
		{
			pBulletObject->m_pLockedObject = pLockedObject;

		}
	}


}


void CTankObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent) {
	if (!die) {
		Update(fTimeElapsed);
		if (m_pBullet->m_bActive)m_pBullet->Animate(fTimeElapsed, m_pTankObjectUpdatedContext);

		CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
	}
}
void CTankObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	/*CPlayer* pPlayer = pCamera->GetPlayer();
	XMFLOAT3 PlayerPos = static_cast<CTankPlayer*>(pPlayer)->m_pPoshin->GetPosition();
	m_pPoshin->SetLookAt(PlayerPos,m_pPoshin->GetUp());*/
	if (!die) {
		m_pBullet->UpdateTransform(NULL);
		if (m_pBullet->m_bActive) {
			m_pBullet->Render(pd3dCommandList, pCamera);
		}
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}
void CTankObject::ReleaseUploadBuffers()
{
	if (m_pBullet)m_pBullet->ReleaseUploadBuffers();
	CGameObject::ReleaseUploadBuffers();
}
//==================================================

TreesObject::TreesObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

TreesObject::~TreesObject()
{
}

void TreesObject::PrepareAnimate()
{

}

void TreesObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	//CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}


CMultiSpriteObject::CMultiSpriteObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* filePath, int nRows, int nCols) :CGameObject(0, 1)
{

	m_bActive = false;
	CTexturedRectMesh* pSpriteMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	SetMesh(pSpriteMesh);
	CTexture* pMultiSpriteTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, nRows, nCols);
	pMultiSpriteTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, filePath, RESOURCE_TEXTURE2D, 0);

	CMultiSpriteObjectsShader* pMultiSpriteShader = new CMultiSpriteObjectsShader();
	pMultiSpriteShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pMultiSpriteShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	CScene::CreateShaderResourceViews(pd3dDevice, pMultiSpriteTexture, 0, PARAMETER_2D_TEXTURE);

	CMaterial* pMultiSpriteMaterial = new CMaterial();
	pMultiSpriteMaterial->SetTexture(pMultiSpriteTexture);
	pMultiSpriteMaterial->SetShader(pMultiSpriteShader);
	SetMaterial(0, pMultiSpriteMaterial);
	m_fSpeed = 3.0f / (pMultiSpriteTexture->m_nRows * pMultiSpriteTexture->m_nCols);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CMultiSpriteObject::~CMultiSpriteObject()
{
}

void CMultiSpriteObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255); //256의 배수
	m_pd3dcbObjectTexture = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbObjectTexture->Map(0, NULL, (void**)&m_pcbMappedObjectTexture);
}

void CMultiSpriteObject::ReleaseShaderVariables()
{
	if (m_pd3dcbObjectTexture) {
		m_pd3dcbObjectTexture->Unmap(0, NULL);
		m_pd3dcbObjectTexture->Release();

	}
}

void CMultiSpriteObject::Animate(float fTimeElapsed)
{
	for (int i = 0; i < m_nMaterials; i++) {
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			m_fTime += fTimeElapsed * 0.5f;
			if (m_fTime >= m_fSpeed) m_fTime = 0.0f;
			m_ppMaterials[i]->m_pTexture->AnimateRowColumn(m_fTime);
		}
	}

}

void CMultiSpriteObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	if (m_nMaterials > 0) {
		for (int i = 0; i < m_nMaterials; i++) {
			if (m_ppMaterials[i]->m_pTexture) {


				XMStoreFloat4x4(m_pcbMappedObjectTexture, XMMatrixTranspose(XMLoadFloat4x4(&m_ppMaterials[i]->m_pTexture->m_xmf4x4Texture)));
				D3D12_GPU_VIRTUAL_ADDRESS d3dcbObjectTextureGpuVirtualAddress = m_pd3dcbObjectTexture->GetGPUVirtualAddress();
				pd3dCommandList->SetGraphicsRootConstantBufferView(PARAMETER_SPRITE_ANIMATION_MATRIX, d3dcbObjectTextureGpuVirtualAddress); //

			}
		}
	}

	CGameObject::Render(pd3dCommandList, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}
