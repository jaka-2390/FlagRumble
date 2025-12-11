#include <DxLib.h>
#include "EnemyCactus.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/FlagManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Player.h"

EnemyCactus::EnemyCactus() :EnemyBase()
{
}

void EnemyCactus::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/Cactus/Cactus.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE, path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN, path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DAMAGE, path, ANIM_SPEED, ANIM_DAMAGE_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH, path, ANIM_SPEED, ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyCactus::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::SABO));

	transform_.scl = { AsoUtility::VECTOR_ONE };						// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(DEGREE), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { AsoUtility::VECTOR_ZERO };						// 右方向に移動する

	speed_ = SPEED;		// 移動スピード

	isAlive_ = true;	// 初期は生存状態

	hp_ = HP;	// HPの設定
	maxHp_ = HP;

	collisionRadius_ = COLLOSION_RADIUS;	// 衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		// 攻撃判定用と攻撃範囲の球体半径

	// 初期状態
	ChangeState(STATE::PLAY);
}

void EnemyCactus::ChasePlayer(void)
{
    if (!isAlive_) return;

    //現在のアニメーションと違う場合のみRUNアニメーションを再生する
    if (animtype_ != ANIM_TYPE::RUN)
    {
        animationController_->Play((int)ANIM_TYPE::RUN, true);
    }

    FlagBase* targetFlag = nullptr;

    // プレイヤーが持っている旗を探す
    for (int i = 0; i < flagManager_->GetFlagMax(); ++i)
    {
        FlagBase* f = flagManager_->GetFlag(i);
        if (f && f->IsOwnedByPlayer())
        {
            targetFlag = f;
            break;
        }
    }

	VECTOR playerPos = player_->GetTransform().pos;

	VECTOR toPlayer = VSub(playerPos, transform_.pos);
	toPlayer.y = VALUE_ZERO;  //高さ無視

	float distance = VSize(toPlayer);

	if (distance <= VIEW_RANGE
		&& state_ == STATE::PLAY
		&& player_->pstate_ == Player::PlayerState::NORMAL)
	{
		//プレイヤーと接敵
		encounter_ = true;
	}
	else
	{
		//プレイヤーから離れた
		encounter_ = false;
	}

    VECTOR targetPos;
    if (targetFlag)
    {
        targetPos = targetFlag->GetPosition();

        // 旗に近づいたら奪う
        float distSq = VSize(VSub(transform_.pos, targetPos));
        if (distSq < 100.0f) //エリアの範囲内
        {
			captureTimer_ += scnMng_.GetDeltaTime();

			if (captureTimer_ >= FLAG_CHANGE)
			{
				targetFlag->SetState(Flag::STATE::ENEMY);
				captureTimer_ = 0.0f;
			}
        }
		else
		{
			// 移動
			VECTOR toTarget = VSub(targetPos, transform_.pos);
			toTarget.y = 0;
			VECTOR moveVec = VScale(VNorm(toTarget), speed_);
			transform_.pos = VAdd(transform_.pos, moveVec);
			transform_.quaRot = Quaternion::LookRotation(VNorm(toTarget));
		}
    }
    else
    {
		//タイマー
		changeDirTimer_ += scnMng_.GetDeltaTime();

		//出現位置を基準にする
		float maxRange = MAX_RANGE;

		//現在の出現位置からの距離
		float distanceStart = VSize(VSub(transform_.pos, startPos_));

		//2秒ごとに方向変更
		if (changeDirTimer_ >= WANDER_CHANGE_TIME || distanceStart > maxRange)
		{
			changeDirTimer_ = 0.0f;

			VECTOR toStart = VSub(startPos_, transform_.pos);

			if (distanceStart > maxRange)
			{
				//範囲外ならスタート地点へ
				wanderDir_ = VNorm(toStart);
			}
			else
			{
				//ランダム方向
				float angle = GetRand(360) * DX_PI_F / 180.0f;
				wanderDir_ = VGet(cosf(angle), 0.0f, sinf(angle));
			}
		}

		//徘徊
		VECTOR moveVec = VScale(wanderDir_, speed_ * WANDER_SPEED_SCALE);
		transform_.pos = VAdd(transform_.pos, moveVec);
		transform_.quaRot = Quaternion::LookRotation(wanderDir_);
    }
}

void EnemyCactus::SetFlagManager(FlagManager* manager)
{
    flagManager_ = manager;
}
