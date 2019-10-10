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
#include <functional>
//#include <Scene.h>
class Scene;

class DynamicCubeReflection {

	System									*system = nullptr;
	Scene									*sceneClass;

public:

	ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	ID3D11RenderTargetView* mDynamicCubeMapRTV;
	ID3D11DepthStencilView* DynamicCubeMapDSV;
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ID3D11Texture2D*cubeMapTexture = nullptr;
	CBufferCube* cBufferDyanmicCamera = nullptr;
	ID3D11Buffer *cBufferDyanmicCameraGPU = nullptr;
	void setUpDynamicRelfection(ID3D11Device*, ID3D11DeviceContext*);
	void setUpCameraCbuffer(ID3D11Device*, ID3D11DeviceContext*);
	void setUpCamera(ID3D11Device*);

	HRESULT renderDynamicScene(ID3D11DeviceContext *context, function<void(ID3D11DeviceContext*)> renderTwice);


	Camera									*CubeMapCamera[6];
	D3D11_VIEWPORT							CubeMapViewport;


	//HRESULT renderDynamicScene(Box, Model*, XMMATRIX, int);
};