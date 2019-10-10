// Ensure matrices are row-major
#pragma pack_matrix(row_major)

struct vertexOutputPacket {
	float3				texCoord		: TEXCOORD;
	float4				posW			: POSITION;
	float4				posH			: SV_POSITION;
	uint				RTIndex		: SV_RenderTargetArrayIndex;
};
struct vertexInputPacket
{
	float3				texCoord		: TEXCOORD;
	float4				posW			: POSITION;
	float4				posH			: SV_POSITION;
};
cbuffer CBufferCube : register(b6)
{
	float4x4		viewMatrix[6];
	float4x4		projMatrix;
}

[maxvertexcount(18)]
void main(triangle vertexInputPacket input[3],
	inout TriangleStream<vertexOutputPacket> CubeMapStream)
{
	// For each triangle
	for (int f = 0; f < 6; ++f)
	{
		// Compute screen coordinates 
		vertexOutputPacket output;
		// Assign the ith triangle to the ith render target. 
		output.RTIndex = f;
		// For each vertex in the triangle 
		for (int v = 0; v < 3; v++) {
			output.posW = input[v].posW;
			output.posH = input[v].posH;
			output.texCoord = input[v].texCoord;
			// Transform to the view space of the ith cube face. 
			output.posH = mul(float4(input[v].posW), viewMatrix[f]);
			// Transform to homogeneous clip space. 
			output.posH = mul(output.posH, projMatrix).xyww;
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}