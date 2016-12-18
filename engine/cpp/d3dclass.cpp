/*
d3dclass.cpp
------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes
Bernard Llanos
Mark Wilkes

Incorporated September 20, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5
  -Modified to incorporate transparency, as described at http://www.rastertek.com/dx11tut26.html
   (Rastertek DirectX 11 Tutorial 26: Transparency)

Description
  -Implementation of the D3DClass class
*/

////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"
#include "engineGlobals.h"
#include <exception>

D3DClass::D3DClass()
{
	//set all member pointers to 0
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;

	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;

	m_backBufferPtr = 0;

	// Set regular 32-bit surface for the back buffer.
	m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
}

D3DClass::~D3DClass()
{
}

/*
The Initialize function is what does the entire setup of Direct3D for DirectX 11.  
*/
int D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;  
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	D3D11_BLEND_DESC blendStateDescription;

	/*
	Before we can initialize Direct3D we have to get the refresh rate from the video card/monitor. 
	Each computer may be slightly different so we will need to query for that information. 
	We query for the numerator and denominator values and then pass them to DirectX during the setup 
	and it will calculate the proper refresh rate. 
	If we don't do this and just set the refresh rate to a default value which may not exist on all 
	computers then DirectX will respond by performing a blit instead of a buffer flip 
	which will degrade performance and give us annoying errors in the debug output. 
	*/
	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return C_BAD_SETUP;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return C_BAD_CNSTRCT;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	/*for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}*/

	numerator = 800;
	denominator = 600;
	/*
	We now have the numerator and denominator for the refresh rate. 
	The last thing we will retrieve using the adapter is the name of the video card and the 
	amount of memory on the video card. 
	*/

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return C_LIB_ERR;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	/*
	Now that we have the refresh rate from the system we can start the DirectX initialization. 
	The first thing we'll do is fill out the description of the swap chain. 
	The swap chain is the front and back buffer to which the graphics will be drawn. 
	Generally you use a single back buffer, do all your drawing to it, and then swap it to the 
	front buffer which then displays on the user's screen. That is why it is called a swap chain. 
	*/

	// Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1; //count does not include the front buffer

	// Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

	swapChainDesc.BufferDesc.Format = m_format;

	/*
	The next part of the description of the swap chain is the refresh rate. 
	The refresh rate is how many times a second it draws the back buffer to the front buffer. 
	If vsync is set to true in our graphicsclass.h header then this will lock the refresh rate 
	to the system settings (for example 60hz). 
	That means it will only draw the screen 60 times a second 
	(or higher if the system refresh rate is more than 60). 
	However if we set vsync to false then it will draw the screen as many times a second as it can, 
	however this can cause some visual artifacts. 
	*/
	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //we will be drawing on this buffer

	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1; //single sample per pixel
    swapChainDesc.SampleDesc.Quality = 0; 

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	/*
	After setting up the swap chain description we also need to setup one more variable 
	called the feature level. This variable tells DirectX what version we plan to use. 
	Here we set the feature level to 11.0 which is DirectX 11. 
	*/
	// Set the feature level to DirectX 10.
	featureLevel = D3D_FEATURE_LEVEL_10_0;

	/*
	Now that the swap chain description and feature level have been filled out we can create the swap chain, 
	the Direct3D device, and the Direct3D device context. 
	The Direct3D device and Direct3D device context are very important, they are the 
	interface to all of the Direct3D functions. 
	We will use the device and device context for almost everything from this point forward. 

    Those of you reading this who are familiar with the previous versions of DirectX will 
	recognize the Direct3D device but will be unfamiliar with the new Direct3D device context. 
	Basically they took the functionality of the Direct3D device and split it up into two 
	different devices so you need to use both now. 

    Note that if the user does not have a DirectX 11 video card this function call will 
	fail to create the device and device context. 
	Also if you are testing DirectX 11 functionality yourself and don't have a 
	DirectX 11 video card then you can replace D3D_DRIVER_TYPE_HARDWARE with 
	D3D_DRIVER_TYPE_REFERENCE and DirectX will use your CPU to draw instead 
	of the video card hardware. 
	Note that this runs 1/1000 the speed but it is good for people who don't 
	have DirectX 11 video cards yet on all their machines. 
	*/
	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(
		 NULL,   //use default graphics adapter
		 D3D_DRIVER_TYPE_HARDWARE, //hardware graphics acceleration
		 NULL, //no software driver, we are using hardware driver
		 D3D11_CREATE_DEVICE_DEBUG, //set up for single threaded use
		 &featureLevel, //pointer to D3D feature levels array (enum)
		 1, //number of elements in previous parameter
		 D3D11_SDK_VERSION, //macro to set DirectX SDK version number correctly
		 &swapChainDesc, //pointer to our swap chain description based on our window
		 &m_swapChain, // out: set our m_swapCain pointer
		 &m_device,    //out: set our m_device pointer
		 NULL,       //out: ptr to D3D_FEATURE_LEVEL of created device
		 &m_deviceContext); //out: set our m_device_context pointer

	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	/*
	Now that we have a swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. 
	We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain. 
	*/
	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBufferPtr);
	if(FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(m_backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	/*
	We will also need to set up a depth buffer description. 
	We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. 
	At the same time we will attach a stencil buffer to our depth buffer. 
	The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things. 
	*/
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	/*
	Create the depth/stencil buffer using that description. 
	You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture. 
	The reason for this is that once your polygons are sorted and then rasterized they just end up being 
	colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen. 
	*/
	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	/*
	Now we need to setup the depth stencil description. 
	This allows us to control what type of depth test Direct3D will do for each pixel. 
	*/
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	/*
	With the description filled out we can now create a depth stencil state. 
	*/
	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	/*
	With the created depth stencil state we can now set it so that it takes effect. Notice we use the device context to set it. 
	*/
	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	/*
	The next thing we need to create is the description of the view of the depth stencil buffer. 
	We do this so that Direct3D knows to use the depth buffer as a depth stencil texture. 
	After filling out the description we then call the function CreateDepthStencilView to create it. 
	*/
	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	/*
	Now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer 
	to the output render pipeline. This way the graphics that the pipeline renders will 
	get drawn to our back buffer that we previously created. 
	With the graphics written to the back buffer we can then swap it to the front and display 
	our graphics on the user's screen. 
	*/
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	/*
	Now that the render targets are setup we can continue on to some extra functions 
	that will give us more control over our scenes for future tutorials. 
	First thing is we'll create is a rasterizer state. This will give us control over how polygons 
	are rendered. We can do things like make our scenes render in wireframe mode or have 
	DirectX draw both the front and back faces of polygons. 
	By default DirectX already has a rasterizer state set up and working 
	the exact same as the one below but you have no control to change it unless you set up one yourself. 
	*/
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return C_BAD_CNSTRCT;
	}

	// Now set the rasterizer state using the device context.
	m_deviceContext->RSSetState(m_rasterState);
	
	/*
	The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. 
	Set this to be the entire size of the window. 
	*/
	// Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

	// Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

	/*
	Now we will create the projection matrix. The projection matrix is used to translate the 3D scene 
	into the 2D viewport space that we previously created. 
	We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes. 
	*/
	// Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth));

	/*
	We will also create another matrix called the world matrix. This matrix is used to convert the vertices of our 
	objects into vertices in the 3D scene. This matrix will also be used to rotate, translate, 
	and scale our objects in 3D space. From the start we will just initialize the matrix to the 
	identity matrix and keep a copy of it in this object. The copy will be needed to be passed 
	to the shaders for rendering also.
	*/
    // Initialize the world matrix to the identity matrix.
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());

	/*
	And the final thing we will setup in the Initialize function is an orthographic projection matrix. 
	This matrix is used for rendering 2D elements like user interfaces on the screen allowing us to skip the 3D rendering. 
	You will see this used in later tutorials when we look at rendering 2D graphics and fonts to the screen. 
	*/
	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&m_orthoMatrix, XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth));


	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	// blendStateDescription.AlphaToCoverageEnable = true; // This is a workaround for drawing transparent objects out of Z-order mentioned by classmates
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return C_LIB_ERR;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return C_LIB_ERR;
	}


    return C_OK;
}


