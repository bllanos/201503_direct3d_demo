/*
d3dclass.h
----------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 18, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
  -Modified to incorporate transparency, as described at http://www.rastertek.com/dx11tut26.html
   (Rastertek DirectX 11 Tutorial 26: Transparency)

Description
  -Direct3D 11 initialization and shutdown
*/

////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


/////////////
// LINKING //
/////////////
/*
Specify the libraries to link when using this object module. 
These libraries contain all the Direct3D functionality for setting up and drawing 3D graphics in DirectX
as well as tools to interface with the hardware on the computer to obtain information about the refresh 
rate of the monitor, the video card being used, and so forth. 
*/
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <DirectXMath.h> //replaces XNAMath and previous math libraries since VS2010

using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
/*
This class is our encapsulation and gateway to the DirectX 11 3D pipeline
*/
class D3DClass
{
public:
	D3DClass();
	~D3DClass();

	int Initialize(int, int, bool, HWND, bool, float, float); //initialize the DirectX object/pipeline
	int Shutdown(); //release resources
	
	int BeginScene(float, float, float, float); //clear back buffer and depth  and get it ready to render on
	int EndScene(); //present back buffer to the screen

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	int GetProjectionMatrix(XMFLOAT4X4&);
	int GetWorldMatrix(XMFLOAT4X4&);
	int GetOrthoMatrix(XMFLOAT4X4&);
	// view matrix will be obtained from a camera object when needed

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	int GetVideoCardInfo(char*, int&);

	ID3D11Texture2D* GetBackBuffer(void);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain; //front-back buffer swap chain
	ID3D11Device* m_device;  //the DirectX 11 device
	ID3D11DeviceContext* m_deviceContext; //the DirectX 11 device context
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_orthoMatrix;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	ID3D11Texture2D* m_backBufferPtr;

	// Swap chain texture format
	DXGI_FORMAT m_format;

	// Currently not implemented - will cause linker errors if called
private:
	D3DClass(const D3DClass& other);
	D3DClass& operator=(const D3DClass& other);
};

#endif