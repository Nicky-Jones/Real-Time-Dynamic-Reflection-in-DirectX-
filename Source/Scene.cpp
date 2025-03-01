
//
// Scene.cpp
//

#include <stdafx.h>
#include <string.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <System.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <CGDClock.h>
#include <Scene.h>
#include <DynamicCubeReflection.h>

#include <Effect.h>
#include <VertexStructures.h>
#include <Texture.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//
// Methods to handle initialisation, update and rendering of the scene
HRESULT Scene::rebuildViewport(){
	// Binds the render target view and depth/stencil view to the pipeline.
	// Sets up viewport for the main window (wndHandle) 
	// Called at initialisation or in response to window resize
	ID3D11DeviceContext *context = system->getDeviceContext();
	if (!context)
		return E_FAIL;
	// Bind the render target view and depth/stencil view to the pipeline.
	ID3D11RenderTargetView* renderTargetView = system->getBackBufferRTV();
	context->OMSetRenderTargets(1, &renderTargetView, system->getDepthStencil());
	// Setup viewport for the main window (wndHandle)
	RECT clientRect;
	GetClientRect(wndHandle, &clientRect);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(clientRect.right - clientRect.left);
	viewport.Height = static_cast<FLOAT>(clientRect.bottom - clientRect.top);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//Set Viewport
	context->RSSetViewports(1, &viewport);
	return S_OK;
}

