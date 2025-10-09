#pragma once
#include "../EnemyBase.h"

class EnemyThorn : public EnemyBase
{

public:

	static constexpr  float SPEED = 2.0f;
	static constexpr  int HP = 10;

	EnemyThorn();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};