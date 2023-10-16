//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer() : CGameObject(0, 0)
{
	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;

	if (m_pShader) m_pShader->Release();
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		/*if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);*/
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		if (!bullet_camera_mode)
			Move(xmf3Shift, bUpdateVelocity);
	}
}



void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{


	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) /*|| (nCurrentCameraMode == THIRD_PERSON_CAMERA)*/)
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +60.0f) { z -= (m_fRoll - 60.0f); m_fRoll = +60.0f; }
			if (m_fRoll < -60.0f) { z -= (m_fRoll + 60.0f); m_fRoll = -60.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA || nCurrentCameraMode == LEFT_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA || nCurrentCameraMode == LEFT_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	case LEFT_CAMERA:
		pNewCamera = new FourthCamera(m_pCamera);
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;

	UpdateTransform(NULL);
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA || nCameraMode == LEFT_CAMERA)
	{
		if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera, 0);
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

void CPlayer::SetOOBB(float fWidth, float fHeight, float fDepth)
{
	m_xmCollision = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fWidth * 0.5f, fHeight * 0.5f, fDepth * 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//if(m_pCollider)
	//	m_pCollider->multiplyScale(fWidth, fHeight, fDepth);
}
void CPlayer::UpdateBoundingBox()
{
	//m_xmCollision.Transform(m_xmCollision, XMLoadFloat4x4(&m_xmf4x4World));
	m_xmCollision.Center = { GetPosition() };
	XMStoreFloat4(&m_xmCollision.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmCollision.Orientation)));
	//바운딩 박스의 현재 방향을 읽어와 XMVECTOR로 로드 하여 읽어온 방향 쿼터니언을 정규화-> 정규화는 쿼터니언 단위 쿼터니언으로 만드는 작업으로
	// 방향을 표현하는 쿼터니언의 길이를 1로 만든다. -> 정규화된 방향 쿼터니언을 다시 저장. 
	// Orientation에 저장된 바운딩 박스의 방향 쿼터니언을 정규화된 단위 쿼터니언으로 대체. 이렇게 하여 바운딩 박스의 방향이 유효하고 정규화된 상태로 유지.
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

CAirplanePlayer::CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pCamera = ChangeCamera(/*SPACESHIP_CAMERA*/THIRD_PERSON_CAMERA, 0.0f);

	m_pShader = new CPlayerShader();
	m_pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 4); //Mi24(1)

	CGameObject* pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/PlayerModel.bin", m_pShader);
	SetChild(pGameObject);
	pGameObject->SetScale(4.0, 4.0, 4.0);

	m_pPlayerShader = new CBulletsShader();
	m_pPlayerShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlayerShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);

	/*BulletShader = new CShader * [BULLETS];
	for (int i = 0; i < BULLETS; i++) {
		BulletShader[i] = new CStandardShader();
		BulletShader[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		BulletShader[i]->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	}

	for (int i = 0; i < BULLETS; i++)
	{
		CGameObject* pBulletMesh = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/dagger.bin", BulletShader[i]);
		pBulletObject = new CBulletObject(m_fBulletEffectiveRange);
		pBulletObject->SetScale(0.01, 0.01, 0.01);

		pBulletObject->SetChild(pBulletMesh, true);
		pBulletObject->SetMovingSpeed(1000.0f);
		pBulletObject->SetRotationSpeed(20.0f);
		pBulletObject->SetActive(false);
		m_ppBullets[i] = pBulletObject;
	}
	*/

	PrepareAnimate();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CAirplanePlayer::~CAirplanePlayer()
{
	//for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]) delete[] m_ppBullets[i];
	if (m_pPlayerShader) {
		m_pPlayerShader->ReleaseShaderVariables();
		m_pPlayerShader->ReleaseObjects();
		//m_pPlayerShader->Release();
		delete m_pPlayerShader;
	}

}

