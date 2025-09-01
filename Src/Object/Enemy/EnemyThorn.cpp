#include "EnemyThorn.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
//#include "../Common/Capsule.h"

EnemyThorn::EnemyThorn():EnemyBase()
{
}

void EnemyThorn::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/thorn/thorn.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE,  path, 17.0f, 1);
	animationController_->Add((int)ANIM_TYPE::RUN,   path, 16.0f, 2);
	animationController_->Add((int)ANIM_TYPE::ATTACK,path, 25.0f, 3);
	animationController_->Add((int)ANIM_TYPE::DAMAGE,path, 22.0f, 4);
	animationController_->Add((int)ANIM_TYPE::DEATH, path, 22.0f, 5);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyThorn::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::TOGE));

	transform_.scl = { 1.0f, 1.0f, 1.0f };						// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f), AsoUtility::Deg2RadF(180.0f), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { 0.0f, 0.0f, 0.0f };						// 右方向に移動する

	speed_ = 2.0f;		// 移動スピード

	isAlive_ = true;	// 初期は生存状態

	hp_ = 10;	// HPの設定

	collisionRadius_ = 100.0f;	// 衝突判定用の球体半径
	collisionLocalPos_ = { 0.0f, 60.0f, 0.0f };	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		// 攻撃判定用と攻撃範囲の球体半径
	
	// 初期状態
	ChangeState(STATE::PLAY);
}

Item::TYPE EnemyThorn::GetDropItemType() const
{
	if (GetRand(RANDOM_VALUE))
	{
		return Item::TYPE::WATER;
	}
	else
	{
		return Item::TYPE::MUTEKI;
	}
}
