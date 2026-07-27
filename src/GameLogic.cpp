// GameLogic.cpp
#include "GameLogic.h"
#include "GameUI.h"   // 需要用到 Draw_Board, Draw_Info, Clear_Highlight 等
#include <tchar.h>

//=====================/回合计时管理/=====================

void Turn_Timer_Start() {
    turn_start_time = time(NULL);
    turn_remain_time = TIME_LIMIT;
    game_time_running = true;
}

void Turn_Timer_Update(int player) {
    if (!game_time_running) return;
    
    time_t current_time = time(NULL);
    turn_remain_time = TIME_LIMIT - (int)difftime(current_time, turn_start_time);
    int used_time = TIME_LIMIT - turn_remain_time;
    if (used_time < 0) used_time = 0;
    if (used_time > TIME_LIMIT) used_time = TIME_LIMIT;

    // 静态变量记录本回合已扣除的秒数（累计，跨多次调用）
    static int turn_passed_last = 0;
    int delta = used_time - turn_passed_last;
    if (delta < 0) delta = 0;
    turn_passed_last = used_time;

    if (delta > 0) {
        if (player == BLACK) {
            time_total_black -= delta;
            if (time_total_black < 0) time_total_black = 0;
        } else if (player == WHITE) {
            time_total_white -= delta;
            if (time_total_white < 0) time_total_white = 0;
        }
    }

    if (turn_remain_time < 0) {
        turn_remain_time = 0;
        game_time_running = false;
    }
    if (time_total_black == 0 || time_total_white == 0) {
        player_time_running = false;
    }
}

//=====================/判赢函数/=====================

int Judge_Win_Chess(int Board[LINE_NUM][LINE_NUM], int x, int y, int player) {
    if (player == EMPTY) return EMPTY;
    
    int dir[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
    for (int i = 0; i < 4; i++) {
        int count = 1;
        // 正方向
        for (int j = 1; j < 5; j++) {
            int cx = x + dir[i][0] * j;
            int cy = y + dir[i][1] * j;
            if (cx >= 0 && cx < LINE_NUM && cy >= 0 && cy < LINE_NUM && Board[cx][cy] == player)
                count++;
            else
                break;
        }
        // 反方向
        for (int j = 1; j < 5; j++) {
            int cx = x - dir[i][0] * j;
            int cy = y - dir[i][1] * j;
            if (cx >= 0 && cx < LINE_NUM && cy >= 0 && cy < LINE_NUM && Board[cx][cy] == player)
                count++;
            else
                break;
        }
        if (count >= 5) {
            // 显示胜利信息
            setorigin(0, 0);
            putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS,
                     (int)(PIECE_SIZE * 4 * 0.75), PIECE_SIZE, &img_info_background,
                     (int)(INFO_SIZE / 2 - 1.5 * PIECE_SIZE), FONT_POS);
            setbkmode(TRANSPARENT);
            LOGFONT fontStyle;
            gettextstyle(&fontStyle);
            fontStyle.lfQuality = ANTIALIASED_QUALITY;
            fontStyle.lfWeight = FONT_WEIGHT;
            fontStyle.lfHeight = PIECE_SIZE;
            _tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
            settextstyle(&fontStyle);
            settextcolor(BLACK);
            if (player == BLACK) {
                printf_s("黑方胜利\n");
                outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
            } else {
                printf_s("白方胜利\n");
                outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
            }
            FlushBatchDraw();
            setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
            return player;
        }
    }
    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    return EMPTY;
}

int Judge_Win_Timer(int player) {
    if (!player_time_running) {
        setorigin(0, 0);
        putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS,
                 (int)(PIECE_SIZE * 4 * 0.75), PIECE_SIZE, &img_info_background,
                 (int)(INFO_SIZE / 2 - 1.5 * PIECE_SIZE), FONT_POS);
        setbkmode(TRANSPARENT);
        LOGFONT fontStyle;
        gettextstyle(&fontStyle);
        fontStyle.lfQuality = ANTIALIASED_QUALITY;
        fontStyle.lfWeight = FONT_WEIGHT;
        fontStyle.lfHeight = PIECE_SIZE;
        _tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
        settextstyle(&fontStyle);
        settextcolor(BLACK);
        if (player == BLACK && time_total_black <= 0) {
            printf_s("白方胜利（黑方超时）\n");
            outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
            FlushBatchDraw();
            setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
            return WHITE;
        } else if (player == WHITE && time_total_white <= 0) {
            printf_s("黑方胜利（白方超时）\n");
            outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
            FlushBatchDraw();
            setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
            return BLACK;
        }
    }
    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    return EMPTY;
}

//=====================/重启与悔棋/=====================

void Restart_Game(int Board[LINE_NUM][LINE_NUM], int* player) {
    // 清空棋盘
    for (int i = 0; i < LINE_NUM; i++)
        for (int j = 0; j < LINE_NUM; j++)
            Board[i][j] = EMPTY;
    
    *player = BLACK;
    time_total_black = TIME_LIMIT_TOTAL;
    time_total_white = TIME_LIMIT_TOTAL;
    game_time_running = true;
    player_time_running = true;
    Turn_Timer_Start();
    Clear_Highlight(Board);
    
    // 根据网络模式重置先手
    if (NetworkMode == NETWORK_MODE_SERVER)
        isMyturn = true;
    else if (NetworkMode == NETWORK_MODE_CLIENT)
        isMyturn = false;
    else
        isMyturn = true;

    setorigin(0, 0);
    putimage(0, 0, &img_game_background);
    Draw_Board();
    Draw_Info();
    game_hover_button = -1;
    game_pressed_button = -1;
    FlushBatchDraw();
}

void Take_Back_Move() {
    // 占位，未实现
    return;
}