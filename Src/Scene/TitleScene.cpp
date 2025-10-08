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
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	imgTitle_ = -1;
	imgBackTitle_ = -1;
	imgUDCursor_ = -1;

	animationControllerPlayer_ = nullptr;
	animationControllerEnemy_ = nullptr;
	endLoadFlame_ = true;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	cnt = 0;

	// 重力制御の初期化
	GravityManager::GetInstance().Init();
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE).handleId_;			//タイトル名前画像
	imgBackTitle_ = resMng_.Load(ResourceManager::SRC::BACK_TITLE).handleId_;	//タイトル背景
	imgConfirmEnd_ = resMng_.Load(ResourceManager::SRC::CONFIRM_END).handleId_;	//本当に終了しますか？画像
	imgYes_ = resMng_.Load(ResourceManager::SRC::YES).handleId_;				//はい画像
	imgNo_ = resMng_.Load(ResourceManager::SRC::NO).handleId_;					//いいえ画像
	imgYesSel_ = resMng_.Load(ResourceManager::SRC::SELECT_YES).handleId_;		//選択中はい画像
	imgNoSel_ = resMng_.Load(ResourceManager::SRC::SELECT_NO).handleId_;		//選択中いいえ画像

	img3D_ = LoadGraph("Data/Image/Title/3D.png");
	imgP1_[0] = LoadGraph("Data/Image/Title/1player1.png");
	imgP1_[1] = LoadGraph("Data/Image/Title/1player2.png");
	imgP2_[0] = LoadGraph("Data/Image/Title/2player1.png");
	imgP2_[1] = LoadGraph("Data/Image/Title/2player2.png");

	selectedIndex_ = 0;

	SetUseASyncLoadFlag(true);

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::TITLE_BGM, Sound::TIMES::LOOP);
	charactor_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	enemy_.SetModel(resMng_.Load(ResourceManager::SRC::DOG).handleId_);
	SetUseASyncLoadFlag(false);
	float size;

	// 定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::NewFunction()
{
	// 初期位置は左端付近にプレイヤー、その左側に敵
	charactor_.pos = { PLAYER_POS_X, CHARACTER_POS_Y, CHARACTER_POS_Z };
	charactor_.scl = { PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE };
	charactor_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	charactor_.Update();

	enemy_.pos = { ENEMY_POS_X, CHARACTER_POS_Y, CHARACTER_POS_Z };
	enemy_.scl = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE };
	enemy_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	enemy_.Update();

	// 右向きスタート（1:右向き、-1:左向き）
	enemyDirection_ = 1;

	// プレイヤーのアニメーション
	std::string path = Application::PATH_MODEL + "NPlayer/";
	animationControllerPlayer_ = std::make_unique<AnimationController>(charactor_.modelId);
	animationControllerPlayer_->Add(0, path + "Player.mv1", ANIMATION_TIME, PLAYER_ANIM_NUM);
	animationControllerPlayer_->Play(0);

	// 敵のアニメーション
	std::string path1 = Application::PATH_MODEL + "Enemy/Yellow/";
	animationControllerEnemy_ = std::make_unique<AnimationController>(enemy_.modelId);
	animationControllerEnemy_->Add(0, path1 + "Yellow.mv1", ANIMATION_TIME, ENEMY_ANIM_NUM);
	animationControllerEnemy_->Play(0);
}

