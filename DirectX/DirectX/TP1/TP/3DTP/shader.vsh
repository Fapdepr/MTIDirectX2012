shared  matrix WorldViewProj; 

struct VertexInput 
{ 
 float3 Position  : POSITION; 
 float3 Color  : COLOR0; 
}; 


struct VertexOutput 
{ 
 float4 Position  : POSITION;
 float3 Color  : COLOR0; 
}; 


VertexOutput DiffuseVS(VertexInput input)  
{ 
 VertexOutput output; 
  
 output.Position = mul( float4(input.Position, 1.0f), WorldViewProj );
 //output.Position = float4(input.Position, 1.0f);
 output.Color = input.Color; 

 return output; 
} 

float4 DiffusePS( VertexOutput input) : COLOR0 
{ 
 //return float4(1.0f, 1.0f,0.0f, 1.0f); 
 return float4(input.Color, 1.0f); 
} 

technique diffuse 
{ 
 pass p0 
 { 
   VertexShader = compile vs_3_0 DiffuseVS(); 
   PixelShader  = compile ps_3_0 DiffusePS(); 
 } 
}