// Main resource setup for the application.  These are setup around a given Direct3D device.
HRESULT Scene::initialiseSceneResources() {
	ID3D11DeviceContext *context = system->getDeviceContext();
	ID3D11Device *device = system->getDevice();
	if (!device)
		return E_FAIL;
	// Set up viewport for the main window (wndHandle) 
	rebuildViewport();

	blurUtility = new BlurUtility(device, context);

	// Setup main effects (pipeline shaders, states etc)

	// The Effect class is a helper class similar to the depricated DX9 Effect. It stores pipeline shaders, pipeline states  etc and binds them to setup the pipeline to render with a particular Effect. The constructor requires that at least shaders are provided along a description of the vertex structure.
	Effect *basicColourEffect = new Effect(device, "Shaders\\cso\\basic_colour_vs.cso", "Shaders\\cso\\basic_colour_ps.cso", basicVertexDesc, ARRAYSIZE(basicVertexDesc));
	Effect *basicLightingEffect = new Effect(device, "Shaders\\cso\\basic_lighting_vs.cso", "Shaders\\cso\\basic_colour_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	perPixelLightingEffect = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	perPixelLightingEffectGS = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", "Shaders\\cso\\per_pixel_lighting_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	// Setup Textures
	// The Texture class is a helper class to load textures
	Texture* cubeDayTexture = new Texture(device, L"Resources\\Textures\\grassenvmap1024.dds");
	Texture* treeTexture = new Texture(device, context, L"Resources\\Textures\\tree.tif");
	Texture* grassAlpha = new Texture(device, L"Resources\\Textures\\grassAlpha.tif");
	Texture* grassTexture = new Texture(device, L"Resources\\Textures\\grass.png");
	Texture* heightMap = new Texture(device, L"Resources\\Textures\\heightmap2.bmp");
	Texture* normalMap = new Texture(device, L"Resources\\Textures\\normalmap.bmp");
	Texture* knightTexture = new Texture(device, L"Resources\\Textures\\knight_spec.jpg");
	Texture* bridgeTexture = new Texture(device, L"Resources\\Textures\\brick_DIFFUSE.jpg");
	Texture* castleTexture = new Texture(device, L"Resources\\Textures\\castle.jpg");


	//Setup SkyBox effect and custom pipeline states
	skyBoxEffect = new Effect(device, "Shaders\\cso\\sky_box_vs.cso", "Shaders\\cso\\sky_box_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	skyBoxEffectGS = new Effect(device, "Shaders\\cso\\sky_box_vs.cso", "Shaders\\cso\\sky_box_ps.cso", "Shaders\\cso\\sky_box_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	// The Effect class constructor sets default depth/stencil, rasteriser and blend states
	// The Effect binds these states to the pipeline whenever an object using the effect is rendered
	// We can customise states if required
	ID3D11DepthStencilState *skyBoxDSState = skyBoxEffect->getDepthStencilState();
	D3D11_DEPTH_STENCIL_DESC skyBoxDSDesc;
	skyBoxDSState->GetDesc(&skyBoxDSDesc);
	skyBoxDSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	skyBoxDSState->Release(); device->CreateDepthStencilState(&skyBoxDSDesc, &skyBoxDSState);
	skyBoxEffect->setDepthStencilState(skyBoxDSState);
	skyBoxEffectGS->setDepthStencilState(skyBoxDSState);

	ID3D11RasterizerState *rsStateSky=skyBoxEffect->getRasterizerState();;
	D3D11_RASTERIZER_DESC		RSdesc; 
	rsStateSky->GetDesc(&RSdesc);
	RSdesc.CullMode = D3D11_CULL_NONE;
	device->CreateRasterizerState(&RSdesc, &rsStateSky);
	skyBoxEffect->setRasterizerState(rsStateSky);
	skyBoxEffectGS->setRasterizerState(rsStateSky);

	//Setup SkyBox effect and custom pipeline states
	grassEffect = new Effect(device, "Shaders\\cso\\grass_vs.cso", "Shaders\\cso\\grass_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	treeEffect = new Effect(device, "Shaders\\cso\\tree_vs.cso", "Shaders\\cso\\tree_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	grassEffectGS = new Effect(device, "Shaders\\cso\\grass_vs.cso", "Shaders\\cso\\grass_ps.cso", "Shaders\\cso\\per_pixel_lighting_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	treeEffectGS = new Effect(device, "Shaders\\cso\\tree_vs.cso", "Shaders\\cso\\tree_ps.cso", "Shaders\\cso\\per_pixel_lighting_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));

	// Set up custome effect states for grass and tree effects
	ID3D11BlendState *grassBlendState = grassEffect->getBlendState();
	D3D11_BLEND_DESC grassBlendDesc;
	grassBlendState->GetDesc(&grassBlendDesc);
	grassBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	grassBlendDesc.AlphaToCoverageEnable = TRUE; // Use pixel coverage info from rasteriser 
	grassBlendState->Release(); device->CreateBlendState(&grassBlendDesc, &grassBlendState);
	grassEffect->setBlendState(grassBlendState);
	treeEffect->setBlendState(grassBlendState);
	grassEffectGS->setBlendState(grassBlendState);
	treeEffectGS->setBlendState(grassBlendState);

	Material logMat; logMat.setSpecular(XMCOLOR(0, 0, 0, 0));
	Material *logMatArray[] = { &logMat };
	dC = new DynamicCubeReflection();
	dC->setUpDynamicRelfection(device, context);

	//setUpDynamicRelfection(device);

	// The BaseModel class supports multitexturing and the constructor takes a pointer to an array of shader resource views of textures. 
	// Even if we only need 1 texture/shader resource view for an effect we still need to create an array.
	ID3D11ShaderResourceView *skyBoxTextureArray[] = { cubeDayTexture->getShaderResourceView() }; //cubeDayTexture->getShaderResourceView()
	ID3D11ShaderResourceView *grassTextureArray[] = { grassTexture->getShaderResourceView(), grassAlpha->getShaderResourceView() };
	ID3D11ShaderResourceView *treeTextureArray[] = { treeTexture->getShaderResourceView(), grassTexture->getShaderResourceView() };
	ID3D11ShaderResourceView *knightTextureArray[] = { knightTexture->getShaderResourceView()}; //knightTexture->getShaderResourceView()
	ID3D11ShaderResourceView *bridgeTextureArray[] = { bridgeTexture->getShaderResourceView() };
	ID3D11ShaderResourceView *castleTextureArray[] = { castleTexture->getShaderResourceView() };
	// Setup Objects - the object below are derived from the Base model class
	// The constructors for all objects derived from BaseModel require at least a valid pointer to the main DirectX device
	// And a valid effect with a vertex shader input structure that matches the object vertex structure.
	// In addition to the Texture array pointer mentioned above an additional optional parameters of the BaseModel class are a pointer to an array of Materials along with an int that contains the number of Materials
	// The baseModel class now manages a CBuffer containing model/world matrix properties. It has methods to update the cbuffers if the model/world changes 
	// The render methods of the objects attatch the world/model Cbuffers to the pipeline at slot b0 for vertex and pixel shaders
	
	// Create a skybox
	// The box class is derived from the BaseModel class 
	box = new Box(device, skyBoxEffect, NULL, 0, skyBoxTextureArray,1);
	// Add code here scale the box x1000
	box->setWorldMatrix(box->getWorldMatrix()*XMMatrixScaling(1000, 1000, 1000));
	box->update(context);

	//Create grassy ground terrain
	ground = new Terrain(context, heightMap->getTexture(), normalMap->getTexture(), device, grassEffect, NULL, 0, grassTextureArray, 2);
	ground->setWorldMatrix(ground->getWorldMatrix()*XMMatrixTranslation(-20, 0, -25)*XMMatrixScaling(4, 0.5, 4)*XMMatrixRotationY(XMConvertToRadians(45)));
	ground->update(context);

	// Add Trees
	// The Model class is derived from the BaseModel class with an additional load method that loads 3d data from a file
	// The load method makes use of the ASSIMP (open ASSet IMPort) Library for loading 3d data http://assimp.sourceforge.net/.
	Material treeMat;
	treeMat.setSpecular(XMCOLOR(0, 0, 0, 1));
	Material *matArray[] = { &treeMat };
	treeMat.setDiffuse(XMCOLOR(1, 0.5, 0.2, 1));
	trees[0] = new  Model(device, wstring(L"Resources\\Models\\tree.3ds"), treeEffect, matArray, 1, treeTextureArray, 2);
	treeMat.setDiffuse(XMCOLOR(0.7, 1, 0.7, 1));
	trees[1] = new  Model(device, wstring(L"Resources\\Models\\tree.3ds"), treeEffect, matArray, 1, treeTextureArray, 2);
	treeMat.setDiffuse(XMCOLOR(1, 0.5, 0.0, 1));
	trees[2] = new  Model(device, wstring(L"Resources\\Models\\tree.3ds"), treeEffect, matArray, 1, treeTextureArray, 2);

	//Move trees
	for (int i = 0; i < numTrees; i++)
	{
		treeTypeArray[i] = (int)((((float)rand() / RAND_MAX))*(float)numTreeTypes);
		float x = (((float)rand() / RAND_MAX)) - 0.5;
		float z = (((float)rand() / RAND_MAX)) - 0.5;
		float scale = (((float)rand() / RAND_MAX)*0.5 + 0.4);

		x *= 80;
		z *= 80;
		float y;

		y = ground->CalculateYValueWorld(x, z);
		treeArray[i] = XMMatrixRotationY((((float)rand() / RAND_MAX) - 0.5))*XMMatrixScaling(scale, scale, scale)*XMMatrixTranslation(x, y, z);
	}

	// Add a chrome orb 
	// The Model class is derived from the BaseModel class with an additional load method that loads 3d data from a file
	// The load method makes use of the ASSIMP (open ASSet IMPort) Library for loading 3d data http://assimp.sourceforge.net/.
	reflectionMapEffect = new Effect(device, "Shaders\\cso\\reflection_map_vs.cso", "Shaders\\cso\\reflection_map_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	reflectionMapEffectGS = new Effect(device, "Shaders\\cso\\reflection_map_vs.cso", "Shaders\\cso\\reflection_map_ps.cso", "Shaders\\cso\\reflection_map_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));

	// Lense Flares
	Effect *flareEffect = new Effect(device, "Shaders\\cso\\flare_vs.cso", "Shaders\\cso\\flare_ps.cso", flareVertexDesc, ARRAYSIZE(flareVertexDesc));

	//Enable Alpha Blending for Flare
	ID3D11BlendState *flareBlendState = flareEffect->getBlendState();
	D3D11_BLEND_DESC blendDesc;
	flareBlendState->GetDesc(&blendDesc);
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	// Create custom flare blend state object
	flareBlendState->Release(); device->CreateBlendState(&blendDesc, &flareBlendState);
	flareEffect->setBlendState(flareBlendState);

	Texture* flare1Texture = new Texture(device, L"Resources\\Textures\\flares\\divine.png");
	Texture* flare2Texture = new Texture(device, L"Resources\\Textures\\flares\\extendring.png");
	ID3D11ShaderResourceView *flare1TextureArray[] = { flare1Texture->getShaderResourceView() };
	ID3D11ShaderResourceView *flare2TextureArray[] = { flare2Texture->getShaderResourceView() };

	for (int i = 0; i < numFlares; i++)
		if (randM1P1()>0)
			flares[i] = new Flare(XMFLOAT3(-125.0, 60.0, 70.0), XMCOLOR(randM1P1()*0.5 + 0.5, randM1P1()*0.5 + 0.5, randM1P1()*0.5 + 0.5, (float)i / numFlares), device, flareEffect, NULL, 0, flare1TextureArray, 1);
		else
			flares[i] = new Flare(XMFLOAT3(-125.0, 60.0, 70.0), XMCOLOR(randM1P1()*0.5 + 0.5, randM1P1()*0.5 + 0.5, randM1P1()*0.5 + 0.5, (float)i / numFlares), device, flareEffect, NULL, 0, flare2TextureArray, 1);
	

	// Add skinned dragons
	skinningEffect = new Effect(device, "Shaders\\cso\\skinning_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", skinVertexDesc, ARRAYSIZE(skinVertexDesc));
	skinningEffectGS = new Effect(device, "Shaders\\cso\\skinning_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", "Shaders\\cso\\per_pixel_lighting_gs.cso", skinVertexDesc, ARRAYSIZE(skinVertexDesc));
	Texture *redDragonTexture = new Texture(device, context, L"Resources\\Other\\Free3D\\Black Dragon NEW\\textures\\RedDragon.jpg");
	Texture *greenDragonTexture = new Texture(device,context, L"Resources\\Other\\Free3D\\Black Dragon NEW\\textures\\GreenDragon.jpg");
	Texture *dragonNormalTexture = new Texture(device, context, L"Resources\\Other\\Free3D\\Black Dragon NEW\\textures\\Dragon_Nor_mirror2.jpg");

	redDragonTextureArray[0] = redDragonTexture->getShaderResourceView();
	context->GenerateMips(redDragonTexture->getShaderResourceView());
	redDragonTextureArray[1] = dragonNormalTexture->getShaderResourceView();
	greenDragonTextureArray[0] = greenDragonTexture->getShaderResourceView();
	greenDragonTextureArray[1] = dragonNormalTexture->getShaderResourceView();
	
	Material *dragonMat = new Material(); dragonMat->setSpecular(XMCOLOR(0.3, 0.3, 0.3, 0.08)); dragonMat->setDiffuse(XMCOLOR(0.8, 0.8, 0.5, 1.0));
	Material *dragonMatArray[] = { dragonMat };

	ID3D11ShaderResourceView *orbTextureArray[] = { nullptr ,dC->mDynamicCubeMapSRV };

	Material *orbMat = new Material();
	orbMat->setSpecular(XMCOLOR(1.0, 1.0, 1.0, 10.0));
	orbMat->setDiffuse(XMCOLOR(1.0, 1.0, 1.0, 1.0));
	Material *orbMatArray[] = { orbMat };

	orb = new  Model(device, wstring(L"Resources\\Models\\sphere.3ds"), reflectionMapEffectGS, orbMatArray, 1, orbTextureArray, 2);
	orb->setWorldMatrix(XMMatrixTranslation(0.9, 3.6, -1)*XMMatrixScaling(6.0, 6.0, 6.0));
	//orb->setWorldMatrix(XMMatrixScaling(6.0, 6.0, 6.0)*XMMatrixTranslation(-7, 7.6, -7));
	orb->update(context);

	castle = new Model(device, wstring(L"Resources\\Models\\castle.3ds"), perPixelLightingEffect, logMatArray, 1, castleTextureArray, 1);
	castle->setWorldMatrix(castle->getWorldMatrix()*XMMatrixScaling(6.0, 6.0, 6.0)*XMMatrixRotationY(-0.39)*XMMatrixTranslation(5, 1, -6));
	castle->update(context);

	mattWhite = new Material();
	mattWhite->setSpecular(XMCOLOR(0, 0, 0, 0));
	Material *whiteMatArray[] = { mattWhite };
	dragon = new  SkinnedModel(device, wstring(L"Resources\\Other\\Free3D\\Black Dragon NEW\\Dragon_Baked_Actions_fbx_7.4_binary.fbx"), skinningEffect, dragonMatArray, 1, redDragonTextureArray, 2);
	dragon->setWorldMatrix(XMMatrixScaling(0.001, 0.001, 0.001)*XMMatrixTranslation(17, 0, 0)*XMMatrixRotationY(-2));
	dragon->setCurrentAnim(1);
	dragon->update(context,0);

	knight = new Model(device, wstring(L"Resources\\Models\\knight.3ds"), perPixelLightingEffect, whiteMatArray, 1, knightTextureArray, 1);
	knight->setWorldMatrix(knight->getWorldMatrix()*XMMatrixTranslation(100, 110, 100)*XMMatrixScaling(0.02, 0.02, 0.02)*XMMatrixRotationY(-0.29));
	knight->update(context);

	bridge = new Model(device, wstring(L"Resources\\Models\\bridge.3ds"), perPixelLightingEffect, whiteMatArray, 1, bridgeTextureArray, 1);
	bridge->setWorldMatrix(bridge->getWorldMatrix()*XMMatrixTranslation(-70, 70, 110)*XMMatrixScaling(0.02, 0.02, 0.02)*XMMatrixRotationY(20));
	bridge->update(context);
	// Setup a camera
	// The LookAtCamera is derived from the base Camera class. The constructor for the Camera class requires a valid pointer to the main DirectX device
	// and and 3 vectors to define the initial position, up vector and target for the camera.
	// The camera class  manages a Cbuffer containing view/projection matrix properties. It has methods to update the cbuffers if the camera moves changes  
	// The camera constructor and update methods also attaches the camera CBuffer to the pipeline at slot b1 for vertex and pixel shaders
	//mainCamera =  new LookAtCamera(device, XMVectorSet(0.0, 0.0, -10.0, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorZero());
	mainCamera = new FirstPersonCamera(device, XMVectorSet(-9.0, 2.0, 17.0, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorSet(0.8f, 0.0f, -1.0f, 1.0f));
	mainCamera->setFlying(false);


	static const int numLights = 3;
	// Add a CBuffer to store light properties - you might consider creating a Light Class to manage this CBuffer
	// Allocate 16 byte aligned block of memory for "main memory" copy of cBufferLight
	cBufferLightCPU = (CBufferLight *)_aligned_malloc(sizeof(CBufferLight)*numLights, 16);

	// Fill out cBufferLightCPU
	cBufferLightCPU[0].lightVec = XMFLOAT4(-125.0, 100.0, 70.0,1.0);
	cBufferLightCPU[0].lightAmbient = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	cBufferLightCPU[0].lightDiffuse = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	cBufferLightCPU[0].lightSpecular = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	cBufferLightCPU[0].lightAttenuation = XMFLOAT4(1, 0.00, 0.0000, 1000.0);//const = 0.0; linear = 0.0; quad = 0.05; cutOff = 200.0;


	cBufferLightCPU[1].lightVec = XMFLOAT4(0.0, 0.0, 0.0, 1.0);
	cBufferLightCPU[1].lightAmbient = XMFLOAT4(0.0, 0.0, 0.0, 1.0);
	cBufferLightCPU[1].lightDiffuse = XMFLOAT4(0.5, 0.0, 0.5, 1.0);
	cBufferLightCPU[1].lightSpecular = XMFLOAT4(0.3, 0.0, 0.3, 1.0);
	cBufferLightCPU[1].lightAttenuation = XMFLOAT4(0.0, 0.0, 0.05, 10.0);//const = 0.0; linear = 0.0; quad = 0.05; cutOff = 10.0;



	// Create GPU resource memory copy of cBufferLight
	// fill out description (Note if we want to update the CBuffer we need  D3D11_CPU_ACCESS_WRITE)
	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));

	cbufferDesc.ByteWidth = sizeof(CBufferLight)*2;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferLightCPU;// Initialise GPU CBuffer with data from CPU CBuffer

	HRESULT hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData,
		&cBufferLightGPU);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferBridge);
	// We dont strictly need to call map here as the GPU CBuffer was initialised from the CPU CBuffer at creation.
	// However if changes are made to the CPU CBuffer during update the we need to copy the data to the GPU CBuffer 
	// using the mapCbuffer helper function provided the in Util.h   

	mapCbuffer(context, cBufferLightCPU, cBufferLightGPU, sizeof(CBufferLight)*2);
	context->PSSetConstantBuffers(2, 1, &cBufferLightGPU);// Attach CBufferLightGPU to register b2 for the Pixel shader.

	// Add a Cbuffer to stor world/scene properties
	// Allocate 16 byte aligned block of memory for "main memory" copy of cBufferLight
	cBufferSceneCPU = (CBufferScene *)_aligned_malloc(sizeof(CBufferScene), 16);

	// Fill out cBufferSceneCPU
	cBufferSceneCPU->windDir = XMFLOAT4(1, 0, 0, 1);
	cBufferSceneCPU->Time = 0.0;
	cBufferSceneCPU->grassHeight = 0.0;
	cBufferSceneCPU->USE_SHADOW_MAP = false;
	cBufferSceneCPU->REFLECTION_PASS = false;
	
	cbufferInitData.pSysMem = cBufferSceneCPU;// Initialise GPU CBuffer with data from CPU CBuffer
	cbufferDesc.ByteWidth = sizeof(CBufferScene);

	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferSceneGPU);

	mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
	context->VSSetConstantBuffers(3, 1, &cBufferSceneGPU);// Attach CBufferSceneGPU to register b3 for the vertex shader. Not strictly needed as our vertex shader doesnt require access to this CBuffer
	context->PSSetConstantBuffers(3, 1, &cBufferSceneGPU);// Attach CBufferSceneGPU to register b3 for the Pixel shader

	return S_OK;
}

