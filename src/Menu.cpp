// Menu.cpp
#include "Menu.h"
#include "Utils.h"
#include <cwchar>

static void Draw_Menu_Button(int left, int top, const wchar_t* text, int button_state, IMAGE* background_img) {
    int padding = 14;
    int refresh_x = left - padding;
    int refresh_y = top - padding;
    int refresh_w = button_width + 2 * padding;
    int refresh_h = button_height + 2 * padding;
    if (refresh_x < 0) {
        refresh_w += refresh_x;
        refresh_x = 0;
    }
    if (refresh_y < 0) {
        refresh_h += refresh_y;
        refresh_y = 0;
    }
    if (refresh_x + refresh_w > BOARD_SIZE + INFO_SIZE) {
        refresh_w = BOARD_SIZE + INFO_SIZE - refresh_x;
    }
    if (refresh_y + refresh_h > BOARD_SIZE) {
        refresh_h = BOARD_SIZE - refresh_y;
    }
    if (refresh_w > 0 && refresh_h > 0 && background_img != NULL) {
        putimage(refresh_x, refresh_y, refresh_w, refresh_h, background_img, refresh_x, refresh_y);
    }
    Draw_Button_Frame(left, top, left + button_width, top + button_height, 24, button_state);
    Draw_Button_Text(left, top, button_width, button_height, text, button_state);
}

static void Draw_Main_Button(int button_index, int button_state) {
    switch (button_index) {
    case NETWORK_MODE_LOCAL:
        Draw_Menu_Button(button_mainall_x, button_singleplayer_y, L"本地模式", button_state, &img_menu_background);
        break;
    case NETWORK_MODE_MULTIPLAYER:
        Draw_Menu_Button(button_mainall_x, button_multiplayer_y, L"多人模式", button_state, &img_menu_background);
        break;
    case MAIN_INSTRUCTION:
        Draw_Menu_Button(button_mainall_x, button_instruction_y, L"玩法介绍", button_state, &img_menu_background);
        break;
    case MAIN_SETTING:
        Draw_Menu_Button(button_mainall_x, button_mainsetting_y, L"游戏设置", button_state, &img_menu_background);
        break;
    case MAIN_EXIT:
        Draw_Menu_Button(button_mainall_x, button_mainexit_y, L"退出游戏", button_state, &img_menu_background);
        break;
    }
}

static int Main_Hit_Button(int x, int y) {
    if (Is_InRect(x, y, button_mainall_x, button_singleplayer_y, button_mainall_x + button_width, button_singleplayer_y + button_height))
        return NETWORK_MODE_LOCAL;
    if (Is_InRect(x, y, button_mainall_x, button_multiplayer_y, button_mainall_x + button_width, button_multiplayer_y + button_height))
        return NETWORK_MODE_MULTIPLAYER;
    if (Is_InRect(x, y, button_mainall_x, button_instruction_y, button_mainall_x + button_width, button_instruction_y + button_height))
        return MAIN_INSTRUCTION;
    if (Is_InRect(x, y, button_mainall_x, button_mainsetting_y, button_mainall_x + button_width, button_mainsetting_y + button_height))
        return MAIN_SETTING;
    if (Is_InRect(x, y, button_mainall_x, button_mainexit_y, button_mainall_x + button_width, button_mainexit_y + button_height))
        return MAIN_EXIT;
    return -1;
}

void Draw_Main() {
    settextcolor(RGB(0,0,0));
    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = 0;
    fontStyle.lfHeight = PIECE_SIZE / 2;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);

    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_menu_background);
    Put_Transparent_Image(headline_x, headline_y, &img_headline_opp, &img_headline);

    main_hover_button = -1;
    main_pressed_button = -1;
    Draw_Main_Button(NETWORK_MODE_LOCAL, BUTTON_STATE_NORMAL);
    Draw_Main_Button(NETWORK_MODE_MULTIPLAYER, BUTTON_STATE_NORMAL);
    Draw_Main_Button(MAIN_INSTRUCTION, BUTTON_STATE_NORMAL);
    Draw_Main_Button(MAIN_SETTING, BUTTON_STATE_NORMAL);
    Draw_Main_Button(MAIN_EXIT, BUTTON_STATE_NORMAL);
}

