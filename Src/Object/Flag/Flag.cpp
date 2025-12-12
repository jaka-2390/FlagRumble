#include "Flag.h"

Flag::Flag(VECTOR pos, ENEMY_TYPE type, STATE state) : FlagBase(pos, type, state)
{
}

Flag::~Flag(void)
{
	//ƒ‚ƒfƒ‹‚Ìíœ
	MV1DeleteModel(flag_.modelId);
}

//void Flag::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
//{
//	FlagBase::Update(playerPos, enemies);
//}
