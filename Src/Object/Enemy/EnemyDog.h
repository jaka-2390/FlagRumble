#pragma once
#include "../EnemyBase.h"

class EnemyDog : public EnemyBase
{

public:

	static constexpr int   ANIM_IDLE_INDEX = 0;
	static constexpr int   ANIM_RUN_INDEX = 1;
	static constexpr int   ANIM_ATTACK_INDEX = 2;
	static constexpr int   ANIM_KICK_INDEX = 3;
	static constexpr int   ANIM_DAMAGE_INDEX = 4;
	static constexpr int   ANIM_DEATH_INDEX = 5;
	static constexpr int   ANIM_DEFENSE_INDEX = 7;
	static constexpr int   ANIM_DEFENSE_HIT_INDEX = 8;

	static constexpr  float SPEED = 7.0f;
	static constexpr  int HP = 8;

	EnemyDog();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};