int Main_Switch(ExMessage msg) {
    int current_button = Main_Hit_Button(msg.x, msg.y);
    if (msg.message == WM_MOUSEMOVE) {
        if (main_pressed_button != -1) {
            if (main_hover_button != current_button) {
                int old_hover = main_hover_button;
                main_hover_button = current_button;
                if (old_hover != -1 && old_hover != main_pressed_button) {
                    Draw_Main_Button(old_hover, BUTTON_STATE_NORMAL);
                }
                Draw_Main_Button(main_pressed_button, current_button == main_pressed_button ? BUTTON_STATE_PRESSED : BUTTON_STATE_HOVER);
                FlushBatchDraw();
            }
        } else if (main_hover_button != current_button) {
            int old_hover = main_hover_button;
            main_hover_button = current_button;
            if (old_hover != -1) {
                Draw_Main_Button(old_hover, BUTTON_STATE_NORMAL);
            }
            if (main_hover_button != -1) {
                Draw_Main_Button(main_hover_button, BUTTON_STATE_HOVER);
            }
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONDOWN) {
        main_pressed_button = current_button;
        if (main_pressed_button != -1) {
            Draw_Main_Button(main_pressed_button, BUTTON_STATE_PRESSED);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONUP) {
        int pressed_button = main_pressed_button;
        main_pressed_button = -1;
        if (pressed_button != -1) {
            Draw_Main_Button(pressed_button, current_button == pressed_button ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL);
            FlushBatchDraw();
        }
        if (pressed_button == current_button && current_button != -1) {
            return current_button;
        }
    }
    return -1;
}

// 多人模式
static void Draw_Multi_Button(int button_index, int button_state) {
    if (button_index == NETWORK_MODE_SERVER) {
        Draw_Menu_Button(button_multiall_x, button_createserver_y, L"创建服务器", button_state, &img_universal_background);
    } else if (button_index == NETWORK_MODE_CLIENT) {
        Draw_Menu_Button(button_multiall_x, button_joinserver_y, L"加入服务器", button_state, &img_universal_background);
    }
}

static int Multi_Hit_Button(int x, int y) {
    if (Is_InRect(x, y, button_multiall_x, button_createserver_y, button_multiall_x + button_width, button_createserver_y + button_height))
        return NETWORK_MODE_SERVER;
    if (Is_InRect(x, y, button_multiall_x, button_joinserver_y, button_multiall_x + button_width, button_joinserver_y + button_height))
        return NETWORK_MODE_CLIENT;
    return -1;
}

void Draw_Multi() {
    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_universal_background);

    multi_hover_button = -1;
    multi_pressed_button = -1;
    back_hover_button = -1;
    back_pressed_button = -1;

    Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
    Draw_Multi_Button(NETWORK_MODE_SERVER, BUTTON_STATE_NORMAL);
    Draw_Multi_Button(NETWORK_MODE_CLIENT, BUTTON_STATE_NORMAL);
}

int Multi_Switch(ExMessage msg) {
    int back_result = Back_Switch(msg, &img_universal_background);
    if (back_result == MAIN_BACK) return MAIN_BACK;

    int current_button = Multi_Hit_Button(msg.x, msg.y);
    if (msg.message == WM_MOUSEMOVE) {
        if (multi_pressed_button != -1) {
            if (multi_hover_button != current_button) {
                int old_hover = multi_hover_button;
                multi_hover_button = current_button;
                if (old_hover != -1 && old_hover != multi_pressed_button) {
                    Draw_Multi_Button(old_hover, BUTTON_STATE_NORMAL);
                }
                Draw_Multi_Button(multi_pressed_button, current_button == multi_pressed_button ? BUTTON_STATE_PRESSED : BUTTON_STATE_HOVER);
                FlushBatchDraw();
            }
        } else if (multi_hover_button != current_button) {
            int old_hover = multi_hover_button;
            multi_hover_button = current_button;
            if (old_hover != -1) {
                Draw_Multi_Button(old_hover, BUTTON_STATE_NORMAL);
            }
            if (multi_hover_button != -1) {
                Draw_Multi_Button(multi_hover_button, BUTTON_STATE_HOVER);
            }
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONDOWN) {
        multi_pressed_button = current_button;
        if (multi_pressed_button != -1) {
            Draw_Multi_Button(multi_pressed_button, BUTTON_STATE_PRESSED);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONUP) {
        int pressed_button = multi_pressed_button;
        multi_pressed_button = -1;
        if (pressed_button != -1) {
            Draw_Multi_Button(pressed_button, current_button == pressed_button ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL);
            FlushBatchDraw();
        }
        if (pressed_button == current_button && current_button != -1) {
            return current_button;
        }
    }
    return -1;
}

// 通用文字页面
void Draw_Text_Page(const wchar_t* title, const wchar_t* content) {
    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_universal_background);

    settextcolor(RGB(0,0,0));
    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);

    int title_x = (BOARD_SIZE + INFO_SIZE - textwidth(title)) / 2;
    int title_y = headline_y;
    outtextxy(title_x, title_y, title);

    fontStyle.lfWeight = 0;
    fontStyle.lfHeight = PIECE_SIZE / 2;
    settextstyle(&fontStyle);
    int content_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;
    int content_y = title_y + PIECE_SIZE + 4 * ELEMENT_GAP;
    outtextxy(content_x, content_y, content);

    back_hover_button = -1;
    back_pressed_button = -1;
    Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
}

void Draw_Instruction() {
    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_universal_background);

    int card_width = 600, card_height = 420;
    int card_left = (BOARD_SIZE + INFO_SIZE - card_width) / 2;
    int card_top = (BOARD_SIZE - card_height) / 2;
    int card_right = card_left + card_width;
    int card_bottom = card_top + card_height;

    setlinestyle(PS_SOLID, 2);
    setlinecolor(RGB(200,200,200));
    setfillcolor(RGB(255,255,255));
    fillroundrect(card_left, card_top, card_right, card_bottom, 20, 20);
    roundrect(card_left, card_top, card_right, card_bottom, 20, 20);

    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE * 0.85;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);
    settextcolor(RGB(30,30,30));

    int title_x = card_left + (card_width - textwidth(L"🎯 玩法介绍")) / 2;
    int title_y = card_top + 30;
    outtextxy(title_x, title_y, L"🎯 玩法介绍");

    setlinecolor(RGB(180,180,180));
    setlinestyle(PS_SOLID, 1);
    line(card_left + 40, title_y + 45, card_right - 40, title_y + 45);

    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE * 0.55;
    settextstyle(&fontStyle);
    settextcolor(RGB(50,50,50));

    const wchar_t* lines[] = {
        L"● 双击棋盘交叉点落子",
        L"● 黑棋先行，白棋后手",
        L"● 率先在横、竖、斜方向连成五子的一方获胜",
        L"● 每方总时间和单步限时可在设置中切换",
        L"● 点击右下角按钮可退出或重启游戏"
    };
    int line_count = sizeof(lines)/sizeof(lines[0]);
    int line_spacing = 40;
    int start_y = title_y + 70;
    for (int i=0; i<line_count; i++) {
        outtextxy(card_left + 50, start_y + i*line_spacing, lines[i]);
    }

    back_hover_button = -1;
    back_pressed_button = -1;
    Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
}

