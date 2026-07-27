// Globals.h
#pragma once

#define _WIN32_WINNT 0x0A00
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// 定义 UNICODE 以匹配 EasyX 的宽字符版本
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mmsystem.h>
#include <shellscalingapi.h>

#include <easyx/easyx.h>
#include <conio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <random>
#include <math.h>
#include <cwchar>   // for swprintf

// 取消 easyx 中 BLACK 和 WHITE 的定义，防止冲突
#undef BLACK
#undef WHITE

#define LINE_NUM 15
#define BOARD_SIZE 650
#define BOARD_MARGIN 280
#define BOARD_MARGIN_REAL 45
#define OFFSET 345
#define DOT_POS 160
#define DOT_RADIUS 5
#define BOARD_ORIGIN_X 325
#define BOARD_ORIGIN_Y 325
#define PIECE_SIZE 40

#define INFO_SIZE 450
#define BUTTON_SIZE 45
#define BUTTON_GAP 18
#define BUTTON_POS 2.5
#define ELEMENT_GAP 10

#define FONT_WEIGHT 700
#define FONT_POS 2.5
#define FONT_NUM 100
#define HEADLINE_WIDTH 670
#define HEADLINE_HEIGHT 200

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define TIME_LIMIT_TOTAL 600
#define TIME_LIMIT 30
#define TIME_WARN 10
#define FLASH_CYCLE 500

#define PORT 8888
#define NETWORK_MODE_LOCAL 1
#define NETWORK_MODE_MULTIPLAYER 2
#define NETWORK_MODE_SERVER 3
#define NETWORK_MODE_CLIENT 4
#define NETWORK_MODE_AI 5

#define MAIN_INSTRUCTION 6
#define MAIN_SETTING 7
#define MAIN_EXIT 8
#define MAIN_BACK 9

#define GAME_BUTTON_RESTART 10
#define GAME_BUTTON_TAKEBACK 11
#define GAME_BUTTON_MUSIC 12
#define GAME_BUTTON_SETTING 13
#define GAME_BUTTON_EXIT 14

#define MSG_MOVE 0
#define MSG_QUIT 1
#define MSG_CHAT 2
#define MSG_TIMEOUT 3
#define MSG_TIMEOUT_TOTAL 4

#define BUTTON_STATE_NORMAL 0
#define BUTTON_STATE_HOVER 1
#define BUTTON_STATE_PRESSED 2

enum GameState {
    MENU,
    INSTRUCTION,
    MULTIPLAYER,
    MULTIPLAYER_SERVER,
    MULTIPLAYER_CLIENT,
    PLAYING,
    SETTING,
    EXIT
};

struct NetworkMessage {
    int msgtype;
    int x, y;
    int player;
    char data[256];
};

// 全局变量声明（所有使用宽字符字符串的地方都使用 wchar_t）
extern int highlightX, highlightY;
extern time_t game_start_time;
extern time_t turn_start_time;
extern int time_total_black;
extern int time_total_white;
extern int game_remain_time;
extern int turn_remain_time;
extern bool game_time_running;
extern bool player_time_running;

extern IMAGE img_headline, img_headline_opp, img_menu_background;
extern IMAGE img_universal_background, img_game_background, img_info_background;
extern IMAGE img_white, img_white_opp, img_black, img_black_opp;

extern wchar_t inputStr[FONT_NUM];   // 改为 wchar_t

// 按钮坐标等...
extern int button_width;
extern int button_height;
extern int headline_x;
extern int button_mainall_x;
extern int headline_y;
extern int button_main_gap;
extern int button_singleplayer_y;
extern int button_multiplayer_y;
extern int button_instruction_y;
extern int button_mainsetting_y;
extern int button_mainexit_y;

extern int main_hover_button;
extern int main_pressed_button;

extern int button_multiall_x;
extern int button_createserver_y;
extern int button_joinserver_y;
extern int multi_hover_button;
extern int multi_pressed_button;

extern int button_back_x;
extern int button_back_y;
extern int button_back_width;
extern int button_back_height;
extern int back_hover_button;
extern int back_pressed_button;

extern int button_game_left_margin;
extern int button_game_right_margin;
extern int button_restart_x;
extern int button_takeback_x;
extern int button_exit_x;
extern int button_setting_x;
extern int button_music_x;
extern int button_playingall_y;
extern int game_hover_button;
extern int game_pressed_button;

extern int game_mode;
extern int music_flag;
extern char choice;

extern SOCKET ServerSocket;
extern SOCKET ClientSocket;
extern int NetworkMode;
extern char ServerIP[20];
extern bool isConnected;
extern bool isMyturn;

extern GameState currentState;