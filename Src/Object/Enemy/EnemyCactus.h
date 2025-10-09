#pragma once
#include "../EnemyBase.h"

class EnemyCactus : public EnemyBase
{

public:

	static constexpr  float SPEED = 6.0f;
	static constexpr  int HP = 4;

	EnemyCactus();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};