void Draw_Setting() {
    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_universal_background);

    int card_width = 580, card_height = 380;
    int card_left = (BOARD_SIZE + INFO_SIZE - card_width) / 2;
    int card_top = (BOARD_SIZE - card_height) / 2;
    int card_right = card_left + card_width;
    int card_bottom = card_top + card_height;

    setlinestyle(PS_SOLID, 2);
    setlinecolor(RGB(200,200,200));
    setfillcolor(RGB(255,255,255));
    fillroundrect(card_left, card_top, card_right, card_bottom, 20, 20);
    roundrect(card_left, card_top, card_right, card_bottom, 20, 20);

    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE * 0.85;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);
    settextcolor(RGB(30,30,30));

    int title_x = card_left + (card_width - textwidth(L"⚙ 游戏设置")) / 2;
    int title_y = card_top + 30;
    outtextxy(title_x, title_y, L"⚙ 游戏设置");

    setlinecolor(RGB(180,180,180));
    setlinestyle(PS_SOLID, 1);
    line(card_left + 40, title_y + 45, card_right - 40, title_y + 45);

    fontStyle.lfWeight = FONT_WEIGHT;
    fontStyle.lfHeight = PIECE_SIZE * 0.55;
    settextstyle(&fontStyle);
    settextcolor(RGB(50,50,50));

    int start_y = title_y + 75;
    int line_spacing = 55;
    wchar_t music_status[50];
    if (music_flag == 1)
        swprintf(music_status, 50, L"🎵 背景音乐：开启");
    else
        swprintf(music_status, 50, L"🔇 背景音乐：关闭");
    outtextxy(card_left + 50, start_y, music_status);
    setfillcolor(RGB(245, 235, 215));
    setlinecolor(RGB(125, 82, 45));
    fillroundrect(card_right - 170, start_y - 8, card_right - 50, start_y + 32, 10, 10);
    roundrect(card_right - 170, start_y - 8, card_right - 50, start_y + 32, 10, 10);
    outtextxy(card_right - 145, start_y, music_flag == 1 ? L"关闭" : L"开启");
    outtextxy(card_left + 50, start_y + line_spacing, L"🎨 棋盘主题：木质经典");
    wchar_t total_status[80], turn_status[80];
    swprintf(total_status, 80, L"⏱ 总时间：%d 分钟", game_time_limit / 60);
    swprintf(turn_status, 80, L"⌛ 单次下棋：%d 秒", turn_time_limit);
    outtextxy(card_left + 50, start_y + 2*line_spacing, total_status);
    outtextxy(card_left + 50, start_y + 3*line_spacing, turn_status);
    setfillcolor(RGB(245, 235, 215));
    setlinecolor(RGB(125, 82, 45));
    fillroundrect(card_right - 170, start_y + 2*line_spacing - 8,
                  card_right - 50, start_y + 2*line_spacing + 32, 10, 10);
    roundrect(card_right - 170, start_y + 2*line_spacing - 8,
              card_right - 50, start_y + 2*line_spacing + 32, 10, 10);
    fillroundrect(card_right - 170, start_y + 3*line_spacing - 8,
                  card_right - 50, start_y + 3*line_spacing + 32, 10, 10);
    roundrect(card_right - 170, start_y + 3*line_spacing - 8,
              card_right - 50, start_y + 3*line_spacing + 32, 10, 10);
    outtextxy(card_right - 135, start_y + 2*line_spacing, L"切换");
    outtextxy(card_right - 135, start_y + 3*line_spacing, L"切换");

    fontStyle.lfHeight = PIECE_SIZE * 0.45;
    settextstyle(&fontStyle);
    settextcolor(RGB(150,150,150));
    outtextxy(card_left + 50, card_bottom - 45, L"更多设置项将在后续版本中开放");

    back_hover_button = -1;
    back_pressed_button = -1;
    Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
}

