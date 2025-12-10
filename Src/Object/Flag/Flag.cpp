#include <DxLib.h>
#include<EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
#include"../../Application.h"
#include "Flag.h"

Flag::Flag(VECTOR pos, ENEMY_TYPE type, STATE state):pos_(pos), enemyType_(type), state_(state), resMng_(ResourceManager::GetInstance())
{
	//ResourceManagerから複製モデルを取得
	flag_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::FLAG));

	//エフェクト
	effectEnemyAreaPlayId_ = 0;
	effectEnemyAreaPlayId_ = 0;

	effectPlayerAreaPlayId_ = 0;
	effectPlayerAreaPlayId_ = 0;
}

Flag::~Flag(void)
{
	//モデルの削除
	MV1DeleteModel(flag_.modelId);
}

void Flag::Init(void)
{
	//旗
	flag_.pos = { pos_.x, pos_.y, pos_.z };
	flag_.scl = { 2.0f, 2.0f, 2.0f };
	flag_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	flag_.Update();

	enemySpawned_ = false;
	playerInRange_ = false;
	enemyNear_ = false;
	clearGauge_ = 0.0f;
	clearGaugeMax_ = 100.0f;
	flagRadius_ = 100.0f;
	enemyCheckRadius_ = 500.0f;

	//エフェクト
	effectEnemyAreaResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::ENEMY_AREA).handleId_;
	effectPlayerAreaResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER_AREA).handleId_;
}

void Flag::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	CheckCircle(playerPos, enemies);

	EffectAreaRange();

	if (enemyNear_)
	{
		state_ = STATE::ENEMY;
	}
	else if(!enemyNear_ && IsOwnedByPlayer())
	{
		state_ = STATE::PLAYER;
	}
}

void Flag::Draw(void)
{
	// 円を表示
	if (IsOwnedByEnemy())
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(255, 0, 0));

	}
	else if (IsOwnedByPlayer())
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(0, 255, 0));

		MV1SetScale(flag_.modelId, scl_);
		MV1SetRotationXYZ(flag_.modelId, rot_);
		MV1SetPosition(flag_.modelId, pos_);
		MV1DrawModel(flag_.modelId);
	}
	
	if(!enemyNear_ && IsOwnedByEnemy())
	{
		DrawGauge3D(pos_, clearGauge_ / clearGaugeMax_);
	}
}

void Flag::CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	if (IsOwnedByPlayer()) return;

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
			state_ = STATE::PLAYER;
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

void Flag::EffectAreaRange(void)
{
	if (IsOwnedByEnemy())
	{
		if (effectEnemyAreaPlayId_ >= 0)
		{
			StopEffekseer3DEffect(effectEnemyAreaPlayId_);
		}

		float scale = 300.0f;  // デフォルト値

		effectEnemyAreaPlayId_ = PlayEffekseer3DEffect(effectEnemyAreaResId_);
		SetScalePlayingEffekseer3DEffect(effectEnemyAreaPlayId_, scale, scale / 2, scale);
		SetPosPlayingEffekseer3DEffect(effectEnemyAreaPlayId_, pos_.x, pos_.y, pos_.z);
	}
	else if (IsOwnedByPlayer())
	{
		if (effectPlayerAreaPlayId_ >= 0)
		{
			StopEffekseer3DEffect(effectPlayerAreaPlayId_);
		}

		float scale = 300.0f;  // デフォルト値

		effectPlayerAreaPlayId_ = PlayEffekseer3DEffect(effectPlayerAreaResId_);
		SetScalePlayingEffekseer3DEffect(effectPlayerAreaPlayId_, scale, scale / 2, scale);
		SetPosPlayingEffekseer3DEffect(effectPlayerAreaPlayId_, pos_.x, pos_.y, pos_.z);
	}
}

VECTOR Flag::GetPosition() const
{
	return pos_;
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
