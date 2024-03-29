#include "LightingPass.h"
#include "GBufferPass.h"

#include "RenderTarget.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "ShaderResourceViewDesc.h"
#include "RootSignature.h"
#include "GraphicsPipelineState.h"
#include "ShaderLibrary.h"
#include "InputLayout.h"

#include "LightSource.h"

#include "Dx12GraphicsEngine.h"

using namespace NamelessEngine::Core;
using namespace NamelessEngine::DX12API;
using namespace NamelessEngine::Utility;
using namespace NamelessEngine::Component;

constexpr UINT LIGHTING_PARAM_INDEX = 0;
constexpr UINT DIRECTIONAL_LIGHT_INDEX = 1;

constexpr UINT SHADOWFACT_INDEX = 6;

namespace NamelessEngine::Graphics
{
	LightingPass::LightingPass()
		: _rootSignature(new RootSignature()), _pipelineState(new GraphicsPipelineState()),
		_descriptorHeap(new DescriptorHeapCBV_SRV_UAV()),
		_paramBuffer(new ConstantBuffer()), _directionalLightBuffer(new ConstantBuffer()),
		_renderTarget(new RenderTarget())
	{
	}
	LightingPass::~LightingPass()
	{
	}

	Utility::RESULT LightingPass::Init()
	{
		ID3D12Device& device = Dx12GraphicsEngine::Instance().Device();

		RESULT result = CreateRootSignature(device);
		if (result == RESULT::FAILED) {
			MessageBox(NULL, L"ルートシグネチャ生成失敗", L"エラーメッセージ", MB_OK);
		}
		result = CreateGraphicsPipelineState(device);
		if (result == RESULT::FAILED) {
			MessageBox(NULL, L"グラフィクスパイプライン生成失敗", L"エラーメッセージ", MB_OK);
		}
		result = CreateRenderTarget(device);
		if (result == RESULT::FAILED) {
			MessageBox(NULL, L"レンダーターゲット生成失敗", L"エラーメッセージ", MB_OK);
		}
		result = CreateDescriptorHeap(device);
		if (result == RESULT::FAILED) {
			MessageBox(NULL, L"ディスクリプタヒープ生成失敗", L"エラーメッセージ", MB_OK);
		}
		result = CreateBuffer(device);
		if (result == RESULT::FAILED) {
			MessageBox(NULL, L"パラメーター用コンスタントバッファー生成失敗", L"エラーメッセージ", MB_OK);
		}
		_descriptorHeap->RegistConstantBuffer(device, *_paramBuffer, LIGHTING_PARAM_INDEX);
		_descriptorHeap->RegistConstantBuffer(device, *_directionalLightBuffer, DIRECTIONAL_LIGHT_INDEX);

		SIZE windowSize = AppWindow::GetWindowSize();

		_viewport = CD3DX12_VIEWPORT(
			0.f, 0.f, static_cast<float>(windowSize.cx), static_cast<float>(windowSize.cy));
		_scissorRect = CD3DX12_RECT(0, 0, windowSize.cx, windowSize.cy);

		return result;
	}

	Utility::RESULT LightingPass::CreateRootSignature(ID3D12Device& device)
	{
		RootSignatureData rootSigData;
		rootSigData._descRangeData.cbvDescriptorNum = 2;
		// カラー、法線、位置、メタリック・ラフネス、深度
		rootSigData._descRangeData.srvDescriptorNum = 7;

		Utility::RESULT result = _rootSignature->Create(device, rootSigData);
		if (result == Utility::RESULT::FAILED) { return result; }
		return Utility::RESULT::SUCCESS;
	}
	Utility::RESULT LightingPass::CreateGraphicsPipelineState(ID3D12Device& device)
	{
		// ルートシグネチャとシェーダーセット
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState = {};
		pipelineState.pRootSignature = &_rootSignature->GetRootSignature();
		pipelineState.VS = CD3DX12_SHADER_BYTECODE(&ShaderLibrary::Instance().GetShader("PBRVS")->GetShader());
		pipelineState.PS = CD3DX12_SHADER_BYTECODE(&ShaderLibrary::Instance().GetShader("PBRPS")->GetShader());

		// サンプルマスク設定
		pipelineState.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// ブレンド
		pipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		// ラスタライズ設定
		pipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineState.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// インプットレイアウトの設定
		pipelineState.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		pipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// デプスステンシル設定
		pipelineState.DepthStencilState.DepthEnable = false;
		pipelineState.DepthStencilState.StencilEnable = false;

		// レンダーターゲットの設定
		pipelineState.NumRenderTargets = 1;
		pipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		// アンチエイリアシングのためのサンプル数設定
		pipelineState.SampleDesc.Count = 1;			// サンプリングは1ピクセルにつき1
		pipelineState.SampleDesc.Quality = 0;		// クオリティは最低

		return _pipelineState->Create(device, pipelineState);
	}
	Utility::RESULT LightingPass::CreateRenderTarget(ID3D12Device& device)
	{
		SIZE windowSize = AppWindow::GetWindowSize();

		RenderTargetData data;
		data.renderTargetBufferData.width = windowSize.cx;
		data.renderTargetBufferData.height = windowSize.cy;
		data.depthStencilBufferData.width = windowSize.cx;
		data.depthStencilBufferData.height = windowSize.cy;
		data.useDepth = false;

		return _renderTarget->Create(device, data);
	}
	Utility::RESULT LightingPass::CreateBuffer(ID3D12Device& device)
	{
		RESULT result = _paramBuffer->Create(device, &_paramData, sizeof(LightingParam));
		if (result == RESULT::FAILED) { return result; }

		return _directionalLightBuffer->Create(device, nullptr, sizeof(DirectionalLight));
	}
	Utility::RESULT LightingPass::CreateDescriptorHeap(ID3D12Device& device)
	{
		return _descriptorHeap->Create(device);
	}

	void LightingPass::UpdateParamData(LightingParam& param)
	{
		_paramData = param;
		_paramBuffer->UpdateData(&_paramData);
	}
	void LightingPass::UpdateDirectionalLight(DirectionalLight& directionalLight)
	{
		_directionalLightBuffer->UpdateData(&directionalLight);
	}
	void LightingPass::Render()
	{
		RenderingContext& renderContext = Dx12GraphicsEngine::Instance().GetRenderingContext();

		renderContext.SetPipelineState(*_pipelineState);
		renderContext.SetGraphicsRootSignature(*_rootSignature);

		_renderTarget->BeginRendering(renderContext, _viewport, _scissorRect);
		{
			renderContext.SetDescriptorHeap(*_descriptorHeap);
			renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			renderContext.DrawInstanced(4, 1, 0, 0);
		}
		_renderTarget->EndRendering(renderContext);
	}
	void LightingPass::SetGBuffer(GBUFFER_TYPE type, DX12API::Texture& texture)
	{
		ShaderResourceViewDesc desc(texture);
		// 0: カラー, 1: 法線, 2: ワールド座標, 3: オクルージョン・ラフネス・メタリック, 4: エミッション・シャドウファクタ―
		_descriptorHeap->RegistShaderResource(
			Dx12GraphicsEngine::Instance().Device(), texture, desc, static_cast<int>(type));
	}
	DX12API::Texture& LightingPass::GetOffscreenTexture()
	{
		return _renderTarget->GetRenderTargetTexture();
	}
}
