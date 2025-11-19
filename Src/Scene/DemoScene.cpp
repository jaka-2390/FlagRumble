#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/FlagManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Object/EnemyBase.h"
#include "../Object/Enemy/EnemyCactus.h"
#include "../Object/Enemy/EnemyDog.h"
#include "../Object/Planet.h"
#include "../Object/Flag/Flag.h"
#include "DemoScene.h"

DemoScene::DemoScene(void)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgOpeGear_ = -1;
}

DemoScene::~DemoScene(void)
{
}

void DemoScene::Init(void)
{
	cnt = 0;
	//プレイヤー
	player_ = std::make_shared<Player>();
	GravityManager::GetInstance().SetPlayer(player_);
	player_->Init();

	player_->SetEnemy(&enemys_);

	//ステージ
	stage_ = std::make_unique<Stage>(*player_);
	stage_->Init();

	//ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	flagManager_ = std::make_shared<FlagManager>();
	flagManager_->Clear();
	flagManager_->AddFlag(VGet(-250.0f, 254.0f, 1000.0f), Flag::ENEMY_TYPE::DOG);

	//画像
	imgOpeGear_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR).handleId_;

	pauseImg_ = LoadGraph("Data/Image/pause.png");

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE).handleId_;	//操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEITEM).handleId_;	//アイテム概要

	//カウンタ
	uiFadeStart_ = false;
	uiFadeFrame_ = 0;
	uiDisplayFrame_ = 0;

	//ポーズ
	isPaused_ = false;
	pauseSelectIndex_ = 0;

	//カメラのポーズ解除
	camera_ = SceneManager::GetInstance().GetCamera().lock();
	if (camera_) {
		camera_->SetPaused(false); //← ここが重要！

		//ミニマップ用カメラ
		camera_->SetFollow(&player_->GetTransform());
		camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_BGM, Sound::TIMES::LOOP);

	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);
}

void DemoScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	if (PauseMenu()) return; //ポーズ中ならここで止める

	//-------------------------
	//通常時のゲーム進行（ポーズされてないときだけ）
	//-------------------------

	if (ins.IsNew(KEY_INPUT_RETURN) )
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	switch (state_)
	{
	case STATE::MOVE:
		UpdateMove();
		break;
	case STATE::ATTACK:
		UpdateAttack();
		break;
	case STATE::FLAG:
		UpdateFlag();
		break;
	case STATE::SABO:
		UpdateSabo();
		break;
	case STATE::FINISH:
		if (ins.IsNew(KEY_INPUT_RETURN))
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
		break;
	}

	uiDisplayFrame_++;

	skyDome_->Update();
	stage_->Update();
	player_->Update();

	for (auto enemy : enemys_)
	{
		enemy->Update();
	}

	// 敵全滅情報をFlagに伝える
	flagManager_->Update(player_->GetTransform().pos, enemys_);

	// フラッグでクリアしたらシーン遷移
	int cleared = flagManager_->GetClearedFlagCount();
	int total = flagManager_->GetFlagMax();
}

void DemoScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	for (auto enemy : enemys_)
	{
		enemy->Draw();
	}
	player_->Draw();

	flagManager_->Draw();

	DrawMessage();

	DrawRotaGraph(UI_GEAR, UI_GEAR, IMG_OPEGEAR_UI_SIZE, 0.0, imgOpeGear_, true);

	//入力チェック or 時間経過でフェード開始
	if (!uiFadeStart_)
	{
		if ((CheckHitKey(KEY_INPUT_W)
			|| CheckHitKey(KEY_INPUT_A)
			|| CheckHitKey(KEY_INPUT_S)
			|| CheckHitKey(KEY_INPUT_D))
			|| uiDisplayFrame_ >= AUTO_FADE)  //時間経過による自動フェード
		{
			uiFadeStart_ = true;
			uiFadeFrame_ = 0;
		}
	}

	if (isPaused_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (pauseState_ == PauseState::Menu)
		{
			DrawRotaGraph((Application::SCREEN_SIZE_X / 2), UI_PAUSE_IMG_HEIGHT, PAUSE_IMG_UI_SIZE, 0, pauseImg_, true);
			SetFontSize(DEFAULT_FONT_SIZE * 5.0);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 0)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 1)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 2)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 3)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", yellow);

			SetFontSize(DEFAULT_FONT_SIZE);
		}
		else if (pauseState_ == PauseState::ShowControls)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[0], true);
			SetFontSize(DEFAULT_FONT_SIZE * 2.5);
			DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", yellow);
			if (cnt % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white);
			SetFontSize(DEFAULT_FONT_SIZE);
		}
		else if (pauseState_ == PauseState::ShowItems)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[1], true);
			SetFontSize(DEFAULT_FONT_SIZE * 2.5);
			DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", yellow);
			if (cnt % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white);
			SetFontSize(DEFAULT_FONT_SIZE);
		}
		return;
	}
#pragma region UI
	SetFontSize(DEFAULT_FONT_SIZE * 2.0);
	DrawString(UI_ATTACK_X, UI_NORMAL_ATTACK_Y, "E:通常攻撃", white);
	SetFontSize(DEFAULT_FONT_SIZE);
