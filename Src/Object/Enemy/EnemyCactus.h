#pragma once
#include "../EnemyBase.h"

class FlagManager;

class EnemyCactus : public EnemyBase
{

public:

	static constexpr  float SPEED = 6.0f;
	static constexpr  int HP = 6;

	static constexpr  float FLAG_CHANGE = 3.0f;

	EnemyCactus();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//
	void ChasePlayer(void) override;

	void SetFlagManager(FlagManager* manager);

private:

	FlagManager* flagManager_ = nullptr;

	float captureTimer_ = 0.0f;

};