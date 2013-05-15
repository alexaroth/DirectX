#include "graphics.h"


graphics::graphics(int Width, int Height)
{   
    lightDropped = false;
    rainActive = true;
    MainNode = new Node(0, "Main");
    MainNode -> AddChildNode(new Node(nullptr, "Child1"));
    MainNode -> AddChildNode(new Node(nullptr, "Child2"));
    MainNode -> GetChildNodeByName("Child1") -> AddChildNode(new Node(nullptr, "Child1_1"));
    MainNode -> GetChildNodeByName("Child2") -> AddChildNode(new Node(nullptr, "Child2_1"));
    MainNode -> GetChildNodeByName("Child2") -> AddChildNode(new Node(nullptr, "Child2_2"));

    this -> Width = Width;
    this -> Height = Height;
}


bool graphics::InitDirectInput(HINSTANCE hInstance, HWND hwnd)
{
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput,	nullptr); 
	DirectInput->CreateDevice(GUID_SysKeyboard,	&KeyboardInput, nullptr);
	DirectInput->CreateDevice(GUID_SysMouse,&MouseInput, nullptr);

	KeyboardInput -> SetDataFormat(&c_dfDIKeyboard);
	KeyboardInput -> SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	MouseInput -> SetDataFormat(&c_dfDIMouse);
	MouseInput -> SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}


bool graphics::InitializeDirect3d11App(int Width, int Height, HWND hwnd)
{	
	//Describe our SwapChain Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd; 
	swapChainDesc.Windowed = TRUE; 
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create DXGI factory to enumerate adapters
	IDXGIFactory1 *DXGIFactory;

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);	

	// Use the first adapter	
	IDXGIAdapter1 *Adapter;

	hr = DXGIFactory->EnumAdapters1(0, &Adapter);

	DXGIFactory->Release();	

	//Create our Direct3D 11 Device and SwapChain
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, NULL,	D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, nullptr, &d3d11DevCon);



	//Release the Adapter interface
	Adapter->Release();

	//Create our BackBuffer and Render Target
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer11 );
	hr = d3d11Device->CreateRenderTargetView( BackBuffer11, nullptr, &renderTargetView );

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = Width;
	depthStencilDesc.Height    = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	//Create the Depth/Stencil View
	d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);

	return true;
}


void graphics::CleanUp()
{
	//Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
	BackBuffer11->Release();
	cbPerFrameBuffer->Release();
}


void graphics::detectInput()
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];
	KeyboardInput -> Acquire();
	MouseInput -> Acquire();

	MouseInput -> GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	KeyboardInput -> GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);
    
	float speed = 15.0f * 0.01;

	if(keyboardState[DIK_A])
	{
        //Sound.resume_Walk();
		camera1.moveLeftRight(-speed);
	}

	if(keyboardState[DIK_D])
	{
        //Sound.resume_Walk();
		camera1.moveLeftRight(speed);
	}

	if(keyboardState[DIK_W])
	{
        //Sound.resume_Walk();
		camera1.moveBackForward(speed);
	}

	if(keyboardState[DIK_S])
	{
        //Sound.resume_Walk();
		camera1.moveBackForward(-speed);
	}

    if ((!keyboardState[DIK_S]) && (!keyboardState[DIK_W]) && (!keyboardState[DIK_A]) && (!keyboardState[DIK_D]))
    {
        //Sound.pause_Walk();
    }

    if(keyboardState[DIK_E])
	{
        if(!lightDroppedPressed)
        {
            lightDroppedPressed = true;
            lightDropped = !lightDropped;
        }   
	}

    else if(!keyboardState[DIK_E])
    {     
        lightDroppedPressed = false;
    }

    if(keyboardState[DIK_R])
	{
        if(!rainActivePressed)
        {
            //Sound.pause_resume_ambient();
            rainActivePressed = true;
            rainActive = !rainActive;
            rain_spawner.setEnabled(rainActive);
        }   
	}

    else if(!keyboardState[DIK_R])
    {             
        rainActivePressed = false;
    }

	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camera1.camYaw += mouseLastState.lX * 0.0012f;
		camera1.camPitch += mouseCurrState.lY * 0.0012f;
		mouseLastState = mouseCurrState;
	}

	camera1.UpdateCamera();

	return;
}


