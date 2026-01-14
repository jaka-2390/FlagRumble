#pragma once
#include <memory>
#include <chrono>

// 推奨しませんが、どうしても使いたい方は
#define mainCamera SceneManager::GetInstance().GetCamera().lock()

class SceneBase;
class Fader;
class Camera;

class SceneManager
{

public:

	//背景色
	static constexpr int BG_COLOR_R = 0;
	static constexpr int BG_COLOR_G = 139;
	static constexpr int BG_COLOR_B = 139;

	//ライト方向
	static constexpr float LIGHT_DIR_X = 0.3f;
	static constexpr float LIGHT_DIR_Y = -3.0f;
	static constexpr float LIGHT_DIR_Z = 0.3f;

	//環境光
	static constexpr float AMBIENT_LIGHT = 0.3f;

	//フォグ
	static constexpr int FOG_COLOR = 5;
	static constexpr float FOG_START = 10000.0f;
	static constexpr float FOG_END = 20000.0f;

	//ナノ秒
	static constexpr double NANOSECONDS_PER_SECOND = 1000000000.0;

	//フレーム
	static constexpr float FRAME_TIME = 1.0f / 60.0f;	//1フレーム = 1/60秒
	static constexpr float RESET_TIME = 0.016f;			//60FPS の 1 フレーム時間
	static constexpr int LOADING_FRAMES = 60;			//約1秒（60FPS）

	//フォントサイズ
	static constexpr int TITLE_FONT_SIZE = 15;	//タイトルのフォントサイズ
	static constexpr int DEMO_FONT_SIZE = 55;	//チュートリアルのフォントサイズ

	//60FPS固定の定数
	static constexpr float DEFAULT_FPS = 60.0f;

	//シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		DEMO,
		GAME,
		OVER,
		CLEAR
	};
	
	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	// カメラの取得
	std::weak_ptr<Camera> GetCamera(void) const;

private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// フェード
	std::unique_ptr<SceneBase> scene_;

	// 各種シーン
	std::unique_ptr<Fader> fader_;

	// カメラ
	std::shared_ptr<Camera> camera_;

	// シーン遷移中判定
	bool isSceneChanging_;

	bool isLoading_ = false;
	int loadingTimer_ = 0;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;
	
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// コピーコンストラクタも同様
	SceneManager(const SceneManager& manager) = default;
	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};
