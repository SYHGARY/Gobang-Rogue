// Network.cpp
#include "Network.h"
#include "GameUI.h"      // 用于 Put_Transparent_Image, Clear_Highlight 等
#include "GameLogic.h"   // 用于 Judge_Win_Chess, Turn_Timer_Start
#include <tchar.h>

//=====================/Winsock 初始化/=====================
bool Init_Winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (result == 0);
}

//=====================/服务器创建/=====================
bool Create_Server() {
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET) {
        printf_s("Socket创建失败：%d\n", WSAGetLastError());
        return false;
    }
    sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(PORT);

    int reuse = 1;
    setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    if (bind(ServerSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {
        printf_s("绑定失败：%d\n", WSAGetLastError());
        closesocket(ServerSocket);
        return false;
    }
    if (listen(ServerSocket, 1) == SOCKET_ERROR) {
        printf_s("监听失败：%d\n", WSAGetLastError());
        closesocket(ServerSocket);
        return false;
    }
    printf("服务器在端口%d监听\n", PORT);
    return true;
}

//=====================/客户端连接服务器/=====================
bool Connect_Server(const char* ip, int port) {
    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSocket == INVALID_SOCKET) {
        printf_s("Socket创建失败：%d\n", WSAGetLastError());
        return false;
    }
    sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(port);
    ServerAddr.sin_addr.s_addr = inet_addr(ip);
    if (ServerAddr.sin_addr.s_addr == INADDR_NONE) {
        struct hostent* remoteHost = gethostbyname(ip);
        if (remoteHost == NULL) {
            printf("域名解析失败：%d\n", WSAGetLastError());
            closesocket(ClientSocket);
            return false;
        }
        ServerAddr.sin_addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
    }
    if (connect(ClientSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {
        printf("连接失败：%d\n", WSAGetLastError());
        closesocket(ClientSocket);
        return false;
    }
    printf("客户端成功连接 %s:%d\n", ip, port);
    return true;
}

//=====================/服务端接受连接/=====================
bool Accept_Connection() {
    sockaddr_in ClientAddr;
    int addrLen = sizeof(ClientAddr);
    ClientSocket = accept(ServerSocket, (sockaddr*)&ClientAddr, &addrLen);
    if (ClientSocket == INVALID_SOCKET) {
        printf("接受连接失败：%d\n", WSAGetLastError());
        return false;
    }
    printf("连接已建立\n");
    closesocket(ServerSocket);
    ServerSocket = INVALID_SOCKET;
    return true;
}

//=====================/可靠发送（全部发送）/=====================
static bool Send_All(SOCKET s, const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int n = send(s, data + sent, len - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

bool Send_Network_Message(SOCKET socket, const NetworkMessage& msg) {
    return Send_All(socket, reinterpret_cast<const char*>(&msg), sizeof(msg));
}

bool Receive_Network_Message(SOCKET socket, NetworkMessage& msg) {
    int recvBytes = recv(socket, (char*)&msg, sizeof(msg), 0);
    return (recvBytes > 0);
}

//=====================/网络事件处理（非阻塞轮询）/=====================
void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player, bool& isMyturn, bool& game_over) {
    if (!isConnected || game_over) return;
    setorigin(BOARD_SIZE / 2, BOARD_SIZE / 2);

    u_long mode = 1;
    ioctlsocket(ClientSocket, FIONBIO, &mode);

    NetworkMessage msg;
    if (Receive_Network_Message(ClientSocket, msg)) {
        switch (msg.msgtype) {
        case MSG_GAME_SETTINGS: {
            if (NetworkMode == NETWORK_MODE_CLIENT && msg.x > 0 && msg.y > 0) {
                game_time_limit = msg.x;
                turn_time_limit = msg.y;
                time_total_black = game_time_limit;
                time_total_white = game_time_limit;
                turn_remain_time = turn_time_limit;
                player_time_running = true;
                Turn_Timer_Start();
                Draw_Info();
                FlushBatchDraw();
            }
            break;
        }
        case MSG_MOVE: {
            if (!Is_Valid_Move_Message(msg)) break;
            if (Board[msg.x][msg.y] != EMPTY) break;
            Clear_Highlight(Board);
            int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * msg.x - OFFSET;
            int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * msg.y - OFFSET;
            if (msg.player == BLACK) {
                Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);
                Board[msg.x][msg.y] = BLACK;
                if (move_history_count < MAX_MOVE_COUNT)
                    move_history[move_history_count++] = { msg.x, msg.y, BLACK };
                if (Judge_Win_Chess(Board, msg.x, msg.y, msg.player) == msg.player) {
                    _getch();   // 等待按键继续
                }
                player = WHITE;
            } else if (msg.player == WHITE) {
                Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
                Board[msg.x][msg.y] = WHITE;
                if (move_history_count < MAX_MOVE_COUNT)
                    move_history[move_history_count++] = { msg.x, msg.y, WHITE };
                if (Judge_Win_Chess(Board, msg.x, msg.y, msg.player) == msg.player) {
                    _getch();
                }
                player = BLACK;
            }
            isMyturn = true;
            Turn_Timer_Start();
            FlushBatchDraw();
            break;
        }
        case MSG_TAKEBACK_REQUEST: {
            int opponent_player = NetworkMode == NETWORK_MODE_SERVER ? WHITE : BLACK;
            bool valid_request = isMyturn && move_history_count > 0 &&
                move_history[move_history_count - 1].player == opponent_player;
            int result = valid_request
                ? MessageBoxW(GetHWnd(), L"对方申请悔棋，是否同意？", L"悔棋申请", MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND | MB_TOPMOST)
                : IDNO;
            NetworkMessage response = {};
            response.msgtype = MSG_TAKEBACK_RESPONSE;
            response.player = result == IDYES ? 1 : 0;
            Send_Network_Message(ClientSocket, response);
            if (result == IDYES) {
                Take_Back_Moves(Board, &player, 1);
                isMyturn = false;
            }
            break;
        }
        case MSG_TAKEBACK_RESPONSE: {
            takeback_pending = false;
            if (msg.player == 1) {
                Take_Back_Moves(Board, &player, 1);
                isMyturn = true;
            }
            else MessageBoxW(GetHWnd(), L"对方拒绝了悔棋申请。", L"悔棋", MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
            break;
        }
        case MSG_QUIT: {
            printf_s("对方断开连接\n");
            isConnected = false;
            game_over = true;
            break;
        }
        case MSG_TIMEOUT: {
            if (msg.player == player) {
                player = BLACK + WHITE - player;
                isMyturn = !isMyturn;
                Turn_Timer_Start();
                Clear_Highlight(Board);
                FlushBatchDraw();
            }
            break;
        }
        case MSG_TIMEOUT_TOTAL: {
            setorigin(0, 0);
            setbkmode(TRANSPARENT);
            LOGFONT fontStyle;
            gettextstyle(&fontStyle);
            fontStyle.lfQuality = ANTIALIASED_QUALITY;
            fontStyle.lfWeight = FONT_WEIGHT;
            fontStyle.lfHeight = PIECE_SIZE;
            _tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
            settextstyle(&fontStyle);
            putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS,
                     (int)(PIECE_SIZE * 4 * 0.75), PIECE_SIZE, &img_info_background,
                     (int)(INFO_SIZE / 2 - 1.5 * PIECE_SIZE), FONT_POS);
            if (msg.player == BLACK) {
                outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
            } else if (msg.player == WHITE) {
                outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
            }
            FlushBatchDraw();
            _getch();
            game_over = true;
            break;
        }
        default:
            break;
        }
    }

    // 恢复阻塞模式
    mode = 0;
    ioctlsocket(ClientSocket, FIONBIO, &mode);
    setorigin(0, 0);
}

//=====================/辅助解析与验证/=====================
bool Parse_IPPort(const char* input, char* ip, int host_len, int& port) {
    if (input == NULL || input[0] == '\0') return false;
    const char* colon = strchr(input, ':');
    if (colon == NULL) {
        // 无冒号，整体作为IP，使用默认端口
        strncpy_s(ip, host_len, input, _TRUNCATE);
        port = PORT;
        return true;
    }
    int host_part_len = colon - input;
    if (host_part_len <= 0 || host_part_len >= host_len) return false;
    strncpy_s(ip, host_len, input, host_part_len);
    ip[host_part_len] = '\0';
    const char* port_str = colon + 1;
    char* endptr;
    long p = strtol(port_str, &endptr, 10);
    if (*endptr != '\0' || p <= 0 || p > 65535) return false;
    port = (int)p;
    return true;
}

bool Is_Valid_Move_Message(const NetworkMessage& msg) {
    return msg.msgtype == MSG_MOVE &&
           msg.x >= 0 && msg.x < LINE_NUM &&
           msg.y >= 0 && msg.y < LINE_NUM &&
           (msg.player == BLACK || msg.player == WHITE);
}
