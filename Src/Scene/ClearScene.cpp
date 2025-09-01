#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
{
	imgClear_ = -1;
	imgBackGameClaer_ = -1;
	imgClearWolrd_ = -1;
	imgReplay_ = -1;
	imgReturn_ = -1;
	imgPressKey_ = -1;

	cheackCounter_ = 0;

	animationControllerEnemy_ = nullptr;
}

ClearScene::~ClearScene(void)
{
}

void ClearScene::Init(void)
{

	// 画像読み込み
	imgClear_ = resMng_.Load(ResourceManager::SRC::GAMECLEAR).handleId_;
	imgBackGameClaer_ = resMng_.Load(ResourceManager::SRC::BACK_GAMECLEAR).handleId_;
	imgClearWolrd_ = resMng_.Load(ResourceManager::SRC::CLEARWOLEDBORN).handleId_;
	imgReplay_ = resMng_.Load(ResourceManager::SRC::REPLAY).handleId_;
	imgReturn_ = resMng_.Load(ResourceManager::SRC::GOTITLE).handleId_;
	imgPressKey_ = resMng_.Load(ResourceManager::SRC::PRESS_KEY).handleId_;

	cheackCounter_ = 0;

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAMECLEAR_BGM, Sound::TIMES::LOOP);

	// アニメーション用です
	// ---------------------------------------------
	// メッセージ画像のサイズと位置取得
	// 画像サイズ取得

	GetGraphSize(imgClearWolrd_, &imgW_, &imgH_);
	msgX_ = Application::SCREEN_SIZE_X / 2 - imgW_ / 2;
	msgY_ = 800;

	// マスクの初期位置（完全に隠れている状態）
	maskLeftX_ = msgX_;
	maskSpeed_ = 5;

	// フェード処理
	clearAlpha_ = 0;     // 完全に透明から始める
	fadeSpeed_ = 2;      // 徐々に表示（速さはお好みで）

	// presskey用
	pressKeyY_ = Application::SCREEN_SIZE_Y + 100;         // 画面下 + α からスタート
	targetPressKeyY_ = 600;         // 目標位置
	pressKeyAlpha_ = 0;             // 透明から始める
	isPressKeyAnimStart_ = false;
	isPressKeyAnimEnd_ = false;

	// 敵
	enemy_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MUSH));
	enemy_.pos = { -490.0f, -600.0f, 50.0f };
	enemy_.scl = { 1.3f, 1.3f, 1.3f };
	enemy_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(-20.0f), 0.0f);
	enemy_.Update();

	// 敵のアニメーション
	std::string path1 = Application::PATH_MODEL + "Enemy/mushroom/";
	animationControllerEnemy_ = std::make_unique<AnimationController>(enemy_.modelId);
	animationControllerEnemy_->Add(0, path1 + "mushroom.mv1", 40.0f, 1);
	animationControllerEnemy_->Play(0);
	isAnimEnd_ = false;
	// ---------------------------------------------

	// 定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);
}

void ClearScene::Update(void)
{
	cheackCounter_++;

	// アニメーション更新
	if (maskLeftX_ < msgX_ + imgW_) {
		maskLeftX_ += maskSpeed_;
		if (maskLeftX_ > msgX_ + imgW_) {
			maskLeftX_ = msgX_ + imgW_;
		}
	}

	if (clearAlpha_ < 255) {
		clearAlpha_ += fadeSpeed_;
		if (clearAlpha_ > 255) clearAlpha_ = 255;
	}

	// アニメーション終了チェック
	if (!isAnimEnd_ && maskLeftX_ >= msgX_ + imgW_ && clearAlpha_ >= 255) {
		isAnimEnd_ = true;
		isPressKeyAnimStart_ = true; // ← ここで pressKey のアニメーション開始
	}

	// 入力受付（アニメーション後）
	if (isAnimEnd_ && CheckHitKeyAll() > 0) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		// 音楽
		SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
	}

	// 強制遷移
	if (cheackCounter_ >= 3600) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	if (isPressKeyAnimStart_ && !isPressKeyAnimEnd_) {
		// Y座標を補間（滑らかに近づける）
		pressKeyY_ -= 6;  // スライドスピード
		enemy_.pos.y += 4.0f;
		if (pressKeyY_ <= targetPressKeyY_) {
			pressKeyY_ = targetPressKeyY_;
		}

		// アルファ値を増加（フェードイン）
		if (pressKeyAlpha_ < 255) {
			pressKeyAlpha_ += 5;  // フェードスピード
			if (pressKeyAlpha_ > 255) pressKeyAlpha_ = 255;
		}

		// アニメーション完了フラグ更新
		if (pressKeyY_ == targetPressKeyY_ && pressKeyAlpha_ == 255) {
			isPressKeyAnimEnd_ = true;
		}
	}

	enemy_.Update();
	animationControllerEnemy_->Update();
}

void ClearScene::Draw(void)
{
	// 背景を描く
	DrawGraph(0, 0, imgBackGameClaer_, true);

	DrawRotaGraph(Application::SCREEN_SIZE_X/2,200,1.3,0,imgClear_,true);

	// メッセージ（下のテキスト）を表示
	SetFontSize(128);
	DrawString(Application::SCREEN_SIZE_X/2-128*5.5,830, "こうして世界はうまれた", true);
	SetFontSize(16);

	// メッセージの上に背景で覆う（横方向）
	int horizontalMaskW = msgX_ + imgW_ - maskLeftX_;
	if (horizontalMaskW > 0) {
		DrawRectGraph(
			maskLeftX_, msgY_,                    // 表示先（画面上）
			maskLeftX_, msgY_, horizontalMaskW, imgH_, // 背景画像の一部
			imgBackGameClaer_, TRUE, FALSE
		);
	}

	// 敵モデル
	MV1DrawModel(enemy_.modelId);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, pressKeyAlpha_);
	DrawGraph(0, pressKeyY_, imgPressKey_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::Release(void)
{
	DeleteGraph(imgClear_);
	DeleteGraph(imgBackGameClaer_);
	DeleteGraph(imgClearWolrd_);
	DeleteGraph(imgReplay_);
	DeleteGraph(imgReturn_);
	DeleteGraph(imgPressKey_);

	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMECLEAR_BGM);
}
