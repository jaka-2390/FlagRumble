#pragma once
#include <memory>
#include <vector>
#include "../Object/EnemyBase.h"
#include "SceneBase.h"

class Stage;
class SkyDome;
class Player;
class MiniMap;
class Camera;
class FlagManager;

//スポーン場所
struct SpawnArea
{
	VECTOR center;     //中心座標
	float radius;      //半径
	bool triggered;    //もうスポーン済みか（1回きりの場合）
};

class GameScene : public SceneBase
{

public:

	static constexpr int ENCOUNT = 60;				//エンカウンタ
	static constexpr int ENEMY_MAX = 3;				//最大出現数
	static constexpr int ENE_ENC = 30;				//最大許容量
	static constexpr int BORN_DIR = 3;				//敵の出現方向
	static constexpr int STAGE_WIDTH = 20000;		//ステージの全体
	static constexpr int STAGE_LANGE = 10000;		//ステージの幅
	static constexpr int HALF_DIVISOR = 2;			//÷2
	static constexpr int DOUBLE_MULTIPLIER = 2;		//×2
	static constexpr float SPAWN_RADIUS = 100.0f;	//スポーン場所

	static constexpr VECTOR CUCTUS_POS = { 2500.0f, 254.0f, 4700.0f };
	static constexpr VECTOR BOSS_POS = { 80.0f, 254.0f, 2300.0f };

	//UI関係-----------------------------------------------------
	//-------------------------------------------------------------------

	static constexpr int GAME_HEIGHT_1 = 80;			//ゲーム開始時の注意書き

	//ミニマップ
	static constexpr float MINIMAP_RANGE = 30000.0f;	//描画するマップの範囲
	static constexpr int   MINIMAP_SIZE = 300;			//マップのサイズ

	//画像サイズ
	static constexpr float IMG_GAME_UI_1_SIZE = 0.5;	//imgGameUi1_のサイズ
	static constexpr float IMG_OPEGEAR_UI_SIZE = 0.8;	//imgOpeGear_のサイズ
	static constexpr float PAUSE_IMG_UI_SIZE = 0.65;	//pauseImg_のサイズ

	//ポーズメニュー関連
	static constexpr double TITLE_FONT_SCALE = 5.0;						//フォントのスケール
	static constexpr double ENTER_FONT_SCALE = 2.5;						//Enterのスケール
	static constexpr double ATTACK_FONT_SCALE = 2.0;					//攻撃のスケール
	static constexpr int PAUSE_MENU_CONTROLS = 1;						//操作説明
	static constexpr int PAUSE_MENU_TITLE = 2;							//タイトルへ
	static constexpr int PAUSE_MENU_ITEM_COUNT = 3;						//ポーズメニューの数
	static constexpr int PAUSE_MENU_DOWN = 1;							//下に移動
	static constexpr int PAUSE_MENU_UP = PAUSE_MENU_ITEM_COUNT - 1;		//上に移動

	//フェード系
	static constexpr int AUTO_FADE = 240;				//自動フェード
	static constexpr int FLASH = 45;					//点滅
	static constexpr double FLASH_RATE = 2.0;			//点滅レート
	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	//設定系
	static constexpr int UI_GEAR = 100;					//imgOpeGear_のX,Yの場所

	//ポーズメニュー
	static constexpr int PAUSE_BG_ALPHA = 200;			//ポーズメニューのアルファ値
	static constexpr int PAUSE_WHITE_ALPHA = 150;

	static constexpr int UI_PAUSE_IMG_HEIGHT = 150;		//pauseImg_の高さ

	static constexpr int UI_WIDTH_PAUSE_1 = 160;		//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_2 = 200;		//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_3 = 240;		//UIを調整する

	static constexpr int UI_HEIGHT_PAUSE_1 = 350;		//１個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_2 = 470;		//２個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_3 = 590;		//３個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_4 = 710;		//４個目のUIの高さ

	static constexpr int UI_ATTACK_X = 10;				//攻撃の文字のX座標
	static constexpr int UI_NORMAL_ATTACK_Y = 450;		//通常攻撃のY座標
	static constexpr int UI_SLASH_ATTACK_Y = 500;		//スラッシュのY座標
	static constexpr int UI_EX_ATTACK_Y = 550;			//回転斬りのY座標

	static constexpr int BACK_PAUSE_WIDTH = 1600;		//ポーズに戻るときのENTERのX
	static constexpr int BACK_PAUSE_HEIGHT = 1020;		//ポーズに戻るときのENTERのY

	static constexpr float FRAME_TIME = 1.0f / 60.0f;	//1フレーム = 1/60秒

	//フラッグ
	static constexpr float GAUGE_INCREMENT = 0.5f;		//flagゲージの上昇速度(フレーム単位)
	static constexpr float FLAG_RADIUS = 100.0f;		//フラッグ範囲円の半径

	const float CACTUS_SPAWN_INTERVAL = 17.0f;			//SABOのインターバル
	const float SPAWN_INTERVAL = 12.0f;					//敵のインターバル

	//クリアゲージ
	static constexpr int GAUGE_X = 20;                //左上X位置
	static constexpr int GAUGE_Y = 20;                //左上Y位置
	static constexpr int GAUGE_WIDTH = 200;           //ゲージの全幅
	static constexpr int GAUGE_HEIGHT = 20;           //ゲージの高さ

	//-------------------------------------------------------------------

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	GameScene(void);	//コンストラクタ
	~GameScene(void);	//デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void DrawMiniMap(void);

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const;	//enemyの情報(pos)を見る

private:

	int cnt;

	void EnemyCreate(int count);

	void EnemyCreateRand(VECTOR flagPos, int count, EnemyBase::TYPE type);

	void SpawnBoss(void);

	void SpawnCactus(void);

	bool PauseMenu(void);

	void ClearCheck(void);

	std::vector<SpawnArea> spawnAreas_;	//スポーン場所
	std::unique_ptr<Stage> stage_;		//ステージ
	std::unique_ptr<SkyDome> skyDome_;	//スカイドーム
	std::shared_ptr<Player> player_;	//プレイヤー
	std::unique_ptr<MiniMap> map_;		//ミニマップ
	std::shared_ptr<Camera> camera_;	//カメラ
	std::shared_ptr<FlagManager> flagManager_;		//フラッグ

	int enemyModelId_;
	int uiDisplayFrame_;	//カウンタ

	bool uiFadeStart_ = false;
	int uiFadeFrame_ = 0;

	//設定開く
	int imgOpeGear_;

	std::vector<std::shared_ptr<EnemyBase>> enemys_;
	int enCounter;//敵の出現頻度

	//ポーズ
	bool isPaused_;           //ポーズ中かどうか
	int pauseSelectIndex_;    //ポーズメニューの選択項目（上下選択）
	int pauseExplainImgs_[2];

	enum class PauseState
	{
		Menu,        //通常のポーズメニュー
		ShowControls,//操作説明画面
		ShowItems    //アイテム概要画面
	};

	//旗関連
	float flagRadius_ = 100.0f;       //接近判定の距離

	int lastSpawnTime_;  //最後に敵を出現させた時間

	bool bossSpawned_ = false;

	float SpawnTimer_ = 0.0f;
	float cactusSpawnTimer_ = 0.0f;

	PauseState pauseState_ = PauseState::Menu;
	int  pauseImg_;

};