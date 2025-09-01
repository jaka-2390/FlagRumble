#pragma once
#include <memory>
#include <map>
#include "Common/Transform.h"
class ResourceManager;
class Planet;
class Player;

class Stage
{

public:

	// ステージ名
	enum class NAME
	{
		MAIN_PLANET,
	};

	// コンストラクタ
	Stage(Player& player);

	// デストラクタ
	~Stage(void);

	void Init(void);
	void Update(void);
	void Draw(void)const;

	// ステージ変更
	void ChangeStage(NAME type);

	// 対象ステージを取得
	std::weak_ptr<Planet> GetPlanet(NAME type);

	std::vector<VECTOR> GetPlanetsPositions() const;

private:

	// シングルトン参照
	ResourceManager& resMng_;

	Player& player_;

	// ステージアクティブになっている惑星の情報
	NAME activeName_;
	std::weak_ptr<Planet> activePlanet_;

	// 惑星
	std::map<NAME, std::shared_ptr<Planet>> planets_;

	// 空のPlanet
	std::shared_ptr<Planet> nullPlanet = nullptr;

	float step_;

	// 最初の惑星
	void MakeMainStage(void);

	// ゴールスター
	void MakeGoalStar(void);

	// メインステージ
	void MainStage(void);

};
