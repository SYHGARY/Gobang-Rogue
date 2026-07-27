// GameUI.cpp
#include "GameUI.h"
#include <cwchar>

void Draw_Board() {
    setorigin(0, 0);
    putimage(0, 0, &img_game_background);

    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    setlinestyle(PS_SOLID, 1);
    setlinecolor(BLACK);
    setfillcolor(BLACK);

    for (int i = 0; i < LINE_NUM; i++) {
        line(-BOARD_MARGIN + PIECE_SIZE * i, BOARD_MARGIN, -BOARD_MARGIN + PIECE_SIZE * i, -BOARD_MARGIN);
        line(-BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i, BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i);
    }
    solidcircle(0, 0, DOT_RADIUS);
    solidcircle(-DOT_POS, DOT_POS, DOT_RADIUS);
    solidcircle(DOT_POS, -DOT_POS, DOT_RADIUS);
    solidcircle(-DOT_POS, -DOT_POS, DOT_RADIUS);
    solidcircle(DOT_POS, DOT_POS, DOT_RADIUS);
}

void Draw_Info() {
    setorigin(0, 0);
    putimage(BOARD_SIZE, 0, &img_info_background);

    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE / 2;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);
    settextcolor(RGB(255, 255, 255));

    Draw_All_Game_Buttons(BUTTON_STATE_NORMAL);

    double single_char = 0.35;
    double game_mode_textx = BOARD_SIZE + ELEMENT_GAP;
    double network_mode_textx = BOARD_SIZE + INFO_SIZE - 2 * ELEMENT_GAP - 8 * single_char * PIECE_SIZE;
    double game_mode_texty = PIECE_SIZE + 2 * ELEMENT_GAP + PIECE_SIZE + 3 * ELEMENT_GAP;
    double network_mode_texty = game_mode_texty;
    settextcolor(BLACK);
    outtextxy((int)game_mode_textx, (int)game_mode_texty, L"当前模式：");
    settextcolor(BLUE);
    if (game_mode == NETWORK_MODE_LOCAL) {
        outtextxy((int)(game_mode_textx + 5 * single_char * PIECE_SIZE), (int)game_mode_texty, L"本地模式");
    } else if (game_mode == NETWORK_MODE_SERVER || game_mode == NETWORK_MODE_CLIENT) {
        outtextxy((int)(game_mode_textx + 5 * single_char * PIECE_SIZE), (int)game_mode_texty, L"联网模式");
    } else if (game_mode == NETWORK_MODE_AI) {
        outtextxy((int)(game_mode_textx + 5 * single_char * PIECE_SIZE), (int)game_mode_texty, L"人机模式");
    }

    settextcolor(BLACK);
    outtextxy((int)network_mode_textx, (int)network_mode_texty, L"连接状态：");
    if (isConnected) {
        settextcolor(GREEN);
        outtextxy((int)(network_mode_textx + 5 * single_char * PIECE_SIZE), (int)network_mode_texty, L"已连接");
    } else {
        settextcolor(RED);
        outtextxy((int)(network_mode_textx + 5 * single_char * PIECE_SIZE), (int)network_mode_texty, L"未连接");
    }

    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 2);
    double card_board_left = BOARD_SIZE + ELEMENT_GAP;
    double card_board_top = game_mode_texty + single_char * PIECE_SIZE + 2 * ELEMENT_GAP;
    double card_board_right = BOARD_SIZE + INFO_SIZE - ELEMENT_GAP;
    double card_board_bottom = card_board_top + INFO_SIZE - 8 * ELEMENT_GAP;
    roundrect((int)card_board_left, (int)card_board_top, (int)card_board_right, (int)card_board_bottom, 15, 15);

    double chat_board_left = card_board_left;
    double chat_board_top = card_board_bottom + ELEMENT_GAP;
    double chat_board_right = card_board_right;
    double chat_board_bottom = chat_board_top + 2 * ELEMENT_GAP;
    roundrect((int)chat_board_left, (int)chat_board_top, (int)chat_board_right, (int)chat_board_bottom, 10, 10);

    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
}