// Update scene state (perform animations etc)
HRESULT Scene::updateScene(ID3D11DeviceContext *context,Camera *camera) {

	// mainClock is a helper class to manage game time data
	mainClock->tick();
	double dT = mainClock->gameTimeDelta();
	double gT = mainClock->gameTimeElapsed();

	// update camera
	if (!mainCamera->getFlying())//if not flying stick to ground
	{
		XMVECTOR camPos = camera->getPos();
		float camHeight = 1 + ground->CalculateYValueWorld(camPos.vector4_f32[0], camPos.vector4_f32[2]);
		mainCamera->setHeight(camHeight);
	}
	//camera->setPos(camPos);
	camera->update(context);

	// Update the scene time as it is needed for animated effects
	cBufferSceneCPU->Time = gT;
	mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
	

	// Animate orb

	//double movement = cos(gT * 2) + 3;
	/*orb->setWorldMatrix(XMMatrixTranslation(0, 4, 0));
	orb->update(context);*/
	//XMFLOAT3 changeMovement(0.0f, movement, 0.0f);
	//upDateCameras(0, movement, 0);

	// Animate Dragons

	// move to next animation sequence after 10 seconds
	static double nextAnimTimer = 0.0;
	nextAnimTimer += dT;
	if (nextAnimTimer > 10.0)
	{
		nextAnimTimer = 0.0;
		if (dragon->getCurrentAnim() == dragon->getNumAnimimations() - 1)
			dragon->setCurrentAnim(0);
		else
			dragon->setCurrentAnim(dragon->getCurrentAnim() + 1);
	}

	float r = 0;
	if (dragon->getCurrentAnim() == 2)r = -0.4*dT;
	else if (dragon->getCurrentAnim() == 3)r = -0.2*dT;
	dragon->setWorldMatrix(dragon->getWorldMatrix()*XMMatrixRotationY(r));
	XMVECTOR dragonPos = XMVectorZero();
	dragonPos = XMVector2TransformCoord(dragonPos, dragon->getWorldMatrix());
	float dragonHeight = ground->CalculateYValueWorld(dragonPos.vector4_f32[0], dragonPos.vector4_f32[2]);
	dragon->setWorldMatrix(dragon->getWorldMatrix()*XMMatrixTranslation(0, dragonHeight - dragonPos.vector4_f32[1], 0));
	dragon->update(context, gT);
	return S_OK;
}

