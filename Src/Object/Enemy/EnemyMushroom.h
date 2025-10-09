#pragma once
#include "../EnemyBase.h"

class EnemyMushroom : public EnemyBase
{

public:

	static constexpr  float SPEED = 5.0f;
	static constexpr  int HP = 3;

	EnemyMushroom();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};