int Setting_Switch(ExMessage msg) {
    int card_width = 580;
    int card_left = (BOARD_SIZE + INFO_SIZE - card_width) / 2;
    int card_right = card_left + card_width;
    int card_top = (BOARD_SIZE - 380) / 2;
    int start_y = card_top + 30 + 75;
    int line_spacing = 55;
    if (msg.message == WM_LBUTTONUP &&
        Is_InRect(msg.x, msg.y, card_right - 170, start_y - 8, card_right - 50, start_y + 32)) {
        music_flag = music_flag == 1 ? 0 : 1;
        Draw_Setting();
        FlushBatchDraw();
        return MAIN_SETTING;
    }
    static const int game_limits[] = { 300, 600, 900, 1800 };
    static const int turn_limits[] = { 15, 30, 45, 60 };
    if (msg.message == WM_LBUTTONUP &&
        Is_InRect(msg.x, msg.y, card_right - 190, start_y + 2 * line_spacing - 8,
                  card_right - 40, start_y + 2 * line_spacing + 32)) {
        int game_index = 0;
        for (int i = 0; i < 4; i++) {
            if (game_limits[i] == game_time_limit) game_index = i;
        }
        game_time_limit = game_limits[(game_index + 1) % 4];
        Draw_Setting();
        FlushBatchDraw();
        return MAIN_SETTING;
    }
    if (msg.message == WM_LBUTTONUP &&
        Is_InRect(msg.x, msg.y, card_right - 190, start_y + 3 * line_spacing - 8,
                  card_right - 40, start_y + 3 * line_spacing + 32)) {
        int turn_index = 0;
        for (int i = 0; i < 4; i++)
            if (turn_limits[i] == turn_time_limit) turn_index = i;
        turn_time_limit = turn_limits[(turn_index + 1) % 4];
        Draw_Setting();
        FlushBatchDraw();
        return MAIN_SETTING;
    }
    return -1;
}

void Draw_Message_Page(const wchar_t* message) {
    setbkcolor(RGB(255,255,255));
    cleardevice();
    setorigin(0,0);
    putimage(0,0,&img_universal_background);

    setbkmode(TRANSPARENT);
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FW_BOLD;
    fontStyle.lfHeight = 50;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);
    settextcolor(RGB(40,40,40));

    int textW = textwidth(message);
    int textH = textheight(message);
    int x = (BOARD_SIZE + INFO_SIZE - textW) / 2;
    int y = (BOARD_SIZE - textH) / 2;
    outtextxy(x, y, message);
    FlushBatchDraw();
}

