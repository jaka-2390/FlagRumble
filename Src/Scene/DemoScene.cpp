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
	//Enterの点滅用
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
	flagManager_->AddFlag(FLAG_POS, Flag::ENEMY_TYPE::DOG, Flag::STATE::ENEMY);

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

	if (ins.IsNew(KEY_INPUT_RETURN) ||
		ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		skipSecond_ += FRAME_TIME;	//1フレーム = 1/60秒
		skipActive_ = true;

		if (skipSecond_ >= SKIP_TIME)
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}
	else
	{
		skipSecond_ = 0.0f;		//離したらリセット
		skipActive_ = false;
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
		UpdateFinish();
		break;
	}

	if (spawnCactus_)
	{
		SpawnCactus();
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

	if (skipActive_)
	{
		float gaugeRate = skipSecond_ / SKIP_TIME;
		DrawSkip(Application::SCREEN_SIZE_X - SKIP_UI_OFFSET, Application::SCREEN_SIZE_Y - SKIP_UI_OFFSET, 1.0f, GAUGE_INNER_RADIUS, GAUGE_OUTER_RADIUS, gray);

		DrawSkip(Application::SCREEN_SIZE_X - SKIP_UI_OFFSET, Application::SCREEN_SIZE_Y - SKIP_UI_OFFSET, gaugeRate, GAUGE_INNER_RADIUS, GAUGE_OUTER_RADIUS, red);

		DrawString(Application::SCREEN_SIZE_X - SKIP_TEX_OFSET_X, Application::SCREEN_SIZE_Y - SKIP_TEX_OFSET_Y, "Skip", white);
	}

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
		DrawPause();
	}
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
	// 入力があれば進むフラグ
	bool moved = false;

	if (GetJoypadNum() > 0)
	{
		// ゲームパッド操作
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// スティックが一定以上動いていたら移動入力と判定
		if (fabs(padState.AKeyLX) > DEAD_ZONE || fabs(padState.AKeyLY) > DEAD_ZONE)
		{
			moved = true;
		}
	}
	else
	{
		if (CheckHitKey(KEY_INPUT_W) ||
			CheckHitKey(KEY_INPUT_A) ||
			CheckHitKey(KEY_INPUT_S) ||
			CheckHitKey(KEY_INPUT_D)) {

			moved = true;
		}
	}

	// 何か入力があったら次へ
	if (moved)
	{
		MessageTime();

		if (!messageActive_)
		{
			messageActive_ = true;
			state_ = STATE::ATTACK;
		}
	}
}

void DemoScene::UpdateAttack()
{
	//Dogがいなかったら生成
	if (!isDog_)
	{
		EnemyCreate(DOG_POS, 1, EnemyBase::TYPE::DOG); // 各フラッグの周囲に1体
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
		MessageTime();

		if (!messageActive_)
		{
			messageActive_ = true;
			state_ = STATE::FLAG;
		}
	}
}

void DemoScene::UpdateFlag()
{
	flagManager_->Update(player_->GetTransform().pos, {}); // 敵なし

	// 旗ゲージが規定値に達したらクリア
	if (flagManager_->GetClearedFlagCount() >= 1) 
	{
		MessageTime();

		if (!messageActive_)
		{
			messageActive_ = true;
			state_ = STATE::SABO;
		}
	}
}

void DemoScene::UpdateSabo()
{
	spawnCactus_ = true;

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

	if (spawnCactus_ && allDead) 
	{
		MessageTime();

		if (!messageActive_)
		{
			messageActive_ = true;
			state_ = STATE::FINISH;
		}
	}
}

void DemoScene::UpdateFinish()
{
	if (ins.IsNew(KEY_INPUT_RETURN) || 
		ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void DemoScene::DrawPause()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_BG_ALPHA);
	DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (pauseState_ == PauseState::Menu)
	{
		DrawRotaGraph((Application::SCREEN_SIZE_X / HALF_DIVISOR), UI_PAUSE_IMG_HEIGHT, PAUSE_IMG_UI_SIZE, 0, pauseImg_, true);
		SetFontSize(DEFAULT_FONT_SIZE * TITLE_FONT_SCALE);

		DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_4, UI_HEIGHT_PAUSE_1, "チュートリアルに戻る", white);
		if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 0)
			DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_4, UI_HEIGHT_PAUSE_1, "チュートリアルに戻る", yellow);

		DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", white);
		if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == PAUSE_MENU_CONTROLS)
			DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", yellow);

		DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_3, "タイトルへ", white);
		if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == PAUSE_MENU_TITLE)
			DrawString((Application::SCREEN_SIZE_X / HALF_DIVISOR) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_3, "タイトルへ", yellow);

		SetFontSize(DEFAULT_FONT_SIZE);
	}
	else if (pauseState_ == PauseState::ShowControls || pauseState_ == PauseState::ShowItems)
	{
		//白い背景
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, PAUSE_WHITE_ALPHA);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//操作説明かどうか
		int imgIndex = (pauseState_ == PauseState::ShowControls) ? 0 : 1;
		DrawGraph(0, 0, pauseExplainImgs_[imgIndex], true);

		//文字を黄色に点滅
		SetFontSize(DEFAULT_FONT_SIZE * ENTER_FONT_SCALE);
		DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", yellow);
		if (cnt % FLASH * FLASH_RATE <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white);
		SetFontSize(DEFAULT_FONT_SIZE);
	}
	return;
}

