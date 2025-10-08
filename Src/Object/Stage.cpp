#include <vector>
#include <map>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "Player.h"
#include "Planet.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(Player& player) 
	: resMng_(ResourceManager::GetInstance()), player_(player)
{
	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;
}

Stage::~Stage(void)
{
	// 惑星
	planets_.clear();
}

void Stage::Init(void)
{
	MakeMainStage();
	MakeGoalStar();

	step_ = -1.0f;
}

void Stage::Update(void)
{
	// 重力範囲が重なっていた場合、惑星がコロコロ切り替わらないように
	// 一定時間ステージが変わらないようにする
	bool isPossibleChange = true;
	if (step_ > 0.0f)
	{
		step_ -= SceneManager::GetInstance().GetDeltaTime();
		isPossibleChange = false;
	}

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
		// 演習の実装箇所
		//①一定期間経過
		//②プレイヤーの座標が惑星の重力圏内に入った
		// InRangeGravity
		//③現在の惑星と異なる場合
		//④InRangeGravity関数内の実装(球体と点の衝突判定)
		if (isPossibleChange
			&& s.second->InRangeGravity(player_.GetTransform().pos)
			&& activeName_ != s.second->GetName()
			)
		{
			//次のステージへ遷移
			ChangeStage(s.second->GetName());

			//以降のループでステージが変わらないようにする
			isPossibleChange = false;
		}
	}
}

void Stage::Draw(void)const
{
	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}
}

void Stage::ChangeStage(NAME type)
{
	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_.ClearCollider();
	player_.AddCollider(activePlanet_.lock()->GetTransform().collider);


	// 重力制御に惑星を渡す
	GravityManager::GetInstance().ChangeActivePlanet(activePlanet_);
}

std::weak_ptr<Planet> Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

std::vector<VECTOR> Stage::GetPlanetsPositions() const
{
	std::vector<VECTOR> positions;
	for (const auto& p : planets_)
	{
		positions.push_back(p.second->GetTransform().pos);
	}
	return positions;
}

void Stage::MakeMainStage(void)
{
	// 最初の惑星
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.Load(ResourceManager::SRC::MAIN_PLANET).handleId_);
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { 0.0f, 0.0f, 0.0f };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET;
	std::shared_ptr<Planet> planet =
		std::make_shared<Planet>(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, std::move(planet));
	//------------------------------------------------------------------------------
}

void Stage::MakeGoalStar(void)
{
}

void Stage::MainStage(void)
{	
}
