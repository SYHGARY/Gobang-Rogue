// Globals.cpp
#include "Globals.h"

int highlightX = -1, highlightY = -1;
time_t game_start_time;
time_t turn_start_time;
int time_total_black = TIME_LIMIT_TOTAL;
int time_total_white = TIME_LIMIT_TOTAL;
int game_remain_time;
int turn_remain_time;
int game_time_limit = TIME_LIMIT_TOTAL;
int turn_time_limit = TIME_LIMIT;
bool game_time_running = false;
bool player_time_running = true;
MoveRecord move_history[MAX_MOVE_COUNT] = {};
int move_history_count = 0;
int turn_passed_last = 0;

IMAGE img_headline, img_headline_opp, img_menu_background;
IMAGE img_universal_background, img_game_background, img_info_background;
IMAGE img_white, img_white_opp, img_black, img_black_opp;

wchar_t inputStr[FONT_NUM] = L"";

int button_width = 360;
int button_height = 52;
int headline_x = (BOARD_SIZE + INFO_SIZE - HEADLINE_WIDTH) / 2;
int button_mainall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;
int headline_y = 35;
int button_main_gap = 16;
int button_singleplayer_y = headline_y + HEADLINE_HEIGHT + 3 * ELEMENT_GAP;
int button_multiplayer_y = button_singleplayer_y + button_height + button_main_gap;
int button_instruction_y = button_multiplayer_y + button_height + button_main_gap;
int button_mainsetting_y = button_instruction_y + button_height + button_main_gap;
int button_mainexit_y = button_mainsetting_y + button_height + button_main_gap;

int main_hover_button = -1;
int main_pressed_button = -1;

int button_multiall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;
int button_createserver_y = (BOARD_SIZE - 8 * ELEMENT_GAP - 2 * button_height) / 2;
int button_joinserver_y = button_createserver_y + button_height + 8 * ELEMENT_GAP;
int multi_hover_button = -1;
int multi_pressed_button = -1;

int button_back_x = ELEMENT_GAP * 2;
int button_back_y = ELEMENT_GAP * 2;
int button_back_width = 120;
int button_back_height = 45;
int back_hover_button = -1;
int back_pressed_button = -1;

int button_game_left_margin = 20;
int button_game_right_margin = 20;
int button_restart_x = BOARD_SIZE + button_game_left_margin + BUTTON_SIZE / 2;
int button_takeback_x = button_restart_x + BUTTON_SIZE + BUTTON_GAP;
int button_exit_x = BOARD_SIZE + INFO_SIZE - button_game_right_margin - BUTTON_SIZE / 2;
int button_setting_x = button_exit_x - BUTTON_SIZE - BUTTON_GAP;
int button_music_x = button_setting_x - BUTTON_SIZE - BUTTON_GAP;
int button_playingall_y = BOARD_SIZE - BUTTON_SIZE / 2 - 25;
int game_hover_button = -1;
int game_pressed_button = -1;

int game_mode;
int music_flag = 1;
char choice;

SOCKET ServerSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
int NetworkMode = NETWORK_MODE_LOCAL;
char ServerIP[20] = "127.0.0.1";
bool isConnected = false;
bool isMyturn = true;
bool takeback_pending = false;

GameState currentState = MENU;
