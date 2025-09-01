#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/ResourceManager.h"
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
#include "../Object/Tree.h"
#include "../Object/Planet.h"
#include "../Object/Item.h"
#include "MiniMap.h"
#include "GameScene.h"

//担当いけだ

GameScene::GameScene(void)
{
	player_ = nullptr;
	tree_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgGameUi1_ = -1;
	imgOpeGear_ = -1;
	//item_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	cnt = 0;
	// プレイヤー
	player_ = std::make_shared<Player>();
	GravityManager::GetInstance().SetPlayer(player_);
	player_->Init();

	//木
	tree_ = std::make_shared<Tree>();
	tree_->Init();
	tree_->SetPlayer(player_.get());

	// プレイヤーにTreeを渡す(条件付き攻撃用)
	player_->SetTree(tree_.get());

	// 敵のモデル
	EnemyCreate();

	player_->SetEnemy(&enemys_);

	// ステージ
	stage_ = std::make_unique<Stage>(*player_);
	stage_->Init();

	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	map_ = std::make_unique<MiniMap>(30000.0f, 300);
	map_->Init();

	// 画像
	imgGameUi1_ = resMng_.Load(ResourceManager::SRC::GAMEUI_1).handleId_;
	imgOpeGear_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR).handleId_;

	pauseImg_ = LoadGraph("Data/Image/pause.png");

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE).handleId_; // 操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEITEM).handleId_;   // アイテム概要

	// カウンタ
	uiFadeStart_ = false;
	uiFadeFrame_ = 0;
	uiDisplayFrame_ = 0;

	// ポーズ
	isPaused_ = false;
	pauseSelectIndex_ = 0;

	// カメラのポーズ解除
	camera_ = SceneManager::GetInstance().GetCamera().lock();
	if (camera_) {
		camera_->SetPaused(false); // ← ここが重要！

		//ミニマップ用カメラ
		camera_->SetFollow(&player_->GetTransform());
		camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_BGM, Sound::TIMES::LOOP);

	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);

	isB_ = 0;
}

void GameScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	// TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB)) 
	{
		if (pauseState_ == PauseState::Menu) 
		{
			isPaused_ = !isPaused_;
			pauseSelectIndex_ = 0;

			// カメラのポーズ切り替え
			mainCamera->SetPaused(isPaused_);
		}
		return; // TABを押したら他の処理はしない
	}

	// -------------------------
	// ポーズ中：ゲームロジック停止、メニューだけ操作可
	// -------------------------
	if (isPaused_) 
	{
		if (pauseState_ == PauseState::Menu) 
		{
			if (ins.IsTrgDown(KEY_INPUT_DOWN)) 
			{
				pauseSelectIndex_ = (pauseSelectIndex_ + 1) % 4;
			}
			if (ins.IsTrgDown(KEY_INPUT_UP)) 
			{
				pauseSelectIndex_ = (pauseSelectIndex_ + 3) % 4;
			}
			if (ins.IsTrgDown(KEY_INPUT_RETURN)) 
			{
				switch (pauseSelectIndex_)
				{
				case 0: // ゲームに戻る
					isPaused_ = false;
					pauseState_ = PauseState::Menu;  // ← 必須
					mainCamera->SetPaused(false);    // カメラ復帰も忘れずに
					break;
				case 1:
					pauseState_ = PauseState::ShowControls;
					break;
				case 2:
					pauseState_ = PauseState::ShowItems;
					break;
				case 3:
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
					break;
				}
			}
		}
		else {
			// 操作説明 or アイテム概要画面中 → Enterで戻る
			if (ins.IsTrgDown(KEY_INPUT_RETURN)) 
			{
				pauseState_ = PauseState::Menu;
			}
		}
		return;
	}

	// -------------------------
	// 通常時のゲーム進行（ポーズされてないときだけ）
	// -------------------------

	uiDisplayFrame_++;

	if (tree_->GetLv() >= 75 && isB_ == 0)
	{
		isB_ = 1;
		EnemyCreate();
		isB_ = 2;
	}
	if (tree_->GetLv() >= 100) 
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
	if (tree_->GetHp() <= 0) 
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
	}

	skyDome_->Update();
	stage_->Update();
	tree_->Update();
	player_->Update();

	for (auto& item : items_) 
	{
		item->Update();
	}
	for (auto enemy : enemys_) 
	{
		enemy->Update();
	}

	// 敵のエンカウント処理
	enCounter++;
	if (enCounter > ENCOUNT) 
	{
		enCounter = 0;
		if (ENEMY_MAX >= enemys_.size()) 
		{
			EnemyCreate();
		}
	}

	//if (!unlockedQ && tree_->GetLv() >= 25) {
	//	unlockedQ = true;
	//	showQFlash = true;
	//	qUnlockTime = GetNowCount();  // 現在時刻（ミリ秒）を記録
	//}
}

void GameScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	for (auto& item : items_)
	{
		item->Draw();
	}
	for (auto enemy : enemys_)
	{
		enemy->Draw();
	}
	tree_->Draw();
	player_->Draw();

	for (auto enemy : enemys_)
	{
		enemy->DrawBossHpBar();
	}

	DrawMiniMap();
	
	DrawRotaGraph(100, 100, 0.8, 0.0, imgOpeGear_, true);

	// 入力チェック or 時間経過でフェード開始
	if (!uiFadeStart_) 
	{
		if ((CheckHitKey(KEY_INPUT_W)
			|| CheckHitKey(KEY_INPUT_A)
			|| CheckHitKey(KEY_INPUT_S)
			|| CheckHitKey(KEY_INPUT_D))
			|| uiDisplayFrame_ >= 240)  // 時間経過による自動フェード
		{
			uiFadeStart_ = true;
			uiFadeFrame_ = 0;
		}
	}
	if (!uiFadeStart_) 
	{
		// フェード前（通常表示）
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawRotaGraph(Application::SCREEN_SIZE_X/2,80,0.5,0,imgGameUi1_,true);
		//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else if (uiFadeFrame_ < 60) 
	{
		// フェード中（60フレームで徐々に消す）
		int alpha = static_cast<int>(255 * (60 - uiFadeFrame_) / 60.0f);
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRotaGraph(Application::SCREEN_SIZE_X/2,80,0.5,0,imgGameUi1_,true);
		//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		uiFadeFrame_++;
	}

	if (isPaused_) 
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (pauseState_ == PauseState::Menu)
		{
			DrawRotaGraph(Application::SCREEN_SIZE_X/2,150,0.65,0,pauseImg_,true);
			SetFontSize(80);
			DrawString(Application::SCREEN_SIZE_X/2-80*3,350,"ゲームに戻る",0xffffff);
			if(pauseSelectIndex_%4==0)DrawString(Application::SCREEN_SIZE_X/2-80*3,350,"ゲームに戻る",0xffff00);
			DrawString(Application::SCREEN_SIZE_X/2-80*2,470,"操作説明",0xffffff);
			if(pauseSelectIndex_%4==1)DrawString(Application::SCREEN_SIZE_X/2-80*2,470,"操作説明",0xffff00);
			DrawString(Application::SCREEN_SIZE_X/2-80*3,590,"アイテム概要",0xffffff);
			if(pauseSelectIndex_%4==2)DrawString(Application::SCREEN_SIZE_X/2-80*3,590,"アイテム概要",0xffff00);
			DrawString(Application::SCREEN_SIZE_X/2-80*2.5,710,"タイトルへ",0xffffff);
			if(pauseSelectIndex_%4==3)DrawString(Application::SCREEN_SIZE_X/2-80*2.5,710,"タイトルへ",0xffff00);
			SetFontSize(16);
		}
		else if (pauseState_ == PauseState::ShowControls) 
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, 1920, 1080,0xffffff,true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[0], true);
			SetFontSize(40);
			DrawString(1600, 1020,"Enterキーで戻る",0xffff00);
			if(cnt%90 <=45)DrawString(1600, 1020,"Enterキーで戻る",0xffffff);
			SetFontSize(16);
		}
		else if (pauseState_ == PauseState::ShowItems) 
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, 1920, 1080,0xffffff,true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[1], true);
			SetFontSize(40);
			DrawString(1600, 1020, "Enterキーで戻る", 0xffff00);
			if (cnt % 90 <= 45)DrawString(1600, 1020, "Enterキーで戻る", 0xffffff);
			SetFontSize(16);
		}
		return;
	}

#pragma region UI
	SetFontSize(32);
	DrawString(10,450,"E:通常攻撃"      ,0xffffff);
	if (tree_->GetLv()>=25)
	{
		DrawString(10, 500, "Q:なぎ払い", 0xffffff);

		if (showQFlash)
		{
			int now = GetNowCount();
			int elapsed = now - qUnlockTime;

			if (elapsed >= 3000)
			{
				showQFlash = false;
			}
			else
			{
				int flashColor = ((elapsed / 400) % 2 == 0) ? 0xff0000 : 0xffffff;

				int x = 10 + GetDrawStringWidth("Q:なぎ払い", strlenDx("Q:なぎ払い"));
				DrawString(x, 500, " 解放", flashColor);
			}
		}
	}
	if (tree_->GetLv()>=50)	DrawString(10,550,"R:回転斬り　解放",0xffffff);
	SetFontSize(16);
#pragma endregion
}

void GameScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAME_BGM);
}

