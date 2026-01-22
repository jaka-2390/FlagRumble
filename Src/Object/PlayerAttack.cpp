#include <EffekseerForDXLib.h>
#include "Common/AnimationController.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Utility/AsoUtility.h"
#include "Player.h"
#include "PlayerAttack.h"

PlayerAttack::PlayerAttack(Player* owner)
{
	owner_ = owner;
	isAttack_ = false;
	hasHit_ = false;
	normalAttack_ = NORMAL_ATTACK;
	isHitStop_ = false;
	hitStopFrame_ = 0;
	effectSwordResId_ = -1;
	effectSwordPlayId_ = -1;
}

void PlayerAttack::Init(void)
{
	//攻撃エフェクト
	effectSwordResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::SWORD).handleId_;

	std::string path = Application::PATH_MODEL + "Player/";
	owner_->GetAnimation()->Add((int)ATTACK_TYPE::NORMALATTACK, path + "Player.mv1", ANIM_SPEED, ANIM_NORMALATTACK_INDEX);
}

void PlayerAttack::Update(void)
{
	ProcessInput();

	if (isAttack_)
	{
		UpdateNormalAttack();
	}

	if (isHitStop_)
	{
		hitStopFrame_--;

		if (hitStopFrame_ <= 0)
		{
			isHitStop_ = false;
			owner_->GetAnimation()->SetStop(false);
		}
		return;
	}

}

bool PlayerAttack::IsAttacking(void) const
{
    return isAttack_;
}

void PlayerAttack::ProcessInput(void)
{
	if (isAttack_) return;

	if (CheckHitKey(KEY_INPUT_E) || 
		ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::TOP))
	{
		owner_->GetAnimation()->Play((int)ATTACK_TYPE::NORMALATTACK, false);

		isAttack_ = true;
		hasHit_ = false;
	}
}

void PlayerAttack::UpdateNormalAttack(void)
{
	auto anim = owner_->GetAnimation();
	const auto playAnim = anim->GetPlayAnim();

	if (!hasHit_ && playAnim.step >= ATTACK_FRAME)
	{
		//衝突(攻撃)
		CollisionNormalAttack();

		//攻撃音①
		SoundManager::GetInstance().Play(SoundManager::SRC::ATK_SE1, Sound::TIMES::FORCE_ONCE);

		hasHit_ = true;
	}

	if (anim->IsEnd())
	{
		isAttack_ = false;
	}
}

void PlayerAttack::CollisionNormalAttack(void)
{
	const auto enemies = owner_->GetEnemies();
	if (!enemies) return;

	//敵の当たり判定とサイズ
	const VECTOR& pos = owner_->GetPos();
	const Quaternion& rot = owner_->GetRotation();

	//攻撃の方向(プレイヤーの前方)
	VECTOR forward = rot.GetForward();
	//攻撃の開始位置と終了位置
	VECTOR attackPos = VAdd(pos, VScale(forward, ATTACK_FORWARD));

	for (const auto& enemy : *enemies)
	{
		if (!enemy || !enemy->IsAlive()) continue;

		VECTOR enemyPos = enemy->GetCollisionPos();
		float enemyRadius = enemy->GetCollisionRadius();

		//球体同士の当たり判定
		if (AsoUtility::IsHitSpheres(attackPos, ATTACK_RADIUS, enemyPos, enemyRadius))
		{
			VECTOR dir = VSub(enemyPos, attackPos);
			dir = VNorm(dir);	//正規化
			VECTOR hitPos = VAdd(attackPos, VScale(dir, ATTACK_RADIUS));

			StartHitStop();

			EffectSword(attackPos);

			SetPosPlayingEffekseer3DEffect(effectSwordPlayId_, hitPos.x, hitPos.y, hitPos.z);

			enemy->Damage(normalAttack_);
		}
	}
}

void PlayerAttack::StartHitStop(void)
{
	isHitStop_ = true;
	hitStopFrame_ = HIT_STOP;

	//アニメーション停止
	owner_->GetAnimation()->SetStop(true);
}

void PlayerAttack::EffectSword(const VECTOR& pos)
{
	//エフェクト再生
	effectSwordPlayId_ = PlayEffekseer3DEffect(effectSwordResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectSwordPlayId_, EFFECT_SCALE, EFFECT_SCALE, EFFECT_SCALE);
}
