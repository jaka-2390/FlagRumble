#pragma once
class FontManager
{
public:

    static void Init();

    static void DrawStringEx(int x, int y, const char* text, int color, int size);

    static int GetAkazuki() { return kakuG_; }

private:
    
    static int kakuG_;

};

