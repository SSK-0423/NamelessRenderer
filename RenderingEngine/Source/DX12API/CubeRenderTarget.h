#pragma once

#include "EngineUtility.h"

#include "Texture.h"
#include "DescriptorHeapCBV_SRV_UAV.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"
#include "DescriptorHeapRTV.h"
#include "DescriptorHeapDSV.h"

namespace NamelessEngine::DX12API
{
	class RenderingContext;

	struct CubeRenderTargetData
	{
		RenderTargetBufferData renderTargetBufferData;	// レンダーターゲットバッファー用データ
		DepthStencilBufferData depthStencilBufferData;	// デプスステンシルバッファー用データ
		bool useDepth;
	};

	/// <summary>
	/// レンダーターゲットクラス
	/// </summary>
	class CubeRenderTarget
	{
	private:
		Texture _renderTargetTexture;				// オフスクリーンテクスチャ
		DescriptorHeapCBV_SRV_UAV _textureHeap;		// オフスクリーンテクスチャ用ヒープ

		RenderTargetBuffer _renderTargetBuffer;		// レンダーターゲットバッファー
		DescriptorHeapRTV _rtvHeap;					// レンダーターゲット用ヒープ

		Texture _depthStencilTexture;				// デプスステンシルテクスチャ
		DepthStencilBuffer _depthStencilBuffer;		// デプスステンシルバッファー
		DescriptorHeapDSV _dsvHeap;					// デプスステンシル用ヒープ

		CubeRenderTargetData _renderTargetData;			// レンダーターゲットデータ

	public:
		/// <summary>
		/// レンダーターゲット生成
		/// </summary>
		/// <param name="device">デバイス</param>
		/// <returns></returns>
		Utility::RESULT Create(ID3D12Device& device, CubeRenderTargetData& renderTargetData);

		/// <summary>
		/// レンダリング開始
		/// </summary>
		/// <param name="renderContext">レンダリングコンテキスト</param>
		void BeginRendering(RenderingContext& renderContext, CD3DX12_VIEWPORT& viewport, CD3DX12_RECT& scissorRect);

		/// <summary>
		/// レンダリング終了
		/// </summary>
		/// <param name="renderContext">レンダリングコンテキスト</param>
		void EndRendering(RenderingContext& renderContext);

		/// <summary>
		/// レンダーターゲットのテクスチャ取得
		/// </summary>
		/// <returns>レンダーターゲットテクスチャ</returns>
		Texture& GetRenderTargetTexture() {
			return _renderTargetTexture;
		}

		/// <summary>
		/// デプスステンシルのテクスチャ取得
		/// </summary>
		/// <returns>デプスステンシルテクスチャ</returns>
		Texture& GetDepthStencilTexture() {
			return _depthStencilTexture;
		}

		/// <summary>
		/// マルチレンダーターゲットでのレンダリング開始
		/// </summary>
		/// <param name="renderTargets"></param>
		/// <param name="renderContext"></param>
		/// <param name="viewport"></param>
		/// <param name="scissorRect"></param>
		static void BeginMultiRendering(
			CubeRenderTarget* renderTargets, const size_t& length, RenderingContext& renderContext,
			CD3DX12_VIEWPORT& viewport, CD3DX12_RECT& scissorRect);

		/// <summary>
		/// マルチレンダーターゲットでのレンダリング終了
		/// </summary>
		/// <param name="renderTargets"></param>
		/// <param name="length"></param>
		/// <param name="renderContext"></param>
		static void EndMultiRendering(
			CubeRenderTarget* renderTargets, const size_t& length, RenderingContext& renderContext);
	};
}