#include <stdafx.h>
#include <string.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <System.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <CGDClock.h>
#include <DynamicCubeReflection.h>
#include <Scene.h>
#include <Effect.h>
#include <VertexStructures.h>
#include <Texture.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;



void DynamicCubeReflection::setUpDynamicRelfection(ID3D11Device *device, ID3D11DeviceContext* context)
{
	//ID3D11DeviceContext *context = system->getDeviceContext();
	// set up custom bind render target
	static const int CubeMapSize = 256;

	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS |
		D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HRESULT hr = device->CreateTexture2D(&texDesc, 0, &cubeTex);


	// set up render target view texture
	//D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//rtvDesc.Format = texDesc.Format;
	//rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	//rtvDesc.Texture2DArray.MipSlice = 0;
	//// Only create a view description to one array element.
	//rtvDesc.Texture2DArray.ArraySize = 1;
	//// Create a render target view to each texture in the array
	//for (int i = 0; i < 6; ++i)
	//{
	//	rtvDesc.Texture2DArray.FirstArraySlice = i;
	//	hr = device->CreateRenderTargetView(
	//		cubeTex, &rtvDesc, &mDynamicCubeMapRTV[i]);
	//}





	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = texDesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	DescRT.Texture2DArray.FirstArraySlice = 0;
	DescRT.Texture2DArray.ArraySize = 6;
	DescRT.Texture2DArray.MipSlice = 0;
	hr = device->CreateRenderTargetView(
		cubeTex, &DescRT, &mDynamicCubeMapRTV);
	
	



	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;
	hr = device->CreateShaderResourceView(
		cubeTex, &srvDesc, &mDynamicCubeMapSRV);
	// View saves reference.renderTargetView->Release();
	cubeTex->Release();



	depthTexDesc.Width = CubeMapSize; //256
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 6;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	ID3D11Texture2D* depthTex = 0;
	hr = device->CreateTexture2D(&depthTexDesc, 0, &depthTex);


	// Create the depth stencil view for the entire cube
	//D3D11_DEPTH_STENCIL_VIEW_DESC DescDS;		//working version!!!
	//DescDS.Format = DXGI_FORMAT_D32_FLOAT;
	//DescDS.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//DescDS.Flags = 0;
	//DescDS.Texture2DArray.FirstArraySlice = 0;
	//DescDS.Texture2DArray.MipSlice = 0;
	//hr = device->CreateDepthStencilView(
	//	depthTex, &DescDS, &DynamicCubeMapDSV);

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC DescDS;
	DescDS.Format = DXGI_FORMAT_D32_FLOAT;
	DescDS.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DescDS.Texture2DArray.FirstArraySlice = 0;
	DescDS.Texture2DArray.ArraySize = 6;
	DescDS.Texture2DArray.MipSlice = 0;
	DescDS.Flags = 0;
	hr = device->CreateDepthStencilView(
		depthTex, &DescDS, &DynamicCubeMapDSV);



	//D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	//dsvDesc.Format = depthTexDesc.Format;
	//dsvDesc.Flags = 0;
	//dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dsvDesc.Texture2D.MipSlice = 0;
	//hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &DynamicCubeMapDSV);

	if (depthTex)
		depthTex->Release();


	CubeMapViewport.TopLeftX = 0.0f;
	CubeMapViewport.TopLeftY = 0.0f;
	CubeMapViewport.Width = (float)CubeMapSize;
	CubeMapViewport.Height = (float)CubeMapSize;
	CubeMapViewport.MinDepth = 0.0f;
	CubeMapViewport.MaxDepth = 1.0f;

	setUpCamera(device);

	setUpCameraCbuffer(device, context);
}

void DynamicCubeReflection::setUpCameraCbuffer(ID3D11Device *device, ID3D11DeviceContext* context)
{
	cBufferDyanmicCamera = (CBufferCube *)_aligned_malloc(sizeof(CBufferCube), 16);

	for (int i = 0; i < 6; i++)
	{
		cBufferDyanmicCamera->viewMatrix[i] = CubeMapCamera[i]->getViewMatrix();
	}
	cBufferDyanmicCamera->projMatrix = CubeMapCamera[0]->getProjMatrix();
	// Create GPU resource memory copy of cBufferLight
	// fill out description (Note if we want to update the CBuffer we need  D3D11_CPU_ACCESS_WRITE)
	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));

	cbufferDesc.ByteWidth = sizeof(CBufferCube);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferDyanmicCamera;// Initialise GPU CBuffer with data from CPU CBuffer

	HRESULT hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData,
		&cBufferDyanmicCameraGPU);

	// We dont strictly need to call map here as the GPU CBuffer was initialised from the CPU CBuffer at creation.
	// However if changes are made to the CPU CBuffer during update the we need to copy the data to the GPU CBuffer 
	// using the mapCbuffer helper function provided the in Util.h   

	mapCbuffer(context, cBufferDyanmicCamera, cBufferDyanmicCameraGPU, sizeof(CBufferCube));
	context->GSSetConstantBuffers(6, 1, &cBufferDyanmicCameraGPU);// Attach CBufferLightGPU to register b2 for the Pixel shader.

}

