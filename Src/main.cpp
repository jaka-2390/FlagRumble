#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <memory>
#include <DxLib.h>
#include "Object/Player.h"
#include "Application.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// WinMain関数
//---------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*int* test = new int();
	delete test;

	Player* p = new Player();
	delete p;*/

	// ①実体の作成
	int test1 = 0;

	// ②生ポインタの作成
	int* test2 = new int(0);
	delete test2;

	// ③ユニークポインタ
	std::unique_ptr<int> testI = std::make_unique<int>(0);

	// ④ユニークポインタの注意点（コピー禁止）
	//std::unique_ptr<int> testI2 = testI;

	// ⑤ユニークポインタの注意点（ムーブ）
	std::unique_ptr<int> testI2 = std::move(testI);
	if (testI == nullptr)
	{
		testI = std::make_unique<int>(5);
	}

	// インスタンスの生成
	Application::CreateInstance();

	// インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		// 初期化失敗
		return -1;
	}

	// 実行
	instance.Run();

	// 解放
	instance.Destroy();

	return 0;

}
