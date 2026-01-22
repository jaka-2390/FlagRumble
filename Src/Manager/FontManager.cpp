#include <DxLib.h>
#include "FontManager.h"

int FontManager::kakuG_ = -1;
int FontManager::currentSize_ = -1;

void FontManager::Init()
{
    //Windowsにフォントがインストール済みならこっちを使う
    //akazuki_ = CreateFontToHandle("07あかずきんポップ Heavy", FONT_SIZE_AKAZUKI, 1);

    //フォントファイルをWindowsの一時的なリソースとして登録する
    const char* fontPath = "Data/Font/HGRSGU.TTC";

    //FR_PRIVATEを指定すると、このアプリが動いている間だけ有効になり、
    //アプリ終了時に自動的に登録が解除されます。
    AddFontResourceEx(fontPath, FR_PRIVATE, NULL);
}

/// <summary>
/// 登録したフォントを使用する
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="text">出力する内容</param>
/// <param name="color">色</param>
/// <param name="size">サイズ</param>
void FontManager::DrawStringEx(int x, int y, const char* text, int color, int size)
{
    //サイズが変わった時だけ、古いハンドルを捨てて作り直す
    if (size != currentSize_)
    {
        if (kakuG_ != -1) DeleteFontToHandle(kakuG_);

        kakuG_ = CreateFontToHandle("HGP創英角ゴシックUB", size, 3, DX_FONTTYPE_ANTIALIASING_4X4);
        currentSize_ = size;
    }

    //描画
    DrawStringToHandle(x, y, text, color, kakuG_);
}
void FontManager::Release(void)
{
    if (kakuG_ != -1) {
        DeleteFontToHandle(kakuG_);
        kakuG_ = -1;

        //使い終わったら登録を解除する(お作法として重要)
        RemoveFontResourceEx("Data/Font/HGRSGU.TTC", FR_PRIVATE, NULL);
    }
}