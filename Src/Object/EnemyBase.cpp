#include <DxLib.h>
#include <string>
#include <vector>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Scene/GameScene.h"
#include "../Utility/AsoUtility.h"
#include "Common/AnimationController.h"
#include "ActorBase.h"
#include "Player.h"
#include "Tree.h"
#include "EnemyBase.h"

//担当　田中良明

EnemyBase::EnemyBase() 
	: 
	scene_(nullptr),
	movePow_(AsoUtility::VECTOR_ZERO)
{
	animationController_ = nullptr;

	state_ = STATE::NONE;

	attackPow_ = VALUE_ONE;	//攻撃力

	// 状態管理
	stateChanges_.emplace(
		STATE::NONE, std::bind(&EnemyBase::ChangeStateNone, this));
	stateChanges_.emplace(
		STATE::IDLE, std::bind(&EnemyBase::ChangeStateIdle, this));
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&EnemyBase::ChangeStatePlay, this));
	stateChanges_.emplace(
		STATE::ATTACK, std::bind(&EnemyBase::ChangeStateAttack, this));
	stateChanges_.emplace(
		STATE::DAMAGE, std::bind(&EnemyBase::ChangeStateDamage, this));
	stateChanges_.emplace(
		STATE::DEATH, std::bind(&EnemyBase::ChangeStateDeath, this));
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
 	SetParam();
	InitAnimation();
}

void EnemyBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	transform_.Update();

	//アニメーション再生
	animationController_->Update();


	// 更新ステップ
	if (stateUpdate_)
	{
		stateUpdate_();
	}
}

#pragma region StateごとのUpdate

void EnemyBase::UpdateIdle(void)
{
	animationController_->Play((int)ANIM_TYPE::IDLE, false);
	if (animationController_->IsEnd() || state_ != STATE::IDLE)
	{
		AttackCollisionPos();
	}
}

void EnemyBase::UpdatePlay(void)
{
	if (!isAlive_)
	{
		return;
	}

	ChasePlayer();

	// 衝突判定
	Collision();

	//攻撃範囲に入ったかを見る
	AttackCollisionPos();
}

void EnemyBase::UpdateAttack(void)
{
	animationController_->Play((int)ANIM_TYPE::ATTACK, false);

	// 攻撃タイミング
	if (!isAttack_ && isAttack_P)
	{
		isAttack_ = true; // 多重ヒット防止用フラグ
		isAttack_P = false;
	}
	else if (!isAttack_ && isAttack_T)
	{
		isAttack_ = true;
		isAttack_T = false;
		tree_->eHit();
	}

	 //アニメーション終了で次の状態に遷移
	if (animationController_->IsEnd() || state_ != STATE::ATTACK) {
		isAttack_ = false;
		CheckHitAttackHit();
		ChangeState(STATE::IDLE);
	}
}

void EnemyBase::UpdateDamage(void)
{
	animationController_->Play((int)ANIM_TYPE::DAMAGE, false);
	if (animationController_->IsEnd())
	{
		ChangeState(STATE::PLAY);
	}
}

void EnemyBase::UpdateDeath(void)
{
	animationController_->Play((int)ANIM_TYPE::DEATH, false);

	if (animationController_->IsEnd())
	{
		isAlive_ = false;
		
		//アイテムドロップ
		VECTOR dropPos = this->GetTransform().pos;

		// マップ中心との距離を計算
		float distance = VSize(VSub(dropPos, AsoUtility::VECTOR_ZERO));

		// ドロップアイテムを取得
		Item::TYPE dropType = GetDropItemType();

		//エネミーがボスのとき
		if (enemyType_ == TYPE::BOSS)
		{
			// ボスのアイテムはスケール固定
			float scale = DROP_SCALE_LARGE;
			
			scene_->CreateItem(dropPos, scale, dropType);
		}
		else
		{
			// 通常の敵は1つだけアイテムドロップ
			// 距離でサイズを変える
			float scale = DROP_SCALE_SMALL;
			if (dropType == Item::TYPE::WATER)
			{
				if (distance >= DROP_DISTANCE_LARGE) {	// 中心から一定距離以上離れたら
					scale = DROP_SCALE_LARGE;
				}
				else if (distance >= DROP_DISTANCE_MEDIUM) {	// 中心から一定距離以上離れたら
					scale = DROP_SCALE_MEDIUM;
				}
			}
			// アイテムを1つ出す（サイズ調整）
			scene_->CreateItem(dropPos, scale, dropType);
		}
	}
}

#pragma endregion


void EnemyBase::ChasePlayer(void)
{
	if (!player_) {
		return;
	}

	VECTOR playerPos = player_->GetTransform().pos;

	VECTOR toPlayer = VSub(playerPos, transform_.pos);
	toPlayer.y = ZERO;  // 高さ無視

	float distance = VSize(toPlayer);

	// 現在のアニメーションと違う場合のみRUNアニメーションを再生する
	if (animtype_ != ANIM_TYPE::RUN)
	{
		animationController_->Play((int)ANIM_TYPE::RUN, true);
	}

	//エネミーの視野内に入ったら追いかける
	VECTOR dirToPlayer = VNorm(toPlayer);
	VECTOR moveVec = VScale(dirToPlayer, speed_);

	transform_.pos = VAdd(transform_.pos, moveVec);

	// 方向からクォータニオンに変換
	transform_.quaRot = Quaternion::LookRotation(dirToPlayer);
}

void EnemyBase::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	Collision();

	// モデル反映
	MV1SetScale(transform_.modelId, transform_.scl);
	MV1SetPosition(transform_.modelId, transform_.pos);

	MV1DrawModel(transform_.modelId);


	//デッバグ
	//DrawDebug();

	// 視野範囲の描画
	//DrawDebugSearchRange();
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(transform_.modelId);
}

