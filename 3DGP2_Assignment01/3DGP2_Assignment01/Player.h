#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;



	LPVOID						m_pPlayerUpdatedContext = NULL;
	LPVOID						m_pCameraUpdatedContext = NULL;

	CCamera* m_pCamera = NULL;
public:
	CShader* m_pShader = NULL;
	CShader* m_pPlayerShader = NULL; //총알 셰이더.
	//CShader** BulletShader = NULL;
	bool						bullet_camera_mode = false;
	bool machine_mode = false;
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	BoundingOrientedBox				m_xmCollision;
	virtual void UpdateBoundingBox();
	void SetOOBB(float fWidth, float fHeight, float fDepth);

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera* GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }



	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);

	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	virtual void Rotate(float x, float y, float z);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual void ReleaseUploadBuffers() {}
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

#define BULLETS				10
class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CAirplanePlayer();

	float						m_fBulletEffectiveRange = 800.0f;

	CPlayerBulletObject* pBulletObject = NULL;
	CGameObject* m_pMainMotorFrame = NULL;
	CGameObject* m_pLowMotorFrame = NULL;
	CGameObject* m_pSubEngine1MotorFrame = NULL;
	CGameObject* m_pSubEngine2MotorFrame = NULL;
	CGameObject* m_pSubEngine3MotorFrame = NULL;
	CGameObject* m_pSubEngine4MotorFrame = NULL;
	CGameObject* m_pSubEngine5MotorFrame = NULL;
	CGameObject* m_pUpMotorFrame = NULL;




private:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

public:
	//CBulletObject* m_ppBullets[BULLETS];
	void FireBullet(CGameObject* pLockedObject);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	float m_fdelrot = 1.0;
	BoundingOrientedBox xoobb = BoundingOrientedBox(GetPosition(), XMFLOAT3(15.0, 10.0, 30.0), XMFLOAT4(0.0, 0.0, 0.0, 1.0));

};

class CTankPlayer :public CPlayer {
public:
	CTankPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext);
	virtual ~CTankPlayer();

private:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

public:
	/*float						m_fBulletEffectiveRange = 800.0f;

	CBulletObject* pBulletObject = NULL;*/
	//CGameObject* m_pMainMotorFrame = NULL;
	virtual void Update(float fTimeElapsed)override;

	bool MousePressed = false;

	array<CGameObject*, 16> m_pWheel{};
	int m_nWheels = 16;
	bool is_RotateWheel = false; // 움직(회전 포함)이고 있는지
	bool RotateWheel_Forward = true; //바퀴 굴러가도록 false면 animate에서 뒤로 굴러가게
	bool is_Going = false;   // 좌우 누를때 판단
	bool Fired_Bullet = false; //총 쏘면 반동위해

	float turret_rotate_value = 0.0f; //시간에 비례한 회전량을 통해 회전 시키게
	float poshin_rotate_value = 0.0f; // 이는 a d키 누르면 씬에서 조작함.

	bool poshin_direction = false; //
	bool show_Track = false;

	/*CShader* m_pTrackShader = NULL;
	CGameObject* m_pTrack = NULL;*/

	CGameObject* m_pTurret = NULL;
	CGameObject* m_pPoshin = NULL;
	virtual void Rotate(float x, float y, float z)override;
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();

	void OnPlayerUpdateCallback(float fTimeElapsed);
	void OnCameraUpdateCallback(float fTimeElapsed);
	
	void UpdateTankPosition(float fTimeElpased);
	//---------------------
	bool Shake = false;
	float ShakeEffectTimeElapsed = 0.f;
	float ShakeRotationDuration = 0.065f;
	void ShakeEffect(float fTimeElapsed);
	//-----------
	bool Float_in_Water = false;
	float FloatEffectTimeElapsed = 0.f;
	float FloatUpDuration = 0.5f;
	void FloatEffect(float fTimeElapsed);
	//-------
	void FireEffect(float fTimeElapsed);
	float FireEffectTimeElapsed = 0.f;
	float FireEffectDuration = 0.15f;
	//----------------------
	void FireBullet(CGameObject* pLockedObject);
	float m_fdelrot = 1.0;
	BoundingOrientedBox xoobb = BoundingOrientedBox(GetPosition(), XMFLOAT3(15.0, 10.0, 30.0), XMFLOAT4(0.0, 0.0, 0.0, 1.0));

	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};