bool graphics::InitScene()
{
    camera1 = CameraClass();

	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"Tutorial02.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"Tutorial02.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), nullptr, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), nullptr, &PS);


	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);


	light.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	light.range = 100.0f;
	light.att = XMFLOAT3(0.3f, 0.019f, 0.0f);
	light.ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	light.diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
    light.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
    light.specularPower = 500;

    D3D11_INPUT_ELEMENT_DESC layout[] =     
    {
	    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	    { "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    numElements = ARRAYSIZE(layout);

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertLayout );

	//Set the Input Layout
	d3d11DevCon->IASetInputLayout( vertLayout );

	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, nullptr, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, nullptr, &cbPerFrameBuffer);

	//Set the Projection matrix
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, Width/Height, 1.0f, 1000.0f);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
    rtbd.SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;
    
    d3d11Device->CreateBlendState(&blendDesc, &Transparency);
    

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = d3d11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );
    

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CWcullMode);


    MainObject = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "strange2.obj", L"face.jpg", camera1, 1.0, true, nullptr);    
       
    ground = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ground.obj", L"ground.bmp", camera1, 1.0, true, nullptr);    
    cube1 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "cube.obj", L"tex3.png", camera1, 1, true, nullptr);
    cube3 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "cube.obj", L"tex2.jpg", camera1, 1, true, cube1.getRawModel());
    cube2 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "cube.obj", L"tex1.jpg", camera1, 1, true, cube1.getRawModel());
       
    ChildObject1 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.5, true, MainObject.getRawModel());
    ChildObject1_1 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.5, true, ChildObject1.getRawModel());
    ChildObject2 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.5, true, ChildObject1.getRawModel());
    ChildObject2_1 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.5, true, ChildObject1.getRawModel());
    ChildObject2_2 = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.3, true, ChildObject1.getRawModel());

    flashlight = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "ball2.obj", L"face.jpg", camera1, 0.1, true, ChildObject1.getRawModel());
   
    rainDrop = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "rainDrop.obj", L"rain.png", camera1, 0.02, false, nullptr);
    MG = GameObject(d3d11Device, CubesTexSamplerState, CWcullMode, CCWcullMode, "mg2.obj", L"face.jpg", camera1, 0.3, true, nullptr);

    
    rain_spawner = rainSpawner(rainDrop, camera1);

    MainNode -> setNodeGameObject(MainObject);
    MainNode -> GetChildNodeByName("Child1") -> setNodeGameObject(ChildObject1);
    MainNode -> GetChildNodeByName("Child2") -> setNodeGameObject(ChildObject2);
    MainNode -> GetChildNodeByName("Child1_1") ->setNodeGameObject(ChildObject1_1);
    MainNode -> GetChildNodeByName("Child2_1") ->setNodeGameObject(ChildObject2_1);
    MainNode -> GetChildNodeByName("Child2_2") ->setNodeGameObject(ChildObject2_2);

    MainObject.setWorldPos(15,0,10);
    ChildObject1.setWorldPos(8,0,0);
    ChildObject1_1.setWorldPos(4.0f,0,0);
    ChildObject2.setWorldPos(-5,0,-5);
    ChildObject2_1.setWorldPos(3.0f,0,3);
    ChildObject2_2.setWorldPos(3.0f,0,-3);

    ground.setWorldPos(0,-10,0);
    flashlight.setWorldPos(0,0,0);
    MG.setWorldPos(15,-2,-10);
    cube1.setWorldPos(-15,0.1,-6);
    cube2.setWorldPos(-15,0.1,-12);
    cube3.setWorldPos(-15,0.1,-18);
    
    MainObject.setRotating(true, -0.8);    
    ChildObject1.setRotating(true, 2);
    ChildObject1_1.setRotating(true, -7);
    ChildObject2.setRotating(true, -1);
    ChildObject2_1.setRotating(true, -10);
    ChildObject2_2.setRotating(true, 4);


    rainDrop.setRotating(false, 1);
    MG.setRotating(true, 2);
    cube1.setRotating(true, 0.5);
    cube2.setRotating(true, 1.5);
    cube3.setRotating(true, 1.0);

    //Sound.playAmbient();
    //Sound.playWalk();
    //Sound.pause_Walk();

	return true;
}


void graphics::UpdateScene(float tStep)
{    
    if(!lightDropped)
    {
        light.pos.x = XMVectorGetX(camera1.getPos());
	    light.pos.y = XMVectorGetY(camera1.getPos());
	    light.pos.z = XMVectorGetZ(camera1.getPos());     
    }

    flashlight.setWorldPos(light.pos.x, light.pos.y, light.pos.z);

    flashlight.update();
    ground.update();
    MG.update();
    MainNode -> Update(); 

    cube1.update();
    cube2.update();
    cube3.update();

    // Force the 1'st person camera to stay on the ground
   // camera1.SetPosition(XMVectorGetX(camera1.getPos()), 0,  XMVectorGetZ(camera1.getPos()));    
}


void graphics::DrawScene()
{    
	//Clear our render target and depth/stencil view
    float ClearColor[4] = {0.3f, 0.3f, 0.3f, 1.0f}; 
    
	d3d11DevCon -> ClearRenderTargetView( renderTargetView,  ClearColor);	
	d3d11DevCon -> ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    //Reset Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);
    
	constbuffPerFrame.light = light;
	d3d11DevCon-> UpdateSubresource( cbPerFrameBuffer, 0, nullptr, &constbuffPerFrame, 0, 0);
	d3d11DevCon-> PSSetConstantBuffers( 0, 1, &cbPerFrameBuffer);	
        
	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView);

	//Set transaprency
	d3d11DevCon->OMSetBlendState(Transparency, 0, 0xffffffff);
     

    MainObject.draw(d3d11DevCon, cbPerObjectBuffer);

    ground.draw(d3d11DevCon, cbPerObjectBuffer);   
   
    ChildObject1.draw(d3d11DevCon, cbPerObjectBuffer);
    ChildObject1_1.draw(d3d11DevCon, cbPerObjectBuffer);
    ChildObject2.draw(d3d11DevCon, cbPerObjectBuffer);
    ChildObject2_1.draw(d3d11DevCon, cbPerObjectBuffer);
    ChildObject2_2.draw(d3d11DevCon, cbPerObjectBuffer);

    MG.draw(d3d11DevCon, cbPerObjectBuffer);

        if(lightDropped)
	flashlight.draw(d3d11DevCon, cbPerObjectBuffer);

    cube2.draw(d3d11DevCon, cbPerObjectBuffer);
    cube3.draw(d3d11DevCon, cbPerObjectBuffer);
    cube1.draw(d3d11DevCon, cbPerObjectBuffer);

    rain_spawner.draw(d3d11DevCon, cbPerObjectBuffer);    


    //Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}


graphics::~graphics()
{
}
