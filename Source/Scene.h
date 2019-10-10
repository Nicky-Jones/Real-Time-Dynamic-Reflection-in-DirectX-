
//
// Scene.h
//

// DirectX interfaces and scene variables (model)
#pragma once
#include <Windows.h>
#include <CGDClock.h>
#include <Camera.h>
#include <LookAtCamera.h>
#include <Triangle.h>
#include <Model.h>
#include <Box.h>
#include <Grid.h>
#include <Flare.h>
#include <BlurUtility.h>
#include <CBufferStructures.h>
#include <SkinnedModel.h>
#include <FirstPersonCamera.h>
#include <Terrain.h>
#include <DynamicCubeReflection.h>

class DynamicCubeReflection;

class Scene{

	HINSTANCE								hInst = NULL;
	HWND									wndHandle = NULL;

	// Strong reference to associated Direct3D device and rendering context.
	System									*system = nullptr;

	D3D11_VIEWPORT							viewport;
	D3D11_VIEWPORT							CubeMapViewport;
	CGDClock								*mainClock;
	//LookAtCamera							*mainCamera;
	FirstPersonCamera							*mainCamera;
	Camera									*CubeMapCamera[6];
	DynamicCubeReflection					*dC;

	ID3D11RasterizerState *rsStateCullFront;
	ID3D11RasterizerState *rsStateCullBack;
	ID3D11RasterizerState *rsStateCullNone;

	CBufferScene* cBufferSceneCPU = nullptr;
	ID3D11Buffer *cBufferSceneGPU = nullptr;
	CBufferLight* cBufferLightCPU = nullptr;
	ID3D11Buffer *cBufferLightGPU = nullptr;
	ID3D11Texture2D*cubeMapTexture = nullptr;




	
	
	SkinnedModel							*dragon = nullptr;
	ID3D11ShaderResourceView *redDragonTextureArray[2];
	ID3D11ShaderResourceView *greenDragonTextureArray[2];
	ID3D11ShaderResourceView *purpleDragonTextureArray[2];
	//ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	//ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	//ID3D11RenderTargetView* g_pEnvMapRTV;
	//ID3D11DepthStencilView* DynamicCubeMapDSV; 
	//D3D11_TEXTURE2D_DESC depthTexDesc;


	Grid									*water = nullptr;
	BlurUtility								*blurUtility = nullptr;
	static const int					numFlares = 6;
	Flare								*flares[numFlares];

	// Private constructor
	Scene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc);
	// Return TRUE if the window is in a minimised state, FALSE otherwise
	BOOL isMinimised();

public:
	// Public methods
	// Method to create the main Scene
	static Scene* CreateScene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc);
	

	// Add objects to the scene
	Triangle								*triangle = nullptr; //pointer to a Triangle the actual triangle is created in initialiseSceneResources
	Box										*box = nullptr; //pointer to a Triangle the actual triangle is created in initialiseSceneResources
	Model									*orb = nullptr;
	Model									*knight = nullptr;
	Model									*castle = nullptr;
	Model									*bridge = nullptr;
	ID3D11Buffer							*cBufferBridge = nullptr;
	CBufferExt								*cBufferExtSrc = nullptr;
	Material								*mattWhite;
	static const int					numTreeTypes = 3;
	static const int					numTrees = 10;
	XMMATRIX treeArray[numTrees];
	int treeTypeArray[numTrees];
	Model									*trees[3];
	Grid									*grass = nullptr;
	Terrain									*ground = nullptr;
	float									grassLength = 0.01f;
	int										numGrassPasses = 20;

	Effect *grassEffect;
	Effect *treeEffect;
	Effect *grassEffectGS;
	Effect *treeEffectGS;
	Effect *skyBoxEffect;
	Effect *skyBoxEffectGS;
	Effect *reflectionMapEffect;
	Effect *reflectionMapEffectGS;
	Effect *skinningEffect;
	Effect *skinningEffectGS;
	Effect *perPixelLightingEffect;
	Effect *perPixelLightingEffectGS;

	// Methods to handle initialisation, update and rendering of the scene
	HRESULT rebuildViewport();
	HRESULT initialiseSceneResources();
	HRESULT updateScene(ID3D11DeviceContext *context, Camera *camera);
	HRESULT renderScene();
	void DrawFlare(ID3D11DeviceContext *context);
	void DrawGrass(ID3D11DeviceContext *context);
	// Clock handling methods
	void startClock();
	void stopClock();
	void reportTimingData();
	void upDateCameras(float, float, float);
	void setUpDynamicRelfection(ID3D11Device*);
	void setUpCamera(ID3D11Device*);
	void renderTwice(ID3D11DeviceContext *context);
	// Event handling methods
	// Process mouse move with the left button held down
	void handleMouseLDrag(const POINT &disp);
	// Process mouse wheel movement
	void handleMouseWheel(const short zDelta);
	// Process key down event.  keyCode indicates the key pressed while extKeyFlags indicates the extended key status at the time of the key down event (see http://msdn.microsoft.com/en-gb/library/windows/desktop/ms646280%28v=vs.85%29.aspx).
	void handleKeyDown(const WPARAM keyCode, const LPARAM extKeyFlags);
	// Process key up event.  keyCode indicates the key released while extKeyFlags indicates the extended key status at the time of the key up event (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms646281%28v=vs.85%29.aspx).
	void handleKeyUp(const WPARAM keyCode, const LPARAM extKeyFlags);
	
	// Helper function to call updateScene followed by renderScene
	HRESULT updateAndRenderScene();

	// Destructor
	~Scene();
	// Decouple the encapsulated HWND and call DestoryWindow on the HWND
	void destoryWindow();
	// Resize swap chain buffers and update pipeline viewport configurations in response to a window resize event
	HRESULT resizeResources();
};