#pragma endregion
}

void DemoScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAME_BGM);
}

const std::vector<std::shared_ptr<EnemyBase>>& DemoScene::GetEnemies() const
{
	return enemys_;
}

void DemoScene::UpdateMove()
{
	if (CheckHitKey(KEY_INPUT_W) ||
		CheckHitKey(KEY_INPUT_A) ||
		CheckHitKey(KEY_INPUT_S) ||
		CheckHitKey(KEY_INPUT_D)) {

		state_ = STATE::ATTACK;
	}
}

void DemoScene::UpdateAttack()
{
	//Dogがいなかったら生成
	if (!isDog_)
	{
		EnemyCreateAt(VGet(-250.0f, 254.0f, 1000.0f), 1, EnemyBase::TYPE::DOG); // 各フラッグの周囲に1体
		isDog_ = true;
		return;
	}

	//敵が死んだかどうか
	bool allDead = true;
	for (auto& enemy : enemys_)
	{
		if (enemy->IsAlive())
		{
			allDead = false;
			break;
		}
	}

	//Dogが出て、死んだら
	if (isDog_ && allDead)
	{
		state_ = STATE::FLAG;
	}
}

void DemoScene::UpdateFlag()
{
	flagManager_->Update(player_->GetTransform().pos, {}); // 敵なし

	// 旗ゲージが規定値に達したらクリア
	if (flagManager_->GetClearedFlagCount() >= 1) {
		state_ = STATE::SABO;
	}
}

void DemoScene::UpdateSabo()
{
	SpawnCactus();

	if (CheckHitKey(KEY_INPUT_A)) {
		state_ = STATE::FINISH;
	}
}

void DemoScene::DrawMessage()
{
	int x = 50;
	int y = 600;

	switch (state_) {

	case STATE::MOVE:
		DrawString(x, y, "WASD で移動してみよう！", white);
		break;

	case STATE::ATTACK:
		DrawString(x, y, "黄色い敵は陣地を守っています", white);
		DrawString(x, y + 40, "E キーで攻撃して倒してみよう！", white);
		break;

	case STATE::FLAG:
		DrawString(x, y, "エリアに近づいて陣地を奪還してみよう！", white);
		DrawString(x, y + 40, "ゲージが100%になると奪還成功です", white);
		break;
		
	case STATE::SABO:
		DrawString(x, y, "サボテンは、あなたの陣地を奪いに来る敵です", white);
		//DrawString(x, y + 40, "ゲージが100%になると次へ進みます", white);
		break;

	case STATE::FINISH:
		DrawString(x, y, "チュートリアル完了！", white);
		DrawString(x, y + 40, "Enter でゲーム開始", white);
		break;
	}
}

void DemoScene::EnemyCreateAt(VECTOR flagPos, int count, EnemyBase::TYPE type)
{
	for (int i = 0; i < count; ++i)
	{
		VECTOR randPos = flagPos;
		randPos.x += GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;
		randPos.z += GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;

		std::shared_ptr<EnemyBase> enemy;

		switch (type)
		{
		case EnemyBase::TYPE::SABO:
		{
			auto cactus = std::make_shared<EnemyCactus>();
			cactus->SetFlagManager(flagManager_.get());
			enemy = cactus;  // EnemyBase型に代入
			break;
		}

		case EnemyBase::TYPE::DOG:
			enemy = std::make_shared<EnemyDog>();
			break;
		default:
			enemy = std::make_shared<EnemyDog>();
			break;
		}

		enemy->SetDemoScene(this);
		enemy->SetPos(randPos);
		enemy->SetPlayer(player_);
		enemy->Init();

		enemys_.emplace_back(std::move(enemy));
	}
}

void DemoScene::SpawnCactus(void)
{
	// すでにボスが出現しているなら何もしない
	for (auto& enemy : enemys_) {
		if (std::dynamic_pointer_cast<EnemyCactus>(enemy)) {
			return;
		}
	}

	// ENEMY状態の旗を探す
	EnemyCreateAt(VGet(-250.0f, 254.0f, 2000.0f), 1, EnemyBase::TYPE::SABO);
	
}

bool DemoScene::PauseMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	//TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::STRAT) && pauseState_ == PauseState::Menu)
	{
		isPaused_ = !isPaused_;
		pauseSelectIndex_ = 0;
		mainCamera->SetPaused(isPaused_);
		return true;
	}

	if (!isPaused_) return false; //ポーズ中でなければ通常更新

	if (pauseState_ == PauseState::Menu)
	{
		if (ins.IsTrgDown(KEY_INPUT_DOWN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_DOWN) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_UP) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			switch (pauseSelectIndex_)
			{
			case 0:
				isPaused_ = false;
				mainCamera->SetPaused(false);
				break;
			case 1: pauseState_ = PauseState::ShowControls; break;
			case 2: pauseState_ = PauseState::ShowItems; break;
			case 3: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
			}
		}
	}
	else
	{
		if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			pauseState_ = PauseState::Menu;
	}

	return true;
}
