#pragma once

#include <Windows.h>
#include <memory>

#include "Dx12ApplicationImpl.h"

/// <summary>
/// レンダリング開発中のテスト用クラス
/// </summary>
class Dx12Application
{
public:
	Dx12Application(Dx12ApplicationImpl& applicationImpl);
	~Dx12Application();

	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>成功：RESULT::SUCCESS 失敗：RESULT::FAILED</returns>
	NamelessEngine::Utility::RESULT Init();

	/// <summary>
	/// メインループ
	/// </summary>
	void Run();

	/// <summary>
	/// 終了処理
	/// </summary>
	void End();

private:
	NamelessEngine::Core::Input& _input;
	NamelessEngine::Core::Dx12GraphicsEngine& _graphicsEngine; // 描画の基礎部分を担当するエンジン
	std::unique_ptr<NamelessEngine::Core::AppWindow> _window; // アプリケーションのウィンドウ
	Dx12ApplicationImpl& _applicationImpl;				// アプリケーションの本体
};