#pragma once
#include "../EnemyBase.h"

class EnemyOnion : public EnemyBase
{

public:

	static constexpr  float SPEED = 3.0f;
	static constexpr  int HP = 2;

	EnemyOnion();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};