void EnemyBase::SetPos(VECTOR pos)
{
	transform_.pos = pos;
}

EnemyBase::STATE EnemyBase::GetState(void)
{
	return state_;
}

bool EnemyBase::IsAlive(void)
{
	return isAlive_;
}

void EnemyBase::SetAlive(bool alive)
{
	isAlive_ = alive;
}

EnemyBase::TYPE EnemyBase::GetEnemyType(void) const
{
	return enemyType_;
}

void EnemyBase::Damage(int damage)
{
	hp_ -= damage;
	// ダメージ音
	SoundManager::GetInstance().Play(SoundManager::SRC::E_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);
	isAttack_ = false;
	if (hp_ <= ZERO && isAlive_)
	{
		ChangeState(STATE::DEATH);	
		SoundManager::GetInstance().Play(SoundManager::SRC::E_DOWN_SE, Sound::TIMES::ONCE);
	}
	else if (hp_ >= VALUE_ONE && isAlive_ && enemyType_ != TYPE::BOSS)
	{
		ChangeState(STATE::DAMAGE);
	}
}

#pragma region コリジョン

void EnemyBase::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	collisionPos_ = VAdd(transform_.pos, collisionLocalPos_);
}

void EnemyBase::SetCollisionPos(const VECTOR collision)
{
	collisionPos_ = collision;
}

VECTOR EnemyBase::GetCollisionPos(void)const
{
	return VAdd(collisionLocalPos_, transform_.pos);
}

float EnemyBase::GetCollisionRadius(void)
{
	return collisionRadius_;
}
#pragma endregion

void EnemyBase::AttackCollisionPos(void)
{
	//プレイヤーとの衝突判定
	// 攻撃の方向（エネミー）
	VECTOR forward = transform_.quaRot.GetForward();
	// 攻撃の開始位置と終了位置
	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;  // 攻撃の高さ調整

	//プレイヤーを見る
	EnemyToPlayer();
	//treeを見る
	EnemyToTree();
}

void EnemyBase::EnemyToPlayer(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
			&& player_->pstate_ != Player::PlayerState::DOWN)
	{
		isAttack_P = true;
		ChangeState(STATE::ATTACK);
	}
	else if (!AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
		|| player_->pstate_ == Player::PlayerState::DOWN)
	{
		ChangeState(STATE::PLAY);
	}
}

void EnemyBase::EnemyToTree(void)
{
	//プレイヤーの当たり判定とサイズ
	treeCenter_ = tree_->GetCollisionPos();
	treeRadius_ = tree_->GetCollisionRadius();

	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, treeCenter_,treeRadius_))
	{
		isAttack_T = true;
		ChangeState(STATE::ATTACK);
	}
}

void EnemyBase::CheckHitAttackHit(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if(AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_,playerCenter_, playerRadius_))
	{
		player_->Damage(attackPow_);
	}
}

Item::TYPE EnemyBase::GetDropItemType() const
{
	return Item::TYPE::WATER;
}

void EnemyBase::SetGameScene(GameScene* scene)
{
	scene_ = scene;
}

#pragma region Stateの切り替え

void EnemyBase::ChangeState(STATE state)
{
	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();
}

void EnemyBase::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateNone, this);
}

void EnemyBase::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateIdle, this);
}
void EnemyBase::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdatePlay, this);
}

void EnemyBase::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateAttack, this);
}

void EnemyBase::ChangeStateDamage(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateDamage, this);
}

void EnemyBase::ChangeStateDeath(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateDeath, this);
}

#pragma endregion

void EnemyBase::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void EnemyBase::SetTree(std::shared_ptr<Tree> tree)
{
	tree_ = tree;
}

void EnemyBase::DrawDebug(void)
{
	VECTOR v;
	VECTOR s;
	VECTOR a;

	// キャラ基本情報
	//-------------------------------------------------------
	//// キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 120, white, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)",v.x, v.y, v.z);

	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, black, black, false);
	DrawFormatString(20, 180, white, "スフィア座標 ： (%0.2f, %0.2f, %0.2f)",s.x, s.y, s.z);
	DrawFormatString(20, 210, white, "エネミーの移動速度 ： %0.2f", speed_);
	
	a = attackCollisionPos_;
	DrawSphere3D(a, attackCollisionRadius_, 8, yellow, yellow, false);

}

void EnemyBase::DrawDebugSearchRange(void)
{
	VECTOR centerPos = transform_.pos;

	// プレイヤーの座標
	VECTOR playerPos = player_->GetTransform().pos; // プレイヤーオブジェクトの参照を持っている想定

	// プレイヤーと敵の距離（XZ平面）
	float dx = playerPos.x - centerPos.x;
	float dz = playerPos.z - centerPos.z;
	float distance = sqrtf(dx * dx + dz * dz);

	// 範囲内か判定
	bool inRange = (distance <= VIEW_RANGE);

	float angleStep = AsoUtility::FULL_ROTATION_RAD / VALUE_SIXTY;

	for (int i = ZERO; i < VALUE_SIXTY; ++i)
	{
		float angle1 = angleStep * i;
		float angle2 = angleStep * (i + VALUE_ONE);

		VECTOR p1 = {
			centerPos.x + VIEW_RANGE * sinf(angle1),
			centerPos.y,
			centerPos.z + VIEW_RANGE * cosf(angle1)
		};
		VECTOR p2 = {
			centerPos.x + VIEW_RANGE * sinf(angle2),
			centerPos.y,
			centerPos.z + VIEW_RANGE * cosf(angle2)
		};
	}
}