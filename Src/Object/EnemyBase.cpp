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
#include "EnemyBase.h"

EnemyBase::EnemyBase() : scene_(nullptr),movePow_(AsoUtility::VECTOR_ZERO)
{
	animationController_ = nullptr;

	state_ = STATE::NONE;

	attackPow_ = static_cast<int>(VALUE_ONE);	//攻撃力

	//状態管理
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

	is1damage_ = false;
	is2damage_ = false;
	is4damage = false;
	is8damage = false;
	is16damage = false;
	is32damage = false;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
	SetParam();
	InitAnimation();

	startPos_ = transform_.pos;

	encounter_ = false;

	damageCnt_ = 0;

	damage1img_ = LoadGraph("Data/Image/1.png");
	damage2img_ = LoadGraph("Data/Image/2.png");
	damage4img_ = LoadGraph("Data/Image/4.png");
	damage8img_ = LoadGraph("Data/Image/8.png");
	damage16img_ = LoadGraph("Data/Image/16.png");
	damage32img_ = LoadGraph("Data/Image/32.png");
	damage64img_ = LoadGraph("Data/Image/64.png");
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


	//更新ステップ
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

	//衝突判定
	Collision();

	//攻撃範囲に入ったかを見る
	AttackCollisionPos();
}

void EnemyBase::UpdateAttack(void)
{
	animationController_->Play((int)ANIM_TYPE::ATTACK, false);

	const auto& anim = animationController_->GetPlayAnim();
	if (anim.step > 15.0f && isAttack_ == true)
	{
		isAttack_ = false;
		CheckHitAttackHit();
	}
	else if(anim.step > 30.0f && enemyType_ == TYPE::BOSS && isAttack_P)
	{
		isAttack_P = false;
		CheckHitAttackHit();
		return;
	}

	//アニメーション終了で次の状態に遷移
	if (animationController_->IsEnd() || state_ != STATE::ATTACK) {
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

		//マップ中心との距離を計算
		float distance = VSize(VSub(dropPos, AsoUtility::VECTOR_ZERO));

		//ドロップアイテムを取得
		//Item::TYPE dropType = GetDropItemType();

		////エネミーがボスのとき
		//if (enemyType_ == TYPE::BOSS)
		//{
		//	//ボスのアイテムはスケール固定
		//	float scale = DROP_SCALE_LARGE;

		//	scene_->CreateItem(dropPos, scale, dropType);
		//}
		//else
		//{
		//	//通常の敵は1つだけアイテムドロップ
		//	//距離でサイズを変える
		//	float scale = DROP_SCALE_SMALL;
		//	if (dropType == Item::TYPE::WATER)
		//	{
		//		if (distance >= DROP_DISTANCE_LARGE)
		//		{	//中心から一定距離以上離れたら
		//			scale = DROP_SCALE_LARGE;
		//		}
		//		else if (distance >= DROP_DISTANCE_MEDIUM)
		//		{	//中心から一定距離以上離れたら
		//			scale = DROP_SCALE_MEDIUM;
		//		}
		//	}
		//	//アイテムを1つ出す（サイズ調整）
		//	scene_->CreateItem(dropPos, scale, dropType);
		//}
	}
}
#pragma endregion