// Input_Box (宽字符版本)
bool Input_Box() {
    inputStr[0] = L'\0';
    int index = 0;
    int inputBoxWidth = 400, inputBoxHeight = 50;
    int inputBoxLeft = (BOARD_SIZE + INFO_SIZE - inputBoxWidth) / 2;
    int inputBoxTop = BOARD_SIZE / 2 + 20;
    int inputBoxRight = inputBoxLeft + inputBoxWidth;
    int inputBoxBottom = inputBoxTop + inputBoxHeight;
    int promptY = inputBoxTop - 60;

    int cancelWidth = 120, cancelHeight = 45;
    int cancelLeft = (BOARD_SIZE + INFO_SIZE - cancelWidth) / 2;
    int cancelTop = inputBoxBottom + 25;
    int cancelRight = cancelLeft + cancelWidth;
    int cancelBottom = cancelTop + cancelHeight;

    bool cancelHover = false, cancelPressed = false;

    setbkcolor(RGB(255, 255, 255));
    cleardevice();
    setorigin(0, 0);
    putimage(0, 0, &img_universal_background);

    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FW_BOLD;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);
    setbkmode(TRANSPARENT);

    while (true) {
        ExMessage msg;
        while (peekmessage(&msg, -1)) {
            bool hover = Is_InRect(msg.x, msg.y, cancelLeft, cancelTop, cancelRight, cancelBottom);
            if (msg.message == WM_MOUSEMOVE) {
                cancelHover = hover;
            } else if (msg.message == WM_LBUTTONDOWN) {
                if (hover) cancelPressed = true;
            } else if (msg.message == WM_LBUTTONUP) {
                if (cancelPressed && hover) {
                    inputStr[0] = L'\0';
                    return false;
                }
                cancelPressed = false;
            }
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_RETURN) {
                    return index > 0;
                } else if (msg.vkcode == VK_ESCAPE) {
                    inputStr[0] = L'\0';
                    return false;
                } else if (msg.vkcode == VK_BACK && index > 0) {
                    inputStr[--index] = L'\0';
                }
            } else if (msg.message == WM_CHAR) {
                wchar_t ch = (wchar_t)msg.ch;
                if (ch >= 32 && ch <= 126 && index < FONT_NUM - 1) {
                    inputStr[index++] = ch;
                    inputStr[index] = L'\0';
                }
            }
        }

        cleardevice();
        putimage(0, 0, &img_universal_background);

        fontStyle.lfHeight = 30;
        settextstyle(&fontStyle);
        settextcolor(RGB(50, 50, 50));
        int promptW = textwidth(L"请输入服务器IP:端口，按回车确认");
        outtextxy((BOARD_SIZE + INFO_SIZE - promptW) / 2, promptY, L"请输入服务器IP:端口，按回车确认");

        setlinestyle(PS_SOLID, 3);
        setlinecolor(RGB(30, 30, 30));
        rectangle(inputBoxLeft, inputBoxTop, inputBoxRight, inputBoxBottom);

        fontStyle.lfHeight = 26;
        settextstyle(&fontStyle);
        settextcolor(RGB(20, 20, 20));
        int textY = inputBoxTop + (inputBoxHeight - textheight(L"A")) / 2;
        outtextxy(inputBoxLeft + 10, textY, inputStr);

        if ((GetTickCount() / 500) % 2 == 0) {
            int caretX = inputBoxLeft + 10 + textwidth(inputStr);
            setlinecolor(RGB(0, 0, 0));
            setlinestyle(PS_SOLID, 2);
            line(caretX, textY + 2, caretX, textY + textheight(L"A") - 2);
        }

        int btnState = BUTTON_STATE_NORMAL;
        if (cancelPressed && cancelHover) btnState = BUTTON_STATE_PRESSED;
        else if (cancelHover) btnState = BUTTON_STATE_HOVER;

        int padding = 10;
        int refreshX = cancelLeft - padding, refreshY = cancelTop - padding;
        int refreshW = cancelWidth + 2 * padding, refreshH = cancelHeight + 2 * padding;
        if (refreshX < 0) { refreshW += refreshX; refreshX = 0; }
        if (refreshY < 0) { refreshH += refreshY; refreshY = 0; }
        if (refreshX + refreshW > BOARD_SIZE + INFO_SIZE) refreshW = BOARD_SIZE + INFO_SIZE - refreshX;
        if (refreshY + refreshH > BOARD_SIZE) refreshH = BOARD_SIZE - refreshY;
        if (refreshW > 0 && refreshH > 0) {
            putimage(refreshX, refreshY, refreshW, refreshH, &img_universal_background, refreshX, refreshY);
        }
        Draw_Button_Frame(cancelLeft, cancelTop, cancelRight, cancelBottom, 18, btnState);
        Draw_Button_Text(cancelLeft, cancelTop, cancelWidth, cancelHeight, L"取消", btnState);

        FlushBatchDraw();
        Sleep(10);
    }
    return true;
}
