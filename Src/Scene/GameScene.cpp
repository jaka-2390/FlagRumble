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
#include "../Object/Enemy/EnemyMimic.h"
#include "../Object/Enemy/EnemyMushroom.h"
#include "../Object/Enemy/EnemyOnion.h"
#include "../Object/Enemy/EnemyThorn.h"
#include "../Object/Enemy/EnemyVirus.h"
#include "../Object/Enemy/EnemyBoss.h"
#include "../Object/Planet.h"
#include "MiniMap.h"
#include "GameScene.h"

//担当いけだ

GameScene::GameScene(void)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgGameUi1_ = -1;
	imgOpeGear_ = -1;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	cnt = 0;
	//プレイヤー
	player_ = std::make_shared<Player>();
	GravityManager::GetInstance().SetPlayer(player_);
	player_->Init();

	//敵のモデル
	//EnemyCreate(0);
	spawnAreas_.push_back({ VGet(0.0f, 0.0f, 0.0f), SPAWN_RADIUS, false });
	lastSpawnTime_ = GetNowCount();  //開始時の時間を記録

	player_->SetEnemy(&enemys_);

	//ステージ
	stage_ = std::make_unique<Stage>(*player_);
	stage_->Init();

	//ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	map_ = std::make_unique<MiniMap>(MINIMAP_RANGE, MINIMAP_SIZE);
	map_->Init();

	flagManager_ = std::make_shared<FlagManager>();
	flagManager_->Init();

	//画像
	imgGameUi1_ = resMng_.Load(ResourceManager::SRC::GAMEUI_1).handleId_;
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

void GameScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	if (PauseMenu()) return; //ポーズ中ならここで止める

	//-------------------------
	//通常時のゲーム進行（ポーズされてないときだけ）
	//-------------------------

	uiDisplayFrame_++;

	skyDome_->Update();
	stage_->Update();
	player_->Update();

	for (auto enemy : enemys_)
	{
		enemy->Update();
	}

	enCounter++;
	if (enCounter > ENCOUNT)
	{
		enCounter = 0;
		if (ENEMY_MAX >= enemys_.size())
		{
			int spawnCount = 1; //まとめて出したい数
			//EnemyCreate(spawnCount);
		}
	}

	//敵全滅チェック
	if (!enemys_.empty()) {
		allEnemyDefeated_ = true;
		for (auto& enemy : enemys_)
		{
			if (enemy->IsAlive()) {
				allEnemyDefeated_ = false;
				break;
			}
		}
	}
	else {
		//敵がまだ出現していない時はfalseにしておく
		allEnemyDefeated_ = false;
	}

	// 次に立てる旗の位置で敵を生成
    if (enemys_.empty()) {
        // 敵生成タイミングをここで制御
        int nextFlagIdx = flagManager_->GetNextFlagIndex();
        if (nextFlagIdx < FLAG_MAX) // 3つまで
        {
            VECTOR flagPos = flagManager_->GetFlagPosition(nextFlagIdx);
            int spawnCount = 1;
            EnemyCreateAt(flagPos, spawnCount); // ここでランダム位置も加える
        }
    }

	// 敵全滅情報をFlagに伝える
	flagManager_->Update(player_->GetTransform().pos, allEnemyDefeated_);

	// フラッグでクリアしたらシーン遷移
	if (flagManager_->GetClearedFlagCount() >= FLAG_MAX && !bossSpawned_)
	{
		SpawnBoss();
		bossSpawned_ = true;
	}

	ClearCheck();
}

void GameScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	for (auto enemy : enemys_)
	{
		enemy->Draw();
	}
	player_->Draw();

	for (auto enemy : enemys_)
	{
		enemy->DrawBossHpBar();
	}

	flagManager_->Draw();

	DrawMiniMap();

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
	if (!uiFadeStart_)
	{
		//フェード前（通常表示）
		DrawRotaGraph((Application::SCREEN_SIZE_X / 2), GAME_HEIGHT_1, IMG_GAME_UI_1_SIZE, 0, imgGameUi1_, true);
	}
	else if (uiFadeFrame_ < ONE_SECOND_FRAME)
	{
		//フェード中（60フレームで徐々に消す）
		int alpha = static_cast<int>(255 * (ONE_SECOND_FRAME - uiFadeFrame_) / ONE_SECOND_FRAME);
		DrawRotaGraph((Application::SCREEN_SIZE_X / 2), GAME_HEIGHT_1, IMG_GAME_UI_1_SIZE, 0, imgGameUi1_, true);
		uiFadeFrame_++;
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

void GameScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAME_BGM);
}

