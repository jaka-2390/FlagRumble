#include "PlayerAttack.h"

PlayerAttack::PlayerAttack(Player* owner)
{
}

void PlayerAttack::Init(void)
{
	isAttack_ = false;
	hasHit_ = false;
	normalAttack_ = NORMAL_ATTACK;
	isHitStop_ = false;
	hitStopFrame_ = 0;
}

void PlayerAttack::Update(void)
{
}

bool PlayerAttack::IsAttacking() const
{
    return false;
}

void PlayerAttack::ProcessInput()
{
}

void PlayerAttack::UpdateNormalAttack()
{
}

void PlayerAttack::CollisionNormalAttack()
{
}

void PlayerAttack::StartHitStop()
{
}

void PlayerAttack::EffectSword(const VECTOR& pos)
{
}
