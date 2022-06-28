#pragma once

#include <array>

#include "FBXLoader.h"

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RootSignature.h"
#include "GraphicsPipelineState.h"

#include "DescriptorHeapCBV_SRV_UAV.h"
#include "ConstantBuffer.h"
#include "Texture.h"

#include "RenderingContext.h"

#include "EngineUtility.h"

/// <summary>
/// メッシュ生成用データ
/// </summary>
struct FBXMeshCreateData {
	const char* modelPath;								// 3Dモデルへのファイルパス
	Shader vertexShader;	                            // 頂点シェーダー
	Shader pixelShader;									// ピクセルシェーダー
	RootSignatureData rootSignatureData;				// ルートシグネチャ
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;	// 頂点レイアウト
	std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> colorFormats = {
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	};	// レンダーターゲットのカラーフォーマット
	
	/// <summary>
	/// カラーフォーマット配列を調べてレンダーターゲット数を返す
	/// </summary>
	/// <returns>レンダーターゲット数</returns>
	size_t GetRenderTargetNum() const;
};

/// <summary>
/// メッシュクラス
/// </summary>
class FBXMesh {
public:
	FBXMesh();
	~FBXMesh();

private:
	FBXLoader* _fbxLoader = nullptr;

	std::vector<FBXMeshData> _meshDataList;

	std::vector<VertexBuffer> _vertexBuffers;
	MYRESULT CreateVertexBuffers(ID3D12Device& device);

	std::vector<IndexBuffer> _indexBuffers;
	MYRESULT CreateIndexBuffers(ID3D12Device& device);

	GraphicsPipelineState _graphicsPipelineState;
	MYRESULT CreateGraphicsPipelineState(ID3D12Device& device, FBXMeshCreateData& meshData);

	RootSignature _rootSignature;

	// マテリアル用コンスタントバッファー構造体
	struct MaterialBuff {
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		float alpha;
	};
	// マテリアル
	MaterialBuff _material;
	void SetMaterial(const FBXMaterial& material);

	// マテリアル用のコンスタントバッファー
	ConstantBuffer _materialConstantBuffer;
	MYRESULT CreateMaterialConsnantBuffer(ID3D12Device& device);
	
	// ディスクリプタヒープ
	DescriptorHeapCBV_SRV_UAV _descriptorHeap;
	MYRESULT CreateDescriptorHeap(ID3D12Device& device);

public:
	/// <summary>
	/// メッシュ読み込み
	/// </summary>
	/// <param name="modelPath">ファイルパス</param>
	/// <returns></returns>
	MYRESULT LoadFBX(ID3D12Device& device, FBXMeshCreateData& meshData);

	void Draw(RenderingContext& renderContext);

	void SetConstantBuffer(ID3D12Device& device, ConstantBuffer& constantBuffer);
	
	void SetTexture(ID3D12Device& device, Texture& texture);

};