void Scene::upDateCameras(float x, float y, float z)
{
	//for (int i = 0; i < 6; i++)
	//{
	//	CubeMapCamera[i]->setPos(XMVectorSet(x, y, z, 1));
	//}
	CubeMapCamera[0]->setPos(XMVectorSet(x, y, z, 1));
}
void Scene::DrawGrass(ID3D11DeviceContext *context)
{
	// Draw the Grass
	if (ground) {
		ground->render(context);
		//// Render grass layers from base to tip
		for (int i = 0; i < numGrassPasses; i++)
		{
			cBufferSceneCPU->grassHeight = (grassLength / numGrassPasses)*i;
			mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
			ground->render(context);
		}
	}
}
void Scene::DrawFlare(ID3D11DeviceContext *context)
{

if (flares) {

	ID3D11RenderTargetView * tempRT[1] = { 0 };
	ID3D11DepthStencilView *tempDS = nullptr;

	// Set NULL depth buffer so we can also use the Depth Buffer as a shader resource
	// This is OK as we dont need depth testing for the flares

	ID3D11DepthStencilView * nullDSV[1]; nullDSV[0] = NULL;
	context->OMGetRenderTargets(1, tempRT, &tempDS);
	context->OMSetRenderTargets(1, tempRT, NULL);
	ID3D11ShaderResourceView *depthSRV = system->getDepthStencilSRV();
	context->VSSetShaderResources(1, 1, &depthSRV);

	for (int i = 0; i < numFlares; i++)
		flares[i]->render(context);

	ID3D11ShaderResourceView * nullSRV[1]; nullSRV[0] = NULL; // Used to release depth shader resource so it is available for writing
	context->VSSetShaderResources(1, 1, nullSRV);
	// Return default (read and write) depth buffer view.
	context->OMSetRenderTargets(1, tempRT, tempDS);

	}
}

