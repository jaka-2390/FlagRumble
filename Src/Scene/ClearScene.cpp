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
	messageX_ = Application::SCREEN_SIZE_X / VALUE_TWO - imgW_ / VALUE_TWO;
	messageY_ = MESSAGE_Y;

	// マスクの初期位置（完全に隠れている状態）
	maskLeftX_ = messageX_;
	maskSpeed_ = MASK_SPEED;

	// フェード処理
	clearAlpha_ = 0;     // 完全に透明から始める
	fadeSpeed_ = FADE_SPEED;      // 徐々に表示（速さはお好みで）

	// presskey用
	pressKeyY_ = Application::SCREEN_SIZE_Y + PRESS_KEY_OFFSET_Y;	// 画面下 + α からスタート
	targetPressKeyY_ = PRESS_KEY_TARGET_Y;	// 目標位置
	pressKeyAlpha_ = 0;	// 透明から始める
	isPressKeyAnimStart_ = false;
	isPressKeyAnimEnd_ = false;

	// 敵
	enemy_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MUSH));
	enemy_.pos = ENEMY_INIT_POS;
	enemy_.scl = ENEMY_INIT_SCL;
	enemy_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(ENEMY_INIT_ROT_Y_DEG), 0.0f);
	enemy_.Update();

	// 敵のアニメーション
	std::string path1 = Application::PATH_MODEL + "Enemy/mushroom/";
	animationControllerEnemy_ = std::make_unique<AnimationController>(enemy_.modelId);
	animationControllerEnemy_->Add(0, path1 + "mushroom.mv1", -ENEMY_INIT_ROT_Y_DEG, 1);
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
	if (maskLeftX_ < messageX_ + imgW_) {
		maskLeftX_ += maskSpeed_;
		if (maskLeftX_ > messageX_ + imgW_) {
			maskLeftX_ = messageX_ + imgW_;
		}
	}

	if (clearAlpha_ < ALPHA_MAX) {
		clearAlpha_ += fadeSpeed_;
		if (clearAlpha_ > ALPHA_MAX) clearAlpha_ = ALPHA_MAX;
	}

	// アニメーション終了チェック
	if (!isAnimEnd_ && maskLeftX_ >= messageX_ + imgW_ && clearAlpha_ >= ALPHA_MAX) {
		isAnimEnd_ = true;
		isPressKeyAnimStart_ = true; // ← ここで pressKey のアニメーション開始
	}

	// 入力受付（アニメーション後）
	if (isAnimEnd_ && CheckHitKeyAll() > 0 || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		// 音楽
		SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
	}

	// 強制遷移
	if (cheackCounter_ >= AUTO_RETURN_FRAME) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	if (isPressKeyAnimStart_ && !isPressKeyAnimEnd_) {
		// Y座標を補間（滑らかに近づける）
		pressKeyY_ -= PRESS_KEY_SLIDE_SPEED;  // スライドスピード
		enemy_.pos.y += ENEMY_FLOAT_SPEED_Y;
		if (pressKeyY_ <= targetPressKeyY_) {
			pressKeyY_ = targetPressKeyY_;
		}

		// アルファ値を増加（フェードイン）
		if (pressKeyAlpha_ < ALPHA_MAX) {
			pressKeyAlpha_ += PRESS_KEY_FADE_SPEED;  // フェードスピード
			if (pressKeyAlpha_ > ALPHA_MAX) pressKeyAlpha_ = ALPHA_MAX;
		}

		// アニメーション完了フラグ更新
		if (pressKeyY_ == targetPressKeyY_ && pressKeyAlpha_ == ALPHA_MAX) {
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

	DrawRotaGraph(Application::SCREEN_SIZE_X / VALUE_TWO, CLEAR_IMG_Y, CLEAR_IMG_SCALE, 0, imgClear_, true);

	// メッセージ（下のテキスト）を表示
	SetFontSize(MESSAGE_FONT_SIZE);
	DrawString(Application::SCREEN_SIZE_X / VALUE_TWO - MESSAGE_FONT_SIZE * MESSAGE_X_SCALE, MESSAGE_Y,
		"陣地を奪還した", true);
	SetFontSize(FONT_SIZE_DEFAULT);

	// メッセージの上に背景で覆う（横方向）
	int horizontalMaskW = messageX_ + imgW_ - maskLeftX_;
	if (horizontalMaskW > 0) {
		DrawRectGraph(
			maskLeftX_, messageY_,                    // 表示先（画面上）
			maskLeftX_, messageY_, horizontalMaskW, imgH_, // 背景画像の一部
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