void CAirplanePlayer::PrepareAnimate()
{
	m_pMainMotorFrame = FindFrame("upper_engine");
	m_pLowMotorFrame = FindFrame("low_engine");
	m_pUpMotorFrame = FindFrame("upper_engine");
	m_pSubEngine1MotorFrame = FindFrame("core_engine_ribs001");
	m_pSubEngine2MotorFrame = FindFrame("core_engine_ribs002");
	m_pSubEngine3MotorFrame = FindFrame("core_engine_ribs003");
	m_pSubEngine4MotorFrame = FindFrame("core_engine_ribs004");
	m_pSubEngine5MotorFrame = FindFrame("core_engine_ribs005");


	//upper_engine,low_engine,core_engine_ribs005
}

void CAirplanePlayer::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	/*for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i]->m_bActive) {


			m_ppBullets[i]->Animate(fTimeElapsed);
		}

	}*/
	if (m_pPlayerShader) m_pPlayerShader->AnimateObjects(fTimeElapsed);
	if (m_pMainMotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainMotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainMotorFrame->m_xmf4x4Transform);
	}
	if (m_pLowMotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pLowMotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pLowMotorFrame->m_xmf4x4Transform);
	}
	if (m_pUpMotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pUpMotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pUpMotorFrame->m_xmf4x4Transform);
	}
	if (m_pSubEngine1MotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pSubEngine1MotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pSubEngine1MotorFrame->m_xmf4x4Transform);
	}
	if (m_pSubEngine2MotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pSubEngine2MotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pSubEngine2MotorFrame->m_xmf4x4Transform);
	}
	if (m_pSubEngine3MotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pSubEngine3MotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pSubEngine3MotorFrame->m_xmf4x4Transform);
	}
	if (m_pSubEngine4MotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pSubEngine4MotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pSubEngine4MotorFrame->m_xmf4x4Transform);
	}
	if (m_pSubEngine5MotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationZ(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pSubEngine5MotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pSubEngine5MotorFrame->m_xmf4x4Transform);
	}

	CPlayer::Animate(fTimeElapsed, pxmf4x4Parent);
}

void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
}

CCamera* CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(2.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(2.5f);
		SetMaxVelocityY(40.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(100.5f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(40.0f);
		SetMaxVelocityY(40.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(20.5f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(25.5f);
		SetMaxVelocityY(20.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CAirplanePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CPlayer::Render(pd3dCommandList, pCamera);
	if (m_pPlayerShader)m_pPlayerShader->Render(pd3dCommandList, pCamera);
	//for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) { m_ppBullets[i]->Render(pd3dCommandList, pCamera); }
}

void CAirplanePlayer::FireBullet(CGameObject* pLockedObject)
{

	if (pLockedObject)
	{
		SetLookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		UpdateTransform();
	}


	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i]->m_bActive && ((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i]->m_fMovingDistance == 0)
		{
			pBulletObject = ((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i];
			break;
		}
	}

	XMFLOAT3 PlayerLook = this->GetLookVector();
	XMFLOAT3 CameraLook = m_pCamera->GetLookVector();
	XMFLOAT3 TotalLookVector = Vector3::Normalize(Vector3::Add(PlayerLook, CameraLook));

	if (pBulletObject)
	{

		XMFLOAT3 xmf3Position = this->GetPosition();

		XMFLOAT3 xmf3Direction = TotalLookVector;
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 30.0f, true));
		pBulletObject->m_xmf4x4Transform = m_xmf4x4World;
		xmf3Direction.y += 0.15f;
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetScale(15.5, 15.5, 1.5);
		pBulletObject->SetActive(true);
		if (pLockedObject)
		{
			pBulletObject->m_pLockedObject = pLockedObject;

		}
	}


}

CTankPlayer::CTankPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext) {

	m_pCamera = ChangeCamera(/*SPACESHIP_CAMERA*/THIRD_PERSON_CAMERA, 0.0f);
	m_pShader = new CPlayerShader();
	m_pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 5);

	CGameObject* pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/MainTank.bin", m_pShader);
	SetChild(pGameObject);
	//pGameObject->SetScale(0.5, 0.5, 0.5);

	//SetOOBB(18.0, 12.0f, 38.0);
	SetOOBB(9.0, 15.0f, 19.0);
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetPosition(XMFLOAT3(pTerrain->GetWidth() * 0.5f, 400.0f, pTerrain->GetLength() * 0.5f));
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

	m_pPlayerShader = new CBulletsShader();
	((CBulletsShader*)m_pPlayerShader)->m_pPlayer = this;
	((CBulletsShader*)m_pPlayerShader)->m_pCamera = m_pCamera;
	m_pPlayerShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlayerShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);


	PrepareAnimate();

	/*m_pTrackShader = new CStandardShader();
	m_pTrackShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pTrackShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	CGameObject* pTrackObject= CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/TankTraces.bin", m_pTrackShader);
	m_pTrack = new CGameObject();
	m_pTrack->SetChild(pTrackObject);*/


	//SetChild(m_pTrack);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