// Render scene
HRESULT Scene::renderScene() {

	ID3D11DeviceContext *context = system->getDeviceContext();
	static const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	// Validate window and D3D context
	if (isMinimised() || !context)
		return E_FAIL;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->PSSetShaderResources(1, 1, &nullSRV);
	ID3D11RenderTargetView* renderTargets[1];
	//knight->setEffect(perPixelLightingEffectGS);
	//bridge->setEffect(perPixelLightingEffectGS);
	box->setEffect(skyBoxEffectGS);
	ground->setEffect(grassEffectGS);
	//castle->setEffect(perPixelLightingEffectGS);
	for (int i = 0; i < numTrees; i++)
	{
		int type = treeTypeArray[i];
		trees[type]->setEffect(treeEffectGS);

	}
	dragon->setEffect(skinningEffectGS);

	dC->renderDynamicScene(context, std::bind(&Scene::renderTwice, this, std::placeholders::_1
	));
	//knight->setEffect(perPixelLightingEffect);
	//bridge->setEffect(perPixelLightingEffect);
	//castle->setEffect(perPixelLightingEffect);
	ground->setEffect(grassEffect);
	box->setEffect(skyBoxEffect);
	for (int i = 0; i < numTrees; i++)
	{
		int type = treeTypeArray[i];
		trees[type]->setEffect(treeEffect);

	}
	dragon->setEffect(skinningEffect);



	mainCamera->update(context);
	ID3D11DepthStencilView* depthStencil = system->getDepthStencil();
	// Clear the screen
	context->RSSetViewports(1, &viewport);
	renderTargets[0] = system->getBackBufferRTV();
	context->OMSetRenderTargets(
		1, renderTargets, depthStencil);

	//static const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(system->getBackBufferRTV(), clearColor);
	context->ClearDepthStencilView(system->getDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render Scene objects
	
	renderTwice(context);

	// Move dragon model and render red dragon instance
	dragon->setWorldMatrix(dragon->getWorldMatrix()*XMMatrixRotationY(XMConvertToRadians(80)));
	XMVECTOR dragonPos = XMVectorZero();
	dragonPos = XMVector2TransformCoord(dragonPos, dragon->getWorldMatrix());
	float dragonHeight = ground->CalculateYValueWorld(dragonPos.vector4_f32[0], dragonPos.vector4_f32[2]);
	dragon->setWorldMatrix(dragon->getWorldMatrix()*XMMatrixTranslation(0, dragonHeight - dragonPos.vector4_f32[1], 0));
	dragon->update(context, mainClock->gameTimeElapsed() + 3.0);
	dragon->setTextures(redDragonTextureArray, 2);
	dragon->render(context);

	// return original dragon model orientation
	dragon->setWorldMatrix(dragon->getWorldMatrix()*XMMatrixRotationY(XMConvertToRadians(-80)));

	if (knight)
		knight->render(context);
	if (bridge)
	{
		bridge->render(context);
	}
	if (castle)
	{
		castle->render(context);
	}
	// Render grass
	if (ground)
		DrawGrass(context);

	// Render chrome sphere
	if (orb) {
		orb->render(context);
		
	}

	DrawFlare(context);

	// Present current frame to the screen
	HRESULT hr = system->presentBackBuffer();

	return S_OK;
}

void Scene::renderTwice(ID3D11DeviceContext *context)
{

	// Render SkyBox
	if (box)
		box->render(context);
	//if (knight)
	//	knight->render(context);
	//if (bridge)
	//{
	//	bridge->render(context);
	//}
	if (ground)
		DrawGrass(context);
	// Render trees
	if (true)
		for (int i = 0; i < numTrees; i++)
		{
			int type = treeTypeArray[i];
			trees[type]->setWorldMatrix(treeArray[i]);
			trees[type]->update(context);
			trees[type]->render(context);
		}
	// Render green dragon
	dragon->setTextures(greenDragonTextureArray, 2);
	dragon->render(context);
}

//
// Event handling methods
//
// Process mouse move with the left button held down
void Scene::handleMouseLDrag(const POINT &disp) {
	//LookAtCamera

	//mainCamera->rotateElevation((float)-disp.y * 0.01f);
	//mainCamera->rotateOnYAxis((float)-disp.x * 0.01f);

	//FirstPersonCamera
	mainCamera->elevate((float)-disp.y * 0.005f);
	mainCamera->turn((float)disp.x * 0.005f);
}

// Process mouse wheel movement
void Scene::handleMouseWheel(const short zDelta) {
	//LookAtCamera

	//if (zDelta<0)
	//	mainCamera->zoomCamera(1.2f);
	//else if (zDelta>0)
	//	mainCamera->zoomCamera(0.9f);
	//cout << "zoom" << endl;
	//FirstPersonCamera
	mainCamera->move(zDelta*0.01);
}

// Process key down event.  keyCode indicates the key pressed while extKeyFlags indicates the extended key status at the time of the key down event (see http://msdn.microsoft.com/en-gb/library/windows/desktop/ms646280%28v=vs.85%29.aspx).
void Scene::handleKeyDown(const WPARAM keyCode, const LPARAM extKeyFlags) {
	// Add key down handler here...

	if (keyCode == VK_PRIOR)
		if (dragon->getCurrentAnim() == 0)
			dragon->setCurrentAnim(dragon->getNumAnimimations() - 1);
		else
			dragon->setCurrentAnim(dragon->getCurrentAnim()-1);
	
	if (keyCode == VK_NEXT)
		if (dragon->getCurrentAnim() == dragon->getNumAnimimations() - 1)
			dragon->setCurrentAnim(0);
		else
			dragon->setCurrentAnim(dragon->getCurrentAnim() +1);

	if (keyCode == VK_HOME)
		mainCamera->elevate(0.05f);
	
	if (keyCode == VK_END)
		mainCamera->elevate(-0.05f);
	
	if (keyCode == VK_LEFT)
		mainCamera->turn(-0.05f);

	if (keyCode == VK_RIGHT)
		mainCamera->turn(0.05f);
	
	if (keyCode == VK_SPACE)
	{
		bool isFlying = mainCamera->toggleFlying();
		if (isFlying)
			cout << "Flying mode is on" << endl;
		else
			cout << "Flying mode is off" << endl;
	}
	if (keyCode == VK_UP)
		mainCamera->move(0.5);
	
	if (keyCode == VK_DOWN)
		mainCamera->move(-0.5);
	



}
// Process key up event.  keyCode indicates the key released while extKeyFlags indicates the extended key status at the time of the key up event (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms646281%28v=vs.85%29.aspx).
void Scene::handleKeyUp(const WPARAM keyCode, const LPARAM extKeyFlags) {
	// Add key up handler here...
}


// Clock handling methods
void Scene::startClock() {
	mainClock->start();
}

void Scene::stopClock() {
	mainClock->stop();
}

void Scene::reportTimingData() {

	cout << "Actual time elapsed = " << mainClock->actualTimeElapsed() << endl;
	cout << "Game time elapsed = " << mainClock->gameTimeElapsed() << endl << endl;
	mainClock->reportTimingData();
}

// Private constructor
Scene::Scene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {
	try
	{
		// 1. Register window class for main DirectX window
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = wndClassName;
		wcex.hIconSm = NULL;
		if (!RegisterClassEx(&wcex))
			throw exception("Cannot register window class for Scene HWND");
		// 2. Store instance handle in our global variable
		hInst = hInstance;
		// 3. Setup window rect and resize according to set styles
		RECT		windowRect;
		windowRect.left = 0;
		windowRect.right = _width;
		windowRect.top = 0;
		windowRect.bottom = _height;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		// 4. Create and validate the main window handle
		wndHandle = CreateWindowEx(dwExStyle, wndClassName, wndTitle, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 200, 100, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInst, this);
		if (!wndHandle)
			throw exception("Cannot create main window handle");
		ShowWindow(wndHandle, nCmdShow);
		UpdateWindow(wndHandle);
		SetFocus(wndHandle);
		// 5. Create DirectX host environment (associated with main application wnd)
		system = System::CreateDirectXSystem(wndHandle);
		if (!system)
			throw exception("Cannot create Direct3D device and context model");
		// 6. Setup application-specific objects
		HRESULT hr = initialiseSceneResources();
		if (!SUCCEEDED(hr))
			throw exception("Cannot initalise scene resources");
		// 7. Create main clock / FPS timer (do this last with deferred start of 3 seconds so min FPS / SPF are not skewed by start-up events firing and taking CPU cycles).
		mainClock = CGDClock::CreateClock(string("mainClock"), 3.0f);
		if (!mainClock)
			throw exception("Cannot create main clock / timer");
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		// Re-throw exception
		throw;
	}
}

// Helper function to call updateScene followed by renderScene
HRESULT Scene::updateAndRenderScene() {
	ID3D11DeviceContext *context = system->getDeviceContext();
	HRESULT hr = updateScene(context, (Camera*)mainCamera);
	if (SUCCEEDED(hr))
		hr = renderScene();
	return hr;
}

// Return TRUE if the window is in a minimised state, FALSE otherwise
BOOL Scene::isMinimised() {

	WINDOWPLACEMENT				wp;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);

	return (GetWindowPlacement(wndHandle, &wp) != 0 && wp.showCmd == SW_SHOWMINIMIZED);
}

//
// Public interface implementation
//
// Method to create the main Scene instance
Scene* Scene::CreateScene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {
	static bool _scene_created = false;
	Scene *system = nullptr;
	if (!_scene_created) {
		system = new Scene(_width, _height, wndClassName, wndTitle, nCmdShow, hInstance, WndProc);
		if (system)
			_scene_created = true;
	}
	return system;
}

// Destructor
Scene::~Scene() {
	//Clean Up
	if (wndHandle)
		DestroyWindow(wndHandle);
}

// Call DestoryWindow on the HWND
void Scene::destoryWindow() {
	if (wndHandle != NULL) {
		HWND hWnd = wndHandle;
		wndHandle = NULL;
		DestroyWindow(hWnd);
	}
}

//
// Private interface implementation
//
// Resize swap chain buffers and update pipeline viewport configurations in response to a window resize event
HRESULT Scene::resizeResources() {
	if (system) {
		// Only process resize if the System *system exists (on initial resize window creation this will not be the case so this branch is ignored)
		HRESULT hr = system->resizeSwapChainBuffers(wndHandle);
		rebuildViewport();
		RECT clientRect;
		GetClientRect(wndHandle, &clientRect);
		if (!isMinimised())
			renderScene();
	}
	return S_OK;
}