void DemoScene::DrawMessage()
{
	bool isPad = (GetJoypadNum() > 0);

	int x = MESSAGE_POS_X;
	int y = MESSAGE_POS_Y;

#pragma region UI
	SetFontSize(DEFAULT_FONT_SIZE * MESSAGE_FONT_SCALE);
	switch (state_) {

	case STATE::MOVE:
		if (isPad)
			DrawString(x, y, "左スティックで移動してみよう！", white);
		else
			DrawString(x, y, "WASD で移動してみよう！", white);
		break;

	case STATE::ATTACK:
		DrawString(x, y, "黄色い敵は陣地を守っています", white);
		if (isPad)
			DrawString(x, y + MESSAGE_OFFSET, "Y ボタンで攻撃して倒してみよう！", white);
		else
			DrawString(x, y + MESSAGE_OFFSET, "E キーで攻撃して倒してみよう！", white);
		break;

	case STATE::FLAG:
		DrawString(x, y, "旗に近づいて陣地を奪還してみよう！", white);
		DrawString(x, y + MESSAGE_OFFSET, "ゲージが溜まり旗が青になると奪還成功です", white);
		break;
		
	case STATE::SABO:
		DrawString(x, y, "緑の敵は、あなたの陣地を奪いに来る敵です", white);
		DrawString(x, y + MESSAGE_OFFSET, "奪われないように倒しましょう！", white);
		break;

	case STATE::FINISH:
		DrawString(x, y, "チュートリアル完了！", white);
		if (isPad)
			DrawString(x, y + MESSAGE_OFFSET, "A ボタンでゲーム開始", white);
		else
			DrawString(x, y + MESSAGE_OFFSET, "Enter でゲーム開始", white);
		break;
	}
	SetFontSize(DEFAULT_FONT_SIZE);
#pragma endregion
}

void DemoScene::DrawSkip(int cx, int cy, float rate, int rOuter, int rInner, int color)
{
	const int segments = CIRCLE_SEGMENTS;		//円を分割する数
	const float angleMax = rate * DX_TWO_PI_F;	//0から2πの角度計算

	for (int i = 0; i < segments; i++)
	{
		float a1 = angleMax * (float)i / segments;
		float a2 = angleMax * (float)(i + 1) / segments;

		// 外側円弧の2点
		float x1o = cx + sinf(a1) * rOuter;
		float y1o = cy - cosf(a1) * rOuter;
		float x2o = cx + sinf(a2) * rOuter;
		float y2o = cy - cosf(a2) * rOuter;

		// 内側円弧の2点
		float x1i = cx + sinf(a1) * rInner;
		float y1i = cy - cosf(a1) * rInner;
		float x2i = cx + sinf(a2) * rInner;
		float y2i = cy - cosf(a2) * rInner;

		// 四角形（外円弧と内円弧の間）を三角形２つで描画
		DrawTriangle(x1i, y1i, x2i, y2i, x1o, y1o, color, TRUE);
		DrawTriangle(x2i, y2i, x2o, y2o, x1o, y1o, color, TRUE);
	}
}

void DemoScene::EnemyCreate(VECTOR flagPos, int count, EnemyBase::TYPE type)
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

	EnemyCreate(CACTUS_POS, 1, EnemyBase::TYPE::SABO);
	
}

void DemoScene::MessageTime(void)
{
	if (messageActive_)
	{
		messageTimer_ += FRAME_TIME; // 1フレーム = 1/60秒
		if (messageTimer_ >= MESSAGE_DISPLAY_SEC)
		{
			messageActive_ = false; // メッセージ終了
			messageTimer_ = 0.0f;
		}
	}
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
