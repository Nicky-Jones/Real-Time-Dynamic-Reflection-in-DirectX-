
// Ensure matrices are row-major
#pragma pack_matrix(row_major)
struct vertexInputPacket
{
	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float3				tangentW		: TANGENT;
	float4				matDiffuse		: DIFFUSE;
	float4				matSpecular		: SPECULAR;
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};
struct vertexOutputPacket
{
	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float3				tangentW		: TANGENT;
	float4				matDiffuse		: DIFFUSE;
	float4				matSpecular		: SPECULAR;
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
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

			output.normalW = input[v].normalW;
			output.tangentW = input[v].tangentW;
			output.matDiffuse = input[v].matDiffuse;
			output.matSpecular = input[v].matSpecular;
			output.texCoord = input[v].texCoord;
			output.posH = input[v].posH;
			output.posW = mul(input[v].posW, viewMatrix[f]);
			// Transform to the view space of the ith cube face. 
			output.posH = mul(float4(input[v].posW, 1), viewMatrix[f]);
			// Transform to homogeneous clip space. 
			output.posH = mul(output.posH, projMatrix);
			//output.texCoord = input[v].texCoord;
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}

}