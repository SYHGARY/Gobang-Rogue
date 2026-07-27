// GameLogic.h
#pragma once
#include "Globals.h"

// 计时管理
void Turn_Timer_Start();
void Turn_Timer_Update(int player);

// 判赢（棋盘与计时）
int Judge_Win_Chess(int Board[LINE_NUM][LINE_NUM], int x, int y, int player);
int Judge_Win_Timer(int player);

// 重启与悔棋
void Restart_Game(int Board[LINE_NUM][LINE_NUM], int* player);
void Take_Back_Move();   // 占位，未实现