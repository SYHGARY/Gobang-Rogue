// Main.cpp
#include "Globals.h"
#include "Menu.h"
#include "GameUI.h"
#include "GameLogic.h"
#include "Network.h"
#include "Utils.h"
#include <cstdio>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!Init_Winsock()) {
        MessageBox(NULL, L"Winsock 初始化失败！", L"错误", MB_OK | MB_ICONERROR);
        return -1;
    }

    Texture_Load();
    initgraph(BOARD_SIZE + INFO_SIZE, BOARD_SIZE, EX_DBLCLKS);
    BeginBatchDraw();
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    // 可选：显示控制台（调试）
    // HWND hConsole = GetConsoleWindow();
    // ShowWindow(hConsole, SW_SHOW);

    while (currentState != EXIT) {
        switch (currentState) {
        case MENU: {
            flushmessage();
            Draw_Main();
            FlushBatchDraw();

            ExMessage msg;
            while (currentState == MENU) {
                getmessage(&msg, EX_MOUSE);
                game_mode = Main_Switch(msg);
                if (game_mode == -1) continue;
                if (game_mode == NETWORK_MODE_LOCAL) {
                    NetworkMode = NETWORK_MODE_LOCAL;
                    currentState = PLAYING;
                    printf("本地对战模式\n");
                    isMyturn = true;
                    break;
                } else if (game_mode == NETWORK_MODE_MULTIPLAYER) {
                    currentState = MULTIPLAYER;
                    break;
                } else if (game_mode == MAIN_INSTRUCTION) {
                    currentState = INSTRUCTION;
                    break;
                } else if (game_mode == MAIN_SETTING) {
                    currentState = SETTING;
                    break;
                } else if (game_mode == MAIN_EXIT) {
                    currentState = EXIT;
                    break;
                }
            }
            break;
        }

        case INSTRUCTION: {
            flushmessage();
            Draw_Instruction();
            FlushBatchDraw();

            ExMessage msg;
            while (currentState == INSTRUCTION) {
                getmessage(&msg, EX_MOUSE | EX_KEY);
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    currentState = MENU;
                    break;
                }
                if (Back_Switch(msg, &img_universal_background) == MAIN_BACK) {
                    currentState = MENU;
                    break;
                }
            }
            break;
        }

        case MULTIPLAYER: {
            flushmessage();
            Draw_Multi();
            FlushBatchDraw();

            ExMessage msg;
            while (currentState == MULTIPLAYER) {
                getmessage(&msg, EX_MOUSE);
                int multi_mode = Multi_Switch(msg);
                if (multi_mode == NETWORK_MODE_SERVER) {
                    currentState = MULTIPLAYER_SERVER;
                    break;
                } else if (multi_mode == NETWORK_MODE_CLIENT) {
                    currentState = MULTIPLAYER_CLIENT;
                    break;
                } else if (multi_mode == MAIN_BACK) {
                    currentState = MENU;
                    break;
                }
            }
            break;
        }

        case MULTIPLAYER_SERVER: {
            flushmessage();
            NetworkMode = NETWORK_MODE_SERVER;
            printf("创建服务器\n");
            if (Create_Server()) {
                printf("服务器创建成功，等待客户端连接\n");
                u_long mode = 1;
                ioctlsocket(ServerSocket, FIONBIO, &mode);
                Draw_Message_Page(L"服务器创建成功，等待连接...");

                int cancel_width = 200, cancel_height = 50;
                int cancel_x = (BOARD_SIZE + INFO_SIZE - cancel_width) / 2;
                int cancel_y = BOARD_SIZE / 2 + 80;
                int cancel_left = cancel_x, cancel_top = cancel_y;
                int cancel_right = cancel_x + cancel_width, cancel_bottom = cancel_y + cancel_height;
                bool cancel_hover = false;

                setbkmode(TRANSPARENT);
                LOGFONT fontStyle;
                gettextstyle(&fontStyle);
                fontStyle.lfQuality = ANTIALIASED_QUALITY;
                fontStyle.lfWeight = 0;
                fontStyle.lfHeight = PIECE_SIZE / 2;
                _tcscpy_s(fontStyle.lfFaceName, L"微软雅黑");
                settextstyle(&fontStyle);
                Draw_Button_Frame(cancel_x, cancel_y, cancel_right, cancel_bottom, 20, BUTTON_STATE_NORMAL);
                Draw_Button_Text(cancel_x, cancel_y, cancel_width, cancel_height, L"取消", BUTTON_STATE_NORMAL);
                FlushBatchDraw();

                bool waiting = true;
                while (waiting) {
                    ExMessage msg;
                    if (peekmessage(&msg, EX_MOUSE)) {
                        bool hover = Is_InRect(msg.x, msg.y, cancel_left, cancel_top, cancel_right, cancel_bottom);
                        if (msg.message == WM_MOUSEMOVE) {
                            if (hover != cancel_hover) {
                                cancel_hover = hover;
                                Draw_Message_Page(L"服务器创建成功，等待连接...");
                                int cancel_state = cancel_hover ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL;
                                Draw_Button_Frame(cancel_x, cancel_y, cancel_right, cancel_bottom, 20, cancel_state);
                                Draw_Button_Text(cancel_x, cancel_y, cancel_width, cancel_height, L"取消", cancel_state);
                                FlushBatchDraw();
                            }
                        } else if (msg.message == WM_LBUTTONUP && hover) {
                            printf("取消服务器\n");
                            closesocket(ServerSocket);
                            ServerSocket = INVALID_SOCKET;
                            isConnected = false;
                            NetworkMode = NETWORK_MODE_LOCAL;
                            waiting = false;
                            currentState = MULTIPLAYER;
                            break;
                        }
                    }

                    sockaddr_in ClientAddr;
                    int addrLen = sizeof(ClientAddr);
                    SOCKET client = accept(ServerSocket, (sockaddr*)&ClientAddr, &addrLen);
                    if (client != INVALID_SOCKET) {
                        ClientSocket = client;
                        closesocket(ServerSocket);
                        ServerSocket = INVALID_SOCKET;
                        isConnected = true;
                        isMyturn = true;
                        waiting = false;
                        Draw_Message_Page(L"客户端已连接，即将开始游戏");
                        FlushBatchDraw();
                        Sleep(1500);
                        currentState = PLAYING;
                        break;
                    } else {
                        int err = WSAGetLastError();
                        if (err != WSAEWOULDBLOCK) {
                            printf("accept error: %d\n", err);
                            closesocket(ServerSocket);
                            ServerSocket = INVALID_SOCKET;
                            waiting = false;
                            currentState = MULTIPLAYER;
                            break;
                        }
                    }
                }
            } else {
                printf("服务器创建失败\n");
                Draw_Message_Page(L"服务器创建失败，返回多人页面");
                Sleep(1500);
                currentState = MULTIPLAYER;
            }
            break;
        }

        case MULTIPLAYER_CLIENT: {
            flushmessage();
            if (!Input_Box()) {
                currentState = MULTIPLAYER;
                break;
            }
            NetworkMode = NETWORK_MODE_CLIENT;
            wprintf(L"输入的服务器IP地址：%ls\n", inputStr);
            printf("正在连接中\n");

            // 将宽字符转换为窄字符
            char ipBuffer[FONT_NUM] = {0};
            WideCharToMultiByte(CP_ACP, 0, inputStr, -1, ipBuffer, sizeof(ipBuffer), NULL, NULL);

            char ip[256]; int port;
            if (!Parse_IPPort(ipBuffer, ip, sizeof(ip), port)) {
                printf("地址格式错误\n");
                Draw_Message_Page(L"地址格式错误，返回多人页面");
                Sleep(1500);
                currentState = MULTIPLAYER;
                break;
            }
            if (Connect_Server(ip, port)) {
                isMyturn = false;
                isConnected = true;
                printf("连接到服务器\n");
                Draw_Message_Page(L"连接成功，即将开始游戏");
                Sleep(1500);
                currentState = PLAYING;
            } else {
                printf("服务器连接失败\n");
                Draw_Message_Page(L"服务器连接失败，返回多人页面");
                Sleep(1500);
                currentState = MULTIPLAYER;
            }
            break;
        }

        case PLAYING: {
            int board[LINE_NUM][LINE_NUM] = {{EMPTY}};
            int player = BLACK;
            bool game_over = false;
            bool timeout_total_sent = false;

            game_remain_time = time(NULL);
            time_total_black = game_time_limit;
            time_total_white = game_time_limit;
            player_time_running = true;
            Turn_Timer_Start();

            if (NetworkMode == NETWORK_MODE_SERVER) isMyturn = true;
            else if (NetworkMode == NETWORK_MODE_CLIENT) isMyturn = false;
            else isMyturn = true;

            flushmessage();
            cleardevice();
            Draw_Board();
            Draw_Info();
            FlushBatchDraw();

            while (!game_over) {
                Player_Point(BLACK + WHITE - player);
                Turn_Timer_Update(player);
                Turn_Draw_Timer(player);
                FlushBatchDraw();

                if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
                    Network_Mode_Event(board, player, isMyturn, game_over);
                }

                ExMessage msg;
                if (peekmessage(&msg, EX_MOUSE, false)) {
                    getmessage(&msg, EX_MOUSE);
                    int game_button_result = Game_Button_Switch(msg);

                    if (game_button_result == GAME_BUTTON_RESTART) {
                        if (NetworkMode != NETWORK_MODE_LOCAL) continue;
                        printf("重启游戏\n");
                        Restart_Game(board, &player);
                        continue;
                    } else if (game_button_result == GAME_BUTTON_TAKEBACK) {
                        printf("悔棋\n");
                        if (NetworkMode == NETWORK_MODE_LOCAL)
                            Take_Back_Move(board, &player);
                        else if (isConnected && !takeback_pending && !isMyturn &&
                                 move_history_count > 0 &&
                                 move_history[move_history_count - 1].player ==
                                     (NetworkMode == NETWORK_MODE_SERVER ? BLACK : WHITE)) {
                            NetworkMessage request = {};
                            request.msgtype = MSG_TAKEBACK_REQUEST;
                            if (Send_Network_Message(ClientSocket, request))
                                takeback_pending = true;
                        }
                        continue;
                    } else if (game_button_result == GAME_BUTTON_MUSIC) {
                        Game_Music_Control(msg);
                        continue;
                    } else if (game_button_result == GAME_BUTTON_SETTING) {
                        printf("跳转到设置面\n");
                        currentState = SETTING;
                        break;
                    } else if (game_button_result == GAME_BUTTON_EXIT) {
                        printf("跳转到主页面\n");
                        if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
                            NetworkMessage quitMsg;
                            quitMsg.msgtype = MSG_QUIT;
                            Send_Network_Message(ClientSocket, quitMsg);
                        }
                        if (ClientSocket != INVALID_SOCKET) { closesocket(ClientSocket); ClientSocket = INVALID_SOCKET; }
                        if (ServerSocket != INVALID_SOCKET) { closesocket(ServerSocket); ServerSocket = INVALID_SOCKET; }
                        isConnected = false;
                        NetworkMode = NETWORK_MODE_LOCAL;
                        currentState = MENU;
                        Restart_Game(board, &player);
                        break;
                    }

                    if (NetworkMode != NETWORK_MODE_LOCAL && !isMyturn) continue;

                    int boardX = (msg.x - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;
                    int boardY = (msg.y - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;
                    int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * boardX - OFFSET;
                    int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * boardY - OFFSET;

                    if (boardX >= 0 && boardX < LINE_NUM && boardY >= 0 && boardY < LINE_NUM) {
                        if (msg.message == WM_MOUSEMOVE && board[boardX][boardY] == EMPTY) {
                            Clear_Highlight(board);
                            Draw_Highlight(boardX, boardY, board);
                        }
                        if (msg.message == WM_LBUTTONDBLCLK && board[boardX][boardY] == EMPTY) {
                            Clear_Highlight(board);
                            if (player == BLACK) {
                                Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);
                                board[boardX][boardY] = BLACK;
                                if (move_history_count < MAX_MOVE_COUNT)
                                    move_history[move_history_count++] = { boardX, boardY, BLACK };
                                if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
                                    NetworkMessage turnMsg;
                                    turnMsg.msgtype = MSG_MOVE;
                                    turnMsg.x = boardX;
                                    turnMsg.y = boardY;
                                    turnMsg.player = BLACK;
                                    if (Send_Network_Message(ClientSocket, turnMsg))
                                        printf("发送黑棋落子：(%d,%d)\n", turnMsg.x, turnMsg.y);
                                    isMyturn = false;
                                }
                            } else if (player == WHITE) {
                                Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
                                board[boardX][boardY] = WHITE;
                                if (move_history_count < MAX_MOVE_COUNT)
                                    move_history[move_history_count++] = { boardX, boardY, WHITE };
                                if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
                                    NetworkMessage turnMsg;
                                    turnMsg.msgtype = MSG_MOVE;
                                    turnMsg.x = boardX;
                                    turnMsg.y = boardY;
                                    turnMsg.player = WHITE;
                                    if (Send_Network_Message(ClientSocket, turnMsg))
                                        printf("发送白棋落子：(%d,%d)\n", turnMsg.x, turnMsg.y);
                                    isMyturn = false;
                                }
                            }

                            if (Judge_Win_Chess(board, boardX, boardY, player) != EMPTY) {
                                // 等待玩家操作
                                while (true) {
                                    if (peekmessage(&msg, EX_MOUSE, false)) {
                                        getmessage(&msg, EX_MOUSE);
                                        int btn_res = Game_Button_Switch(msg);
                                        if (btn_res == GAME_BUTTON_RESTART) {
                                            if (NetworkMode != NETWORK_MODE_LOCAL) continue;
                                            printf("重启游戏\n");
                                            choice = 0;
                                            Restart_Game(board, &player);
                                            break;
                                        } else if (btn_res == GAME_BUTTON_TAKEBACK) {
                                            continue;
                                        } else if (btn_res == GAME_BUTTON_MUSIC) {
                                            Game_Music_Control(msg);
                                            continue;
                                        } else if (btn_res == GAME_BUTTON_SETTING) {
                                            printf("跳转到设置面\n");
                                            currentState = SETTING;
                                            break;
                                        } else if (btn_res == GAME_BUTTON_EXIT) {
                                            choice = 1;
                                            printf("跳转到主页面\n");
                                            if (ClientSocket != INVALID_SOCKET) { closesocket(ClientSocket); ClientSocket = INVALID_SOCKET; }
                                            if (ServerSocket != INVALID_SOCKET) { closesocket(ServerSocket); ServerSocket = INVALID_SOCKET; }
                                            isConnected = false;
                                            NetworkMode = NETWORK_MODE_LOCAL;
                                            currentState = MENU;
                                            Restart_Game(board, &player);
                                            break;
                                        }
                                    }
                                }
                                if (choice == 0) continue;
                                else break;
                            }

                            player = BLACK + WHITE - player;
                            Turn_Timer_Start();
                        }
                    } else {
                        Clear_Highlight(board);
                    }
                    FlushBatchDraw();
                    Turn_Timer_Update(player);
                    Turn_Draw_Timer(player);
                    FlushBatchDraw();
                }

                int total_timer_result = Judge_Win_Timer(player);
                if (total_timer_result != EMPTY) {
                    if (NetworkMode != NETWORK_MODE_LOCAL && isConnected && !timeout_total_sent) {
                        NetworkMessage timeoutTotalMsg;
                        timeoutTotalMsg.msgtype = MSG_TIMEOUT_TOTAL;
                        timeoutTotalMsg.player = total_timer_result;
                        Send_Network_Message(ClientSocket, timeoutTotalMsg);
                        timeout_total_sent = true;
                    }
                    FlushBatchDraw();
                    _getch();
                    game_over = true;
                    break;
                }

                if (!game_time_running) {
                    if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
                        NetworkMessage timeoutMsg;
                        timeoutMsg.msgtype = MSG_TIMEOUT;
                        timeoutMsg.player = player;
                        Send_Network_Message(ClientSocket, timeoutMsg);
                    }
                    player = BLACK + WHITE - player;
                    if (NetworkMode != NETWORK_MODE_LOCAL) isMyturn = !isMyturn;
                    Turn_Timer_Start();
                    Clear_Highlight(board);
                    FlushBatchDraw();
                }
                FlushBatchDraw();
            }
            currentState = MENU;
            break;
        }

        case SETTING: {
            flushmessage();
            Draw_Setting();
            FlushBatchDraw();

            ExMessage msg;
            while (currentState == SETTING) {
                getmessage(&msg, EX_MOUSE | EX_KEY);
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                    currentState = MENU;
                    break;
                }
                if (Back_Switch(msg, &img_universal_background) == MAIN_BACK) {
                    currentState = MENU;
                    break;
                }
                if (Setting_Switch(msg) == MAIN_SETTING) continue;
            }
            break;
        }

        default:
            break;
        }
    }

    EndBatchDraw();
    closegraph();

    if (ClientSocket != INVALID_SOCKET) {
        NetworkMessage quitMsg;
        quitMsg.msgtype = MSG_QUIT;
        Send_Network_Message(ClientSocket, quitMsg);
        closesocket(ClientSocket);
    }
    if (ServerSocket != INVALID_SOCKET) {
        closesocket(ServerSocket);
    }
    WSACleanup();
    return 0;
}
