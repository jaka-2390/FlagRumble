#pragma once
#include "../EnemyBase.h"


class EnemyOnion : public EnemyBase
{
public:
	EnemyOnion();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};
