#pragma once
#include <memory>
#include "../Object/Common/Transform.h"
#include "../Common/Quaternion.h"
class SceneManager;
class Planet;
class Player;
class EnemyBase;

class GravityManager
{

public:

	// 重力方向が完全に変わるまでの時間
	static constexpr float TIME_CHANGE_DIR = 1.0f;

	// 回転力
	static constexpr float DEFAULT_POW_SLERP = 0.08f;

	// 状態
	enum class STATE
	{
		IDLE,
		STAGE_CHANGE
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static GravityManager& GetInstance(void);

	void Init(void);

	void Update(void);
	void UpdateIdle(void);
	void UpdateChangeStage(void);

	void Destroy(void);

	// 重力方向や、正方向の計算
	void Calculate(void);

	// 現在プレイしている惑星
	std::weak_ptr<Planet> GetActivePlanet(void) const;

	// 現在プレイしている惑星を変更する
	void ChangeActivePlanet(std::weak_ptr<Planet> planet);

	// 操作プレイヤーをセット
	void SetPlayer(std::shared_ptr<Player> player);
	//エネミー
	void SetEnemyBase(std::shared_ptr<EnemyBase> enemy);

	const Transform& GetTransform(void) const;
	
	// 重力方向を取得
	VECTOR GetDirGravity(void) const;

	// 重力方向の反対方向を取得
	VECTOR GetDirUpGravity(void) const;

	// 重力の強さを取得
	float GetPower(void) const;

	// 惑星切り替え前の回転情報
	Quaternion GetPreQuaRot(void) const;

	// プレイヤーと足元衝突した法線
	void SetLastHitNormal(const VECTOR& normal);

	// プレイヤーと足元衝突した位置
	void SetLastHitPos(const VECTOR& pos);
	VECTOR GetLastHitPos(void) const;

protected:

	// 静的インスタンス
	static GravityManager* instance_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	std::shared_ptr<EnemyBase>enemyBase_;

	// 現在プレイしている惑星
	std::weak_ptr<Planet> activePlanet_;

	Transform transform_;

	// 状態
	STATE state_;

	// 重力方向
	VECTOR dirGravity_;
	VECTOR preDirGravity_;

	// 重力方向の反対方向
	VECTOR dirUpGravity_;

	// プレイヤーと足元衝突した法線
	VECTOR lastHitNormal_;

	// プレイヤーと足元衝突した位置
	VECTOR lastHitPos_;

	// 更新ステップ
	float step_;

	// 回転力
	float slerpPow_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	GravityManager(void);
	GravityManager(const GravityManager& manager) = default;
	~GravityManager(void) = default;

	// 状態遷移
	void ChangeState(STATE state);

	// 重力方向を計算
	VECTOR CalcDirGravity(void) const;

};
