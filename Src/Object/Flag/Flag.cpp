#include <DxLib.h>
#include"../../Application.h"
#include "Flag.h"

Flag::Flag(VECTOR pos):pos_(pos)
{
}

Flag::~Flag(void)
{
}

void Flag::Init(void)
{
	//モデルの読込
	modelIdB_ = MV1LoadModel((Application::PATH_MODEL + "wood/Baby.mv1").c_str());

	scl_ = { 3.0f, 2.5f, 3.0f };							//大きさ
	rot_ = { 0.0f, 0.0f * DX_PI_F / 180.0f, 0.0f };			//回転

	flagVisible_ = false;
	flagClear_ = false;
	enemySpawned_ = false;
	playerInRange_ = false;
	enemyNear_ = false;
	clearGauge_ = 0.0f;
	clearGaugeMax_ = 100.0f;
	flagRadius_ = 100.0f;
	enemyCheckRadius_ = 500.0f;
}

void Flag::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	CheckCircle(playerPos, enemies);
}

void Flag::Draw(void)
{
	// 円を表示
	DrawCircleOnMap(pos_, flagRadius_, GetColor(255, 0, 0));
		
	// フラッグを表示
	if (flagVisible_)
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(0, 255, 0));
		/*MV1SetScale(modelIdB_, scl_);
		MV1SetRotationXYZ(modelIdB_, rot_);
		MV1SetPosition(modelIdB_, pos_);
		MV1DrawModel(modelIdB_);*/
	}
	else if(!enemyNear_)
	{
		DrawGauge3D(pos_, clearGauge_ / clearGaugeMax_);
	}
}

void Flag::CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	if (flagClear_) return;

	// 円の中心とプレイヤーの距離を計算
	playerInRange_ = false;
	if (DistanceSqXZ(playerPos, pos_) < flagRadius_ * flagRadius_)
	{
		playerInRange_ = true;
	}

	//円の近くに敵がいるか
	enemyNear_ = false;
	for (auto& enemy : enemies)
	{
		if (!enemy) continue;
		if (!enemy->IsAlive()) continue;
		VECTOR ePos = enemy->GetTransform().pos;

		if (DistanceSqXZ(ePos, pos_) < enemyCheckRadius_ * enemyCheckRadius_)
		{
			enemyNear_ = true;
			break;
		}

	}

	//ゲージを貯める
	if (!enemyNear_ && playerInRange_)
	{
		clearGauge_ += 0.5f;
		if (clearGauge_ >= clearGaugeMax_)
		{
			clearGauge_ = clearGaugeMax_;
			flagVisible_ = true; //フラッグ登場
			flagClear_ = true;
		}
	}
	else
	{
		clearGauge_ -= 0.2f; // 離れると少し減るなども可能
		if (clearGauge_ < 0.0f) clearGauge_ = 0.0f;
	}
}

float Flag::DistanceSqXZ(const VECTOR& a, const VECTOR& b) const
{
	float dx = a.x - b.x;
	float dz = a.z - b.z;
	return  dx * dx + dz * dz;
}

bool Flag::SpawnEnemies(const VECTOR& playerPos) const
{
	if (enemySpawned_) return false;  //もう出してる

	float dx = playerPos.x - pos_.x;
	float dz = playerPos.z - pos_.z;
	float distSq = dx * dx + dz * dz;

	return (distSq < flagRadius_* flagRadius_);
}

void Flag::SetEnemySpawned(bool spawned)
{
	enemySpawned_ = spawned;
}
