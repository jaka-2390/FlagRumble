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

// 4x4行列 × 4成分ベクトルの掛け算
OverScene::VECTOR4 OverScene::MulMatVec(const MATRIX& m, const VECTOR4& v)
{
	VECTOR4 r;
	r.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
	r.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
	r.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
	r.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;
	return r;
}

// 3D座標をスクリーン座標に変換
bool OverScene::WorldToScreen(const VECTOR& worldPos, VECTOR& screenPos)
{
	// 画面サイズ
	int screenW = Application::SCREEN_SIZE_X;
	int screenH = Application::SCREEN_SIZE_Y;

	// X座標を画面Xにマッピング（中央を画面中央に）
	screenPos.x = screenW / 2 + static_cast<int>(worldPos.x);

	// Z座標を画面Yにマッピング（手前が下、適当にY軸調整）
	// Yはそのまま画面の下方向に使う例
	screenPos.y = screenH / 2 - static_cast<int>(worldPos.z);

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
	const int msgX = Application::SCREEN_SIZE_X / 2 - 400;
	maskLeftX_ = msgX;                          // 黒帯は最初は画像全体を覆う
	maskRightX_ = msgX + maskWidthMax_;

	float size;

	selectedIndex_ = 0;

	cheackCounter_ = 0;

	isMenuActive_ = false;

	SoundManager::GetInstance().Play(SoundManager::SRC::GAMEOVER_BGM, Sound::TIMES::ONCE);

	// キャラ
	charactor_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	charactor_.pos = { 0.0f,-200.0f,0.0f };
	size = 1.4f;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	charactor_.Update();

	// アニメーションの設定
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationController_->Add(0, path + "Sword And Shield Death.mv1", 20.0f);
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
		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_DOWN)) {
			selectedIndex_ = 1 - selectedIndex_;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN)) {
			if (selectedIndex_ == 0) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			}
			else {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
	}

	// 一定時間がたてば強制的にタイトルに
	if (cheackCounter_ >= 3600) {
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
	DrawBox(0,0,Application::SCREEN_SIZE_X,Application::SCREEN_SIZE_Y,0x0,true);

	// ゲームオーバー画像
	DrawGraph(Application::SCREEN_SIZE_X/2-555,50,imgGameOver_,true);

	// プレイヤーモデル描画
	MV1DrawModel(charactor_.modelId);

	// 光の描画（プレイヤー周辺）
	VECTOR screenPos;
	if (WorldToScreen(charactor_.pos, screenPos))
	{
		int alpha = 55;
		SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

		int w, h;
		GetGraphSize(imgLightCircle_, &w, &h);

		float scaleX = 1.3f;  // 横伸ばし
		float scaleY = 2.05f; // 縦伸ばし

		int drawW = static_cast<int>(w * scaleX);
		int drawH = static_cast<int>(h * scaleY);
		int offsetY = 200;

		DrawExtendGraph(
			(int)screenPos.x - drawW / 2,
			(int)screenPos.y + offsetY - drawH / 2,
			(int)screenPos.x + drawW / 2,
			(int)screenPos.y + offsetY + drawH / 2,
			imgLightCircle_,
			TRUE
		);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (isMenuActive_)
	{
		if (selectedIndex_ % 2 == 1)
		{
			SetFontSize(55);
			DrawString(1500, 800, "もう一度プレイ", 0xffffff);
			DrawString(1500, 890, "タイトルに戻る", 0xffff00);
		}
		else
		{
			SetFontSize(55);
			DrawString(1500, 800, "もう一度プレイ", 0xffff00);
			DrawString(1500, 890, "タイトルに戻る", 0xffffff);
		}
	}

	// 画像描画
	SetFontSize(90);
	DrawString(Application::SCREEN_SIZE_X/2-675,330,"ユグドラシルは死んでしまった…",0xffffff,true);
	SetFontSize(16);

	// 黒帯描画（maskLeftX_ は初期値 msgX + imgW から 徐々に msgX へ移動する想定）
	DrawBox(200+(cnt*4),300,1700,500,0x0,true);
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