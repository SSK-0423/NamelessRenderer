#include "RenderTargetBuffer.h"
#include "d3dx12.h"

using namespace NamelessEngine::Utility;

namespace NamelessEngine::DX12API
{
	RenderTargetBufferData::RenderTargetBufferData(
		const DXGI_FORMAT& colorFormat, const UINT& width, const UINT& height, const float(&color)[4])
		: colorFormat(colorFormat), width(width), height(height)
	{
		clearColor[0] = color[0];
		clearColor[1] = color[1];
		clearColor[2] = color[2];
		clearColor[3] = color[3];
	}

	RESULT RenderTargetBuffer::Create(ID3D12Device& device, const RenderTargetBufferData& data)
	{
		CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(data.colorFormat, data.width, data.height);
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		CD3DX12_CLEAR_VALUE clearValue(data.colorFormat, data.clearColor);

		HRESULT result = device.CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,		// オフスクリーンレンダリング使用時の状態遷移に対応
			&clearValue,
			IID_PPV_ARGS(_rtvBuffer.ReleaseAndGetAddressOf()));

		if (FAILED(result)) { return RESULT::FAILED; }

		return RESULT::SUCCESS;
	}

	RESULT RenderTargetBuffer::Create(ID3D12Device& device, IDXGISwapChain3& swapchain, const size_t& index)
	{
		HRESULT result = swapchain.GetBuffer(index, IID_PPV_ARGS(_rtvBuffer.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { return RESULT::FAILED; }

		// MipLevel = 1
		auto resDesc = _rtvBuffer->GetDesc();

		return RESULT::SUCCESS;
	}
}
