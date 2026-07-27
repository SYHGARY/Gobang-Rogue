// GameUI.h
#pragma once
#include "Globals.h"
#include "Utils.h"

// 棋盘与信息区绘制
void Draw_Board();
void Draw_Info();

// 游戏按钮
void Draw_Game_Button(int button_index, int button_state);
void Draw_All_Game_Buttons(int button_state);
int Game_Hit_Button(int x, int y);
int Game_Button_Switch(ExMessage msg);

// 高光
void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]);
void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]);

// 提示当前玩家
void Player_Point(int player);

// 计时器绘制（更新在GameLogic中，绘制在此）
void Turn_Draw_Timer(int player);

// 音乐控制（游戏内按钮）
void Game_Music_Control(ExMessage msg);