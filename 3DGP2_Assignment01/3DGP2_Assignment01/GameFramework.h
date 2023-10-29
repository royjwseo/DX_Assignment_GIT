#pragma once



#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "GameSound.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();
#ifdef _WITH_DIRECT2D
	void CreateDirect2DDevice();
#endif
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void BuildObjects();
	void ReleaseObjects();

	void UpdateShaderVariables();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();


	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory = NULL;
	IDXGISwapChain3* m_pdxgiSwapChain = NULL;
	ID3D12Device* m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator* m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue* m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;

	ID3D12Fence* m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

#ifdef _WITH_DIRECT2D
	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;
	IDWriteTextFormat* m_pdwFont = NULL;
	IDWriteTextLayout* m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;

#ifdef _WITH_DIRECT2D_IMAGE_EFFECT
	IWICImagingFactory* m_pwicImagingFactory = NULL;
	int m_nUIinterfaces = 0;
	ID2D1Effect* m_pd2dfxBitmapSource[4];
	ID2D1Effect* m_pd2dfxGaussianBlur[4];
	ID2D1Effect* m_pd2dfxEdgeDetection[4];
	ID2D1DrawingStateBlock1* m_pd2dsbDrawingState[4];
	IWICFormatConverter* m_pwicFormatConverter = NULL;
	int							m_nDrawEffectImage = 0;
#endif
#endif
#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController;
#endif

	float elapsedTimeInSeconds = 0.f;
	float ones_x = 0.0f;
	float ones_y = 0.f;
	int ones_cnt{};
	float tens_x = 0.0f;
	float tens_y = 0.f;
	int tens_cnt{};

	float mins_ones_x = 0.0f;
	float mins_ones_y = 0.f;
	int mins_ones_cnt{};

	float width_png = 0.f;

	CGameTimer					m_GameTimer;

	CScene* m_pScene = NULL;
	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[70];



};

