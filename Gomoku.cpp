//=====================/头文件/=====================

//预处理定义
#define _CRT_SECURE_NO_WARNINGS				//禁用CRT安全警告
#define _WINSOCK_DEPRECATED_NO_WARNINGS		//禁用Winsock弃用警告
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX							//避免min/max宏冲突

//Windows API
#include <winsock2.h>	//Winsock2 API
#include<ws2tcpip.h>	//Winsock2 API拓展
#include<windows.h>		//Windows API
#include<mmsystem.h>	//多媒体API
#include <shellscalingapi.h>

//图像库头文件
#include<easyx.h>
#include<conio.h>

//标准头文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<random>
#include<math.h>

//库链接
#pragma comment(lib, "ws2_32.lib")	//Winsock2库
#pragma comment(lib, "winmm.lib")	//连接Windows媒体库，用于播放音乐
#pragma comment(lib, "Msimg32.lib") // Windows图像处理库
#pragma comment(lib, "user32.lib")  // 用户界面库
#pragma comment(lib, "gdi32.lib")   // GDI图形库
#pragma comment(lib, "Shcore.lib")

//=====================/常量设定/=====================

//棋盘绘制位置常量
#define LINE_NUM 15					//线条数
#define BOARD_SIZE 650				//棋盘大小
#define BOARD_MARGIN 280			//棋盘横线边缘
#define BOARD_MARGIN_REAL 45		//棋盘横线边缘真实距离  
#define OFFSET 345					//鼠标坐标与绘图坐标偏移量
#define DOT_POS 160					//星位点位置
#define DOT_RADIUS 5				//星位点半径
#define BOARD_ORIGIN_X 325			//初始X坐标
#define BOARD_ORIGIN_Y 325			//初始Y坐标
#define PIECE_SIZE 40				//棋子大小 / 格子大小

//信息区绘制位置常量
#define INFO_SIZE 450				//信息区大小
#define BUTTON_SIZE 45				//按钮大小
#define BUTTON_GAP	10				//按钮间隔
#define BUTTON_POS 2.5				//按钮高度位置
#define ELEMENT_GAP 10				//元素间隔

//字体常量
#define FONT_WEIGHT 700				//字体粗细
#define FONT_POS 2.5				//字体位置
#define FONT_NUM 100				//字体数量
#define HEADLINE_WIDTH	670			//标题宽度
#define HEADLINE_HEIGHT	200			//标题高度

//棋盘状态常量
#define EMPTY 0						//无棋子
#define BLACK 1						//黑棋
#define WHITE 2						//白棋

//时间计时常量
#define TIME_LIMIT_TOTAL 600		//总时长限制10分钟
#define TIME_LIMIT 30				//单步限制30秒
#define TIME_WARN 10				//剩余10秒警告
#define FLASH_CYCLE 500				//闪烁周期为500个循环一闪

//网络模式常量（后续使用图形化界面替代）
#define PORT 8888					//默认网络端口
#define NETWORK_MODE_LOCAL	1		//本地对战
#define NETWORK_MODE_MULTIPLAYER 2	//多人对战
#define NETWORK_MODE_SERVER	3		//网络对战（服务端）
#define NETWORK_MODE_CLIENT	4		//网络对战（客户端）
#define NETWORK_MODE_AI	5			//人机对战

#define MAIN_INSTRUCTION 6			//游戏说明
#define MAIN_SETTING 7				//游戏设置
#define MAIN_EXIT 8					//退出游戏

//网络状态常量
#define MSG_MOVE 0					//落子信息
#define MSG_QUIT 1					//退出信息
#define MSG_CHAT 2					//聊天信息
#define MSG_TIMEOUT 3				//超时信息
#define MSG_TIMEOUT_TOTAL 4			//总时间耗尽信息

//=====================/枚举量设定/=====================

//状态机设定
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

//=====================/结构体设定/=====================

//网络数据包内容
struct NetworkMessage {
	int msgtype;	//消息类型
	int x, y;		//棋盘坐标
	int player;		//玩家
	char data[256];	//其他数据
};

//=====================/公共变量设定/=====================

//初始设定公共变量
int highlightX = -1, highlightY = -1;			//记录高光坐标

//时间公共变量
time_t game_start_time;							//游戏开始时间
time_t turn_start_time;							//回合开始时间
static int turn_passed_last = 0;				//记录已经扣除的时间
int time_total_black = TIME_LIMIT_TOTAL;		//黑方总剩余时间
int time_total_white = TIME_LIMIT_TOTAL;		//白方总剩余时间
int game_remain_time;							//游戏剩余时间
int turn_remain_time;							//回合剩余时间
bool game_time_running;							//计时器是否运行
bool player_time_running = true;				//是否有玩家总时间耗尽

//图片公共变量
IMAGE img_headline, img_headline_opp;									//游戏标题图片
IMAGE img_start_background, img_game_background, img_info_background;	//背景图片
IMAGE img_white, img_white_opp, img_black, img_black_opp;				//棋子图片

//游戏坐标公共变量

//字体公共变量
wchar_t inputStr[FONT_NUM] = L"";	//需要输入的初始字符

//主界面按钮
int button_width = 400;
int button_height = 50;

int headline_x = (BOARD_SIZE + INFO_SIZE - HEADLINE_WIDTH) / 2;
int button_mainall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;

int headline_y = 50;
int button_singleplayer_y = headline_y + HEADLINE_HEIGHT + 4 * ELEMENT_GAP;
int button_multiplayer_y = button_singleplayer_y + button_height + ELEMENT_GAP;
int button_instruction_y = button_multiplayer_y + button_height + ELEMENT_GAP;
int button_mainsetting_y = button_instruction_y + button_height + ELEMENT_GAP;
int button_mainexit_y = button_mainsetting_y + button_height + ELEMENT_GAP;

//多人界面按钮
int button_multiall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;

int button_createserver_y = (BOARD_SIZE - 8 * ELEMENT_GAP - 2 * button_height) / 2;
int button_joinserver_y = button_createserver_y + button_height + 8 * ELEMENT_GAP;


//游戏界面按钮
int button_restart_x = BOARD_SIZE + BUTTON_GAP + BUTTON_SIZE / 2;																	//重新开始按钮X坐标 -> 第一个按钮
int button_takeback_x = BOARD_SIZE + BUTTON_GAP + BUTTON_SIZE + BUTTON_GAP + BUTTON_SIZE / 2;										//悔棋按钮X坐标 -> 第二个按钮
int button_exit_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;															//退出按钮X坐标	-> 第五个按钮
int button_setting_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;							//设置按钮X坐标 -> 第四个按钮
int button_music_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;	//音乐按钮X坐标 -> 第三个按钮

int button_playingall_y = BOARD_SIZE - BUTTON_POS - BUTTON_SIZE;																	//所有按钮Y坐标

//游戏功能相关公共变量
int game_mode;				//游戏模式
int music_flag = 1;			//音乐是否开启标识
char choice;				//用于让界面暂停

//网络相关公共变量
SOCKET ServerSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
int NetworkMode = NETWORK_MODE_LOCAL;	//网络状态默认本地
char ServerIP[20] = "127.0.0.1";		//默认为回送地址
bool isConnected = false;				//检测是否连接成功
bool isMyturn = true;					//防止玩家在对手回合下棋

//状态机相关公共变量
GameState currentState = MENU;

//=====================/总体功能函数声明/=====================

//1、基础功能函数声明
void Texture_Load();															//素材导入函数
void Game_Music_Control(ExMessage msg);											//游戏音乐控制函数
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img);	//透明图片载入函数
bool Is_InCirecle(int px, int py, int cx, int cy, int radius);					//是否在圆内判断函数
bool Is_InRect(int px, int py, int left, int top, int right, int bottom);		//是否在方框内判断函数

//2、主界面与功能函数
void Draw_Main();
int Main_Switch(ExMessage msg);													//主页面跳转

//3、多人界面与功能函数
void Draw_Multi();
int Multi_Switch(ExMessage msg);
void Input_Box();

//4、游戏界面与功能函数声明
void Draw_Board();	//五子棋盘绘制函数
void Draw_Info();	//信息区域绘制函数

