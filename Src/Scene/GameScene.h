#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"

class Stage;
class EnemyBase;
class SkyDome;
class Player;
class MiniMap;
class Camera;

class GameScene : public SceneBase
{

public:

	static constexpr int ENCOUNT = 300;		//エンカウンタ
	static constexpr int ENEMY_MAX = 200;	//最大出現数
	static constexpr int ENE_ENC = 30;		//最大許容量
	static constexpr int BORN_DIR = 3;		//敵の出現方向
	static constexpr int STAGE_WIDTH = 20000; //ステージの全体
	static constexpr int STAGE_LANGE = 10000;	//ステージの幅

	static constexpr int LV_MAX = 100;		//木のレベル最大
	static constexpr int LV_OLD = 75;		//木の成長段階
	static constexpr int LV_ADULT = 50;		//木の成長段階
	static constexpr int LV_KID = 25;		//木の成長段階
	static constexpr int HP_ZERO = 0;		//木の体力0

	static constexpr int BOSS_WAIT = 0;		//ボス出現待機
	static constexpr int BOSS_ON = 1;		//ボス出現可能
	static constexpr int BOSS_OFF = 2;		//ボス出現不可

	//UI関係-----------------------------------------------------
	//-------------------------------------------------------------------

	static constexpr int GAME_HEIGHT_1 = 80;			//ゲーム開始時の注意書き

	//画像サイズ
	static constexpr float IMG_GAME_UI_1_SIZE = 0.5;	//imgGameUi1_のサイズ
	static constexpr float IMG_OPEGEAR_UI_SIZE = 0.8;	//imgOpeGear_のサイズ
	static constexpr float PAUSE_IMG_UI_SIZE = 0.65;	//pauseImg_のサイズ

	//ポーズメニュー関連
	static constexpr int PAUSE_MENU_ITEM_COUNT = 4;						//ポーズメニューの数
	static constexpr int PAUSE_MENU_DOWN = 1;							//下に移動
	static constexpr int PAUSE_MENU_UP = PAUSE_MENU_ITEM_COUNT - 1;		//上に移動（+3 の代わり）

	//フェード系
	static constexpr int AUTO_FADE = 240;				//自動フェード
	static constexpr int FLASH = 45;					//点滅
	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	//設定系
	static constexpr int UI_GEAR = 100;					//imgOpeGear_のX,Yの場所

	static constexpr int UI_PAUSE_IMG_HEIGHT = 150;				//pauseImg_の高さ

	static constexpr int UI_WIDTH_PAUSE_1 = 160;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_2 = 200;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_3 = 240;				//UIを調整する

	static constexpr int UI_HEIGHT_PAUSE_1 = 350;				//１個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_2 = 470;				//２個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_3 = 590;				//３個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_4 = 710;				//４個目のUIの高さ

	static constexpr int UI_ATTACK_X = 10;						//攻撃の文字のX座標
	static constexpr int UI_NORMAL_ATTACK_Y = 450;				//通常攻撃のY座標
	static constexpr int UI_SLASH_ATTACK_Y = 500;				//スラッシュのY座標
	static constexpr int UI_EX_ATTACK_Y = 550;					//回転斬りのY座標

	static constexpr int BACK_PAUSE_WIDTH = 1600;				//ポーズに戻るときのENTERのX
	static constexpr int BACK_PAUSE_HEIGHT = 1020;				//ポーズに戻るときのENTERのY

	//-------------------------------------------------------------------

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	GameScene(void);	// コンストラクタ
	~GameScene(void);	// デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void DrawMiniMap(void);

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const;	//enemyの情報(pos)を見る

private:

	int cnt;

	void EnemyCreate(void);

	bool PauseMenu(void);

	std::unique_ptr<Stage> stage_;		// ステージ
	std::unique_ptr<SkyDome> skyDome_;	// スカイドーム
	std::shared_ptr<Player> player_;	// プレイヤー
	std::unique_ptr<MiniMap> map_;		//ミニマップ
	std::shared_ptr<Camera> camera_;	//カメラ

	int enemyModelId_;
	int imgGameUi1_;
	int uiDisplayFrame_;	//カウンタ

	bool uiFadeStart_ = false;
	int uiFadeFrame_ = 0;

	// 設定開く
	int imgOpeGear_;

	std::vector<std::shared_ptr<EnemyBase>> enemys_;
	int enCounter;//敵の出現頻度

	int isB_;

	// ポーズ
	bool isPaused_;           // ポーズ中かどうか
	int pauseSelectIndex_;    // ポーズメニューの選択項目（上下選択）
	int pauseExplainImgs_[2];

	enum class PauseState
	{
		Menu,        // 通常のポーズメニュー
		ShowControls,// 操作説明画面
		ShowItems    // アイテム概要画面
	};

	PauseState pauseState_ = PauseState::Menu;
	int  pauseImg_;
};