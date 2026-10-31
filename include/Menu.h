// Menu.h
#pragma once
#include "Globals.h"
#include "Utils.h"

// 主菜单
void Draw_Main();
int Main_Switch(ExMessage msg);

// 多人模式选择
void Draw_Multi();
int Multi_Switch(ExMessage msg);

// 说明、设置、提示页面
void Draw_Instruction();
void Draw_Setting();
int Setting_Switch(ExMessage msg);
void Draw_Message_Page(const TCHAR* message);

// 输入框（用于输入IP）
bool Input_Box();