void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]);	//绘制高光函数
void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]);				//清理高光函数（局部重绘提高效率）
void Player_Point(int player);										//玩家指向函数

void Turn_Timer_Start();											//回合计时初始化函数
void Turn_Timer_Update(int player);									//回合剩余时间计算函数
void Turn_Draw_Timer(int player);									//绘制倒计时函数

int Judge_Win_Chess(int Board[LINE_NUM][LINE_NUM], int x, int y, int player);	//判断游戏结束函数（这里的X，Y代表的是现在下的棋的位置，因为其实只有现在下的棋改变了棋盘的状态，提高搜索效率）
int Judge_Win_Timer(int player);												//时间判断游戏结束函数

bool Switch_To_Setting(ExMessage msg);											//跳转到设置页面
bool Switch_To_Menu(ExMessage msg);												//跳转到主页面

//5、游戏玩法函数声明
void Take_Back_Move();

//6、网络功能函数声明
bool Parse_IPPort(const char* input, char* ip, int host_len, int& port);									//分离IP:端口
bool Init_Winsock();																						//初始化Winsock
bool Create_Server();																						//创建服务器（服务端）
bool Connect_Server(const char* ip, int port);																//连接服务器（客户端）
bool Accept_Connection();																					//同意连接（服务端）
bool Send_Network_Message(SOCKET socket, const NetworkMessage& msg);										//传输数据（客户端）
bool Receive_Network_Message(SOCKET socket, NetworkMessage& msg);											//接收数据（服务端）
void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player, bool& isMyturn, bool& game_over);		//网络模式对战处理

//=====================/函数具体内容/=====================

//1、基础功能函数
void Texture_Load() {

	//MENU界面素材
	loadimage(&img_headline, _T("./素材/headline.png"), HEADLINE_WIDTH, HEADLINE_HEIGHT);				//标题彩色图
	loadimage(&img_headline_opp, _T("./素材/headline1.png"), HEADLINE_WIDTH, HEADLINE_HEIGHT);			//标题掩码图

	//PLAYING界面素材
	loadimage(&img_start_background, _T("./素材/background1.jpg"), BOARD_SIZE, BOARD_SIZE);				//游戏开始背景
	loadimage(&img_game_background, _T("./素材/wood1.jpg"), BOARD_SIZE, BOARD_SIZE);					//游戏界面背景
	loadimage(&img_info_background, _T("./素材/info_background.jpg"), INFO_SIZE, BOARD_SIZE);			//信息框背景

	loadimage(&img_white, _T("./素材/white.png"), PIECE_SIZE, PIECE_SIZE);								//白棋彩色图
	loadimage(&img_white_opp, _T("./素材/white1.png"), PIECE_SIZE, PIECE_SIZE);							//白棋掩码图
	loadimage(&img_black, _T("./素材/black.png"), PIECE_SIZE, PIECE_SIZE);								//黑棋彩色图
	loadimage(&img_black_opp, _T("./素材/black1.png"), PIECE_SIZE, PIECE_SIZE);							//黑棋掩码图
}

void Game_Music_Control(ExMessage msg) {

	if (Is_InCirecle(msg.x, msg.y, button_music_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE / 2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN) {

		if (music_flag == 0) {
			mciSendString(_T("close GameBGM"), NULL, 0, NULL);	//检测防止音乐重复打开
			mciSendString(_T("open ./素材/music1.mp3 alias GameBGM"), NULL, 0, NULL);
			mciSendString(_T("play GameBGM repeat"), NULL, 0, NULL);	//重复播放音乐
			music_flag = 1;
			printf_s("开启音乐\n");
		}
		else if (music_flag == 1) {
			mciSendString(_T("stop GameBGM"), NULL, 0, NULL);    //关闭播放音乐
			music_flag = 0;
			printf_s("关闭音乐\n");
		}
	}
}

void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img) {

	putimage(x, y, mask, SRCAND);	//掩码图，黑色部分代表透明区域
	putimage(x, y, img, SRCPAINT);	//彩色图，保留掩码图黑色部分
}

bool Is_InCirecle(int px, int py, int cx, int cy, int radius) {

	//1、计算坐标距离
	int dx, dy;
	dx = px - cx;	//X轴距离
	dy = py - cy;	//Y轴距离

	//2、判断是否在圆内
	if (radius * radius >= dx * dx + dy * dy) {
		return true;
	}
	else {
		return false;
	}
}

bool Is_InRect(int px, int py, int left, int top, int right, int bottom) {

	//判断是否在方框内
	if ((left <= px && px <= right) && (top <= py && py <= bottom)) {
		return true;
	}
	else {
		return false;
	}
}

//2、主界面与功能函数
void Draw_Main() {

	double text_all_x = (BOARD_SIZE + INFO_SIZE - 4 * (textheight(_T("字")))) / 2;
	double text_local_y = button_singleplayer_y + (button_height - textheight(_T("字"))) / 2;
	double text_multi_y = text_local_y + button_height + ELEMENT_GAP;
	double text_instruction_y = text_multi_y + button_height + ELEMENT_GAP;
	double text_mainsetting_y = text_instruction_y + button_height + ELEMENT_GAP;
	double text_mainexit_y = text_mainsetting_y + button_height + ELEMENT_GAP;

	setlinestyle(PS_SOLID, 2);
	setlinecolor(RGB(0, 0, 0));

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);												//文本背景透明
	LOGFONT fontStyle;													//创建字体结构体
	gettextstyle(&fontStyle);											//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;							//启用抗锯齿
	fontStyle.lfWeight = 0;												//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;								//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("Terminal"));
	settextstyle(&fontStyle);											//应用新字体设置

	setbkcolor(RGB(255, 255, 255));
	cleardevice();																																	//清空画面

	setorigin(0, 0);
	Put_Transparent_Image(headline_x, headline_y, &img_headline_opp, &img_headline);																//绘制标题

	roundrect(button_mainall_x, button_singleplayer_y, button_mainall_x + button_width, button_singleplayer_y + button_height, 20, 20);				//本地模式按钮
	outtextxy(text_all_x, text_local_y, _T("本地模式"));
	roundrect(button_mainall_x, button_multiplayer_y, button_mainall_x + button_width, button_multiplayer_y + button_height, 20, 20);				//多人模式按钮
	outtextxy(text_all_x, text_multi_y, _T("多人模式"));
	roundrect(button_mainall_x, button_instruction_y, button_mainall_x + button_width, button_instruction_y + button_height, 20, 20);				//玩法介绍按钮
	outtextxy(text_all_x, text_instruction_y, _T("玩法介绍"));
	roundrect(button_mainall_x, button_mainsetting_y, button_mainall_x + button_width, button_mainsetting_y + button_height, 20, 20);				//设置按钮
	outtextxy(text_all_x, text_mainsetting_y, _T("游戏设置"));
	roundrect(button_mainall_x, button_mainexit_y, button_mainall_x + button_width, button_mainexit_y + button_height, 20, 20);						//退出按钮
	outtextxy(text_all_x, text_mainexit_y, _T("退出游戏"));
}

