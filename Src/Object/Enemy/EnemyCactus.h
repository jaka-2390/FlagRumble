#pragma once
#include "../EnemyBase.h"

class EnemyCactus : public EnemyBase
{
public:
	EnemyCactus();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};
