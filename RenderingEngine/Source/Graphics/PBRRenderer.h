#pragma once
#include "IRenderer.h"

#include "GBufferPass.h"
#include "ShadowMapPass.h"
#include "ShadowingPass.h"
#include "LightingPass.h"
#include "SkyBoxPass.h"
#include "BlendPass.h"
#include "IBLPass.h"

#include "LightSource.h"

#include "EngineUtility.h"

#include <d3dx12.h>
#include <memory>

namespace NamelessEngine {
	namespace DX12API {
		class Texture;
		class DescriptorHeapCBV_SRV_UAV;
	}
	namespace Scene {
		class Camera;
		class Scene;
	}
}

namespace NamelessEngine::Graphics {
	class PBRRenderer : public IRenderer {
	public:
		PBRRenderer();
		~PBRRenderer();

	private:
		GBufferPass _gbufferPass;
		ShadowMapPass _shadowMapPass;
		ShadowingPass _shadowingPass;
		LightingPass _lightingPass;
		IBLPass _iblPass;
		SkyBoxPass _skyBoxPass;
		BlendPass _blendPass;

		std::unique_ptr<DX12API::Texture> _environment;
		std::unique_ptr<DX12API::Texture> _specularLD;
		std::unique_ptr<DX12API::Texture> _diffuseLD;
		std::unique_ptr<DX12API::Texture> _DFG;

		// ライティングパスで使用するバッファの構造体
		LightingParam _lightingParam;
		Component::DirectionalLight _directionalLight;
		float _dLightColor[3];
		float _dLightDirection[3];

		// IBLパスで使用するバッファの構造体
		IBLParam _iblParam;
		DebugParam _debugParam;

		// シャドウマップパスのバッファ
		ShadowMapParam _shadowMapParam;

		// シャドウイングパスのバッファ
		float _bias;

		// テスト用
		std::unique_ptr<DX12API::DescriptorHeapCBV_SRV_UAV> _heap;
		std::unique_ptr<DX12API::Texture> _testImage;

	public:
		Utility::RESULT Init(Scene::Scene& scene) override;
		void Update(float deltatime, Scene::Scene& scene) override;
		void Render(Scene::Scene& scene) override;
	};
}