int Main_Switch(ExMessage msg) {

	//主界面模式选择
	if (Is_InRect(msg.x, msg.y, button_mainall_x, button_singleplayer_y, button_mainall_x + button_width, button_singleplayer_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("本地模式\n");
		return NETWORK_MODE_LOCAL;
	}
	else if (Is_InRect(msg.x, msg.y, button_mainall_x, button_multiplayer_y, button_mainall_x + button_width, button_multiplayer_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("多人模式\n");
		return NETWORK_MODE_MULTIPLAYER;
	}
	else if (Is_InRect(msg.x, msg.y, button_mainall_x, button_instruction_y, button_mainall_x + button_width, button_instruction_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("游戏说明\n");
		return MAIN_INSTRUCTION;
	}
	else if (Is_InRect(msg.x, msg.y, button_mainall_x, button_mainsetting_y, button_mainall_x + button_width, button_mainsetting_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("游戏设置\n");
		return MAIN_SETTING;
	}
	else if (Is_InRect(msg.x, msg.y, button_mainall_x, button_mainexit_y, button_mainall_x + button_width, button_mainexit_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("退出游戏\n");
		return MAIN_EXIT;
	}

	return -1;	//空值返回码
}

//3、多人界面与功能函数
void Draw_Multi() {

	double text_all_x = (BOARD_SIZE + INFO_SIZE - 5 * (textheight(_T("字")))) / 2;
	double text_createserver_y = button_createserver_y + (button_height - textheight(_T("字"))) / 2;
	double text_joinserver_y = button_joinserver_y + (button_height - textheight(_T("字"))) / 2;

	setlinestyle(PS_SOLID, 2);
	setlinecolor(RGB(0, 0, 0));

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);												//文本背景透明
	LOGFONT fontStyle;													//创建字体结构体
	gettextstyle(&fontStyle);											//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;							//启用抗锯齿
	fontStyle.lfWeight = 0;												//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;								//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("Terminal"));
	settextstyle(&fontStyle);											//应用新字体设置

	setbkcolor(RGB(255, 255, 255));
	cleardevice();	//清空画面

	setorigin(0, 0);

	roundrect(button_multiall_x, button_createserver_y, button_multiall_x + button_width, button_createserver_y + button_height, 20, 20);				//创建服务器按钮
	outtextxy(text_all_x, text_createserver_y, _T("创建服务器"));
	roundrect(button_multiall_x, button_joinserver_y, button_multiall_x + button_width, button_joinserver_y + button_height, 20, 20);					//加入服务器按钮
	outtextxy(text_all_x, text_joinserver_y, _T("加入服务器"));

}

int Multi_Switch(ExMessage msg) {

	//多人界面模式选择
	if (Is_InRect(msg.x, msg.y, button_multiall_x, button_createserver_y, button_multiall_x + button_width, button_createserver_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("创建服务器\n");
		return NETWORK_MODE_SERVER;
	}
	else if (Is_InRect(msg.x, msg.y, button_multiall_x, button_joinserver_y, button_multiall_x + button_width, button_joinserver_y + button_height) && msg.message == WM_LBUTTONDOWN) {
		printf("加入服务器\n");
		return NETWORK_MODE_CLIENT;
	}

	return -1;	//空值返回码
}

void Input_Box() {

	int index = 0;
	double inputBoxLeft = (BOARD_SIZE + INFO_SIZE - button_width) / 2, inputBoxTop = BOARD_SIZE / 2 - 8 * ELEMENT_GAP;
	double inputBoxRight = inputBoxLeft + button_width, inputBoxBottom = inputBoxTop + 2.5 * textheight(_T("A"));
	double text_all_x = (BOARD_SIZE + INFO_SIZE - 16 * (textheight(_T("字")))) / 2;
	double text_input_y = inputBoxTop - 5 * ELEMENT_GAP;

	setlinestyle(PS_SOLID, 2);
	setlinecolor(RGB(0, 0, 0));

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);												//文本背景透明
	LOGFONT fontStyle;													//创建字体结构体
	gettextstyle(&fontStyle);											//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;							//启用抗锯齿
	fontStyle.lfWeight = 0;												//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;								//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("Terminal"));
	settextstyle(&fontStyle);											//应用新字体设置

	setbkcolor(RGB(255, 255, 255));
	cleardevice();	//清空画面

	while (true) {

		if (_kbhit()) {														//检测是否有输入

			wchar_t ch = _getwch();											//输入宽字符
			if (ch == _T('\r')) {											//检测到换行符结束输入
				break;
			}
			else if (ch == _T('\b') && index > 0) {							//检测到退格符删除字符
				inputStr[--index] = _T('\0');
			}
			else if (index < FONT_NUM - 1 && ch >= 32 && ch <= 126) {		//打印可打印字符
				inputStr[index++] = ch;
				inputStr[index] = _T('\0');
			}
		}

		cleardevice();														//删除前一帧
		rectangle(inputBoxLeft, inputBoxTop, inputBoxRight, inputBoxBottom);

		int textY = inputBoxTop + textheight(_T("A")) / 2;
		outtextxy(inputBoxLeft + ELEMENT_GAP / 2, textY, inputStr);
		outtextxy(text_all_x, text_input_y, _T("请输入服务器IP:端口，按回车确认"));

		FlushBatchDraw();													//刷新显示
		//Sleep(10);
	}

}

//4、游戏界面与功能函数
void Draw_Board() {

	//1、放置木质背景
	setorigin(0, 0);	//设置初始点
	putimage(0, 0, &img_game_background);	//放置游戏背景

	//2、绘制网格
	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
	setlinestyle(PS_SOLID, 1);
	setlinecolor(BLACK);
	setfillcolor(BLACK);

	for (int i = 0; i < LINE_NUM; i++) {
		line(-BOARD_MARGIN + PIECE_SIZE * i, BOARD_MARGIN, -BOARD_MARGIN + PIECE_SIZE * i, -BOARD_MARGIN);	//绘制竖线
	}
	for (int i = 0; i < LINE_NUM; i++) {
		line(-BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i, BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i);	//绘制横线
	}
	solidcircle(0, 0, DOT_RADIUS);	//绘制中心点
	solidcircle(-DOT_POS, DOT_POS, DOT_RADIUS);	//绘制左下点
	solidcircle(DOT_POS, -DOT_POS, DOT_RADIUS);	//绘制右上点
	solidcircle(-DOT_POS, -DOT_POS, DOT_RADIUS);	//绘制左上点
	solidcircle(DOT_POS, DOT_POS, DOT_RADIUS);	//绘制右下点

}

void Draw_Info() {

	//1、放置信息背景
	setorigin(0, 0);													//设置初始点
	putimage(BOARD_SIZE, 0, &img_info_background);						//放置信息背景

	setbkmode(TRANSPARENT);												//文本背景透明
	LOGFONT fontStyle;													//创建字体结构体
	gettextstyle(&fontStyle);											//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;							//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;									//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;								//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);											//应用新字体设置
	settextcolor(RGB(255, 255, 255));

	//2、游戏功能区元素
	double font_offset = PIECE_SIZE / 5;
	solidcircle(button_restart_x, button_playingall_y, BUTTON_SIZE / 2);		//重启按钮
	outtextxy(button_restart_x - font_offset, button_playingall_y - font_offset, _T("重"));

	solidcircle(button_takeback_x, button_playingall_y, BUTTON_SIZE / 2);		//悔棋按钮
	outtextxy(button_takeback_x - font_offset, button_playingall_y - font_offset, _T("悔"));

	solidcircle(button_exit_x, button_playingall_y, BUTTON_SIZE / 2);			//退出按钮
	outtextxy(button_exit_x - font_offset, button_playingall_y - font_offset, _T("退"));

	solidcircle(button_setting_x, button_playingall_y, BUTTON_SIZE / 2);		//设置按钮
	outtextxy(button_setting_x - font_offset, button_playingall_y - font_offset, _T("设"));

	solidcircle(button_music_x, button_playingall_y, BUTTON_SIZE / 2);			//音乐按钮
	outtextxy(button_music_x - font_offset, button_playingall_y - font_offset, _T("音"));

	//3、游戏联机区元素

	//3.2游戏模式显示
	double single_char = 0.35;
	double game_mode_textx = BOARD_SIZE + ELEMENT_GAP;
	double network_mode_textx = BOARD_SIZE + INFO_SIZE - 2 * ELEMENT_GAP - 8 * single_char * PIECE_SIZE;
	double game_mode_texty = PIECE_SIZE + 2 * ELEMENT_GAP + PIECE_SIZE + 3 * ELEMENT_GAP;
	double network_mode_texty = game_mode_texty;
	settextcolor(BLACK);
	outtextxy(game_mode_textx, game_mode_texty, _T("当前模式："));
	settextcolor(BLUE);
	if (game_mode == NETWORK_MODE_LOCAL) {
		outtextxy(game_mode_textx + 5 * single_char * PIECE_SIZE, game_mode_texty, _T("本地模式"));
	}
	else if (game_mode == NETWORK_MODE_SERVER || game_mode == NETWORK_MODE_CLIENT) {
		outtextxy(game_mode_textx + 5 * single_char * PIECE_SIZE, game_mode_texty, _T("联网模式"));
	}
	else if (game_mode == NETWORK_MODE_AI) {
		outtextxy(game_mode_textx + 5 * single_char * PIECE_SIZE, game_mode_texty, _T("人机模式"));
	}

	//3.3连接状态显示
	settextcolor(BLACK);
	outtextxy(network_mode_textx, network_mode_texty, _T("连接状态："));
	if (isConnected) {
		settextcolor(GREEN);
		outtextxy(network_mode_textx + 5 * single_char * PIECE_SIZE, network_mode_texty, _T("已连接"));
	}
	else {
		settextcolor(RED);
		outtextxy(network_mode_textx + 5 * single_char * PIECE_SIZE, network_mode_texty, _T("未连接"));
	}

	//4、游戏卡牌区元素
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 2);
	double card_board_left = BOARD_SIZE + ELEMENT_GAP;
	double card_board_top = game_mode_texty + single_char * PIECE_SIZE + 2 * ELEMENT_GAP;
	double card_board_right = BOARD_SIZE + INFO_SIZE - ELEMENT_GAP;
	double card_board_bottom = card_board_top + INFO_SIZE - 8 * ELEMENT_GAP;
	roundrect(card_board_left, card_board_top, card_board_right, card_board_bottom, 15, 15);

	//5、游戏聊天区元素
	double chat_board_left = card_board_left;
	double chat_board_top = card_board_bottom + ELEMENT_GAP;
	double chat_board_right = card_board_right;
	double chat_board_bottom = chat_board_top + 2 * ELEMENT_GAP;
	roundrect(chat_board_left, chat_board_top, chat_board_right, chat_board_bottom, 10, 10);

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点
}

void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]) {

	//1、保存原有数据
	highlightX = x;	//记录高光位置（对应二维棋盘坐标）
	highlightY = y;

	COLORREF origin_line_color = getlinecolor();	//保存原画笔颜色
	LINESTYLE origin_line_style;
	getlinestyle(&origin_line_style);	//保存原画笔线条形态


	//2、设置高光样式
	setlinecolor(RED);	//设置现画笔颜色为红色
	setlinestyle(PS_DASH);	//设置现画笔形态为虚线

	//3、获取图像棋盘对应位置（putimage需要图片左上角位置，circle需要圆心位置，因此不需要OFFSET）
	setorigin(0, 0);	//设置坐标原点
	int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
	int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;

	//4、绘制高光
	circle(centerX, centerY, PIECE_SIZE / 2);

	//5、恢复原画笔
	setlinecolor(origin_line_color);
	setlinestyle(&origin_line_style);
}

void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]) {

	if (highlightX == -1 || highlightY == -1) return;	//没有高光提早退出

	//1、保存原画笔设置
	COLORREF origin_line_color = getlinecolor();	//线条颜色
	LINESTYLE origin_line_style;
	getlinestyle(&origin_line_style);	//线条形态

	//2、现画笔设置
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	setlinestyle(PS_SOLID, 1);

	//3、获得高光中心坐标
	setorigin(0, 0);	//设置新坐标原点
	int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
	int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;

	//4、重绘背景与网格
	int left = centerX - PIECE_SIZE / 2;	//获得背景重绘区域（比高光微大些）
	int top = centerY - PIECE_SIZE / 2;

	//4.1、绘制背景
	putimage(left, top, PIECE_SIZE + 1, PIECE_SIZE + 1, &img_game_background, left, top);	//重绘背景(后面两个left与top代表原图片的x,y坐标)

	//4.2、绘制网格线
	int startX = left;
	int endX = left + PIECE_SIZE;
	if (highlightX == 0) startX = centerX;
	if (highlightX == LINE_NUM - 1) endX = centerX;
	line(startX, centerY, endX, centerY);	// 横线

	int startY = top;
	int endY = top + PIECE_SIZE;
	if (highlightY == 0) startY = centerY;
	if (highlightY == LINE_NUM - 1) endY = centerY;
	line(centerX, startY, centerX, endY);	// 竖线

	//4.3、重新绘制星位点
	if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) {
		solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);	//左上角星位点
	}
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) {
		solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);	//右上角星位点
	}
	else if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) {
		solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);	//左下角星位点
	}
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) {
		solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);	//右下角星位点
	}
	else if (centerX == BOARD_ORIGIN_X && centerY == BOARD_ORIGIN_Y) {
		solidcircle(BOARD_ORIGIN_X, BOARD_ORIGIN_Y, DOT_RADIUS);	//中心星位点
	}

	//5、恢复初始设置
	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复原有坐标
	setlinecolor(origin_line_color);	// 恢复画笔设置
	setlinestyle(&origin_line_style);

	highlightX = -1;	//重置高光坐标
	highlightY = -1;
}