void GameScene::DrawMiniMap(void)
{
	if (!map_) return;

	//プレイヤーの座標
	MapVector2 playerPos;
	playerPos.x = player_->GetTransform().pos.x;
	playerPos.z = player_->GetTransform().pos.z;
	//Y軸回転角を使用(ラジアン or 度数)
	float playerAngle = player_->GetTransform().rot.y;

	float cameraAngleRad = 0.0f;
	if (camera_) {
		cameraAngleRad = camera_->GetAngles().y;
	}

	//敵の座標リストを作成
	std::vector<std::shared_ptr<EnemyBase>> aliveEnemies;
	for (const auto& enemy : enemys_)
	{
		if (enemy->IsAlive())
		{
			aliveEnemies.push_back(enemy);
		}
	}

	//ミニマップ描画呼び出し
	map_->Draw(playerPos, playerAngle, cameraAngleRad, aliveEnemies);
}

const std::vector<std::shared_ptr<EnemyBase>>& GameScene::GetEnemies() const
{
	return enemys_;
}

void GameScene::EnemyCreate(int count)
{
	VECTOR flagPos = flagManager_->GetFlagPosition(0);  //Flagの位置を取得

	for (int i = 0; i < count; ++i)
	{
		VECTOR randPos = flagPos;

		//出現位置をランダムに設定（エリアの周囲に散らばせる）
		randPos.x = flagPos.x + GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;
		randPos.z = flagPos.z + GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;

		//EnemyDogを生成
		std::shared_ptr<EnemyBase> enemy = std::make_shared<EnemyDog>();

		//生成された敵の初期化
		enemy->SetGameScene(this);
		enemy->SetPos(randPos);
		enemy->SetPlayer(player_);
		enemy->Init();

		//リストに追加
		enemys_.emplace_back(std::move(enemy));
	}
}

void GameScene::EnemyCreateAt(VECTOR flagPos, int count)
{
	for (int i = 0; i < count; ++i)
	{
		VECTOR randPos = flagPos;
		randPos.x += GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;
		randPos.z += GetRand(SPAWN_RADIUS * 2) - SPAWN_RADIUS;

		auto enemy = std::make_shared<EnemyDog>();
		enemy->SetGameScene(this);
		enemy->SetPos(randPos);
		enemy->SetPlayer(player_);
		enemy->Init();

		enemys_.emplace_back(std::move(enemy));
	}
}

void GameScene::SpawnBoss(void)
{
	// すでにボスが出現しているなら何もしない
	for (auto& enemy : enemys_) {
		if (std::dynamic_pointer_cast<EnemyBoss>(enemy)) {
			return;
		}
	}

	// ボス出現位置を決める（ステージ中央など）
	VECTOR bossPos = BOSS_POS;

	// EnemyBossを生成
	std::shared_ptr<EnemyBase> boss = std::make_shared<EnemyBoss>();

	boss->SetGameScene(this);
	boss->SetPos(bossPos);
	boss->SetPlayer(player_);
	boss->Init();

	// 敵リストに追加
	enemys_.emplace_back(std::move(boss));
}

bool GameScene::PauseMenu(void)
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

void GameScene::ClearCheck(void)
{
	//ボスが存在するかチェック
	bool bossDefeated = true;
	for (auto& enemy : enemys_) {
		if (auto boss = std::dynamic_pointer_cast<EnemyBoss>(enemy)) {
			if (boss->IsAlive()) {  //isAlive_がtrueならまだ倒されていない
				bossDefeated = false;
				break;
			}
		}
	}

	//全旗を奪還済み && ボス倒したらクリア
	if (bossSpawned_ && bossDefeated) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}
