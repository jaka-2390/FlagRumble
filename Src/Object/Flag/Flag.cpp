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

	circleVisible_ = true;
	flagVisible_ = false;
	flagClear_ = false;
	enemySpawned_ = false;
	playerInRange_ = false;
	clearGauge_ = 0.0f;
	clearGaugeMax_ = 100.0f;
	flagRadius_ = 100.0f;
}

void Flag::Update(const VECTOR& playerPos, bool allEnemyDefeated)
{
	CheckCircle(playerPos, allEnemyDefeated);
}

void Flag::Draw(void)
{
	// 円を表示
	if (circleVisible_)
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(255, 0, 0));
		DrawGauge3D(pos_, clearGauge_ / clearGaugeMax_);
	}

	// フラッグを表示
	if (flagVisible_)
	{
		DrawCircleOnMap(pos_, flagRadius_, GetColor(0, 255, 0));
		/*MV1SetScale(modelIdB_, scl_);
		MV1SetRotationXYZ(modelIdB_, rot_);
		MV1SetPosition(modelIdB_, pos_);
		MV1DrawModel(modelIdB_);*/
	}
}

void Flag::CheckCircle(const VECTOR& playerPos, bool allEnemyDefeated)
{
	if (flagClear_) return;

	// 敵全滅したら円を出現
	if (allEnemyDefeated && !circleVisible_)
	{
		circleVisible_ = true;
	}

	// 円の中心とプレイヤーの距離を計算
	float dx = playerPos.x - pos_.x;
	float dz = playerPos.z - pos_.z;
	float distSq = dx * dx + dz * dz;
	bool inRange = (distSq < flagRadius_* flagRadius_);

	// フラッグ出現前（円の中でゲージを貯める）
	if (allEnemyDefeated && !flagVisible_)
	{
		if (inRange)
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

bool Flag::IsFlagClear() const
{
	return flagClear_;
}

VECTOR Flag::GetPosition() const
{
	return pos_;
}

bool Flag::SpawnEnemies(const VECTOR& playerPos) const
{
	if (enemySpawned_) return false;  // もう出してる

	float dx = playerPos.x - pos_.x;
	float dz = playerPos.z - pos_.z;
	float distSq = dx * dx + dz * dz;

	return (distSq < flagRadius_* flagRadius_);
}

void Flag::SetEnemySpawned(bool spawned)
{
	enemySpawned_ = spawned;
}