void Player_Point(int player) {

	//1、清理上次的文字（字体高宽比约0.75）
	setorigin(0, 0);	//设置坐标中心点
	putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4 * 0.75, PIECE_SIZE, &img_info_background, INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS);

	//2、设置字体
	setbkmode(TRANSPARENT);	//文本背景透明
	settextcolor(BLACK);	//文字颜色黑色
	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//3、根据下棋方绘制文字（字体高宽比约0.75）
	if (player == WHITE) {
		//printf_s("黑棋落子\n");
		outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑棋落子"));
	}
	else if (player == BLACK) {
		//printf_s("白棋落子\n");
		outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白棋落子"));
	}
}

void Turn_Timer_Start() {

	//1、重置回合时间
	turn_start_time = time(NULL);	//记录回合开始时间
	turn_remain_time = TIME_LIMIT;	//重置回合剩余时间
	turn_passed_last = 0;			//重置本回合记录秒数
	game_time_running = true;	//设置计时器状态为开启

	//2、清理上次计时区域
	setorigin(0, 0);	//设置坐标原点
	double single_char = 0.35;
	double round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	double round_time_sizey = PIECE_SIZE / 2;

	double black_piece_x = BOARD_SIZE + 2 * ELEMENT_GAP;	//黑棋X坐标
	double white_piece_x = BOARD_SIZE + INFO_SIZE - 2 * ELEMENT_GAP - PIECE_SIZE;	//白棋X坐标

	double timer_black_x = black_piece_x + PIECE_SIZE + ELEMENT_GAP / 2;
	double timer_black_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;
	double timer_white_x = white_piece_x - 9.5 * single_char * PIECE_SIZE - ELEMENT_GAP / 2;
	double timer_white_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;

	//3、设置字体
	setbkmode(TRANSPARENT);	//文本背景透明
	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//4、绘制清空后的时间
	COLORREF timer_color = BLACK;

	putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
	outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋重置倒计时文字
	putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
	outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋重置倒计时文字
}

void Turn_Timer_Update(int player) {

	if (!game_time_running)	return;	//检查计时器是否启动，没启动提早返回

	//1、更新剩余时间
	time_t current_time = time(NULL);	//记录现在时间
	turn_remain_time = TIME_LIMIT - (int)difftime(current_time, turn_start_time);	//difftime(time1,time2)，time1为较晚时间，time2为较早时间，返回的是double类型，所以要int

	int used_time = TIME_LIMIT - turn_remain_time;	//该回合使用的时间
	if (used_time < 0) {
		used_time = 0;
	}
	if (used_time > TIME_LIMIT) {
		used_time = TIME_LIMIT;
	}

	//2、更新总时间
	int delta = used_time - turn_passed_last;	//计算新增时间 = 总共使用时间 - 已经扣除的时间
	if (delta < 0) {
		delta = 0;
	}
	turn_passed_last = used_time;	//现在的总共使用时间，变成了已经扣除的时间

	if (delta > 0) {
		if (player == BLACK) {
			time_total_black -= delta;
			if (time_total_black <= 0) {
				time_total_black = 0;
			}
		}
		else if (player == WHITE) {
			time_total_white -= delta;
			if (time_total_white <= 0) {
				time_total_white = 0;
			}
		}
	}

	//3、判断是否关闭计时器
	if (turn_remain_time < 0) {
		turn_remain_time = 0;	//重置剩余时间
		game_time_running = false;	//关闭计时器，防止溢出
	}

	if (time_total_black == 0 || time_total_white == 0) {
		player_time_running = false;	//有玩家时间耗尽，游戏结束
	}
}

