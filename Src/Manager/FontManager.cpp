#include <DxLib.h>
#include "FontManager.h"

int FontManager::kakuG_ = -1;

void FontManager::Init()
{
    kakuG_ = CreateFontToHandle("Data/Fonts/HGRSGU.TTC", 28, 1);
}

void FontManager::DrawStringEx(int x, int y, const char* text, int color, int size)
{
    if (kakuG_ != -1) {
        // サイズごとにフォントを作る（ハンドル管理簡易版）
        int fontHandle = CreateFontToHandle("Data/Fonts/HGRSGU.TTC", size, 10);
        DrawStringToHandle(x, y, text, color, fontHandle);
        DeleteFontToHandle(fontHandle); // 使用後削除
    }
}