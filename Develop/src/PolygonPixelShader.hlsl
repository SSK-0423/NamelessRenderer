// サンプラー
sampler smp : register(s0);
// テクスチャ
Texture2D tex : register(t0);

float4 PolygonPS() : SV_TARGET
{
	return float4(0.f, 1.0f, 0.25f, 1.0f);
}