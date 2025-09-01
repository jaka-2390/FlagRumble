#pragma once
#include "../EnemyBase.h"


class EnemyThorn : public EnemyBase
{
public:
	EnemyThorn();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//ドロップアイテム
	virtual Item::TYPE GetDropItemType() const override;
};
