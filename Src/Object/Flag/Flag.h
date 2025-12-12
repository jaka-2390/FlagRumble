#pragma once
#include "../FlagBase.h"

class Flag : public FlagBase
{

public:

	Flag(VECTOR pos, ENEMY_TYPE type, STATE state);	//コンストラクタ
	~Flag(void);						//デストラクタ

	//void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies) override;

private:

};