#include "d3dx12.h"
#include "RenderingContext.h"
#include "DescriptorHeapRTV.h"
#include "DescriptorHeapCBV_SRV_UAV.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GraphicsPipelineState.h"
#include "RootSignature.h"

namespace NamelessEngine::DX12API
{
	RenderingContext::RenderingContext()
	{
	}

	RenderingContext::~RenderingContext()
	{

	}

	void RenderingContext::Init(ID3D12GraphicsCommandList& cmdList)
	{
		_cmdList = &cmdList;
	}

	void RenderingContext::Close()
	{
		_cmdList->Close();
	}

	void RenderingContext::Reset(ID3D12CommandAllocator& cmdAlloc)
	{
		_cmdList->Reset(&cmdAlloc, nullptr);
	}

	void RenderingContext::TransitionResourceState(
		ID3D12Resource& resource, D3D12_RESOURCE_STATES presentState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			&resource, presentState, afterState);
		_cmdList->ResourceBarrier(1, &barrier);
	}

	void RenderingContext::SetRenderTarget(
		const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
	{
		_cmdList->OMSetRenderTargets(1, rtvHandle, false, dsvHandle);
	}

	void RenderingContext::SetRenderTargets(
		const UINT& rtvNum, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[],
		const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
	{
		_cmdList->OMSetRenderTargets(rtvNum, rtvHandles, false, dsvHandle);
	}

	void RenderingContext::ClearRenderTarget(
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const DirectX::XMFLOAT4& color, const UINT& numRects, const D3D12_RECT* rects)
	{
		float clearColor[4] = { color.x,color.y, color.z, color.w };
		_cmdList->ClearRenderTargetView(rtvHandle, clearColor, numRects, rects);
	}

	void RenderingContext::ClearRenderTarget(
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* color, const UINT& numRects, const D3D12_RECT* rects)
	{
		_cmdList->ClearRenderTargetView(rtvHandle, color, numRects, rects);
	}

	void RenderingContext::ClearDepthStencilView(
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CLEAR_FLAGS clearFlags,
		const FLOAT& clearDepth, const UINT8& clearStencil, const UINT& numRects, const D3D12_RECT* rects)
	{
		_cmdList->ClearDepthStencilView(dsvHandle, clearFlags, clearDepth, clearStencil, numRects, rects);
	}

	void RenderingContext::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		_cmdList->RSSetViewports(1, &viewport);
	}

	void RenderingContext::SetViewports(const UINT& viewportNum, const D3D12_VIEWPORT& viewports)
	{
		_cmdList->RSSetViewports(viewportNum, &viewports);
	}

	void RenderingContext::SetScissorRect(const D3D12_RECT& scissorRect)
	{
		_cmdList->RSSetScissorRects(1, &scissorRect);
	}

	void RenderingContext::SetScissorRects(const UINT& scissorRectNum, const D3D12_RECT& scissorRect)
	{
		_cmdList->RSSetScissorRects(scissorRectNum, &scissorRect);
	}

	void RenderingContext::SetVertexBuffer(const UINT& startSlot, const VertexBuffer& vertexBuffer)
	{
		_cmdList->IASetVertexBuffers(startSlot, 1, &vertexBuffer.GetView());
	}

	void RenderingContext::SetIndexBuffer(const IndexBuffer& indexBuffer)
	{
		_cmdList->IASetIndexBuffer(&indexBuffer.GetView());
	}

	void RenderingContext::SetPipelineState(GraphicsPipelineState& pipelineState)
	{
		_cmdList->SetPipelineState(&pipelineState.GetPipelineState());
	}

	void RenderingContext::SetGraphicsRootSignature(RootSignature& rootSignature)
	{
		_cmdList->SetGraphicsRootSignature(&rootSignature.GetRootSignature());
	}

	void RenderingContext::SetPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology)
	{
		_cmdList->IASetPrimitiveTopology(primitiveTopology);
	}

	void RenderingContext::SetDescriptorHeap(ID3D12DescriptorHeap* const* descriptorHeap)
	{
		_cmdList->SetDescriptorHeaps(1, descriptorHeap);
	}

	void RenderingContext::SetDescriptorHeap(DescriptorHeapCBV_SRV_UAV& descriptorHeap)
	{
		_cmdList->SetDescriptorHeaps(1, descriptorHeap.GetDescriptorHeapAddress());

		// ディスクリプタテーブルに登録する
		if (descriptorHeap.IsRegistedShaderResource()) {
			_cmdList->SetGraphicsRootDescriptorTable(0, descriptorHeap.GetGPUDescriptorHandleForHeapStartSRV());
		}
		if (descriptorHeap.IsRegistedConstantBuffer()) {
			_cmdList->SetGraphicsRootDescriptorTable(1, descriptorHeap.GetGPUDescriptorHandleForHeapStartCBV());
		}
		if (descriptorHeap.IsRegistedUnorderedAccessResource()) {
			_cmdList->SetGraphicsRootDescriptorTable(2, descriptorHeap.GetGPUDescriptorHandleForHeapStartUAV());
		}
	}

	void RenderingContext::SetDescriptorHeaps(const UINT& descriptorHeapNum, ID3D12DescriptorHeap* const* descriptorHeaps)
	{
		_cmdList->SetDescriptorHeaps(descriptorHeapNum, descriptorHeaps);
	}

	void RenderingContext::SetGraphicsRootDescriptorTable(const UINT& rootPrameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle)
	{
		_cmdList->SetGraphicsRootDescriptorTable(rootPrameterIndex, handle);
	}

	void RenderingContext::DrawInstanced(const UINT& vertexNumPerInstance, const UINT& instanceNum, const UINT& startVertexLocation, const UINT& startInstanceLocation)
	{
		_cmdList->DrawInstanced(vertexNumPerInstance, instanceNum, startVertexLocation, startInstanceLocation);
	}

	void RenderingContext::DrawIndexedInstanced(const UINT& indexNumPerInstance, const UINT& instanceNum, const UINT startIndexLocation, const INT& baseVertexLocation, const UINT& startInstanceLocation)
	{
		_cmdList->DrawIndexedInstanced(
			indexNumPerInstance, instanceNum, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void RenderingContext::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION& src, const D3D12_TEXTURE_COPY_LOCATION& dst, const UINT& dstX, const UINT& dstY, const UINT& dstZ, D3D12_BOX* srcBox)
	{
		_cmdList->CopyTextureRegion(&dst, dstX, dstY, dstZ, &src, srcBox);
	}
}