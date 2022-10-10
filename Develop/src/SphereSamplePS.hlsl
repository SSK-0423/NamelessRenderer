#include "SphereSampleHeader.hlsli"

struct DirectionalLight
{
    float3 color;
    float3 direction;
    float3 intensity;
};

// �g�U����
float3 lambert(DirectionalLight light, float3 normal, float3 diffuseColor)
{
    return diffuseColor * dot(light.direction, normal) * light.color;
}

// ���ʔ���
float3 phongSpecular(DirectionalLight light, float3 normal, float3 specularColor, float3 eyeDir, float alpha)
{
    float3 specular = float3(0, 0, 0);
    float3 ref = reflect(light.direction, normal);
    ref = normalize(ref);
    
    // �����x�N�g���Ɣ��˃x�N�g���̂Ȃ��p��n��
    return specularColor * pow(saturate(dot(eyeDir, ref)), alpha) * light.color;

}

float3 ambient(float ambientIntensity, float3 ambientColor)
{
    return ambientIntensity * ambientColor;
}

float4 main(VSOutput input) : SV_TARGET
{
    DirectionalLight light;
    light.direction = normalize(float3(1, 1, 1));
    light.intensity = 1.f;
    light.color = float3(1, 1, 1);
    
    float3 normal = normalize(input.normal);
    float3 eyePos = float3(0, 2, -1);
    float3 eyeDir = eyePos - input.pos.xyz;
    eyeDir = normalize(eyeDir);
    
    float3 diffuse = lambert(light, normal, float3(0.69, 0.69, 0.69));
    float3 specular = phongSpecular(light, normal, float3(0.3, 0.3, 0.3), eyeDir, 2);
    float3 ambientColor = ambient(0.1, float3(0.01, 0.01, 0.01));
    
    float3 outColor = light.intensity * (diffuse + specular) + ambientColor;
    
    return float4(outColor, 1.f);
    //float2 v = step(0, sin(25 * input.uv)) * 0.5;
    //return frac(v.x + v.y) * 2;
}