#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Manager/GravityManager.h"
#include "../Scene/TitleScene.h"
#include "../Scene/DemoScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/OverScene.h"
#include "../Scene/ClearScene.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	// 重力管理初期化
	GravityManager::CreateInstance();

	fader_ = std::make_unique<Fader>();
	fader_->Init();

	// カメラ
	camera_ = std::make_shared<Camera>();
	camera_->Init();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);

}

void SceneManager::Init3D(void)
{

	// 背景色設定
	SetBackgroundColor(0, 139, 139);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);
	
	// ライトの設定
	ChangeLightTypeDir({0.3f,-3.0f,0.3f});

	//環境光
	SetGlobalAmbientLight(GetColorF(0.3f, 0.3f, 0.3f, 1.0f));


	// フォグ設定
	SetFogEnable(true);
	SetFogColor(5, 5, 5);
	SetFogStartEnd(10000.0f, 20000.0f);

}

void SceneManager::Update(void)
{
	if (scene_ == nullptr)
	{
		return;
	}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// 重力方向の計算
	GravityManager::GetInstance().Update();

	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		scene_->Update();
	}

	// カメラ更新
	camera_->Update();

}

void SceneManager::Draw(void)
{
	
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	camera_->SetBeforeDraw();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 描画
	scene_->Draw();

	// 主にポストエフェクト用
	camera_->Draw();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	
	// 暗転・明転
	fader_->Draw();

}


void SceneManager::Destroy(void)
{

	// 重力管理初期化
	GravityManager::GetInstance().Destroy();

	delete instance_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

std::weak_ptr<Camera> SceneManager::GetCamera(void) const
{
	return camera_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

	camera_ = nullptr;

}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// リソースの解放
	ResourceManager::GetInstance().Release();

	// 現在のシーンの終了処理を呼ぶ
	if (scene_ != nullptr)
	{
		scene_->Release();
		scene_.reset();
	}

	// シーンを変更する
	sceneId_ = sceneId;

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = std::make_unique<TitleScene>();
		ChangeFont("PixelMplus10");
		SetFontSize(16);
		break;
	case SCENE_ID::DEMO:
		scene_ = std::make_unique<DemoScene>();
		SetFontSize(55);
		break;
	case SCENE_ID::GAME:
		scene_ = std::make_unique<GameScene>();
		break;
	case SCENE_ID::OVER:
		scene_ = std::make_unique<OverScene>();
		break;
	case SCENE_ID::CLEAR:
		scene_ = std::make_unique<ClearScene>();
		break;
	}

	scene_->Init();

	ResetDeltaTime();

	if (fader_) {
		fader_->loadingTimer_ = 0;
	}

	waitSceneId_ = SCENE_ID::NONE;
}

void SceneManager::Fade(void)
{

    Fader::STATE fState = fader_->GetState();
    switch (fState)
    {
    case Fader::STATE::FADE_IN:
        if (fader_->IsEnd())
        {
            fader_->SetFade(Fader::STATE::NONE);
            isSceneChanging_ = false;
        }
        break;

    case Fader::STATE::FADE_OUT:
        if (fader_->IsEnd())
        {
            if (!isLoading_)
            {
                // ローディング表示期間に入る
                isLoading_ = true;
                loadingTimer_ = 0;
            }
            else
            {
                loadingTimer_++;

                if (loadingTimer_ > 60) // 約1秒間ローディングを見せる
                {
                    DoChangeScene(waitSceneId_);
                    fader_->SetFade(Fader::STATE::FADE_IN);
                    isLoading_ = false;
                }
            }
        }
        break;
    }
}


