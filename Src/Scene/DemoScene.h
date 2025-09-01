#pragma once
#include <memory>
#include "SceneBase.h"

#include "string"
class SceneManager;
class DemoScene:public SceneBase
{
public:
	enum class TXT
	{
		T1,
		T2,
		T3,
		T4,
		T5,
		T6,
		T7,
		T8,
		T9,
		T10,
		T11,
		T12,
		T13,
		T14,
		T15,
		T16,
		T17,
		T18,
		T19,
		T20,
		T21,
		MAX
	};

	DemoScene(void);     //コンストラクタ
	~DemoScene(void);    //デストラクタ

	void Init(void)override;      //初期化処理
	void Update(void)override;    //更新処理
	void Draw(void)override;      //描画処理
	void Release(void)override;
private:
	SceneManager* sceneManager_;   //シーンマネジャーのポインタ格納領域
	int demoSound_;

	int tree_;
	int player_;
	int water_;
	int god_[5];
	int gNo_;
	int waku_;
	int tri_;

	int txt_;
	int cnt_;
	int now_;
	int old_;
};