#include <DxLib.h>
#include"../Application.h"
#include "Flag.h"

Flag::Flag(void)
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
	//pos_ = { 2300.0f, 254.0f, 445.0f };						//位置
	pos_ = { -660.0f, 254.0f, -100.0f };

	flagAlive_ = false;
	circleVisible_ = false;
	flagVisible_ = false;
	gameClear_ = false;
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
		DrawCircleOnMap(pos_, flagRadius_, GetColor(0, 255, 0));
		DrawGauge3D(pos_, clearGauge_ / clearGaugeMax_);
	}

	// フラッグを表示
	if (flagVisible_)
	{
		MV1SetScale(modelIdB_, scl_);
		MV1SetRotationXYZ(modelIdB_, rot_);
		MV1SetPosition(modelIdB_, pos_);
		MV1DrawModel(modelIdB_);
	}
}

void Flag::CheckCircle(const VECTOR& playerPos, bool allEnemyDefeated)
{
	if (gameClear_) return;

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
	if (circleVisible_ && !flagVisible_)
	{
		if (inRange)
		{
			clearGauge_ += 0.5f;
			if (clearGauge_ >= clearGaugeMax_)
			{
				clearGauge_ = clearGaugeMax_;
				flagVisible_ = true; // フラッグ登場！
			}
		}
		else
		{
			clearGauge_ -= 0.2f; // 離れると少し減るなども可能
			if (clearGauge_ < 0.0f) clearGauge_ = 0.0f;
		}
	}

	// フラッグ出現後（円の中で再度ゲージを満タンにするとクリア）
	else if (flagVisible_ && !gameClear_)
	{
		if (inRange)
		{
			clearGauge_ += 0.5f;
			if (clearGauge_ >= clearGaugeMax_) // 2段階目
			{
				clearGauge_ = clearGaugeMax_;
				gameClear_ = true;
			}
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

bool Flag::IsGameClear() const
{
	return gameClear_;
}

VECTOR Flag::GetPosition() const
{
	return pos_;
}
