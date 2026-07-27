// Network.h
#pragma once
#include "Globals.h"

bool Init_Winsock();
bool Create_Server();
bool Connect_Server(const char* ip, int port);
bool Accept_Connection();
bool Send_Network_Message(SOCKET socket, const NetworkMessage& msg);
bool Receive_Network_Message(SOCKET socket, NetworkMessage& msg);
void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player, bool& isMyturn, bool& game_over);

bool Parse_IPPort(const char* input, char* ip, int host_len, int& port);
bool Is_Valid_Move_Message(const NetworkMessage& msg);