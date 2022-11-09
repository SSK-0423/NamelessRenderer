#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "EngineUtility.h"

namespace NamelessEngine::DX12API
{
	struct RenderTargetBufferData
	{
		DXGI_FORMAT colorFormat;
		UINT width;
		UINT height;
		float clearColor[4];

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="colorFormat">�J���[�t�H�[�}�b�g �f�t�H���g�FDXGI_FORMAT_R8G8B8A8_UNORM</param>
		/// <param name="width">�����_�[�^�[�Q�b�g�� �f�t�H���g�F1024</param>
		/// <param name="height">�����_�[�^�[�Q�b�g�� �f�t�H���g�F768</param>
		/// <param name="clearColor">�������F �f�t�H���g�F��</param>
		RenderTargetBufferData(
			const DXGI_FORMAT& colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
			const UINT& width = 1024, const UINT& height = 768,
			const float(&clearColor)[4] = { 1.f,1.f,1.f,1.f });
	};

	/// <summary>
	/// �����_�[�^�[�Q�b�g�p�̃o�b�t�@�[�N���X
	/// </summary>
	class RenderTargetBuffer
	{
	public:
		RenderTargetBuffer() = default;
		~RenderTargetBuffer() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> _rtvBuffer = nullptr;

	public:
		/// <summary>
		/// �����_�[�^�[�Q�b�g�o�b�t�@�[����
		/// </summary>
		/// <param name="device">�f�o�C�X</param>
		/// <param name="data">�����_�[�^�[�Q�b�g�̐ݒ�</param>
		/// <returns>Utility::RESULT::SUCCESS: ���� Utility::RESULT::FAILED: ���s</returns>
		Utility::RESULT Create(ID3D12Device& device, const RenderTargetBufferData& data);

		/// <summary>
		/// �t���[���o�b�t�@�p�̃����_�[�^�[�Q�b�g�o�b�t�@�[����
		/// </summary>
		/// <param name="device">�f�o�C�X</param>
		/// <param name="swapchain">�X���b�v�`�F�[��</param>
		/// <param name="index">��������o�b�t�@�̃C���f�b�N�X �_�u���o�b�t�@�����O�Ȃ�0��1</param>
		/// <returns>Utility::RESULT::SUCCESS: ���� Utility::RESULT::FAILED: ���s</returns>
		Utility::RESULT Create(ID3D12Device& device, IDXGISwapChain3& swapchain, const size_t& index);

		/// <summary>
		/// �o�b�t�@�[�擾
		/// </summary>
		/// <returns></returns>
		ID3D12Resource& GetBuffer() {
			return *_rtvBuffer.Get();
		}
	};
}