void Draw_Game_Button(int button_index, int button_state) {
    int center_x = 0;
    const wchar_t* button_text = L"";
    switch (button_index) {
    case GAME_BUTTON_RESTART:  center_x = button_restart_x; button_text = L"重"; break;
    case GAME_BUTTON_TAKEBACK: center_x = button_takeback_x; button_text = L"悔"; break;
    case GAME_BUTTON_MUSIC:    center_x = button_music_x;   button_text = music_flag == 1 ? L"音" : L"静"; break;
    case GAME_BUTTON_SETTING:  center_x = button_setting_x; button_text = L"设"; break;
    case GAME_BUTTON_EXIT:     center_x = button_exit_x;    button_text = L"退"; break;
    default: return;
    }
    int left = center_x - BUTTON_SIZE / 2;
    int top = button_playingall_y - BUTTON_SIZE / 2;
    int padding = 5;
    Clear_Info_Background(left - padding, top - padding, BUTTON_SIZE + 2 * padding, BUTTON_SIZE + 2 * padding);
    setorigin(0, 0);
    Draw_Button_Frame(left, top, left + BUTTON_SIZE, top + BUTTON_SIZE, 16, button_state);
    Draw_Button_Text(left, top, BUTTON_SIZE, BUTTON_SIZE, button_text, button_state);
}

void Draw_All_Game_Buttons(int button_state) {
    Draw_Game_Button(GAME_BUTTON_RESTART, button_state);
    Draw_Game_Button(GAME_BUTTON_TAKEBACK, button_state);
    Draw_Game_Button(GAME_BUTTON_MUSIC, button_state);
    Draw_Game_Button(GAME_BUTTON_SETTING, button_state);
    Draw_Game_Button(GAME_BUTTON_EXIT, button_state);
}

int Game_Hit_Button(int x, int y) {
    int top = button_playingall_y - BUTTON_SIZE / 2;
    int bottom = button_playingall_y + BUTTON_SIZE / 2;
    if (Is_InRect(x, y, button_restart_x - BUTTON_SIZE / 2, top, button_restart_x + BUTTON_SIZE / 2, bottom))
        return GAME_BUTTON_RESTART;
    if (Is_InRect(x, y, button_takeback_x - BUTTON_SIZE / 2, top, button_takeback_x + BUTTON_SIZE / 2, bottom))
        return GAME_BUTTON_TAKEBACK;
    if (Is_InRect(x, y, button_music_x - BUTTON_SIZE / 2, top, button_music_x + BUTTON_SIZE / 2, bottom))
        return GAME_BUTTON_MUSIC;
    if (Is_InRect(x, y, button_setting_x - BUTTON_SIZE / 2, top, button_setting_x + BUTTON_SIZE / 2, bottom))
        return GAME_BUTTON_SETTING;
    if (Is_InRect(x, y, button_exit_x - BUTTON_SIZE / 2, top, button_exit_x + BUTTON_SIZE / 2, bottom))
        return GAME_BUTTON_EXIT;
    return -1;
}

int Game_Button_Switch(ExMessage msg) {
    int current_button = Game_Hit_Button(msg.x, msg.y);
    if (msg.message == WM_MOUSEMOVE) {
        if (game_pressed_button != -1) {
            if (game_hover_button != current_button) {
                game_hover_button = current_button;
                Draw_Game_Button(game_pressed_button, current_button == game_pressed_button ? BUTTON_STATE_PRESSED : BUTTON_STATE_NORMAL);
                FlushBatchDraw();
            }
        } else if (game_hover_button != current_button) {
            int old_hover = game_hover_button;
            game_hover_button = current_button;
            if (old_hover != -1) Draw_Game_Button(old_hover, BUTTON_STATE_NORMAL);
            if (game_hover_button != -1) Draw_Game_Button(game_hover_button, BUTTON_STATE_HOVER);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONDOWN) {
        game_pressed_button = current_button;
        if (game_pressed_button != -1) {
            Draw_Game_Button(game_pressed_button, BUTTON_STATE_PRESSED);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONUP) {
        int pressed_button = game_pressed_button;
        game_pressed_button = -1;
        if (pressed_button != -1) {
            Draw_Game_Button(pressed_button, current_button == pressed_button ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL);
            FlushBatchDraw();
        }
        if (pressed_button != -1 && pressed_button == current_button) {
            return pressed_button;
        }
    }
    return -1;
}

void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]) {
    highlightX = x;
    highlightY = y;
    COLORREF origin_line_color = getlinecolor();
    LINESTYLE origin_line_style;
    getlinestyle(&origin_line_style);

    setlinecolor(RED);
    setlinestyle(PS_DASH);
    setorigin(0, 0);
    int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
    int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;
    circle(centerX, centerY, PIECE_SIZE / 2);

    setlinecolor(origin_line_color);
    setlinestyle(&origin_line_style);
}

