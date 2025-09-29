#pragma once
#include "../EnemyBase.h"


class EnemyDog : public EnemyBase
{
public:
	EnemyDog();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//ドロップアイテム
	//Item::TYPE GetDropItemType() const override;
};