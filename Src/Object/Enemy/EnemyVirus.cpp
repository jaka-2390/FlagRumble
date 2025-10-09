#include "EnemyVirus.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyVirus::EnemyVirus() :EnemyBase()
{
}

void EnemyVirus::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/virus/virus.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE, path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN, path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DAMAGE, path, ANIM_SPEED, ANIM_DAMAGE_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH, path, ANIM_SPEED, ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyVirus::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::VIRUS));

	transform_.scl = { AsoUtility::VECTOR_ONE };						// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(DEGREE), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { AsoUtility::VECTOR_ZERO };						// 右方向に移動する

	speed_ = SPEED;		// 移動スピード

	isAlive_ = true;	// 初期は生存状態

	hp_ = HP;	// HPの設定

	collisionRadius_ = COLLOSION_RADIUS;// 衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		// 攻撃判定用と攻撃範囲の球体半径

	// 初期状態
	ChangeState(STATE::PLAY);
}