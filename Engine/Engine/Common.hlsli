#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);
SamplerState shadowPointSampler : register(s2);
SamplerComparisonState shadowCompareSampler : register(s3);
SamplerState pointWrapSampler : register(s4);
SamplerState linearMirrorSampler : register(s5);
SamplerState pointClampSampler : register(s6);

TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);

Texture2D shadowMaps[MAX_LIGHTS] : register(t15);

struct Light
{
    float3 radiance;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;

    uint type;
    float radius;

    float haloRadius;
    float haloStrength;

    Matrix viewProj;
    Matrix invProj;
};

cbuffer GlobalConstants : register(b0)
{
    Matrix view;
    Matrix proj;
    Matrix invProj;
    Matrix viewProj;
    Matrix invViewProj;
    Matrix invView;

    float3 eyeWorld;
    float strengthIBL;

    int textureToDraw;
    float envLodBias;
    float lodBias;
    float globalTime;

    Light lights[MAX_LIGHTS];
};

#endif // __COMMON_HLSLI__