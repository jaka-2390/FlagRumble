#pragma once
#include "../EnemyBase.h"

class EnemyMimic : public EnemyBase
{

public:

	static constexpr  float SPEED = 4.0f;
	static constexpr  int HP = 5;

	EnemyMimic();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};