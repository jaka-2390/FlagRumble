#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SceneManager.h"
#include "Manager/GravityManager.h"
#include "Manager/FontManager.h"
#include "Application.h"

//staticメンバ変数の実体
Application* Application::instance_ = nullptr;
bool Application::isRunning_ = true;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SOUND = "Data/Sound/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}

void Application::Init(void)
{
	//ウィンドウタイトル
	SetWindowText("AGS2025");

	//画面サイズ設定
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0);
	ChangeWindowMode(true);

	//DxLib初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	FontManager::Init();

	//Effekseer初期化
	InitEffekseer();

	//入力管理など初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	ResourceManager::CreateInstance();
	SoundManager::CreateInstance();
	SceneManager::CreateInstance();
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

void Application::Run(void)
{
	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();

	isRunning_ = true; //フラグ初期化

	while (isRunning_ && ProcessMessage() == 0 /*&& CheckHitKey(KEY_INPUT_ESCAPE) == 0*/)
	{
		inputManager.Update();
		sceneManager.Update();

		sceneManager.Draw();

		ScreenFlip();
	}
}

void Application::Destroy(void)
{
	InputManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();
	SoundManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	FontManager::Release();

	//Effekseer終了
	Effkseer_End();

	//DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;
}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}