void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]) {
    if (highlightX == -1 || highlightY == -1) return;

    COLORREF origin_line_color = getlinecolor();
    LINESTYLE origin_line_style;
    getlinestyle(&origin_line_style);

    setlinecolor(BLACK);
    setfillcolor(BLACK);
    setlinestyle(PS_SOLID, 1);

    setorigin(0, 0);
    int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
    int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;

    int left = centerX - PIECE_SIZE / 2;
    int top = centerY - PIECE_SIZE / 2;
    putimage(left, top, PIECE_SIZE + 1, PIECE_SIZE + 1, &img_game_background, left, top);

    int startX = left, endX = left + PIECE_SIZE;
    if (highlightX == 0) startX = centerX;
    if (highlightX == LINE_NUM - 1) endX = centerX;
    line(startX, centerY, endX, centerY);

    int startY = top, endY = top + PIECE_SIZE;
    if (highlightY == 0) startY = centerY;
    if (highlightY == LINE_NUM - 1) endY = centerY;
    line(centerX, startY, centerX, endY);

    // 星位点重绘
    if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS)
        solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);
    else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS)
        solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);
    else if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS)
        solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);
    else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS)
        solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);
    else if (centerX == BOARD_ORIGIN_X && centerY == BOARD_ORIGIN_Y)
        solidcircle(BOARD_ORIGIN_X, BOARD_ORIGIN_Y, DOT_RADIUS);

    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    setlinecolor(origin_line_color);
    setlinestyle(&origin_line_style);
    highlightX = -1;
    highlightY = -1;
}

void Player_Point(int player) {
    setorigin(0, 0);
    putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS,
             (int)(PIECE_SIZE * 4 * 0.75), PIECE_SIZE, &img_info_background,
             (int)(INFO_SIZE / 2 - 1.5 * PIECE_SIZE), FONT_POS);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);

    if (player == WHITE) {
        outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, L"黑棋落子");
    } else if (player == BLACK) {
        outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, L"白棋落子");
    }
}