CTankPlayer:: ~CTankPlayer() {
	if (m_pPlayerShader) {
		m_pPlayerShader->ReleaseShaderVariables();
		m_pPlayerShader->ReleaseObjects();

		//m_pPlayerShader->Release();
		delete m_pPlayerShader;

	}
	/*if(m_pTrackShader)delete m_pTrackShader;
	if (m_pTrack) delete m_pTrack;*/
}
void CTankPlayer::PrepareAnimate()
{
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


void CTankPlayer::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent) {


	if (m_pPlayerShader) m_pPlayerShader->AnimateObjects(fTimeElapsed);
	for (int i = 0; i < m_nWheels; ++i) {
		if (m_pWheel[i] && (is_RotateWheel || is_Going))
		{
			if (RotateWheel_Forward) {
				XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
				m_pWheel[i]->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pWheel[i]->m_xmf4x4Transform);
			}
			else {
				XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(-360.0f * 2.0f) * fTimeElapsed);
				m_pWheel[i]->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pWheel[i]->m_xmf4x4Transform);
			}

		}
	}
	if (Fired_Bullet && !is_Going && !is_RotateWheel) {
		FireEffect(fTimeElapsed);
	}
	if (Shake) {
		ShakeEffect(fTimeElapsed);
	}
	if (Float_in_Water) {
		FloatEffect(fTimeElapsed);
	}

	/*m_pTrack->SetPosition(GetPosition().x,GetPosition().y,GetPosition().z-3.f);*/
	UpdateBoundingBox();

	if (m_pTurret)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * turret_rotate_value) * fTimeElapsed);

		m_pTurret->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTurret->m_xmf4x4Transform);
	}
	if (m_pPoshin)
	{

		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(180.0f * poshin_rotate_value) * fTimeElapsed);


		m_pPoshin->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pPoshin->m_xmf4x4Transform);
	}
	CPlayer::Animate(fTimeElapsed, pxmf4x4Parent);
}

void CTankPlayer::Update(float fTimeElapsed)
{
	CBulletsShader* pBulletShader = static_cast<CBulletsShader*>(m_pPlayerShader);
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity, false);

	//if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	if (m_pPlayerUpdatedContext) {
		UpdateTankPosition(fTimeElapsed);

	}
	if (bullet_camera_mode) {
		DWORD nCurrentCameraMode = m_pCamera->GetMode();
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->UpdateByBullet(pBulletShader->m_ppBullets[0]->GetPosition(), fTimeElapsed);
		if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAtByBullet(pBulletShader->m_ppBullets[0]->GetPosition());
		m_pCamera->RegenerateViewMatrix();
	}
	else if (!bullet_camera_mode) {

		DWORD nCurrentCameraMode = m_pCamera->GetMode();
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
		if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
		m_pCamera->RegenerateViewMatrix();
	}

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CTankPlayer::OnPrepareRender() {
	CPlayer::OnPrepareRender();
}
void CTankPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA) || (nCurrentCameraMode == LEFT_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}
CCamera* CTankPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) {

	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -400.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(1.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case LEFT_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(LEFT_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(1.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}
void CTankPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pPlayerShader)m_pPlayerShader->Render(pd3dCommandList, pCamera);
	//if(m_pTrack)m_pTrack->Render(pd3dCommandList, pCamera);
	CPlayer::Render(pd3dCommandList, pCamera);

	//for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) { m_ppBullets[i]->Render(pd3dCommandList, pCamera); }
}

void CTankPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 6.0f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}


