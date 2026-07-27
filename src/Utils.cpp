// Utils.cpp
#include "Utils.h"
#include "GameUI.h"
#include <cwchar>
#include <string>
#include <windows.h>

// ==================== 辅助函数：获取 EXE 目录 ====================
static std::wstring GetExeDirectory() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring exePath(path);
    size_t pos = exePath.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        exePath = exePath.substr(0, pos + 1);
    }
    return exePath;
}

static std::wstring GetFullPath(const wchar_t* relativePath) {
    std::wstring fullPath = GetExeDirectory();
    fullPath += relativePath;
    return fullPath;
}

// ==================== 辅助：加载图片并检查 ====================
static bool LoadImageSafe(IMAGE* img, const wchar_t* relativePath, int width = 0, int height = 0) {
    std::wstring fullPath = GetFullPath(relativePath);
    int result = loadimage(img, fullPath.c_str(), width, height);
    if (result != 0) {
        wchar_t msg[512];
        wsprintfW(msg, L"加载图片失败：\n%s\n错误码：%d", fullPath.c_str(), result);
        MessageBoxW(NULL, msg, L"图片加载错误", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

// ==================== 素材加载 ====================
void Texture_Load() {
    // 显示当前 EXE 所在目录（用于调试）
    std::wstring exeDir = GetExeDirectory();
    // MessageBoxW(NULL, (L"EXE 目录：\n" + exeDir).c_str(), L"调试信息", MB_OK);

    // 加载所有图片
    LoadImageSafe(&img_headline, L"assets/image/menu/headline_inverted.png", HEADLINE_WIDTH, HEADLINE_HEIGHT);
    LoadImageSafe(&img_headline_opp, L"assets/image/menu/headline.png", HEADLINE_WIDTH, HEADLINE_HEIGHT);
    LoadImageSafe(&img_menu_background, L"assets/image/menu/menu_background.png", BOARD_SIZE + INFO_SIZE, BOARD_SIZE);

    LoadImageSafe(&img_universal_background, L"assets/image/menu/other_background.png", BOARD_SIZE + INFO_SIZE, BOARD_SIZE);
    LoadImageSafe(&img_game_background, L"assets/image/chessboard/chessboard.jpg", BOARD_SIZE, BOARD_SIZE);
    LoadImageSafe(&img_info_background, L"assets/image/chessboard/infoboard.jpg", INFO_SIZE, BOARD_SIZE);

    LoadImageSafe(&img_white, L"assets/image/pieces/white.png", PIECE_SIZE, PIECE_SIZE);
    LoadImageSafe(&img_white_opp, L"assets/image/pieces/white_inverted.png", PIECE_SIZE, PIECE_SIZE);
    LoadImageSafe(&img_black, L"assets/image/pieces/black.png", PIECE_SIZE, PIECE_SIZE);
    LoadImageSafe(&img_black_opp, L"assets/image/pieces/black_inverted.png", PIECE_SIZE, PIECE_SIZE);
}

// ==================== 音乐控制 ====================
void Game_Music_Control(ExMessage msg) {
    if (msg.message != WM_LBUTTONUP || Game_Hit_Button(msg.x, msg.y) != GAME_BUTTON_MUSIC) {
        return;
    }
    if (music_flag == 0) {
        mciSendString(L"close GameBGM", NULL, 0, NULL);
        std::wstring musicPath = GetFullPath(L"assets/audio/game_music.mp3");
        std::wstring cmd = L"open \"" + musicPath + L"\" alias GameBGM";
        if (mciSendString(cmd.c_str(), NULL, 0, NULL) != 0) {
            MessageBoxW(NULL, L"音乐文件加载失败", L"错误", MB_OK);
        } else {
            mciSendString(L"play GameBGM repeat", NULL, 0, NULL);
            music_flag = 1;
            wprintf(L"开启音乐\n");
        }
    } else {
        mciSendString(L"stop GameBGM", NULL, 0, NULL);
        music_flag = 0;
        wprintf(L"关闭音乐\n");
    }
    Draw_Game_Button(GAME_BUTTON_MUSIC, BUTTON_STATE_HOVER);
}

// ==================== 透明图片绘制 ====================
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img) {
    putimage(x, y, mask, SRCAND);
    putimage(x, y, img, SRCPAINT);
}

// ==================== 清理信息区背景 ====================
void Clear_Info_Background(int x, int y, int width, int height) {
    int src_x = x - BOARD_SIZE;
    int src_y = y;
    if (src_x < 0) {
        width += src_x;
        x -= src_x;
        src_x = 0;
    }
    if (src_y < 0) {
        height += src_y;
        y -= src_y;
        src_y = 0;
    }
    if (src_x + width > INFO_SIZE) {
        width = INFO_SIZE - src_x;
    }
    if (src_y + height > BOARD_SIZE) {
        height = BOARD_SIZE - src_y;
    }
    if (width > 0 && height > 0) {
        putimage(x, y, width, height, &img_info_background, src_x, src_y);
    }
}

// ==================== 碰撞检测 ====================
bool Is_InCircle(int px, int py, int cx, int cy, int radius) {
    int dx = px - cx, dy = py - cy;
    return (radius * radius >= dx * dx + dy * dy);
}

bool Is_InRect(int px, int py, int left, int top, int right, int bottom) {
    return (left <= px && px <= right && top <= py && py <= bottom);
}

// ==================== 通用按钮绘制 ====================
void Draw_Button_Frame(int left, int top, int right, int bottom, int radius, int button_state) {
    COLORREF fill_color = RGB(255, 248, 236);
    COLORREF line_color = RGB(125, 82, 45);
    COLORREF shadow_color = RGB(185, 165, 145);
    COLORREF light_color = RGB(255, 255, 255);
    int shadow_offset = 4;

    if (button_state == BUTTON_STATE_HOVER) {
        fill_color = RGB(255, 235, 205);
        line_color = RGB(95, 55, 25);
        shadow_color = RGB(160, 135, 110);
    } else if (button_state == BUTTON_STATE_PRESSED) {
        fill_color = RGB(232, 205, 170);
        line_color = RGB(75, 45, 25);
        shadow_offset = 2;
    }

    setlinestyle(PS_SOLID, 1);
    setlinecolor(shadow_color);
    setfillcolor(shadow_color);
    fillroundrect(left + shadow_offset, top + shadow_offset, right + shadow_offset, bottom + shadow_offset, radius, radius);

    setlinestyle(PS_SOLID, 2);
    setlinecolor(line_color);
    setfillcolor(fill_color);
    fillroundrect(left, top, right, bottom, radius, radius);
    roundrect(left, top, right, bottom, radius, radius);

    setlinestyle(PS_SOLID, 1);
    setlinecolor(light_color);
    line(left + radius / 2, top + 5, right - radius / 2, top + 5);
}

void Draw_Button_Text(int left, int top, int width, int height, const wchar_t* text, int button_state) {
    LOGFONT fontStyle;
    gettextstyle(&fontStyle);
    fontStyle.lfQuality = ANTIALIASED_QUALITY;
    fontStyle.lfWeight = FONT_WEIGHT;
    if (height >= 64)
        fontStyle.lfHeight = 30;
    else if (height >= 50)
        fontStyle.lfHeight = 24;
    else
        fontStyle.lfHeight = 20;
    wcscpy(fontStyle.lfFaceName, L"微软雅黑");
    settextstyle(&fontStyle);

    int text_offset = (button_state == BUTTON_STATE_PRESSED) ? 2 : 0;
    settextcolor(RGB(65, 38, 18));
    setbkmode(TRANSPARENT);
    outtextxy(left + (width - textwidth(text)) / 2 + text_offset,
              top + (height - textheight(text)) / 2 + text_offset,
              text);
}

// ==================== 返回按钮 ====================
void Draw_Back_Button(int button_state, IMAGE* background_img) {
    int padding = 12;
    int refresh_x = button_back_x - padding;
    int refresh_y = button_back_y - padding;
    int refresh_w = button_back_width + 2 * padding;
    int refresh_h = button_back_height + 2 * padding;
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

    Draw_Button_Frame(button_back_x, button_back_y,
                      button_back_x + button_back_width,
                      button_back_y + button_back_height,
                      18, button_state);
    Draw_Button_Text(button_back_x, button_back_y, button_back_width, button_back_height, L"返回", button_state);
}

int Back_Switch(ExMessage msg, IMAGE* background_img) {
    int current_button = -1;
    if (Is_InRect(msg.x, msg.y, button_back_x, button_back_y,
                  button_back_x + button_back_width, button_back_y + button_back_height)) {
        current_button = MAIN_BACK;
    }

    if (msg.message == WM_MOUSEMOVE) {
        if (back_pressed_button != -1) {
            if (back_hover_button != current_button) {
                back_hover_button = current_button;
                Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_PRESSED : BUTTON_STATE_HOVER, background_img);
                FlushBatchDraw();
            }
        } else if (back_hover_button != current_button) {
            back_hover_button = current_button;
            Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL, background_img);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONDOWN) {
        back_pressed_button = current_button;
        if (back_pressed_button == MAIN_BACK) {
            Draw_Back_Button(BUTTON_STATE_PRESSED, background_img);
            FlushBatchDraw();
        }
    } else if (msg.message == WM_LBUTTONUP) {
        int pressed_button = back_pressed_button;
        back_pressed_button = -1;
        if (pressed_button == MAIN_BACK) {
            Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL, background_img);
            FlushBatchDraw();
        }
        if (pressed_button == MAIN_BACK && current_button == MAIN_BACK) {
            wprintf(L"返回上一级页面\n");
            return MAIN_BACK;
        }
    }
    return -1;
}