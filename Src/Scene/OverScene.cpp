#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome.h" 
#include "OverScene.h"

// 3D座標をスクリーン座標に変換
bool OverScene::WorldToScreen(const VECTOR& worldPos, VECTOR& screenPos)
{
	// 画面サイズ
	int screenW = Application::SCREEN_SIZE_X;
	int screenH = Application::SCREEN_SIZE_Y;

	// X座標を画面Xにマッピング（中央を画面中央に）
	screenPos.x = static_cast<int>(screenW) / VALUE_TWO + static_cast<int>(worldPos.x);

	// Z座標を画面Yにマッピング（手前が下、適当にY軸調整）
	// Yはそのまま画面の下方向に使う例
	screenPos.y = static_cast<int>(screenH) / VALUE_TWO - static_cast<int>(worldPos.z);

	// 簡易判定として画面内ならtrue
	if (screenPos.x < 0 || screenPos.x > screenW || screenPos.y < 0 || screenPos.y > screenH) {
		return false;
	}
	return true;
}

OverScene::OverScene(void)
{
	imgGameOver_ = -1;
	imgLightCircle_ = -1;

	animationController_ = nullptr;

	selectedIndex_ = 0;
	isMenuActive_ = false;

	maskLeftX_ = 0;
	maskRightX_ = 0;

	cheackCounter_ = 0;
}

OverScene::~OverScene(void)
{
}

void OverScene::Init(void)
{
	// 重力制御の初期化
	GravityManager::GetInstance().Init();

	// 画像読み込み
	imgGameOver_ = resMng_.Load(ResourceManager::SRC::GAMEOVER).handleId_;
	imgDieTree_ = resMng_.Load(ResourceManager::SRC::DIETREE).handleId_;
	imgCursor_[0] = LoadGraph("Data/Image/Over/1player1.png");
	imgCursor_[1] = LoadGraph("Data/Image/Over/1player2.png");
	imgLightCircle_ = resMng_.Load(ResourceManager::SRC::LIGHT).handleId_;  // 作成した光画像を読み込む

	// 画像の横幅（例）
	const int massageX = Application::SCREEN_SIZE_X / static_cast<int>(VALUE_TWO) - MASSAGE_X_OFFSET;
	maskLeftX_ = massageX;                          // 黒帯は最初は画像全体を覆う
	maskRightX_ = massageX + maskWidthMax_;

	selectedIndex_ = 0;

	cheackCounter_ = 0;

	isMenuActive_ = false;

	SoundManager::GetInstance().Play(SoundManager::SRC::GAMEOVER_BGM, Sound::TIMES::ONCE);

	// キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	charactor_.pos = { PLAYER_POS };
	charactor_.scl = { PLAYER_SIZE };
	charactor_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	charactor_.Update();

	// アニメーションの設定
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Play(0);

	// 定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);

	cnt = 0;
}

void OverScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	cheackCounter_++;

	//if (ins.IsTrgDown(KEY_INPUT_TAB))SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);

	if (isMenuActive_)
	{
		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_DOWN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN)) {
			selectedIndex_ = 1 - selectedIndex_;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			if (selectedIndex_ == 0) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			}
			else {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
	}

	// 一定時間がたてば強制的にタイトルに
	if (cheackCounter_ >= FORCE_TITLE_TIME) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// 黒帯の左端を右へスライド（文字が左から見える）
	if (maskLeftX_ < maskRightX_)
	{
		maskLeftX_ += revealSpeed_;
		if (maskLeftX_ >= maskRightX_) {
			maskLeftX_ = maskRightX_;
			isMenuActive_ = true; // ← ここで初めて true にする
		}
	}

	animationController_->Update();
}

void OverScene::Draw(void)
{
	// 背景真っ黒
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x0, true);

	// ゲームオーバー画像
	DrawGraph(Application::SCREEN_SIZE_X / static_cast<int>(VALUE_TWO) - GAMEOVER_IMG_X_OFFSET, GAMEOVER_IMG_Y, imgGameOver_, true);

	// プレイヤーモデル描画
	MV1DrawModel(charactor_.modelId);

	// 光の描画（プレイヤー周辺）
	VECTOR screenPos;
	if (WorldToScreen(charactor_.pos, screenPos))
	{
		int alpha = LIGHT_ALPHA;
		SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

		int w, h;
		GetGraphSize(imgLightCircle_, &w, &h);

		float scaleX = LIGHT_SCALE_X;  // 横伸ばし
		float scaleY = LIGHT_SCALE_Y; // 縦伸ばし

		int drawW = static_cast<int>(w * scaleX);
		int drawH = static_cast<int>(h * scaleY);
		int offsetY = LIGHT_OFFSET_Y;

		DrawExtendGraph(
			(int)screenPos.x - drawW / static_cast<int>(VALUE_TWO),
			(int)screenPos.y + offsetY - drawH / static_cast<int>(VALUE_TWO),
			(int)screenPos.x + drawW / static_cast<int>(VALUE_TWO),
			(int)screenPos.y + offsetY + drawH / static_cast<int>(VALUE_TWO),
			imgLightCircle_,
			TRUE
		);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (isMenuActive_)
	{
		if (selectedIndex_ % 2 == 1)
		{
			SetFontSize(MENU_FONT_SIZE);
			DrawString(MENU_POS_X, MENU_PLAY_Y, "もう一度プレイ", white);
			DrawString(MENU_POS_X, MENU_TITLE_Y, "タイトルに戻る", yellow);
		}
		else
		{
			SetFontSize(MENU_FONT_SIZE);
			DrawString(MENU_POS_X, MENU_PLAY_Y, "もう一度プレイ", yellow);
			DrawString(MENU_POS_X, MENU_TITLE_Y, "タイトルに戻る", white);
		}
	}

	// 画像描画
	SetFontSize(STORY_FONT_SIZE);
	DrawString(Application::SCREEN_SIZE_X / static_cast<int>(VALUE_TWO) - STORY_X_OFFSET, STORY_Y, "ユグドラシルは死んでしまった…", white, true);
	SetFontSize(DEFAULT_FONT_SIZE);

	//黒帯描画（maskLeftX_ は初期値 msgX + imgW から 徐々に msgX へ移動する想定）
	DrawBox(BLACK_BOX_X1 + (cnt * BLACK_BOX_SLIDE_SPEED),
		BLACK_BOX_Y1, BLACK_BOX_X2, BLACK_BOX_Y2, black, true);
}

void OverScene::Release(void)
{
	if (charactor_.modelId != -1)
	{
		MV1DeleteModel(charactor_.modelId);
		charactor_.modelId = -1;
	}
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMEOVER_BGM);
}