void Turn_Draw_Timer(int player) {

	//1、设定各元素范围
	setorigin(0, 0);	//设置坐标原点
	//1.1计时区范围
	double round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	double round_time_sizey = PIECE_SIZE / 2;
	double total_time_sizex = PIECE_SIZE * 9 * 0.75 / 2;	//总计时区大小
	double total_time_sizey = PIECE_SIZE / 2;

	//1.2计时区棋子元素
	double single_char = 0.35;
	double black_piece_x = BOARD_SIZE + 2 * ELEMENT_GAP;	//黑棋X坐标
	double black_piece_y = PIECE_SIZE + 2 * ELEMENT_GAP;	//黑棋Y坐标
	double white_piece_x = BOARD_SIZE + INFO_SIZE - 2 * ELEMENT_GAP - PIECE_SIZE;	//白棋X坐标
	double white_piece_y = PIECE_SIZE + 2 * ELEMENT_GAP;	//白棋y坐标
	Put_Transparent_Image(black_piece_x, black_piece_y, &img_black_opp, &img_black);	//黑棋计时区
	Put_Transparent_Image(white_piece_x, white_piece_y, &img_white_opp, &img_white);	//白棋计时区

	//1.3文字坐标
	//回合计时区域
	double timer_black_x = black_piece_x + PIECE_SIZE + ELEMENT_GAP / 2;
	double timer_black_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;
	double timer_white_x = white_piece_x - 9.5 * single_char * PIECE_SIZE - ELEMENT_GAP / 2;
	double timer_white_y = PIECE_SIZE + FONT_POS + round_time_sizey - ELEMENT_GAP / 2;
	//总计时区域
	double timer_black_total_x = timer_black_x;
	double timer_black_total_y = timer_black_y + ELEMENT_GAP / 2 + PIECE_SIZE / 2;
	double timer_white_total_x = timer_white_x;
	double timer_white_total_y = timer_white_y + ELEMENT_GAP / 2 + PIECE_SIZE / 2;

	//2、设置字体
	setbkmode(TRANSPARENT);	//文本背景透明
	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE / 2;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//3、绘制计时器文字
	COLORREF timer_color;
	//3.1、根据情况设定计时器颜色
	if (turn_remain_time <= TIME_WARN && turn_remain_time > 0) {
		timer_color = RED;	//警告时间为红色
	}
	else if (turn_remain_time <= 0) {
		timer_color = RED;	//时间耗尽为红色
	}
	else {
		timer_color = BLACK;	//正常时间用黑色
	}
	settextcolor(timer_color);

	//3.2、设定文字内容（使用宽字符，宽字符一般用于Unicode字符输出，格式更好）
	wchar_t time_text_round[20];
	if (turn_remain_time >= 0) {
		wsprintf(time_text_round, _T("回合剩余：%02d秒"), turn_remain_time);	//wsprintf()的作用是将拼接字符串放入缓冲区
	}
	else {
		wsprintf(time_text_round, _T("超时"));
	}

	wchar_t black_time_total_text[20], white_time_total_text[20];
	int black_min = time_total_black / 60;
	int black_sec = time_total_black % 60;
	int white_min = time_total_white / 60;
	int white_sec = time_total_white % 60;

	wsprintf(black_time_total_text, _T("总时间剩余：%02d:%02d"), black_min, black_sec);
	wsprintf(white_time_total_text, _T("总时间剩余：%02d:%02d"), white_min, white_sec);

	//计时区边框
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 2);
	int black_timearea_left = black_piece_x - ELEMENT_GAP;
	int black_timearea_top = black_piece_y - ELEMENT_GAP;
	int black_timearea_right = black_timearea_left + PIECE_SIZE + 10 * single_char * PIECE_SIZE + 2 * ELEMENT_GAP;
	int black_timearea_bottom = black_timearea_top + PIECE_SIZE + 2 * ELEMENT_GAP;
	roundrect(black_timearea_left, black_timearea_top, black_timearea_right, black_timearea_bottom, 15, 15);

	int white_timearea_right = white_piece_x + PIECE_SIZE + ELEMENT_GAP;
	int white_timearea_top = white_piece_y - ELEMENT_GAP;
	int white_timearea_left = white_timearea_right - PIECE_SIZE - 10 * single_char * PIECE_SIZE - 2 * ELEMENT_GAP;
	int white_timearea_bottom = white_timearea_top + PIECE_SIZE + 2 * ELEMENT_GAP;
	roundrect(white_timearea_left, white_timearea_top, white_timearea_right, white_timearea_bottom, 15, 15);

	//总剩余时间绘制
	putimage(timer_black_total_x, timer_black_total_y, total_time_sizex, total_time_sizey, &img_info_background, total_time_sizex, total_time_sizey);
	putimage(timer_white_total_x, timer_white_total_y, total_time_sizex, total_time_sizey, &img_info_background, total_time_sizex, total_time_sizey);

	settextcolor(BLACK);
	outtextxy(timer_black_total_x, timer_black_total_y, black_time_total_text);
	outtextxy(timer_white_total_x, timer_white_total_y, white_time_total_text);

	//回合剩余时间绘制
	if (player == BLACK) {

		setlinecolor(RED);
		roundrect(black_timearea_left, black_timearea_top, black_timearea_right, black_timearea_bottom, 15, 15);

		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, time_text_round);	//绘制黑棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋倒计时文字
	}
	else if (player == WHITE) {

		setlinecolor(RED);
		roundrect(white_timearea_left, white_timearea_top, white_timearea_right, white_timearea_bottom, 15, 15);

		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, time_text_round);	//绘制白棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋倒计时文字
	}

	//4、添加倒计时5秒闪烁提示
	static int flash_count = 0;	//闪烁节拍器，使用static保证函数调用后数值不变
	if (turn_remain_time <= 5 && turn_remain_time > 0) {
		flash_count++;
		if (flash_count % FLASH_CYCLE < FLASH_CYCLE / 2) {
			if (player == BLACK)	putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
			else if (player == WHITE)	putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		}
		else {
			settextcolor(RED);
			if (player == BLACK) {
				putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
				outtextxy(timer_black_x, timer_black_y, time_text_round);	//绘制黑棋倒计时文字
			}
			else if (player == WHITE) {
				putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
				outtextxy(timer_white_x, timer_white_y, time_text_round);	//绘制白棋倒计时文字
			}
		}
	}
	else {
		flash_count = 0;
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点
}

int Judge_Win_Chess(int Board[LINE_NUM][LINE_NUM], int x, int y, int player) {

	//1、字体结构调整
	setbkmode(TRANSPARENT);	//文本背景透明
	settextcolor(BLACK);	//文字颜色黑色
	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//2、数据初始化
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };	//四个方向：右、上、右上、右下
	int cur_x = x, cur_y = y;	//实时位置
	if (player == EMPTY) return EMPTY;	//如果没有下棋提前退出

	//3、通过检测所下的棋周围的棋子数判断输赢
	for (int i = 0; i < 4; i++) {
		int count = 1;
		//3.1、同一个方向和其反向累加
		for (int j = 1; j < 5; j++) {
			cur_x = x + dir[i][0] * j;
			cur_y = y + dir[i][1] * j;
			if ((cur_x >= 0 && cur_x < LINE_NUM) && (cur_y >= 0 && cur_y < LINE_NUM) && Board[cur_x][cur_y] == player) {
				count++;
			}
			else break;
		}
		for (int j = 1; j < 5; j++) {
			cur_x = x - dir[i][0] * j;
			cur_y = y - dir[i][1] * j;
			if ((cur_x >= 0 && cur_x < LINE_NUM) && (cur_y >= 0 && cur_y < LINE_NUM) && Board[cur_x][cur_y] == player) {
				count++;
			}
			else break;
		}
		//3.2、一个大方向上存在5个连续则输出胜方
		if (count >= 5) {

			setorigin(0, 0);	//重设坐标原点
			putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4 * 0.75, PIECE_SIZE, &img_info_background, INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS);	//清理上次的文字（字体高宽比约0.75）

			if (player == BLACK) {
				printf_s("黑方胜利");
				outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
				FlushBatchDraw();	//刷新显示
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y); //恢复坐标中心
				return BLACK;
			}
			else if (player == WHITE) {
				printf_s("白方胜利");
				outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
				FlushBatchDraw();	//刷新显示
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
				return WHITE;
			}
		}
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
	return EMPTY;
}