void GameScene::DrawMiniMap(void)
{
	if (!map_) return;

	// プレイヤーの座標
	MapVector2 playerPos;
	playerPos.x = player_->GetTransform().pos.x;
	playerPos.z = player_->GetTransform().pos.z;
	// Y軸回転角を使用(ラジアン or 度数)
	float playerAngle = player_->GetTransform().rot.y;

	float cameraAngleRad = 0.0f;
	if (camera_) {
		cameraAngleRad = camera_->GetAngles().y;  // ここ！
	}

	// 敵の座標リストを作成
	std::vector<std::shared_ptr<EnemyBase>> aliveEnemies;
	for (const auto& enemy : enemys_)
	{
		if (enemy->IsAlive())
		{
			aliveEnemies.push_back(enemy);
		}
	}

	// アイテムの座標リストを作成
	std::vector<MapVector2> itemPositions;
	for (const auto& item : items_)
	{
		if (item->GetIsAlive())
		{
			MapVector2 i;
			i.x = item->GetTransform().pos.x;
			i.z = item->GetTransform().pos.z;
			itemPositions.push_back(i);
		}
	}

	// ミニマップ描画呼び出し
	map_->Draw(playerPos, playerAngle, cameraAngleRad, aliveEnemies, itemPositions);
}

void GameScene::AddItem(std::shared_ptr<Item> item)
{
	items_.push_back(item);
}

std::shared_ptr<Item> GameScene::CreateItem(const VECTOR& spawnPos, float scale, Item::TYPE itemType)
{
	// 現在のアクティブ（生きている）アイテム数を数える
	int aliveCount = 0;
	for (const auto& item : items_) {
		if (item->GetIsAlive()) {
			aliveCount++;
		}
	}

	// 再利用可能なアイテムを探す
	if (itemType == Item::TYPE::WATER)
	{
		for (auto& item : items_) {
			if (!item->GetIsAlive() && item->GetItemType() == Item::TYPE::WATER) {
				OutputDebugStringA("再利用アイテムを使用\n");
				item->Respawn(spawnPos);
				item->SetScale(scale);
				return item;
			}
		}
	}

	// 再利用できなければ新しく作成
	OutputDebugStringA("新規アイテムを作成\n");
	auto newItem = std::make_shared<Item>(*player_, Transform{}, itemType,*
		tree_);
	newItem->Init(); // 初期化（モデル読み込み等）
	newItem->Respawn(spawnPos);
	newItem->SetScale(scale);
	items_.push_back(newItem);
	return newItem;
}

const std::vector<std::shared_ptr<EnemyBase>>& GameScene::GetEnemies() const
{
	return enemys_;
}

void GameScene::EnemyCreate(void)
{
	int randDir = GetRand(3);
	VECTOR randPos = VGet(0.0f, 0.0f, 0.0f);
	switch (randDir)//位置
	{
	case 0://前
		randPos.x = GetRand(20000) - 10000;
		randPos.z = 10000;
		break;
	case 1://後
		randPos.x = GetRand(20000) - 10000;
		randPos.z = -10000;
		break;
	case 2://左
		randPos.x = -10000;
		randPos.z = GetRand(20000) - 10000;
		break;
	case 3://右
		randPos.x = 10000;
		randPos.z = GetRand(29000) - 10000;
		break;
	default:
		break;
	}

	std::shared_ptr<EnemyBase> enemy;

	//敵のtype
	if(isB_==1)
	{
		EnemyBase::TYPE type_ = static_cast<EnemyBase::TYPE>(EnemyBase::TYPE::BOSS);
		enemy = std::make_shared<EnemyBoss>();
	}
	else
	{
		EnemyBase::TYPE type_ = static_cast<EnemyBase::TYPE>(GetRand(static_cast<int>(EnemyBase::TYPE::MAX) - 2));
		switch (type_)
		{
		case EnemyBase::TYPE::SABO:
			enemy = std::make_shared<EnemyCactus>();
			break;
		case EnemyBase::TYPE::DOG:
			enemy = std::make_shared<EnemyDog>();
			break;
		case EnemyBase::TYPE::MIMIC:
			enemy = std::make_shared<EnemyMimic>();
			break;
		case EnemyBase::TYPE::MUSH:
			enemy = std::make_shared<EnemyMushroom>();
			break;
		case EnemyBase::TYPE::ONION:
			enemy = std::make_shared<EnemyOnion>();
			break;
		case EnemyBase::TYPE::TOGE:
			enemy = std::make_shared<EnemyThorn>();
			break;
		case EnemyBase::TYPE::VIRUS:
			enemy = std::make_shared<EnemyVirus>();
			break;
		default:
			enemy = std::make_shared<EnemyCactus>();
			break;
		}
	}

	// 生成された敵の初期化
	enemy->SetGameScene(this);
	enemy->SetPos(randPos);
	enemy->SetPlayer(player_);
	enemy->SetTree(tree_);
	enemy->Init();

	enemys_.emplace_back(std::move(enemy));
}