void TitleScene::Update(void)
{
	cnt++;
	cnt_3D = cnt * IMAGE_3D_SPEAD;
	if (cnt_3D >= MAX_CONUT_3D)cnt_3D = MAX_CONUT_3D;
	InputManager& ins = InputManager::GetInstance();

	// === 終了確認中の入力処理 ===
	if (isConfirmingExit_)
	{
		// アニメーション進行（最大まで）
		if (confirmAnimFrame_ < CONFIRM_ANIM_DURATION) {
			confirmAnimFrame_++;
		}

		if (confirmAnimFrame_ >= CONFIRM_ANIM_DURATION) {
			// 入力受付はアニメーション終了後に
			if (ins.IsTrgDown(KEY_INPUT_LEFT) || ins.IsTrgDown(KEY_INPUT_RIGHT)) {
				confirmIndex_ = 1 - confirmIndex_; // 「はい」「いいえ」切替
			}
			if (ins.IsTrgDown(KEY_INPUT_RETURN)) {
				SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);

				if (confirmIndex_ == 0) {
					Application::isRunning_ = false;
				}
				else {
					isConfirmingExit_ = false;
				}
			}
		}

		return;
	}

	// === 点滅更新 ===
	blinkFrameCount_++;
	if (blinkFrameCount_ > ONE_SECOND_FRAME) {
		blinkFrameCount_ = 0;
	}

	// === メニュー選択操作 ===
	if (ins.IsTrgDown(KEY_INPUT_DOWN)) {
		selectedIndex_ = (selectedIndex_ + 1) % MENU_SELECT;
	}
	else if (ins.IsTrgDown(KEY_INPUT_UP)) {
		selectedIndex_ = (selectedIndex_ + 2) % MENU_SELECT;
	}

	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		if (selectedIndex_ == 0) {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
		}
		else if (selectedIndex_ == 1) {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::DEMO);
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
		}
		else if (selectedIndex_ == 2) {
			isConfirmingExit_ = true;
			confirmIndex_ = 1;
			confirmAnimFrame_ = 0;  // ← アニメーション開始
			SoundManager::GetInstance().Play(SoundManager::SRC::WARNING_SE, Sound::TIMES::FORCE_ONCE);
		}
	}

	if (GetASyncLoadNum() != 0)
	{
		return;
	}
	if (endLoadFlame_)
	{
		endLoadFlame_ = false;
		NewFunction();
	}

	// === キャラクターの移動・向き制御 ===

	charactor_.pos.x += PLAYER_SPEED * enemyDirection_;

	float diffX = charactor_.pos.x - enemy_.pos.x;
	if (enemyDirection_ == 1) {
		if (diffX > SAFE_DISTANCE) {
			enemy_.pos.x += ENEMY_SPEED;
			if (enemy_.pos.x > charactor_.pos.x - SAFE_DISTANCE)
				enemy_.pos.x = charactor_.pos.x - SAFE_DISTANCE;
		}
	}
	else {
		if (diffX < -SAFE_DISTANCE) {
			enemy_.pos.x -= ENEMY_SPEED;
			if (enemy_.pos.x < charactor_.pos.x + SAFE_DISTANCE)
				enemy_.pos.x = charactor_.pos.x + SAFE_DISTANCE;
		}
	}

	if (charactor_.pos.x > RIGHT_BOUND) {
		enemyDirection_ = -1;
	}
	else if (charactor_.pos.x < LEFT_BOUND) {
		enemyDirection_ = 1;
	}

	float yRotDeg = (enemyDirection_ == 1) ? -INVERSION : INVERSION;
	enemy_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(yRotDeg), 0.0f);
	charactor_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(yRotDeg), 0.0f);

	if (!endLoadFlame_) {
		enemy_.Update();
		charactor_.Update();

		animationControllerPlayer_->Update();
		animationControllerEnemy_->Update();
	}
}

void TitleScene::Draw(void)
{
	int centerX = (Application::SCREEN_SIZE_X / 2);

	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);

	// タイトルロゴ表示
	int titleW, titleH;
	GetGraphSize(imgTitle_, &titleW, &titleH);
	DrawRotaGraph((Application::SCREEN_SIZE_X / 2), IMG_TITLE_HEIGHT, IMG_TITLE_SIZE, 0, imgTitle_, true);
	DrawRotaGraph(IMG_3D_WIDTH, cnt_3D - IMG_3D_HEIGHT, IMG_3D_SIZE, 0, img3D_, true);

