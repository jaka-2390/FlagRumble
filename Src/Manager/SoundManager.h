#pragma once
#include <map>
#include "Sound.h"

class SoundManager
{

public:

	//リソース名
	enum class SRC
	{
		TITLE_BGM,		//タイトルシーンのBGM
		DEMO_BGM,		//でもシーンのBGM
		GAME_BGM,		//ゲームシーンのBGM
		GAMEOVER_BGM,	//ゲームオーバーBGM
		GAMECLEAR_BGM,	//ゲームクリアBGM
		LEVEL_UP_SE,	//木のレベルアップSE
		GETWATER_SE,	//水を得たときのSE
		ATK_SE1,		//アタックのSE1
		ATK_SE2,		//アタックのSE2
		ATK_SE3,		//アタックのSE3
		SPEEDUP_SE,		//スピードアップSE
		POWERUP_SE,		//パワーアップSE
		MUTEKI_SE,		//木が無敵
		HEAL_SE,		//回復SE
		E_DOWN_SE,		//敵死亡SE
		P_DOWN_SE,		//プレイヤー死亡SE
		P_DAMAGE_SE,	//プレイヤーダメージSE
		T_DAMAGE_SE,	//木がダメージSE
		E_DAMAGE_SE,	//敵がダメージSE
		SET_SE,			//決定のSE
		WARNING_SE,		//警告音SE
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static SoundManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	bool Play(SRC src, Sound::TIMES times);	//二次元音源用
	bool Play(SRC src, Sound::TIMES times, VECTOR pos, float radius);	//三次元音源用

	void Stop(SRC src);	//音源を停止する

	bool CheckMove(SRC src);

	void ChengeVolume(SRC src, float per);	//per 0.0～1.0でパーセントを設定する

private:

	// 静的インスタンス
	static SoundManager* instance_;

	std::map<SRC, Sound> soundMap_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SoundManager(void) = default;

	// デストラクタも同様
	~SoundManager(void) = default;

};


