#include<EffekseerForDXLib.h>
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Utility/AsoUtility.h"
#include "FlagBase.h"

FlagBase::FlagBase(VECTOR pos, ENEMY_TYPE type, STATE state) : 
	pos_(pos), enemyType_(type), state_(state), scnMng_(SceneManager::GetInstance())
{
	//ResourceManagerから複製モデルを取得
	flag_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::FLAG));
	pflag_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::PFLAG));
	nflag_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::NFLAG));

	//エフェクト
	effectEnemyAreaPlayId_ = 0;
	effectEnemyAreaPlayId_ = 0;

	effectPlayerAreaPlayId_ = 0;
	effectPlayerAreaPlayId_ = 0;

	effectNeutralAreaPlayId_ = -1;
	effectNeutralAreaPlayId_ = -1;
}

void FlagBase::Init()
{
	//旗
	flag_.pos = { pos_.x, pos_.y, pos_.z };
	flag_.scl = FLAG_SCALE;
	flag_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	flag_.Update();

	pflag_.pos = { pos_.x, pos_.y, pos_.z };
	pflag_.scl = FLAG_SCALE;
	pflag_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	pflag_.Update();
	
	nflag_.pos = { pos_.x, pos_.y, pos_.z };
	nflag_.scl = FLAG_SCALE;
	nflag_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	nflag_.Update();

	enemySpawned_ = false;
	playerInRange_ = false;
	enemyNear_ = false;
	clearGauge_ = 0.0f;
	clearGaugeMax_ = GAUGE_MAX;
	flagRadius_ = FLAG_RADIUS;
	enemyCheckRadius_ = CHECK_RADIUS;

	//エフェクト
	effectEnemyAreaResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::ENEMY_AREA).handleId_;
	effectPlayerAreaResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER_AREA).handleId_;
	effectNeutralAreaResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::NONE_AREA).handleId_;
}

void FlagBase::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
	CheckCircle(playerPos, enemies);

	//EffectAreaRange();

	if (IsOwnedByEnemy())
	{
		state_ = STATE::ENEMY;
	}
	else if (!enemyNear_ && IsOwnedByPlayer())
	{
		state_ = STATE::PLAYER;
	}
	else
	{
		state_ = STATE::NEUTRAL;
	}
}

void FlagBase::Draw()
{
	// 円を表示
	if (IsOwnedByEnemy())
	{
		MV1SetScale(flag_.modelId, scl_);
		MV1SetRotationXYZ(flag_.modelId, rot_);
		MV1SetPosition(flag_.modelId, pos_);
		MV1DrawModel(flag_.modelId);
	}
	else if (IsOwnedByPlayer())
	{
		MV1SetScale(pflag_.modelId, scl_);
		MV1SetRotationXYZ(pflag_.modelId, rot_);
		MV1SetPosition(pflag_.modelId, pos_);
		MV1DrawModel(pflag_.modelId);
	}
	else
	{
		MV1SetScale(nflag_.modelId, scl_);
		MV1SetRotationXYZ(nflag_.modelId, rot_);
		MV1SetPosition(nflag_.modelId, pos_);
		MV1DrawModel(nflag_.modelId);
	}

	if (!enemyNear_ && playerInRange_ && !IsOwnedByPlayer())
	{
		PlayerDrawGauge(pos_, clearGauge_ / clearGaugeMax_);
	}
	else if(enemyNear_ && !playerInRange_ && !IsOwnedByEnemy())
	{
		EnemyDrawGauge(pos_, clearGauge_ / clearGaugeMax_);
	}
}

VECTOR FlagBase::GetPosition() const
{
    return pos_;
}

bool FlagBase::SpawnEnemies(const VECTOR& playerPos) const
{
	if (enemySpawned_) return false;  //もう出してる

	float dx = playerPos.x - pos_.x;
	float dz = playerPos.z - pos_.z;
	float distSq = dx * dx + dz * dz;

	return (distSq < flagRadius_* flagRadius_);
}

void FlagBase::SetEnemySpawned(bool spawned)
{
	enemySpawned_ = spawned;
}

void FlagBase::CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
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

			//Dogだけを見る
			if (enemy->GetEnemyType() != EnemyBase::TYPE::DOG)
				continue;

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
			clearGauge_ += GAUGE_UP;
			//エフェクト
			if (effectNeutralAreaPlayId_ >= 0)
			{
				StopEffekseer3DEffect(effectNeutralAreaPlayId_);
			}

			float scale = EFFECT_SCALE;  // デフォルト値

			effectNeutralAreaPlayId_ = PlayEffekseer3DEffect(effectNeutralAreaResId_);
			SetScalePlayingEffekseer3DEffect(effectNeutralAreaPlayId_, scale, scale, scale);
			if (clearGauge_ >= clearGaugeMax_)
			{
				clearGauge_ = clearGaugeMax_;
				state_ = STATE::PLAYER;
				SetPosPlayingEffekseer3DEffect(effectNeutralAreaPlayId_, pos_.x, pos_.y + EFFECT_OFFSET, pos_.z);

				//旗の取得音
				SoundManager::GetInstance().Play(SoundManager::SRC::GETFLAG_SE, Sound::TIMES::ONCE);
			}
		}
		else
		{
			clearGauge_ -= GAUGE_DOWN; // 離れると少し減るなども可能
			if (clearGauge_ < 0.0f) clearGauge_ = 0.0f;
		}
}