/*
The Shutdown function will release and clean up all the pointers used in the Initialize function, 
its pretty straight forward. However before doing that I put in a call to force the swap chain to 
go into windowed mode first before releasing any pointers. If this is not done and you try to release 
the swap chain in full screen mode it will throw some exceptions. So to avoid that happening we 
just always force windowed mode before shutting down Direct3D. 
*/
int D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_backBufferPtr) {
		m_backBufferPtr->Release();
		m_backBufferPtr = 0;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return C_OK;
}


/*
In the D3DClass I have a couple helper functions. 
The first two are BeginScene and EndScene. 
BeginScene will be called whenever we are going to draw a new 3D scene at the beginning of each frame. 
All it does is initializes the buffers so they are blank and ready to be drawn to. 
The other function is Endscene, it tells the swap chain to display our 3D scene once all the drawing has 
completed at the end of each frame.
*/
int D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return C_OK;
}


int D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return C_OK;
}


/*
These next functions simply get pointers to the Direct3D device and the Direct3D device context. 
These helper functions will be called by the game framework often.
*/
ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

/*
The next three helper functions give copies of the projection, world, and orthographic matrices to calling functions. 
Most shaders will need these matrices for rendering so there needed to be an easy way for outside objects 
to get a copy of them.
*/
int D3DClass::GetProjectionMatrix(XMFLOAT4X4& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return C_OK;
}


int D3DClass::GetWorldMatrix(XMFLOAT4X4& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return C_OK;
}


int D3DClass::GetOrthoMatrix(XMFLOAT4X4& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return C_OK;
}

/*
The last helper function returns by reference the name of the video card and the amount of dedicated memory on the video card. 
Knowing the video card name and amount of video memory can help in debugging on different configurations.
*/
int D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return C_OK;
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

ID3D11Texture2D*D3DClass::GetBackBuffer(void) {
	return m_backBufferPtr;
}