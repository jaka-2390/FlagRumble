#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	std::unique_ptr<Resource> res;

	// UI画像系
	// -----------------------------------------------------------------
	// タイトル画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/GameTitle.png");
	resourcesMap_.emplace(SRC::TITLE, std::move(res));

	// タイトル背景
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "BackTitle.png");
	resourcesMap_.emplace(SRC::BACK_TITLE, std::move(res));

	// ゲーム画面のUI
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameUI1.png");
	resourcesMap_.emplace(SRC::GAMEUI_1, std::move(res));

	// タイトルに戻る
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GoTitle.png");
	resourcesMap_.emplace(SRC::GOTITLE, std::move(res));

	// ゲームに戻る
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "BackGame.png");
	resourcesMap_.emplace(SRC::GOGAME, std::move(res));

	// アイテム概要
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Item.png");
	resourcesMap_.emplace(SRC::ITEMTEACH, std::move(res));

	// 操作説明
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Operation.png");
	resourcesMap_.emplace(SRC::OPERATION, std::move(res));

	// ポーズの操作
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "PauseOpe.png");
	resourcesMap_.emplace(SRC::PAUSEOPE, std::move(res));

	// ポーズのアイテム
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "PauseItem.png");
	resourcesMap_.emplace(SRC::PAUSEITEM, std::move(res));

	// 設定表示
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "OpeGear.png");
	resourcesMap_.emplace(SRC::OPE_GEAR, std::move(res));

	// パワーアップ状態アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/PowerIcon.png");
	resourcesMap_.emplace(SRC::POWER_UP_ICON, std::move(res));

	// スピードアップ状態アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/SpeedIcon.png");
	resourcesMap_.emplace(SRC::SPEED_UP_ICON, std::move(res));

	// 回転切り使用可能状態アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/RotaAttackIcon.png");
	resourcesMap_.emplace(SRC::ROTA_ATTACK_ICON, std::move(res));

	// ゲームクリア
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Clear.png");
	resourcesMap_.emplace(SRC::GAMECLEAR, std::move(res));

	// こうして世界は生まれた
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "ClearWorld.png");
	resourcesMap_.emplace(SRC::CLEARWOLEDBORN, std::move(res));

	// 何かのキーを押して
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "PressKey.png");
	resourcesMap_.emplace(SRC::PRESS_KEY, std::move(res));

	// ゲームクリア背景
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "BackGameClear.png");
	resourcesMap_.emplace(SRC::BACK_GAMECLEAR, std::move(res));

	// ゲームオーバー
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Over/Over.png");
	resourcesMap_.emplace(SRC::GAMEOVER, std::move(res));

	// ユグドラシルは死んでしまった…
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Over/DieTree.png");
	resourcesMap_.emplace(SRC::DIETREE, std::move(res));

	// はい選択中
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "SelectYes.png");
	resourcesMap_.emplace(SRC::SELECT_YES, std::move(res));

	// いいえ選択中
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "SelectNo.png");
	resourcesMap_.emplace(SRC::SELECT_NO, std::move(res));

	// 吹き出し
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "SpeechBalloon.png");
	resourcesMap_.emplace(SRC::SPEECH_BALLOON, std::move(res));
	
	// プレイヤー
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "NPlayer/Player.mv1");
	resourcesMap_.emplace(SRC::PLAYER, std::move(res));

	// プレイヤー影
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, std::move(res));

	// ライト
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Light.png");
	resourcesMap_.emplace(SRC::LIGHT, std::move(res));

	// スカイドーム
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, std::move(res));

	// 最初の惑星
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Stage/Stage.mv1");
	resourcesMap_.emplace(SRC::MAIN_PLANET, std::move(res));

	// 足煙
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Smoke/Smoke.efkefc");
	resourcesMap_.emplace(SRC::FOOT_SMOKE, std::move(res));

	//アイテム
	// --------------------------------------------
	//水
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/bottle.mv1");
	resourcesMap_.emplace(SRC::WATER, std::move(res));
	
	//パワーアップ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/power.mv1");
	resourcesMap_.emplace(SRC::POWER, std::move(res));
	
	//スピードアップ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/speed.mv1");
	resourcesMap_.emplace(SRC::SPEED, std::move(res));
	
	//回復
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/heal.mv1");
	resourcesMap_.emplace(SRC::HEAL, std::move(res));
	
	//無敵
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/muteki.mv1");
	resourcesMap_.emplace(SRC::MUTEKI, std::move(res));
	
	//全部
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Item/all.mv1");
	resourcesMap_.emplace(SRC::ALL, std::move(res));

	//エネミー
	//----------------------------------
	//犬
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Yellow/Yellow.mv1");
	resourcesMap_.emplace(SRC::DOG, std::move(res));

	//サボテン
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Cactus/Cactus.mv1");
	resourcesMap_.emplace(SRC::SABO, std::move(res));

	//ミミック
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/mimic/mimic.mv1");
	resourcesMap_.emplace(SRC::MIMIC, std::move(res));

	//キノコ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/mushroom/mushroom.mv1");
	resourcesMap_.emplace(SRC::MUSH, std::move(res));

	//玉ねぎ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/onion/onion.mv1");
	resourcesMap_.emplace(SRC::ONION, std::move(res));//敵
	
	//トゲゾー
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/thorn/thorn.mv1");
	resourcesMap_.emplace(SRC::TOGE, std::move(res));

	//ウィルス
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/virus/virus.mv1");
	resourcesMap_.emplace(SRC::VIRUS, std::move(res));

	//ボス
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Boss/Boss.mv1");
	resourcesMap_.emplace(SRC::BOSS, std::move(res));

	//エフェクト
	//-------------------------

	//木の視野
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "TreeRange.efkefc");
	resourcesMap_.emplace(SRC::TREE_RANGE, std::move(res));
	
	//パワーアップ
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "PowerUp.efkefc");
	resourcesMap_.emplace(SRC::EFF_POWER, std::move(res));	

	//スピードアップ
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "SpeedUp.efkefc");
	resourcesMap_.emplace(SRC::EFF_SPEED, std::move(res));
	
	//回復
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "HealEffect.efkefc");
	resourcesMap_.emplace(SRC::EFF_HEAL, std::move(res));
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return lPair->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