void EnemyBase::ChasePlayer(void)
{
	if (!player_)
	{
		return;
	}

	VECTOR playerPos = player_->GetTransform().pos;

	VECTOR toPlayer = VSub(playerPos, transform_.pos);
	toPlayer.y = VALUE_ZERO;  //高さ無視

	float distance = VSize(toPlayer);

	//現在のアニメーションと違う場合のみRUNアニメーションを再生する
	if (animtype_ != ANIM_TYPE::RUN)
	{
		animationController_->Play((int)ANIM_TYPE::RUN, true);
	}

	//ボスはプレイヤーを追いかける
	if (enemyType_ == TYPE::BOSS)
	{
		VECTOR dirToPlayer = VNorm(toPlayer);
		VECTOR moveVec = VScale(dirToPlayer, speed_);

		transform_.pos = VAdd(transform_.pos, moveVec);

		//方向からクォータニオンに変換
		transform_.quaRot = Quaternion::LookRotation(dirToPlayer);

		encounter_ = false;
	}
	//エネミーの視野内に入ったら追いかける
	else if (distance <= VIEW_RANGE
		&& state_ == STATE::PLAY
		&& player_->pstate_ == Player::PlayerState::NORMAL)
	{
		VECTOR dirToPlayer = VNorm(toPlayer);
		VECTOR moveVec = VScale(dirToPlayer, speed_);

		transform_.pos = VAdd(transform_.pos, moveVec);

		//方向からクォータニオンに変換
		transform_.quaRot = Quaternion::LookRotation(dirToPlayer);

		//プレイヤーと接敵
		encounter_ = true;
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

		//プレイヤーから離れた
		encounter_ = false;

		//徘徊
		VECTOR moveVec = VScale(wanderDir_, speed_ * WANDER_SPEED_SCALE);
		transform_.pos = VAdd(transform_.pos, moveVec);
		transform_.quaRot = Quaternion::LookRotation(wanderDir_);
	}
}

void EnemyBase::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	Collision();

	//モデル反映
	MV1SetScale(transform_.modelId, transform_.scl);
	MV1SetPosition(transform_.modelId, transform_.pos);

	MV1DrawModel(transform_.modelId);
	DrawDamage();

	//HPバー描画
	if (encounter_)
	{
		float gaugeRate = static_cast<float>(hp_) / static_cast<float>(maxHp_);
		gaugeRate = std::clamp(gaugeRate, 0.0f, 1.0f); //範囲制限
		DrawHpGauge3D(transform_.pos, gaugeRate);
	}

	//デッバグ
	DrawDebug();

	//視野範囲の描画
	DrawDebugSearchRange();
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
	//ダメージ音
	SoundManager::GetInstance().Play(SoundManager::SRC::E_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);
	isAttack_ = false;
	if (hp_ <= VALUE_ZERO && isAlive_)
	{
		ChangeState(STATE::DEATH);
		SoundManager::GetInstance().Play(SoundManager::SRC::E_DOWN_SE, Sound::TIMES::ONCE);
	}
	else if (hp_ >= static_cast<int>(VALUE_ONE) && isAlive_ && enemyType_ != TYPE::BOSS)
	{
		ChangeState(STATE::DAMAGE);
	}

	if (damage == VALUE_DAMAGE_1)is1damage_ = true;
	if (damage == VALUE_DAMAGE_2)is2damage_ = true;
	if (damage == VALUE_DAMAGE_4)is4damage = true;
	if (damage == VALUE_DAMAGE_8)is8damage = true;
	if (damage == VALUE_DAMAGE_16)is16damage = true;
	if (damage == VALUE_DAMAGE_32)is32damage = true;
	if (damage == VALUE_DAMAGE_64)is64damage = true;
}

void EnemyBase::DrawDamage()
{
	//ダメージ画像の描画時間
	if (is1damage_)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage1img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is1damage_ = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is2damage_)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage2img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is2damage_ = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is4damage)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage4img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is4damage = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is8damage)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage8img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is8damage = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is16damage)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage16img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is16damage = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is32damage)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage32img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is32damage = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
	if (is64damage)
	{
		DrawRotaGraph3D(transform_.pos.x, transform_.pos.y + DAMAGE_POS, transform_.pos.z
			, DAMAGE_IMG_SCL, 0, damage64img_, true);
		if (damageCnt_ >= DAMAGE_CNT)
		{
			is64damage = false;
			damageCnt_ = 0;
		}
		else
		{
			damageCnt_++;
		}
	}
}