void FlagBase::DrawCircleOnMap(VECTOR center, float radius, int color)
{
	const int div = CIRCLE_DIVISION;

	for (int i = 0; i < div; i++)
	{
		float t1 = (float)i / div;
		float t2 = (float)(i + 1) / div;
		float x1 = center.x + cosf(t1 * FULL_CIRCLE) * radius;
		float z1 = center.z + sinf(t1 * FULL_CIRCLE) * radius;
		float x2 = center.x + cosf(t2 * FULL_CIRCLE) * radius;
		float z2 = center.z + sinf(t2 * FULL_CIRCLE) * radius;

		DrawLine3D(VGet(x1, center.y, z1), VGet(x2, center.y, z2), color);
	}
}

void FlagBase::PlayerDrawGauge(VECTOR center, float gaugeRate)
{
	// ゲージバーを円の上（少し高い位置）に表示
	VECTOR gaugePos = VAdd(center, VGet(0.0f, GAUGE_OFFSET_Y, 0.0f)); // 円より上
	VECTOR screenPos = ConvWorldPosToScreenPos(gaugePos);

	int barWidth = GAUGE_WIDTH;
	int barHeight = GAUGE_HEIGHT;

	int x = (int)screenPos.x - barWidth / HALF_DIVISOR;
	int y = (int)screenPos.y - GAUGE_SCREEN_OFFSET_Y; // 少し上にオフセット

	// 外枠
	DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, white, FALSE);

	// 中身
	int fillWidth = (int)(barWidth * gaugeRate);
	DrawBox(x, y, x + fillWidth, y + barHeight, green, TRUE);
}

void FlagBase::EnemyDrawGauge(VECTOR center, float gaugeRate)
{
	// ゲージバーを円の上（少し高い位置）に表示
	VECTOR gaugePos = VAdd(center, VGet(0.0f, GAUGE_OFFSET_Y, 0.0f)); // 円より上
	VECTOR screenPos = ConvWorldPosToScreenPos(gaugePos);

	int barWidth = GAUGE_WIDTH;
	int barHeight = GAUGE_HEIGHT;

	int x = (int)screenPos.x - barWidth / HALF_DIVISOR;
	int y = (int)screenPos.y - GAUGE_SCREEN_OFFSET_Y; // 少し上にオフセット

	// 外枠
	DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, white, FALSE);

	// 中身
	int fillWidth = (int)(barWidth * gaugeRate);
	DrawBox(x, y, x + fillWidth, y + barHeight, red, TRUE);
}

float FlagBase::DistanceSqXZ(const VECTOR& a, const VECTOR& b) const
{
	float dx = a.x - b.x;
	float dz = a.z - b.z;
	return  dx * dx + dz * dz;
}

void FlagBase::EffectAreaRange(void)
{
	if (IsOwnedByEnemy())
	{
		if (effectEnemyAreaPlayId_ >= 0)
		{
			StopEffekseer3DEffect(effectEnemyAreaPlayId_);
		}

		float scale = EFFECT_SCALE;  // デフォルト値

		effectEnemyAreaPlayId_ = PlayEffekseer3DEffect(effectEnemyAreaResId_);
		SetScalePlayingEffekseer3DEffect(effectEnemyAreaPlayId_, scale, scale / HALF_DIVISOR, scale);
		SetPosPlayingEffekseer3DEffect(effectEnemyAreaPlayId_, pos_.x, pos_.y, pos_.z);
	}
	else if (IsOwnedByPlayer())
	{
		if (effectPlayerAreaPlayId_ >= 0)
		{
			StopEffekseer3DEffect(effectPlayerAreaPlayId_);
		}

		float scale = EFFECT_SCALE;  // デフォルト値

		effectPlayerAreaPlayId_ = PlayEffekseer3DEffect(effectPlayerAreaResId_);
		SetScalePlayingEffekseer3DEffect(effectPlayerAreaPlayId_, scale, scale / HALF_DIVISOR, scale);
		SetPosPlayingEffekseer3DEffect(effectPlayerAreaPlayId_, pos_.x, pos_.y, pos_.z);
	}
	else
	{
		if (effectNeutralAreaPlayId_ >= 0)
		{
			StopEffekseer3DEffect(effectNeutralAreaPlayId_);
		}

		float scale = EFFECT_SCALE;  // デフォルト値

		effectNeutralAreaPlayId_ = PlayEffekseer3DEffect(effectNeutralAreaResId_);
		SetScalePlayingEffekseer3DEffect(effectNeutralAreaPlayId_, scale, scale / HALF_DIVISOR, scale);
		SetPosPlayingEffekseer3DEffect(effectNeutralAreaPlayId_, pos_.x, pos_.y, pos_.z);
	}
}
