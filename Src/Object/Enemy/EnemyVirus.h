#pragma once
#include "../EnemyBase.h"

class EnemyVirus : public EnemyBase
{

public:

	static constexpr  float SPEED = 8.0f;
	static constexpr  int HP = 1;

	EnemyVirus();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};