#pragma region		ボタン設定
	const int yGame = BASE_Y;
	const int yRule = BASE_Y + BUTTON_OFFSET;
	const int yExit = BASE_Y + BUTTON_OFFSET * 2;

	int phase = blinkFrameCount_ % ONE_SECOND_FRAME;
	int alpha = (phase < ONE_SECOND_FRAME / 2)
		? (WHITE_ALPHA * phase) / (ONE_SECOND_FRAME / 2)
		: WHITE_ALPHA - (WHITE_ALPHA * (phase - ONE_SECOND_FRAME / 2)) / (ONE_SECOND_FRAME / 2);

	int yPositions[MENU_SELECT] = { yGame, yRule, yExit };
#pragma endregion

	// メニュー描画
	for (int i = 0; i < MENU_SELECT; ++i) {
		if (selectedIndex_ == i) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		}

		if (selectedIndex_ == i) {
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	// カーソル描画
	DrawRotaGraph(CURSOR_1_WIDTH, CURSOR_HEIGHT +
		(selectedIndex_ * INDEX), IMG_CURSOR_SIZE, 0, imgP1_[static_cast<int>(cnt * CURSOR_MOVE_SPEED) % 2], true);

	DrawRotaGraph(CURSOR_2_WIDTH, CURSOR_HEIGHT +
		(selectedIndex_ * INDEX), IMG_CURSOR_SIZE, 0, imgP2_[static_cast<int>(cnt * CURSOR_MOVE_SPEED) % 2], true);

	//テキスト
	SetFontSize(DEFAULT_FONT_SIZE * 3.75);
	DrawString(TEXT_WIDTH, TEXT_HEIGHT_1, "ゲームプレイ", white);
	DrawString(TEXT_WIDTH, TEXT_HEIGHT_2, "神様のお告げ", white);
	DrawString(TEXT_WIDTH, TEXT_HEIGHT_3, "ゲームを終了", white);

	SetFontSize(DEFAULT_FONT_SIZE);
	if (GetASyncLoadNum() != 0)
	{
		return;
	}

	// モデル描画
	if (!endLoadFlame_) {
		MV1DrawModel(charactor_.modelId);
		MV1DrawModel(enemy_.modelId);
	}

	if (isConfirmingExit_) {
		float t = static_cast<float>(confirmAnimFrame_) / CONFIRM_ANIM_DURATION;
		if (t > 1.0f) t = 1.0f;

		int alpha = static_cast<int>(GRAY_ALPHA * t);  // 背景透明度
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ウィンドウ背景画像がある場合はここに描画（省略可）

		// 「本当に終了しますか？」画像描画
		SetFontSize(DEFAULT_FONT_SIZE * 7.8125);
		DrawString(END_STRING_WIDTH, END_STRING_HEIGHT, "本当に終了しますか？", white);

		// 選択中で画像を切り替え
		if (confirmIndex_ == 0)
		{
			SetFontSize(DEFAULT_FONT_SIZE * 8.125);
			DrawString(YES_STRING_WIDTH, YES_STRING_HEIGHT, "はい", yellow);
			DrawString(Application::SCREEN_SIZE_X / 2 + 130, Application::SCREEN_SIZE_Y / 2 + 100, "いいえ", white);
		}
		else
		{
			SetFontSize(DEFAULT_FONT_SIZE * 8.125);
			DrawString(YES_STRING_WIDTH, YES_STRING_HEIGHT, "はい", white);
			DrawString(NO_STRING_WIDTH, NO_STRING_HEIGHT, "いいえ", yellow);
		}
	}
}

void TitleScene::Release(void)
{
	if (charactor_.modelId != -1)
	{
		MV1DeleteModel(charactor_.modelId);
		charactor_.modelId = -1;
	}

	SoundManager::GetInstance().Stop(SoundManager::SRC::TITLE_BGM);
	SoundManager::GetInstance().Stop(SoundManager::SRC::SET_SE);
	SoundManager::GetInstance().Stop(SoundManager::SRC::WARNING_SE);
}