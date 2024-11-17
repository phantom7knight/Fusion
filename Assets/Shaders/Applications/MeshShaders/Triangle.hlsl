
// Mesh Shader (HLSL)
struct MeshOutput
{
    float4 Position : SV_POSITION;
    float3 Color    : COLOR;
};

[numthreads(1, 1, 1)]
[OutputTopology("triangle")]
void main_ms(
	//in uint3 DispatchThreadID : SV_DispatchThreadID,
    out indices uint3 triangles[1],
    out vertices MeshOutput vertices[3]
)
{
    // Output 3 vertices, 1 triangle 
    SetMeshOutputCounts(3,1);

    triangles[0] = uint3(0, 1, 2);

    vertices[0].Position = float4(0.0, 0.5, 0.0, 1.0);
    vertices[0].Color = float3(1.0, 0.0, 0.0);

    vertices[1].Position = float4(0.5, -0.5, 0.0, 1.0);
    vertices[1].Color = float3(0.0, 1.0, 0.0);

    vertices[2].Position = float4(-0.5, -0.5, 0.0, 1.0);
    vertices[2].Color = float3(0.0, 0.0, 1.0);
}

// Pixel Shader (HLSL)

// Input structure for the pixel shader
struct PSInput {
    float4 position : SV_Position; // Position in screen space, interpolated from the mesh shader
    float3 color    : COLOR;       // Interpolated color from the mesh shader
};

// Main pixel shader function
float4 main_ps(PSInput input) : SV_Target {
    // Return the interpolated color as the final color for the pixel
    return float4(input.color, 1.0f); // The alpha is set to 1.0 for full opacity
}