int Judge_Win_Timer(int player) {

	//字体结构调整
	setbkmode(TRANSPARENT);	//文本背景透明
	settextcolor(BLACK);	//文字颜色黑色
	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	setorigin(0, 0);	//重设坐标原点

	if (!player_time_running) {

		putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4 * 0.75, PIECE_SIZE, &img_info_background, INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS);	//清理上次的文字（字体高宽比约0.75）

		if (player == BLACK && time_total_black <= 0) {
			printf_s("白方胜利");
			outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
			FlushBatchDraw();	//刷新显示
			setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
			return WHITE;
		}
		else if (player == WHITE && time_total_white <= 0) {
			printf_s("黑方胜利");
			outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
			FlushBatchDraw();	//刷新显示
			setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y); //恢复坐标中心
			return BLACK;
		}
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
	return EMPTY;
}

bool Switch_To_Setting(ExMessage msg) {

	if (Is_InCirecle(msg.x, msg.y, button_setting_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE / 2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN) {
		printf("跳转到设置面\n");
		return true;	//点击了设置按钮
	}
	else {
		return false;
	}

}

bool Switch_To_Menu(ExMessage msg) {

	if (Is_InCirecle(msg.x, msg.y, button_exit_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE / 2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN) {
		printf("跳转到主页面\n");
		return true;	//点击了退出按钮
	}
	else {
		return false;
	}
}

//5、游戏玩法函数
//悔棋函数
void Take_Back_Move() {
	return;
}

void Restart_Game(int Board[LINE_NUM][LINE_NUM], int* player) {

	//清空棋盘
	for (int i = 0; i < LINE_NUM; i++) {
		for (int j = 0; j < LINE_NUM; j++) {
			Board[i][j] = EMPTY;
		}
	}

	//重置玩家
	*player = BLACK;

	//重置计时器
	time_total_black = TIME_LIMIT_TOTAL;
	time_total_white = TIME_LIMIT_TOTAL;
	game_time_running = true;
	player_time_running = true;
	Turn_Timer_Start();	//重置计时

	//清除高亮
	Clear_Highlight(Board);

	//网络模式重启
	if (NetworkMode == NETWORK_MODE_SERVER) {
		isMyturn = true;	//服务端先手
	}
	else if (NetworkMode == NETWORK_MODE_CLIENT) {
		isMyturn = false;	//服务器后手
	}
	else {
		isMyturn = true;	//本地模式始终先手
	}

	//重绘棋盘和信息区
	setorigin(0, 0);
	putimage(0, 0, &img_game_background);
	Draw_Board();
	Draw_Info();


	FlushBatchDraw();
}

//6、网络功能函数
bool Parse_IPPort(const char* input, char* ip, int host_len, int& port) {

	// 查找冒号
	const char* colon = strchr(input, ':');
	if (colon == NULL) {
		// 没有冒号，整个字符串为主机名，使用默认端口
		strncpy_s(ip, host_len, input, _TRUNCATE);
		port = PORT;  // 默认端口
		return true;
	}

	// 分离主机部分
	int host_part_len = colon - input;
	if (host_part_len >= host_len) {
		return false;  // 主机名太长
	}
	strncpy_s(ip, host_len, input, host_part_len);
	ip[host_part_len] = '\0';

	// 解析端口部分
	const char* port_str = colon + 1;
	char* endptr;
	long p = strtol(port_str, &endptr, 10);
	if (*endptr != '\0' || p <= 0 || p > 65535) {
		return false;  // 端口无效
	}
	port = (int)p;
	return true;
}

bool Init_Winsock() {

	WSADATA wsaData;	//WSADATA用于存储Winsock初始化信息与版本数据
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);	//请求Winsock2.2版本，成功返回0
	if (result != 0) {
		printf_s("Winsock初始化失败：%d\n", result);
		return false;
	}
	else {
		//printf_s("Winsock初始化成功\n");
		return true;
	}
}

bool Create_Server() {

	//服务器 socket() -> bind() -> listen() -> accpet()

	//1、创建Socket
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//使用IPv4、流式数据与TCP连接
	if (ServerSocket == INVALID_SOCKET) {
		printf_s("Socket创建失败：%d\n", WSAGetLastError());
		return false;
	}
	else {
		printf_s("服务器TCP套接字创建成功！\n");
	}

	//2、配置地址
	sockaddr_in ServerAddr;						//存储服务器地址信息
	ServerAddr.sin_family = AF_INET;			//使用IPv4
	ServerAddr.sin_addr.s_addr = INADDR_ANY;	//监听所有网卡
	ServerAddr.sin_port = htons(PORT);			//监听端口

	//3、绑定本地地址（IP地址与端口于套接字），告诉其他主机该主机位置
	if (bind(ServerSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {	//SOCKET_ERROR值是-1，若成功返回应该是0
		printf_s("Socket绑定失败：%d\n", WSAGetLastError());
		closesocket(ServerSocket);	//释放Socket
		return false;
	}
	else {
		printf_s("服务器Socket绑定成功\n");
	}

	//4、开始监听
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		printf_s("监听失败：%d\n");
		closesocket(ServerSocket);	//释放Socket
		return false;
	}
	else {
		printf("服务器在端口%d监听\n", PORT);
	}

	printf("服务器已启动，等待客户端连接...\n");
	return true;
}

bool Connect_Server(const char* ip, int port) {

	//客户端 socket() -> connect()

	//1、创建套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//使用IPv4、流式数据与TCP连接
	if (ClientSocket == INVALID_SOCKET) {
		printf_s("Socket创建失败：%d\n", WSAGetLastError());
		return false;
	}
	else {
		printf_s("客户端TCP套接字创建成功！\n");
	}

	//2、配置地址
	sockaddr_in ServerAddr;						//存储服务器地址信息
	ServerAddr.sin_family = AF_INET;			//使用IPv4
	ServerAddr.sin_port = htons(port);			//监听端口

	//3、将域名转换为IP地址
	ServerAddr.sin_addr.s_addr = inet_addr(ip);	//连接特定IP
	if (ServerAddr.sin_addr.s_addr == INADDR_NONE) {
		struct hostent* remoteHost = gethostbyname(ip);
		if (remoteHost = NULL) {
			printf("域名解析失败:%d\n", WSAGetLastError());
			closesocket(ClientSocket);
			return false;
		}

		ServerAddr.sin_addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
	}

	//4、创建连接
	if (connect(ClientSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {

		printf("客户端连接失败：%d\n", WSAGetLastError());
		closesocket(ClientSocket);	//释放Socket
		return false;
	}

	printf("客户端成功连接%s:%d，等待服务器接受...\n", ip, PORT);
	return true;
}

bool Accept_Connection() {

	sockaddr_in ClientAddr;		//存储客户端地址信息
	int ClientAddr_Size = sizeof(ClientAddr);	//accpet()只能接受长度的地址

	ClientSocket = accept(ServerSocket, (sockaddr*)&ClientAddr, &ClientAddr_Size);
	if (ClientSocket == INVALID_SOCKET) {
		printf("接受连接失败：%d\n", WSAGetLastError());
		return false;
	}

	printf("连接已建立\n");
	return true;
}

bool Send_Network_Message(SOCKET socket, const NetworkMessage& msg) {

	int ByteSend = send(socket, (const char*)&msg, sizeof(msg), 0);
	return ByteSend > 0;
}

bool Receive_Network_Message(SOCKET socket, NetworkMessage& msg) {

	int ByteRecv = recv(socket, (char*)&msg, sizeof(msg), 0);
	return ByteRecv > 0;
}

void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player, bool& isMyturn, bool& game_over) {

	if (!isConnected || game_over)	return;	//没有连接成功提前退出

	setorigin(BOARD_SIZE / 2, BOARD_SIZE / 2);	//设置初始坐标

	//设置成非阻塞状态，防止因等待网络通信而导致计时器等元素卡顿
	u_long mode = 1;	//1为非阻塞
	ioctlsocket(ClientSocket, FIONBIO, &mode);

	NetworkMessage msg;
	if (Receive_Network_Message(ClientSocket, msg)) {	//使用ServerSocket监听，使用ClientSocket双向通信
		switch (msg.msgtype) {
		case MSG_MOVE:
		{
			if (Board[msg.x][msg.y] == EMPTY) {

				Clear_Highlight(Board);

				int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * msg.x - OFFSET;	//计算图像棋盘对应位置
				int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * msg.y - OFFSET;

				if (msg.player == BLACK) {
					Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);	//绘制黑棋
					Board[msg.x][msg.y] = BLACK;

					if (Judge_Win_Chess(Board, msg.x, msg.y, player) == BLACK) {
						choice = _getch();	//判断输赢
					}
					player = WHITE;	//棋方转换
				}
				else if (msg.player == WHITE) {
					Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
					Board[msg.x][msg.y] = WHITE;

					if (Judge_Win_Chess(Board, msg.x, msg.y, player) == WHITE) {
						choice = _getch();	//判断输赢
					}
					player = BLACK;	//棋方转换
				}

				isMyturn = true;	//接受完信息后，轮到我端下棋
				Turn_Timer_Start();	//初始化计时器
				FlushBatchDraw();	//刷新显示
			}
			break;
		}
		case MSG_QUIT:
		{
			printf_s("对方断开连接\n");
			isConnected = false;
			break;
		}
		case MSG_TIMEOUT:
		{
			if (msg.player == player) {

				//执行与本地超时相同的换边逻辑
				player = BLACK + WHITE - player;
				isMyturn = !isMyturn;               //翻转回合标志
				Turn_Timer_Start();                  //重置本地计时器
				Clear_Highlight(Board);
				FlushBatchDraw();
			}
			break;
		}
		case MSG_TIMEOUT_TOTAL:
		{
			// 清除原有文字区域，显示胜利信息
			setorigin(0, 0);

			//字体结构调整
			setbkmode(TRANSPARENT);	//文本背景透明
			settextcolor(BLACK);	//文字颜色黑色
			LOGFONT fontStyle;	//创建字体结构体
			gettextstyle(&fontStyle);	//获取当前字体设置
			fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
			fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
			fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
			_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
			settextstyle(&fontStyle);	//应用新字体设置

			putimage(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4 * 0.75, PIECE_SIZE, &img_info_background, INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS);
			if (msg.player == BLACK) {
				outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
			}
			else if (msg.player == WHITE) {
				outtextxy(BOARD_SIZE + INFO_SIZE / 2 - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
			}
			FlushBatchDraw();
			_getch();          // 等待用户按键
			game_over = true;  // 结束游戏
		}
		break;
		}
		}

	//恢复为阻塞状态
	mode = 0;	//0为阻塞
	ioctlsocket(ClientSocket, FIONBIO, &mode);

	setorigin(0, 0);	//恢复初始坐标
}



//=====================/主函数/=====================
int main() {

	if (!Init_Winsock()) return -1;								//初始化Winsock

	Texture_Load();												//加载素材
	initgraph(BOARD_SIZE + INFO_SIZE, BOARD_SIZE, EX_DBLCLKS);	//创建画布，支持双击参数传入
	BeginBatchDraw();											//启动批量绘制 (采用双缓冲技术减少闪烁)

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);		//去除DPI放缩
	HWND hConsole = GetConsoleWindow();							//展示控制台用于调试
	ShowWindow(hConsole, SW_SHOW);

	while (currentState != EXIT) {
		switch (currentState) {
		case MENU:
		{
			//MENU绘制与功能函数
			flushmessage();          // 清空所有待处理消息

			Draw_Main();	//绘制主界面
			FlushBatchDraw();	//刷新显示

			//选择游戏模式
			ExMessage msg;
			while (peekmessage(&msg, EX_MOUSE, false)) {
				getmessage(&msg, EX_MOUSE);

				game_mode = Main_Switch(msg);
				if (game_mode == -1) break;
				if (game_mode == NETWORK_MODE_LOCAL) {

					NetworkMode = NETWORK_MODE_LOCAL;
					currentState = PLAYING;

					printf_s("本地对战模式\n");
					isMyturn = true;										//本地对战始终本端下棋
					break;
				}
				else if (game_mode == NETWORK_MODE_MULTIPLAYER) {
					currentState = MULTIPLAYER;								//选择联网模式
					break;
				}
				else if (game_mode == MAIN_INSTRUCTION) {
					currentState = INSTRUCTION;
					break;
				}
				else if (game_mode == MAIN_SETTING) {
					currentState = SETTING;
					break;
				}
				else if (game_mode == MAIN_EXIT) {
					currentState = EXIT;
					break;
				}
			}

			break;
		}

		case INSTRUCTION:
		{
			//INSTRUCTION绘制与功能函数
			flushmessage();          // 清空所有待处理消息

			setbkcolor(RGB(255, 255, 255));
			cleardevice();	//清空画面

			FlushBatchDraw();	//刷新显示

			break;
		}
		case MULTIPLAYER:
		{
			//MULTIPLAYER绘制与功能函数

			flushmessage();          // 清空所有待处理消息
			//绘制创建服务器和加入服务器选择页面
			Draw_Multi();
			FlushBatchDraw();	//刷新显示

			//逻辑显示
			ExMessage msg;
			while (peekmessage(&msg, EX_MOUSE, false)) {

				getmessage(&msg, EX_MOUSE);
				int multi_mode = Multi_Switch(msg);

				if (multi_mode == NETWORK_MODE_SERVER) {
					currentState = MULTIPLAYER_SERVER;
					break;
				}
				else if (multi_mode == NETWORK_MODE_CLIENT) {
					currentState = MULTIPLAYER_CLIENT;
					break;
				}
			}
			break;
		}

		case MULTIPLAYER_SERVER:
		{
			//MULTIPLAYER_SERVER绘制与功能函数
			flushmessage();          // 清空所有待处理消息

			NetworkMode = NETWORK_MODE_SERVER;
			printf_s("创建服务器\n");
			if (Create_Server()) {
				printf_s("服务器创建成功，等待客户端连接\n");

				//绘制成功创建服务器提示画面
				cleardevice();

				settextcolor(RGB(0, 0, 0));
				outtextxy((BOARD_SIZE + INFO_SIZE - 15 * textheight(_T("字"))) / 2, (BOARD_SIZE - textheight(_T("字"))) / 2, _T("服务器创建成功，等待连接..."));

				FlushBatchDraw();	//刷新显示


				if (Accept_Connection()) {
					isMyturn = true;				//我方是服务器，先下棋
					isConnected = true;				//成功连接
					printf_s("客户端已连接\n");

					cleardevice();
					//绘制连接成功提示画面
					FlushBatchDraw();	//刷新显示

					Sleep(1500);
					currentState = PLAYING;			//连接成功开始游戏
				}

			}
			else {
				printf_s("服务器创建失败\n");

				cleardevice();
				//绘制创建失败提示
				FlushBatchDraw();
				Sleep(1500);
				currentState = MULTIPLAYER;			//创建失败跳回多人页面
			}

			break;
		}

		case MULTIPLAYER_CLIENT:
		{
			//MULTIPLAYER_CLIENT绘制与功能函数
			flushmessage();          // 清空所有待处理消息
			//绘制输入服务器IP
			Input_Box();			//等待输入

			NetworkMode = NETWORK_MODE_CLIENT;
			printf_s("输入的服务器IP地址：%ls\n", inputStr);
			printf_s("正在连接中\n");

			char ipBuffer[FONT_NUM] = { 0 };
			WideCharToMultiByte(CP_ACP, 0, inputStr, -1, ipBuffer, sizeof(ipBuffer), NULL, NULL);	//使用 WideCharToMultiByte 确保正确转换

			char ip[256];
			int port;
			if (!Parse_IPPort(ipBuffer, ip, sizeof(ip), port)) {
				printf_s("输入的地址格式错误\n");
				// 显示错误信息（可以绘制提示）
				cleardevice();
				outtextxy(100, 100, _T("地址格式错误"));
				FlushBatchDraw();
				currentState = MULTIPLAYER;
				break;
			}

			if (Connect_Server(ip, port)) {
				isMyturn = false;				//我方是客户端，后下棋
				isConnected = true;				//成功连接
				printf_s("连接到服务器\n");

				cleardevice();
				//绘制成功连接页面
				FlushBatchDraw();

				Sleep(1500);
				currentState = PLAYING;
			}
			else {
				printf_s("服务器连接失败\n");

				cleardevice();
				//绘制连接失败提示
				outtextxy(100, 100, _T("服务器连接失败"));
				FlushBatchDraw();
				Sleep(1500);
				currentState = MULTIPLAYER;
			}

			break;
		}

		case PLAYING:
		{
			//PLAYING绘制与功能函数

			//=============================/一、初始化/=============================

			int board[LINE_NUM][LINE_NUM] = { {EMPTY} };		//棋盘二维数组表示，并初始化
			int player = BLACK;									//标记下棋手
			bool game_over = false;								//游戏结束标志
			bool timeout_total_sent = false;					//防止重复发送时间耗尽消息

			game_remain_time = time(NULL);						//初始化总游戏时间开始
			Turn_Timer_Start();									//初始化计时器

			flushmessage();          // 清空所有待处理消息
			cleardevice();										//清空画面
			Draw_Board();										//绘制棋盘
			Draw_Info();										//绘制信息区

			//=============================/二、游戏主体/=============================

			while (!game_over) {

				//绘制指向与剩余时间
				Player_Point(BLACK + WHITE - player);																//显示玩家指向

				Turn_Timer_Update(player);																			//计算剩余时间
				Turn_Draw_Timer(player);																			//绘制倒计时

				FlushBatchDraw();																					//刷新显示

				//处理接受的网络信号
				if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
					Network_Mode_Event(board, player, isMyturn, game_over);
				}
				if (NetworkMode != NETWORK_MODE_LOCAL && !isMyturn) {												//网络模式检查是否是我方下棋（不是则循环跳过）
					continue;
				}

				//处理本地的鼠标信号
				ExMessage msg;
				if (peekmessage(&msg, EX_MOUSE, false)) {

					getmessage(&msg, EX_MOUSE);																		//获取鼠标信息
					Game_Music_Control(msg);																		//音乐开关按键

					if (Switch_To_Setting(msg)) {																	//跳转设置页面
						currentState = SETTING;
						break;
					}
					if (Switch_To_Menu(msg)) {
						currentState = MENU;																		//跳转至主页面
						Restart_Game(board, &player);
						break;
					}

					if (Is_InCirecle(msg.x, msg.y, button_restart_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE / 2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN) {
						printf("重启游戏\n");
						Restart_Game(board, &player);																//重启游戏
						continue;
					}

					//计算图像坐标与数组坐标
					int boardX = (msg.x - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;							//计算二维数组棋盘对应位置
					int boardY = (msg.y - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;							//添加20用于四舍五入提高用户体验
					int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * boardX - OFFSET;									//计算图像棋盘对应位置
					int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * boardY - OFFSET;
					//printf("对应棋盘X=%d，Y=%d\n", boardX, boardY);												//棋盘对应位置测试代码

					//处理高光与下棋
					if ((boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {

						//鼠标移动时绘制高亮提示下棋位置
						if (msg.message == WM_MOUSEMOVE && board[boardX][boardY] == EMPTY) {
							Clear_Highlight(board);																	//清理原有高光
							Draw_Highlight(boardX, boardY, board);													//绘制高光
						}
						//执行下棋机制与绘制棋子操作（为防止误触双击下棋）
						if (msg.message == WM_LBUTTONDBLCLK && board[boardX][boardY] == EMPTY) {

							Clear_Highlight(board);																	//清理高光

							if (player == BLACK) {
								//处理本地下棋
								Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);				//图像绘制黑棋
								board[boardX][boardY] = BLACK;														//数组绘制黑棋

								//发送下棋数据包
								if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
									//设定回合信息
									NetworkMessage turnMsg;
									turnMsg.msgtype = MSG_MOVE;														//消息类型为下棋
									turnMsg.x = boardX;
									turnMsg.y = boardY;
									turnMsg.player = BLACK;

									//发送数据包
									if (Send_Network_Message(ClientSocket, turnMsg)) {
										printf("发送黑棋落子：(%d , %d)\n", turnMsg.x, turnMsg.y);
									}
									isMyturn = false;																//发送完信息后，我端不下棋
								}
							}
							else if (player == WHITE) {
								//处理本地下棋
								Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);				//图像绘制白棋
								board[boardX][boardY] = WHITE;														//数组绘制白棋

								//发送网络数据包
								if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
									//设定回合信息
									NetworkMessage turnMsg;
									turnMsg.msgtype = MSG_MOVE;														//消息类型为下棋
									turnMsg.x = boardX;
									turnMsg.y = boardY;
									turnMsg.player = WHITE;

									//发送数据包
									if (Send_Network_Message(ClientSocket, turnMsg)) {
										printf("发送白棋落子：(%d , %d)\n", turnMsg.x, turnMsg.y);
									}
									isMyturn = false;																//发送完信息后，我端不下棋
								}
							}

							if (Judge_Win_Chess(board, boardX, boardY, player) != EMPTY) {
								choice = _getch();																	//判断输赢
							}

							player = BLACK + WHITE - player;														//棋方转换
							Turn_Timer_Start();																		//初始化计时器

							//FlushBatchDraw();																		//刷新显示
						}

					}
					else {
						Clear_Highlight(board);																		//若不符合条件也进行清理操作
					}
					FlushBatchDraw();																				//刷新显示

					//立即更新计时器
					Turn_Timer_Update(player);																		//计算剩余时间
					Turn_Draw_Timer(player);																		//绘制倒计时
					FlushBatchDraw();																				//刷新显示
				}

				//对时间进行处理
				int total_timer_result = Judge_Win_Timer(player);
				if (total_timer_result != EMPTY) {																	//有玩家总时间耗尽游戏结束
					// 联网模式下发送总时间耗尽消息（仅发送一次）
					if (NetworkMode != NETWORK_MODE_LOCAL && isConnected && !timeout_total_sent) {
						NetworkMessage msg;
						msg.msgtype = MSG_TIMEOUT_TOTAL;
						msg.player = total_timer_result;															//胜方
						Send_Network_Message(ClientSocket, msg);
						timeout_total_sent = true;
					}
					FlushBatchDraw();
					_getch();																						//等待用户按键
					game_over = true;																				//结束游戏
					break;																	
				}
				else if (!game_time_running) {																		//倒计时结束强制换边
					//超时发生：先发送超时消息（联网模式下），再执行本地换边
					if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
						NetworkMessage timeoutMsg;
						timeoutMsg.msgtype = MSG_TIMEOUT;
						timeoutMsg.player = player;																	// 记录超时时的玩家
						Send_Network_Message(ClientSocket, timeoutMsg);
					}

					//统一换边逻辑
					auto handleTimeout = [&]() {
						player = BLACK + WHITE - player;
						if (NetworkMode != NETWORK_MODE_LOCAL) {
							isMyturn = !isMyturn;																	//翻转回合标志
						}
						Turn_Timer_Start();																			//重置本地计时器
						Clear_Highlight(board);																		//清除高光
						FlushBatchDraw();
					};

					handleTimeout();																				//执行换边																	
				}
				FlushBatchDraw();																					//刷新计时器显示
				//Sleep(10);																						//短暂休眠减少CPU占用

			}

			currentState = MENU;
			break;
		}

		case SETTING:
		{
			//SETTING绘制与功能函数

			flushmessage();          // 清空所有待处理消息

			cleardevice();	//清空画面

			FlushBatchDraw();	//刷新显示

			break;
		}

		}
	}

	//清理游戏资源
	EndBatchDraw();	//结束批量绘制
	closegraph();	//关闭画布

	//清理网络资源
	if (ClientSocket != INVALID_SOCKET) {
		NetworkMessage QuitMSG;
		QuitMSG.msgtype = MSG_QUIT;
		Send_Network_Message(ClientSocket, QuitMSG);
		closesocket(ClientSocket);
	}
	if (ServerSocket != INVALID_SOCKET) {
		closesocket(ServerSocket);
	}
	WSACleanup();

	return 0;
}