void Turn_Draw_Timer(int player) {
    setorigin(0, 0);
    double single_char = 0.35;
    double round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;
    double round_time_sizey = PIECE_SIZE / 2;
    double total_time_sizex = PIECE_SIZE * 9 * 0.75 / 2;
    double total_time_sizey = PIECE_SIZE / 2;

    double black_piece_x = BOARD_SIZE + 2 * ELEMENT_GAP;
    double black_piece_y = PIECE_SIZE + 2 * ELEMENT_GAP;
    double white_piece_x = BOARD_SIZE + INFO_SIZE - 2 * ELEMENT_GAP - PIECE_SIZE;
    double white_piece_y = PIECE_SIZE + 2 * ELEMENT_GAP;
    Put_Transparent_Image((int)black_piece_x, (int)black_piece_y, &img_black_opp, &img_black);
    Put_Transparent_Image((int)white_piece_x, (int)white_piece_y, &img_white_opp, &img_white);

    double timer_black_x = black_piece_x + PIECE_SIZE + ELEMENT_GAP / 2;
    double timer_black_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;
    double timer_white_x = white_piece_x - 9.5 * single_char * PIECE_SIZE - ELEMENT_GAP / 2;
    double timer_white_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;
    double timer_black_total_x = timer_black_x;
    double timer_black_total_y = timer_black_y + ELEMENT_GAP / 2 + PIECE_SIZE / 2;
    double timer_white_total_x = timer_white_x;
    double timer_white_total_y = timer_white_y + ELEMENT_GAP / 2 + PIECE_SIZE / 2;

    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE / 2;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);

    COLORREF timer_color;
    if (turn_remain_time <= TIME_WARN && turn_remain_time > 0) timer_color = RED;
    else if (turn_remain_time <= 0) timer_color = RED;
    else timer_color = BLACK;
    settextcolor(timer_color);

    wchar_t time_text_round[30];
    if (turn_remain_time >= 0)
        swprintf(time_text_round, 30, L"回合剩余：%02d秒", turn_remain_time);
    else
        swprintf(time_text_round, 30, L"超时");

    wchar_t black_time_total_text[30], white_time_total_text[30];
    int black_min = time_total_black / 60, black_sec = time_total_black % 60;
    int white_min = time_total_white / 60, white_sec = time_total_white % 60;
    swprintf(black_time_total_text, 30, L"总时间剩余：%02d:%02d", black_min, black_sec);
    swprintf(white_time_total_text, 30, L"总时间剩余：%02d:%02d", white_min, white_sec);

    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 2);
    int black_timearea_left = (int)(black_piece_x - ELEMENT_GAP);
    int black_timearea_top = (int)(black_piece_y - ELEMENT_GAP);
    int black_timearea_right = (int)(black_timearea_left + PIECE_SIZE + 10 * single_char * PIECE_SIZE + 2 * ELEMENT_GAP);
    int black_timearea_bottom = (int)(black_timearea_top + PIECE_SIZE + 2 * ELEMENT_GAP);
    roundrect(black_timearea_left, black_timearea_top, black_timearea_right, black_timearea_bottom, 15, 15);

    int white_timearea_right = (int)(white_piece_x + PIECE_SIZE + ELEMENT_GAP);
    int white_timearea_top = (int)(white_piece_y - ELEMENT_GAP);
    int white_timearea_left = (int)(white_timearea_right - PIECE_SIZE - 10 * single_char * PIECE_SIZE - 2 * ELEMENT_GAP);
    int white_timearea_bottom = (int)(white_timearea_top + PIECE_SIZE + 2 * ELEMENT_GAP);
    roundrect(white_timearea_left, white_timearea_top, white_timearea_right, white_timearea_bottom, 15, 15);

    Clear_Info_Background((int)timer_black_total_x, (int)timer_black_total_y, (int)total_time_sizex, (int)total_time_sizey);
    Clear_Info_Background((int)timer_white_total_x, (int)timer_white_total_y, (int)total_time_sizex, (int)total_time_sizey);
    settextcolor(BLACK);
    outtextxy((int)timer_black_total_x, (int)timer_black_total_y, black_time_total_text);
    outtextxy((int)timer_white_total_x, (int)timer_white_total_y, white_time_total_text);

    if (player == BLACK) {
        setlinecolor(RED);
        roundrect(black_timearea_left, black_timearea_top, black_timearea_right, black_timearea_bottom, 15, 15);
        Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);
        outtextxy((int)timer_black_x, (int)timer_black_y, time_text_round);
        settextcolor(BLACK);
        Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);
        outtextxy((int)timer_white_x, (int)timer_white_y, L"回合剩余：30秒");
    } else if (player == WHITE) {
        setlinecolor(RED);
        roundrect(white_timearea_left, white_timearea_top, white_timearea_right, white_timearea_bottom, 15, 15);
        Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);
        outtextxy((int)timer_white_x, (int)timer_white_y, time_text_round);
        settextcolor(BLACK);
        Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);
        outtextxy((int)timer_black_x, (int)timer_black_y, L"回合剩余：30秒");
    }

    static int flash_count = 0;
    if (turn_remain_time <= 5 && turn_remain_time > 0) {
        flash_count++;
        if (flash_count % FLASH_CYCLE < FLASH_CYCLE / 2) {
            if (player == BLACK)
                Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);
            else if (player == WHITE)
                Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);
        } else {
            settextcolor(RED);
            if (player == BLACK) {
                Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);
                outtextxy((int)timer_black_x, (int)timer_black_y, time_text_round);
            } else if (player == WHITE) {
                Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);
                outtextxy((int)timer_white_x, (int)timer_white_y, time_text_round);
            }
        }
    } else {
        flash_count = 0;
    }
    setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
}