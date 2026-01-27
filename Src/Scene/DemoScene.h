#pragma once
#include <memory>
#include <vector>
#include "../Object/EnemyBase.h"
#include "../Manager/InputManager.h"
#include "SceneBase.h"

class Stage;
class SkyDome;
class Player;
class Camera;
class FlagManager;

class DemoScene :public SceneBase
{

	//スポーン場所
	struct SpawnArea
	{
		VECTOR center;     //中心座標
		float radius;      //半径
		bool triggered;    //もうスポーン済みか（1回きりの場合）
	};

	enum class STATE
	{
		NONE,
		MOVE,
		ATTACK,
		FLAG,
		SABO,
		FINISH
	};

	public:

		static constexpr int HALF_DIVISOR = 2;				//÷2
		static constexpr int DOUBLE_MULTIPLIER = 2;			//×2
		static constexpr VECTOR DOG_POS = { -250.0f, 254.0f, 1000.0f };	//敵の初期位置
		static constexpr VECTOR CACTUS_POS = { -250.0f, 254.0f, 2000.0f };	//敵の初期位置
		static constexpr float SPAWN_RADIUS = 100.0f;//スポーン場所

		//UI関係-----------------------------------------------------
		//-------------------------------------------------------------------

		static constexpr int GAME_HEIGHT_1 = 80;			//ゲーム開始時の注意書き

		//画像サイズ
		static constexpr float IMG_GAME_UI_1_SIZE = 0.5;	//imgGameUi1_のサイズ
		static constexpr float IMG_OPEGEAR_UI_SIZE = 0.8;	//imgOpeGear_のサイズ
		static constexpr float PAUSE_IMG_UI_SIZE = 0.65;	//pauseImg_のサイズ

		//ポーズメニュー関連
		static constexpr int PAUSE_BG_ALPHA = 200;			//ポーズメニューのアルファ値
		static constexpr int PAUSE_WHITE_ALPHA = 150;
		static constexpr float TITLE_FONT_SCALE = 5.0f;		//フォントのスケール
		static constexpr float ENTER_FONT_SCALE = 2.5f;
		static constexpr int PAUSE_MENU_CONTROLS = 1;		//操作説明
		static constexpr int PAUSE_MENU_TITLE = 2;			//タイトルへ
		static constexpr int PAUSE_MENU_ITEM_COUNT = 3;						//ポーズメニューの数
		static constexpr int PAUSE_MENU_DOWN = 1;							//下に移動
		static constexpr int PAUSE_MENU_UP = PAUSE_MENU_ITEM_COUNT - 1;		//上に移動

		//フェード系
		static constexpr int AUTO_FADE = 240;				//自動フェード
		static constexpr int FLASH = 45;					//点滅
		static constexpr float FLASH_RATE = 2.0;			//点滅レート
		static constexpr int ONE_SECOND_FRAME = 60;			//1秒
		static constexpr float FRAME_TIME = 1.0f / 60.0f;	//1フレーム = 1/60秒

		//設定系
		static constexpr int UI_GEAR = 100;					//imgOpeGear_のX,Yの場所

		static constexpr int UI_PAUSE_IMG_HEIGHT = 150;		//pauseImg_の高さ

		static constexpr int UI_WIDTH_PAUSE_1 = 160;		//UIを調整する
		static constexpr int UI_WIDTH_PAUSE_2 = 200;		//UIを調整する
		static constexpr int UI_WIDTH_PAUSE_3 = 240;		//UIを調整する
		static constexpr int UI_WIDTH_PAUSE_4 = 380;		//UIを調整する

		static constexpr int UI_HEIGHT_PAUSE_1 = 350;		//１個目のUIの高さ
		static constexpr int UI_HEIGHT_PAUSE_2 = 470;		//２個目のUIの高さ
		static constexpr int UI_HEIGHT_PAUSE_3 = 590;		//３個目のUIの高さ
		static constexpr int UI_HEIGHT_PAUSE_4 = 710;		//４個目のUIの高さ

		static constexpr int BACK_PAUSE_WIDTH = 1600;		//ポーズに戻るときのENTERのX
		static constexpr int BACK_PAUSE_HEIGHT = 1020;		//ポーズに戻るときのENTERのY

		//フラッグ
		static constexpr float GAUGE_INCREMENT = 0.5f;		//flagゲージの上昇速度(フレーム単位)
		static constexpr float FLAG_RADIUS = 100.0f;		//フラッグ範囲円の半径
		static constexpr VECTOR FLAG_POS = { -250.0f, 254.0f, 1000.0f };		//フラッグの位置

