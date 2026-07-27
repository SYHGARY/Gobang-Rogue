// Utils.h
#pragma once
#include "Globals.h"

// 素材加载
void Texture_Load();

// 音乐控制（此函数在游戏内使用，需包含GameUI相关，但声明放在Utils，实现在GameUI.cpp中）
void Game_Music_Control(ExMessage msg);

// 透明图片绘制
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img);

// 清理信息区背景（局部）
void Clear_Info_Background(int x, int y, int width, int height);

// 点与圆、矩形的碰撞检测
bool Is_InCircle(int px, int py, int cx, int cy, int radius);
bool Is_InRect(int px, int py, int left, int top, int right, int bottom);

// 通用按钮绘制（框架+文字）
void Draw_Button_Frame(int left, int top, int right, int bottom, int radius, int button_state);
void Draw_Button_Text(int left, int top, int width, int height, const TCHAR* text, int button_state);

// 返回按钮绘制与交互
void Draw_Back_Button(int button_state, IMAGE* background_img);
int Back_Switch(ExMessage msg, IMAGE* background_img);