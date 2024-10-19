#include "Particle.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color * textureColor * input.color;
    // texture‚Ìa’l‚ª0.5ˆÈ‰º‚Ì‚Æ‚«‚ÉPixel‚ðŠü‹p
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    // texture‚Ìa’l‚ª0‚Ì‚Æ‚«‚ÉPiel‚ðŠü‹p
    if (textureColor.a == 0.0)
    {
        discard;
    }
    // output.colo‚Ìa’l‚ª0‚Ì‚Æ‚«‚ÉPixel‚ðŠü‹p
    if (output.color.a == 0.0)
    {
        discard;
    }
    return output;
}