void EnemyBase::DrawHpGauge3D(VECTOR center, float gaugeRate)
{
	//HPバーを敵の上(少し高い位置)に表示
	VECTOR gaugePos = VAdd(center, VGet(0.0f, 80.0f, 0.0f));
	VECTOR screenPos = ConvWorldPosToScreenPos(gaugePos);

	int barWidth = 80;
	int barHeight = 8;

	int x = (int)screenPos.x - barWidth / 2;
	int y = (int)screenPos.y - 200; //少し上にオフセット

	//外枠
	DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, GetColor(255, 255, 255), FALSE);

	//中身
	int fillWidth = (int)(barWidth * gaugeRate);
	DrawBox(x, y, x + fillWidth, y + barHeight, GetColor(255, 0, 0), TRUE);
}


#pragma region コリジョン

void EnemyBase::Collision(void)
{
	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//移動
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
	//攻撃の方向（エネミー）
	VECTOR forward = transform_.quaRot.GetForward();
	//攻撃の開始位置と終了位置
	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;  // 攻撃の高さ調整

	//プレイヤーを見る
	EnemyToPlayer();
}

void EnemyBase::EnemyToPlayer(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
		&& player_->pstate_ != Player::PlayerState::DOWN)
	{
		isAttack_ = true;
		isAttack_P = true;
		ChangeState(STATE::ATTACK);
	}
	else if (!AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
		|| player_->pstate_ == Player::PlayerState::DOWN)
	{
		ChangeState(STATE::PLAY);
	}
}

void EnemyBase::CheckHitAttackHit(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_))
	{
		player_->Damage(attackPow_);
	}
}

//Item::TYPE EnemyBase::GetDropItemType(void) const
//{
//	return Item::TYPE::WATER;
//}

void EnemyBase::SetGameScene(GameScene* scene)
{
	scene_ = scene;
}

#pragma region Stateの切り替え

void EnemyBase::ChangeState(STATE state)
{
	//状態変更
	state_ = state;

	//各状態遷移の初期処理
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

void EnemyBase::DrawDebug(void)
{

#ifdef _DEBUG

	VECTOR v;
	VECTOR s;
	VECTOR a;

	//キャラ基本情報
	//-------------------------------------------------------
	//キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 120, white, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)", v.x, v.y, v.z);

	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, black, black, false);
	DrawFormatString(20, 180, white, "スフィア座標 ： (%0.2f, %0.2f, %0.2f)", s.x, s.y, s.z);
	DrawFormatString(20, 210, white, "エネミーの移動速度 ： %0.2f", speed_);

	a = attackCollisionPos_;
	DrawSphere3D(a, attackCollisionRadius_, 8, yellow, yellow, false);

#endif //DEBUG

}

void EnemyBase::DrawDebugSearchRange(void)
{

#ifdef _DEBUG

	VECTOR centerPos = transform_.pos;
	float radius = VIEW_RANGE;
	int segments = 60;

	// プレイヤーの座標
	VECTOR playerPos = player_->GetTransform().pos; // プレイヤーオブジェクトの参照を持っている想定

	// プレイヤーと敵の距離（XZ平面）
	float dx = playerPos.x - centerPos.x;
	float dz = playerPos.z - centerPos.z;
	float distance = sqrtf(dx * dx + dz * dz);

	// 範囲内か判定
	bool inRange = (distance <= radius);

	// 色を決定（範囲内なら赤、範囲外は元の色）
	unsigned int color = inRange ? 0xff0000 : 0xffdead;

	float angleStep = DX_PI * 2.0f / segments;

	for (int i = 0; i < segments; ++i)
	{
		float angle1 = angleStep * i;
		float angle2 = angleStep * (i + 1);

		VECTOR p1 = {
			centerPos.x + radius * sinf(angle1),
			centerPos.y,
			centerPos.z + radius * cosf(angle1)
		};
		VECTOR p2 = {
			centerPos.x + radius * sinf(angle2),
			centerPos.y,
			centerPos.z + radius * cosf(angle2)
		};
		DrawTriangle3D(centerPos, p1, p2, color, false);
	}
	DrawSphere3D(centerPos, 20.0f, 10, 0x00ff00, 0x00ff00, true);

#endif //DEBUG

}