void CTankPlayer::UpdateTankPosition(float fTimeElapsed)
{

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 6.0f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		if (fHeight < 180.0f) {
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			Float_in_Water = true;
		}
		else {
			SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
			xmf3PlayerPosition.y = fHeight;
		}
		SetPosition(xmf3PlayerPosition);
	}
}

void CTankPlayer::ShakeEffect(float fTimeElapsed)
{

	ShakeEffectTimeElapsed += fTimeElapsed;
	if (ShakeEffectTimeElapsed < ShakeRotationDuration) {

		Rotate(0.0f, 0.f, 60.0f * fTimeElapsed);
	}
	else if (ShakeEffectTimeElapsed < 3 * ShakeRotationDuration)
	{
		// 두 번째 회전

		Rotate(0.0f, 0.f, -120.0f * fTimeElapsed);

	}
	else
	{
		// 세 번째 회전
		Rotate(0.0f, 0.0f, 120.0f * fTimeElapsed);
	}

	// 회전 시간이 경과하면 초기화
	if (ShakeEffectTimeElapsed >= 4 * ShakeRotationDuration)
	{
		ShakeEffectTimeElapsed = 0.0f;
		Shake = false;
	}
}

void CTankPlayer::FloatEffect(float fTimeElapsed)
{
	
		FloatEffectTimeElapsed += fTimeElapsed;
		if (FloatEffectTimeElapsed < FloatUpDuration) {
			Move(DIR_DOWN, 0.2, true);
		}
		else if (FloatEffectTimeElapsed < 2 * FloatUpDuration)
		{
			Move(DIR_UP, 0.2, true);

		}
		if (FloatEffectTimeElapsed >= 2 * FloatUpDuration)
		{
			FloatEffectTimeElapsed = 0.0f;
		}
	
}

void CTankPlayer::FireEffect(float fTimeElapsed)
{
	FireEffectTimeElapsed += fTimeElapsed;
	if (FireEffectTimeElapsed < FireEffectDuration) {
		Move(DIR_BACKWARD, 1.25, true);
	}
	else if (FireEffectTimeElapsed < 2 * FireEffectDuration)
	{
		Move(DIR_FORWARD, 1.25, true);;

	}


	// 회전 시간이 경과하면 초기화
	if (FireEffectTimeElapsed >= 2 * FireEffectDuration)
	{
		FireEffectTimeElapsed = 0.0f;
		Fired_Bullet = false;
	}

}



void CTankPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA || m_pCamera->GetMode() == LEFT_CAMERA)
		{
			CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}



void CTankPlayer::FireBullet(CGameObject* pLockedObject)
{

	if (pLockedObject)
	{
		SetLookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		UpdateTransform();
	}


	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i]->m_bActive && ((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i]->m_fMovingDistance == 0)
		{
			pBulletObject = ((CBulletsShader*)m_pPlayerShader)->m_ppBullets[i];
			break;
		}
	}

	XMFLOAT3 PlayerLook = this->m_pPoshin->GetLook();
	XMFLOAT3 CameraLook = m_pCamera->GetLookVector();
	XMFLOAT3 TotalLookVector = Vector3::Normalize(Vector3::Add(PlayerLook, CameraLook));

	if (pBulletObject)
	{

		XMFLOAT3 xmf3Position = this->m_pPoshin->GetPosition();

		XMFLOAT3 xmf3Direction = TotalLookVector;
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 30.0f, true));
		pBulletObject->m_xmf4x4Transform = m_xmf4x4World;
		xmf3Direction.y += 0.15f;
		pBulletObject->SetMovingDirection(PlayerLook);
		pBulletObject->SetFirePosition(xmf3FirePosition);
		//pBulletObject->SetScale(15.5, 15.5, 1.5);
		pBulletObject->SetActive(true);
		if (pLockedObject)
		{
			pBulletObject->m_pLockedObject = pLockedObject;

		}
	}


}