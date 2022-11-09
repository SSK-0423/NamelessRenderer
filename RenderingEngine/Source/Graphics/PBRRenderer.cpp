#include <d3dx12.h>

#include "Dx12GraphicsEngine.h"

#include "PBRRenderer.h"
#include "InputLayout.h"
#include "ShaderLibrary.h"
#include "Texture.h"

#include "Scene.h"
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "Actor.h"

#include "Material.h"

using namespace NamelessEngine::Core;
using namespace NamelessEngine::DX12API;
using namespace NamelessEngine::Utility;
using namespace NamelessEngine::Component;

namespace NamelessEngine::Graphics
{
	PBRRenderer::PBRRenderer()
	{

	}
	PBRRenderer::~PBRRenderer()
	{
	}
	Utility::RESULT PBRRenderer::Init(Scene::Scene& scene)
	{
		RESULT result = _gbufferPass.Init();
		if (result == RESULT::FAILED) { return result; }
		result = _lightingPass.Init();
		if (result == RESULT::FAILED) { return result; }
		result = _iblPass.Init();
		if (result == RESULT::FAILED) { return result; }
		result = _skyBoxPass.Init();
		if (result == RESULT::FAILED) { return result; }
		result = _blendPass.Init();
		if (result == RESULT::FAILED) { return result; }

		// �L���[�u�e�N�X�`������
		_environment = std::make_unique<DX12API::Texture>();
		//result = _environment->CreateCubeTextureFromDDS(
		//	Dx12GraphicsEngine::Instance(), L"res/clarens_night_01/clarens_night_01EnvHDR.dds");
		result = _environment->CreateCubeTextureFromDDS(
			Dx12GraphicsEngine::Instance(), L"res/IBL/iblEnvHDR.dds");
		if (result == RESULT::FAILED) { return result; }

		_specularLD = std::make_unique<DX12API::Texture>();
		//result = _specularLD->CreateCubeTextureFromDDS(
		//	Dx12GraphicsEngine::Instance(), L"res/clarens_night_01/clarens_night_01SpecularHDR.dds");
		result = _specularLD->CreateCubeTextureFromDDS(
			Dx12GraphicsEngine::Instance(), L"res/IBL/iblSpecularHDR.dds");
		if (result == RESULT::FAILED) { return result; }

		_diffuseLD = std::make_unique<DX12API::Texture>();
		//result = _diffuseLD->CreateCubeTextureFromDDS(
		//	Dx12GraphicsEngine::Instance(), L"res/clarens_night_01/clarens_night_01DiffuseHDR.dds");
		result = _diffuseLD->CreateCubeTextureFromDDS(
			Dx12GraphicsEngine::Instance(), L"res/IBL/iblDiffuseHDR.dds");
		if (result == RESULT::FAILED) { return result; }

		_DFG = std::make_unique<DX12API::Texture>();
		//result = _DFG->CreateTextureFromDDS(
		//	Dx12GraphicsEngine::Instance(), L"res/clarens_night_01/clarens_night_01Brdf.dds");
		result = _DFG->CreateTextureFromDDS(
			Dx12GraphicsEngine::Instance(), L"res/IBL/iblBrdf.dds");
		if (result == RESULT::FAILED) { return result; }

		// �e�����_�����O�p�X�ɕK�v�ȃ��\�[�X���Z�b�g
		for (size_t index = 0; index < static_cast<size_t>(GBUFFER_TYPE::GBUFFER_TYPE_NUM); index++) {

			Texture& gbuffer = _gbufferPass.GetGBuffer(static_cast<GBUFFER_TYPE>(index));
			_lightingPass.SetGBuffer(static_cast<GBUFFER_TYPE>(index), gbuffer);
			_iblPass.SetGBuffer(static_cast<GBUFFER_TYPE>(index), gbuffer);
		}

		_iblPass.SetIBLTextures(*_specularLD, *_diffuseLD, *_DFG);
		_iblPass.SetLightedTexture(_lightingPass.GetOffscreenTexture());

		_skyBoxPass.SetCubeTexture(*_environment);
		_skyBoxPass.SetCamera(scene.GetCamera());

		_blendPass.SetLightedTexture(_iblPass.GetOffscreenTexture());
		//_blendPass.SetLightedTexture(*_DFG);
		_blendPass.SetRenderedSkyBoxTexture(_skyBoxPass.GetOffscreenTexture());
		_blendPass.SetDepthTexture(_gbufferPass.GetGBuffer(GBUFFER_TYPE::DEPTH));
	}
	void PBRRenderer::Update(float deltatime, Scene::Scene& scene)
	{
		_lightingParam.eyePosition = scene.GetCamera().GetTransform().Position();
		_lightingPass.UpdateParamData(_lightingParam);

		_lightingPass.UpdateDirectionalLight(_directionalLight);

		_iblParam.eyePosition = scene.GetCamera().GetTransform().Position();
		_iblPass.UpdateParamData(_iblParam);


		Material* material = scene.GetMeshActors()[scene.GetMeshActors().size() - 1]->GetComponent<Material>();
		material->SetBaseColor(_baseColor[0], _baseColor[1], _baseColor[2]);
		material->SetRoughness(_roughness);
		material->SetMetallic(_metallic);
	}
	void PBRRenderer::Render(Scene::Scene& scene)
	{
		// Imgui�����_�[
		{
			ImGui::SetNextWindowPos(ImVec2(900, 0));
			ImGui::Begin("Physically Based Rendering", 0, ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(375, AppWindow::GetWindowSize().cy), ImGuiCond_Once);

			// GBuffer�p�X�֘A
			ImGui::ColorPicker3("BaseColor", _baseColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
			ImGui::SliderFloat("Roughness", &_roughness, 0.f, 1.f);
			ImGui::SliderFloat("Metallic", &_metallic, 0.f, 1.f);
			// ���C�e�B���O�p�X�֘A
			ImGui::ColorPicker3("LightColor", _directionalLight.color, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
			ImGui::SliderFloat3("LightDirection", _directionalLight.direction, -1.f, 1.f);
			ImGui::SliderFloat("LightIntensity", &_directionalLight.intensity, 0.f, 10.f);
			ImGui::RadioButton("CookTorrance", &_lightingParam.brdfModel, 0);
			ImGui::SameLine();
			ImGui::RadioButton("GGX(Trowbridge-Reitz)", &_lightingParam.brdfModel, 1);
			// IBL�p�X�֘A
			ImGui::SliderFloat("IBL_Intensity", &_iblParam.lightIntensity, 0.f, 10.f);
			ImGui::Checkbox("IBL Only", &_iblParam.isIBLOnly);
			ImGui::End();
			ImGui::Render();
		}

		// 1. GBuffer�o�̓p�X
		_gbufferPass.Render(scene.GetMeshActors());
		// 2. ���C�e�B���O�p�X
		_lightingPass.Render();
		// 3. IBL�p�X
		_iblPass.Render();
		// 4. �X�J�C�{�b�N�X�p�X
		_skyBoxPass.Render();
		// 5. ���C�e�B���O���ʂƃX�J�C�{�b�N�X�`�挋�ʂ���������
		_blendPass.Render();
	}
}