void DynamicCubeReflection::setUpCamera(ID3D11Device* device)
{
	//0.9, 3.6, -1
	float x, y, z;
	x = 0.9f;
	y = 3.6f;
	z = -1.0f;
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);
	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
	XMFLOAT3(x + 1.0f, y, z), // +X
	XMFLOAT3(x - 1.0f, y, z), // -X
	XMFLOAT3(x, y + 1.0f, z), // +Y
	XMFLOAT3(x, y - 1.0f, z), // -Y
	XMFLOAT3(x, y, z + 1.0f), // +Z
	XMFLOAT3(x, y, z - 1.0f) // -Z
	};
	// Use world up vector (0,1,0) for all directions except +Y/-Y.
	// In these cases, we are looking down +Y or -Y, so we need a
	// different "up" vector.
	XMFLOAT3 ups[6] =
	{
	XMFLOAT3(0.0f, 1.0f, 0.0f), // +X
	XMFLOAT3(0.0f, 1.0f, 0.0f), // -X
	XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
	XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
	XMFLOAT3(0.0f, 1.0f, 0.0f), // +Z
	XMFLOAT3(0.0f, 1.0f, 0.0f) // -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		CubeMapCamera[i] = new Camera(device, XMVectorSet(center.x, center.y, center.z, 1), XMVectorSet(ups[i].x, ups[i].y, ups[i].z, 1), XMVectorSet(targets[i].x, targets[i].y, targets[i].z, 1));


		CubeMapCamera[i]->setProjMatrix(XMMatrixPerspectiveFovLH(0.5f*3.14, 1.0f, 0.1f, 1000.0f));
	}
}

HRESULT DynamicCubeReflection::renderDynamicScene(ID3D11DeviceContext *context, function<void(ID3D11DeviceContext*)> renderTwice)
{

	static const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->PSSetShaderResources(1, 1, &nullSRV);
	ID3D11RenderTargetView* renderTargets[1] = { mDynamicCubeMapRTV };
	// Generate the cube map by rendering to each cube map face.
	context->RSSetViewports(1, &CubeMapViewport);
	// Clear cube map face and depth buffer.
	context->ClearRenderTargetView(
		mDynamicCubeMapRTV,
		clearColor);
	context->ClearDepthStencilView(
		DynamicCubeMapDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// Bind cube map face as render target.
	renderTargets[0] = mDynamicCubeMapRTV;
	context->OMSetRenderTargets(
		sizeof(renderTargets) / sizeof(renderTargets[0]), renderTargets, DynamicCubeMapDSV);
	// Draw the scene with the exception of the
	// center sphere, to this cube map face.
	CubeMapCamera[0]->update(context);

	renderTwice(context);

	context->GenerateMips(mDynamicCubeMapSRV);
	return 0;
}


//HRESULT DynamicCubeReflection::renderDynamicScene(ID3D11DeviceContext *context, function<void(ID3D11DeviceContext*)> renderTwice)
//{
//
//	static const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	ID3D11ShaderResourceView* nullSRV = nullptr;
//	context->PSSetShaderResources(1, 1, &nullSRV);
//	ID3D11RenderTargetView* renderTargets[1] = { mDynamicCubeMapRTV };
//	// Generate the cube map by rendering to each cube map face.
//	context->RSSetViewports(1, &CubeMapViewport);
//	for (int i = 0; i < 6; ++i)
//	{
//		// Clear cube map face and depth buffer.
//		context->ClearRenderTargetView(
//			mDynamicCubeMapRTV[i],
//			clearColor);
//		context->ClearDepthStencilView(
//			DynamicCubeMapDSV,
//			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//		// Bind cube map face as render target.
//		renderTargets[0] = mDynamicCubeMapRTV[i];
//		context->OMSetRenderTargets(
//			sizeof(renderTargets) / sizeof(renderTargets[0]), renderTargets, DynamicCubeMapDSV);
//		// Draw the scene with the exception of the
//		// center sphere, to this cube map face.
//		CubeMapCamera[i]->update(context);
//
//		renderTwice(context);
//
//
//	}
//	context->GenerateMips(mDynamicCubeMapSRV);
//	return 0;
//}