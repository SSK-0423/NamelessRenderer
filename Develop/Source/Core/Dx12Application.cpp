#include "Dx12Application.h"

const LONG WINDOW_WIDTH = 1024;
const LONG WINDOW_HEIGHT = 768;

using namespace NamelessEngine::Core;
using namespace NamelessEngine::Utility;

Dx12Application::Dx12Application(Dx12ApplicationImpl& applicationImpl)
	: _graphicsEngine(Dx12GraphicsEngine::Instance()),
	_applicationImpl(applicationImpl), _input(Input::Instance())
{
}

Dx12Application::~Dx12Application()
{
}

RESULT Dx12Application::Init()
{
	// ウィンドウ初期化
	_window = std::make_unique<AppWindow>();
	AppWindowInitData initData(TEXT("レンダリングエンジン開発"), WINDOW_WIDTH, WINDOW_HEIGHT);
	_window->CreateAppWindow(initData);

	// グラフィクスエンジン初期化
	SIZE wndSize = _window->GetWindowSize();
	RESULT result = _graphicsEngine.Init(_window->GetHwnd(), wndSize.cx, wndSize.cy);
	if (result == RESULT::FAILED) { return result; }

	// アプリケーション本体の初期化
	result = _applicationImpl.Init(_graphicsEngine, *_window);
	if (result == RESULT::FAILED) { return result; }

	result = _input.Init(*_window);
	if (result == RESULT::FAILED) { return result; }

	return result;
}

void Dx12Application::Run()
{
	// ウィンドウが作成されていないならその時点で終了
	if (_window == nullptr) { return; }
	
	// ゲームループ
	while (_window->DispatchWindowMessage())
	{
		// キーマウス入力情報の更新
		_input.Update();

		// いずれエンジン(フレームワーク側)に吸収させる？？？？
		_applicationImpl.Update(0.f);

		// 1フレームの描画
		_graphicsEngine.BeginDraw();
		{
			_applicationImpl.Draw(_graphicsEngine);
		}
		_graphicsEngine.EndDraw();
	}
}

void Dx12Application::End()
{
	_applicationImpl.Final();
}