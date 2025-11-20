#include <DxLib.h>
#include<EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include"../../Application.h"
#include "Flag.h"

Flag::Flag(VECTOR pos, ENEMY_TYPE type):pos_(pos), enemyType_(type)
{
	//エフェクト
	effectEnemyAreaPlayId_ = 0;
	effectEnemyAreaPlayId_ = 0;

	effectPlayerAreaPlayId_ = 0;
	effectPlayerAreaPlayId_ = 0;
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
	else if (flagVisible_ && IsOwnedByPlayer())
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(0, 255, 0));
		/*MV1SetScale(modelIdB_, scl_);
		MV1SetRotationXYZ(modelIdB_, rot_);
		MV1SetPosition(modelIdB_, pos_);
		MV1DrawModel(modelIdB_);*/
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
			flagVisible_ = true; //フラッグ登場
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

void Flag::DrawCircleOnMap(VECTOR center, float radius, int color)
{
	const int div = 36;

	for (int i = 0; i < div; i++)
	{
		float t1 = (float)i / div;
		float t2 = (float)(i + 1) / div;
		float x1 = center.x + cosf(t1 * 2.0f * DX_PI_F) * radius;
		float z1 = center.z + sinf(t1 * 2.0f * DX_PI_F) * radius;
		float x2 = center.x + cosf(t2 * 2.0f * DX_PI_F) * radius;
		float z2 = center.z + sinf(t2 * 2.0f * DX_PI_F) * radius;

		DrawLine3D(VGet(x1, center.y, z1), VGet(x2, center.y, z2), color);
	}
}

void Flag::DrawGauge3D(VECTOR center, float gaugeRate)
{
	// ゲージバーを円の上（少し高い位置）に表示
	VECTOR gaugePos = VAdd(center, VGet(0.0f, 80.0f, 0.0f)); // 円より上
	VECTOR screenPos = ConvWorldPosToScreenPos(gaugePos);

	int barWidth = 100;
	int barHeight = 10;

	int x = (int)screenPos.x - barWidth / 2;
	int y = (int)screenPos.y - 20; // 少し上にオフセット

	// 外枠
	DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, GetColor(255, 255, 255), FALSE);

	// 中身
	int fillWidth = (int)(barWidth * gaugeRate);
	DrawBox(x, y, x + fillWidth, y + barHeight, GetColor(0, 255, 0), TRUE);
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