		//コントローラー
		static constexpr float DEAD_ZONE = 0.2f;

		//サボテンのインターバル
		static constexpr float CACTUS_SPAWN_INTERVAL = 20.0f;

		//メッセージ
		static constexpr float MESSAGE_DISPLAY_SEC = 1.5f;	//メッセージの表示時間
		static constexpr int MESSAGE_POS_X = 50;			//メッセージの位置X
		static constexpr int MESSAGE_POS_Y = 600;			//メッセージの位置Y
		static constexpr int MESSAGE_OFFSET = 40;			//メッセージの位置調整
		static constexpr float MESSAGE_FONT_SCALE = 2.0f;

		//スキップ
		static constexpr float SKIP_TIME = 2.0f;
		static constexpr int SKIP_UI_OFFSET = 50;			//SkipUIの位置調整用
		static constexpr int GAUGE_INNER_RADIUS = 20;		//Skipゲージのサイズ
		static constexpr int GAUGE_OUTER_RADIUS = 35;
		static constexpr int SKIP_TEX_OFSET_X = 67;			//SkipTexの位置調整
		static constexpr int SKIP_TEX_OFSET_Y = 25;
		static constexpr int CIRCLE_SEGMENTS = 64;			//円弧分割数

		//クリアゲージ
		static constexpr int GAUGE_X = 20;                //左上X位置
		static constexpr int GAUGE_Y = 20;                //左上Y位置
		static constexpr int GAUGE_WIDTH = 200;           //ゲージの全幅
		static constexpr int GAUGE_HEIGHT = 20;           //ゲージの高さ

		//-------------------------------------------------------------------

		//色
		int white = 0xffffff; //白
		int black = 0x000000; //黒
		int gray = 0x808080; //灰
		int red = 0xff0000;	  //赤
		int green = 0x00ff00; //緑
		int blue = 0x0000ff;  //青
		int yellow = 0xffff00;//黄
		int purpl = 0x800080; //紫

		DemoScene(void);	//コンストラクタ
		~DemoScene(void);	//デストラクタ

		void Init(void) override;
		void Update(void) override;
		void Draw(void) override;
		void Release(void) override;

		const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const;	//enemyの情報(pos)を見る

private:

		int cnt;

		void UpdateMove();
		void UpdateAttack();
		void UpdateFlag();
		void UpdateSabo();
		void UpdateFinish();

		void DrawPause();

		void DrawMessage();

		void DrawSkip(int cx, int cy, float rate, int rOuter, int rInner, int color);

		void EnemyCreate(VECTOR flagPos, int count, EnemyBase::TYPE type);

		void SpawnCactus(void);

		void MessageTime(void);

		bool PauseMenu(void);

		std::vector<SpawnArea> spawnAreas_;	//スポーン場所
		std::unique_ptr<Stage> stage_;		//ステージ
		std::unique_ptr<SkyDome> skyDome_;	//スカイドーム
		std::shared_ptr<Player> player_;	//プレイヤー
		std::shared_ptr<Camera> camera_;	//カメラ
		std::shared_ptr<FlagManager> flagManager_;		//フラッグ

		int enemyModelId_;
		int uiDisplayFrame_;	//カウンタ

		bool uiFadeStart_ = false;
		int uiFadeFrame_ = 0;

		//設定開く
		int imgOpeGear_;		//キーボード
		int imgOpeGearCon_;		//コントローラー

		std::vector<std::shared_ptr<EnemyBase>> enemys_;
		int enCounter;//敵の出現頻度

		bool isDog_ = false;

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
		float flagRadius_ = 100.0f;		//接近判定の距離

		int lastSpawnTime_;				//最後に敵を出現させた時間

		float cactusSpawnTimer_ = 0.0f;

		STATE state_ = STATE::MOVE;
		bool stateFinish_ = false;
		bool spawnCactus_ = false;

		float messageTimer_ = 0.0f;		//メッセージ表示用タイマー
		bool messageActive_ = true;		//メッセージを表示中かどうか

		float skipSecond_ = 0.0f;		//スキップまでの時間
		bool skipActive_ = false;		//スキップ中かどうか

		PauseState pauseState_ = PauseState::Menu;
		int  pauseImg_;

		InputManager& ins = InputManager::GetInstance();

};