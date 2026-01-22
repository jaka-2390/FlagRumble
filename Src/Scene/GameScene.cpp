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
#include "../Manager/FontManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Object/EnemyBase.h"
#include "../Object/Enemy/EnemyCactus.h"
#include "../Object/Enemy/EnemyDog.h"
#include "../Object/Enemy/EnemyBoss.h"
#include "../Object/Planet.h"
#include "../Object/Flag/Flag.h"
#include "../Object/Flag/EnemyFlag.h"
#include "MiniMap.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
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
	imgOpeGear_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR).handleId_;

	pauseImg_ = LoadGraph("Data/Image/pause.png");

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE).handleId_;	//操作説明

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

	EnemyCreate(1);
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

	// 敵全滅情報をFlagに伝える
	flagManager_->Update(player_->GetTransform().pos, enemys_);

	SpawnTimer_ += FRAME_TIME; // 1フレーム = 1/60秒として加算

	if (SpawnTimer_ >= SPAWN_INTERVAL)
	{
		SpawnTimer_ = 0.0f; // リセット

		// ENEMY状態の旗を探す
		std::vector<FlagBase*> enemyFlags;
		int flagCount = flagManager_->GetFlagMax();
		for (int i = 0; i < flagCount; ++i)
		{
			FlagBase* flag = flagManager_->GetFlag(i);
			if (flag && flag->GetState() == Flag::STATE::ENEMY)
			{
				enemyFlags.push_back(flag);
			}
		}

		if (!enemyFlags.empty())
		{
			// ランダムで1つ選ぶ
			int randomIndex = GetRand((int)enemyFlags.size() - 1);
			FlagBase* targetFlag = enemyFlags[randomIndex];

			if (targetFlag)
			{
				VECTOR flagPos = targetFlag->GetPosition();
				
				// FlagのEnemyTypeをEnemyBase::TYPEに変換
				EnemyBase::TYPE type;
				switch (targetFlag->GetEnemyType())
				{
				case Flag::ENEMY_TYPE::DOG:   type = EnemyBase::TYPE::DOG; break;
				case Flag::ENEMY_TYPE::SABO: type = EnemyBase::TYPE::SABO; break;
				case Flag::ENEMY_TYPE::BOSS:  type = EnemyBase::TYPE::BOSS; break;
				default:                      type = EnemyBase::TYPE::DOG; break;
				}

				//数秒ごとにCactusを1体ずつ生成
				EnemyCreateRand(targetFlag->GetPosition(), 1, type);
			}
		}
	}


	SpawnCactus();

	// フラッグでクリアしたらシーン遷移
	int cleared = flagManager_->GetClearedFlagCount();
	int total = flagManager_->GetFlagMax();

	if (!bossSpawned_ && cleared >= total)
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

	//DrawMiniMap();

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
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_BG_ALPHA);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (pauseState_ == PauseState::Menu)
		{
			DrawRotaGraph((Application::SCREEN_SIZE_X / HALF_DIVISOR), UI_PAUSE_IMG_HEIGHT, PAUSE_IMG_UI_SIZE, 0, pauseImg_, true);
			
			FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", white, TITLE_FONT_SIZE);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 0)
				FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", yellow, TITLE_FONT_SIZE);

			FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", white, TITLE_FONT_SIZE);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == PAUSE_MENU_CONTROLS)
				FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", yellow, TITLE_FONT_SIZE);

			FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_3, "タイトルへ", white, TITLE_FONT_SIZE);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == PAUSE_MENU_TITLE)
				FontManager::DrawStringEx((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_3, "タイトルへ", yellow, TITLE_FONT_SIZE);
		}
		else if (pauseState_ == PauseState::ShowControls)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_WHITE_ALPHA);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[0], true);
			FontManager::DrawStringEx(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", yellow, ENTER_FONT_SIZE);
			if (cnt % FLASH * FLASH_RATE <= FLASH)FontManager::DrawStringEx(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white, ENTER_FONT_SIZE);
		}
		return;
	}
#pragma region UI
	FontManager::DrawStringEx(UI_ATTACK_X, UI_NORMAL_ATTACK_Y, "E:攻撃", white, ATTACK_FONT_SIZE);
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
	for (int i = 0; i < count; ++i)
	{
		VECTOR randPos = CUCTUS_POS;

		//EnemyDogを生成
		auto enemy = std::make_shared<EnemyCactus>();

		//生成された敵の初期化
		enemy->SetGameScene(this);
		enemy->SetPos(randPos);
		enemy->SetPlayer(player_);
		enemy->SetFlagManager(flagManager_.get());
		enemy->Init();

		//リストに追加
		enemys_.emplace_back(std::move(enemy));
	}
}

void GameScene::EnemyCreateRand(VECTOR flagPos, int count, EnemyBase::TYPE type)
{
	for (int i = 0; i < count; ++i)
	{
		VECTOR randPos = flagPos;
		randPos.x += GetRand(SPAWN_RADIUS * DOUBLE_MULTIPLIER) - SPAWN_RADIUS;
		randPos.z += GetRand(SPAWN_RADIUS * DOUBLE_MULTIPLIER) - SPAWN_RADIUS;

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

void GameScene::SpawnCactus(void)
{
	cactusSpawnTimer_ += FRAME_TIME; //1フレーム = 1/60秒として加算

	if (cactusSpawnTimer_ >= CACTUS_SPAWN_INTERVAL)
	{
		cactusSpawnTimer_ = 0.0f; // リセット

		// ENEMY状態の旗を探す
		std::vector<FlagBase*> enemyFlags;
		int flagCount = flagManager_->GetFlagMax();
		for (int i = 0; i < flagCount; ++i)
		{
			FlagBase* flag = flagManager_->GetFlag(i);
			if (flag && flag->GetState() == Flag::STATE::ENEMY)
			{
				enemyFlags.push_back(flag);
			}
		}

		if (!enemyFlags.empty())
		{
			// ランダムで1つ選ぶ
			int randomIndex = GetRand((int)enemyFlags.size() - 1);
			FlagBase* targetFlag = enemyFlags[randomIndex];

			if (targetFlag)
			{
				VECTOR flagPos = targetFlag->GetPosition();
				EnemyCreateRand(flagPos, 1, EnemyBase::TYPE::SABO);
			}
		}
	}
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
			case 2: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
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
