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
#pragma comment(lib, "Msimg32.lib") //Windows图像处理库
#pragma comment(lib, "user32.lib")  //用户界面库
#pragma comment(lib, "gdi32.lib")   //GDI图形库
#pragma comment(lib, "Shcore.lib")
#pragma comment(linker, "/subsystem:windows")	//隐藏控制台

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
#define BUTTON_SIZE 45				//游戏内按钮大小（恢复原大小）
#define BUTTON_GAP	18				//游戏内按钮间隔（恢复原大小）
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
#define MAIN_BACK 9					//返回上一级页面

#define GAME_BUTTON_RESTART 10		//游戏内重启按钮
#define GAME_BUTTON_TAKEBACK 11		//游戏内悔棋按钮
#define GAME_BUTTON_MUSIC 12		//游戏内音乐按钮
#define GAME_BUTTON_SETTING 13		//游戏内设置按钮
#define GAME_BUTTON_EXIT 14			//游戏内退出按钮

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
IMAGE img_headline, img_headline_opp, img_menu_background;				//游戏标题图片
IMAGE img_universal_background, img_game_background, img_info_background;	//背景图片
IMAGE img_white, img_white_opp, img_black, img_black_opp;				//棋子图片

//游戏坐标公共变量

//字体公共变量
wchar_t inputStr[FONT_NUM] = L"";	//需要输入的初始字符

//主界面按钮（恢复原大小）
int button_width = 360;
int button_height = 52;

int headline_x = (BOARD_SIZE + INFO_SIZE - HEADLINE_WIDTH) / 2;
int button_mainall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;

int headline_y = 35;
int button_main_gap = 16;           // 恢复原间距
int button_singleplayer_y = headline_y + HEADLINE_HEIGHT + 3 * ELEMENT_GAP;
int button_multiplayer_y = button_singleplayer_y + button_height + button_main_gap;
int button_instruction_y = button_multiplayer_y + button_height + button_main_gap;
int button_mainsetting_y = button_instruction_y + button_height + button_main_gap;
int button_mainexit_y = button_mainsetting_y + button_height + button_main_gap;

//主界面按钮交互状态
#define BUTTON_STATE_NORMAL 0		//普通状态
#define BUTTON_STATE_HOVER 1		//鼠标悬停状态
#define BUTTON_STATE_PRESSED 2		//鼠标按压状态
int main_hover_button = -1;			//当前悬停的主界面按钮
int main_pressed_button = -1;		//当前按压的主界面按钮

//多人界面按钮
int button_multiall_x = (BOARD_SIZE + INFO_SIZE - button_width) / 2;

int button_createserver_y = (BOARD_SIZE - 8 * ELEMENT_GAP - 2 * button_height) / 2;
int button_joinserver_y = button_createserver_y + button_height + 8 * ELEMENT_GAP;
int multi_hover_button = -1;			//当前悬停的多人界面按钮
int multi_pressed_button = -1;		//当前按压的多人界面按钮

//返回按钮
int button_back_x = ELEMENT_GAP * 2;
int button_back_y = ELEMENT_GAP * 2;
int button_back_width = 120;
int button_back_height = 45;
int back_hover_button = -1;
int back_pressed_button = -1;

//游戏界面按钮（左右分组，但按钮大小恢复为45）
int button_game_left_margin = 20;                // 左侧组距信息区左边缘
int button_game_right_margin = 20;               // 右侧组距信息区右边缘

// 左侧组（重启、悔棋）
int button_restart_x = BOARD_SIZE + button_game_left_margin + BUTTON_SIZE / 2;
int button_takeback_x = button_restart_x + BUTTON_SIZE + BUTTON_GAP;

// 右侧组（音乐、设置、退出），从右向左排列
int button_exit_x = BOARD_SIZE + INFO_SIZE - button_game_right_margin - BUTTON_SIZE / 2;
int button_setting_x = button_exit_x - BUTTON_SIZE - BUTTON_GAP;
int button_music_x = button_setting_x - BUTTON_SIZE - BUTTON_GAP;

int button_playingall_y = BOARD_SIZE - BUTTON_SIZE / 2 - 25;   // 所有按钮Y坐标（中心）
int game_hover_button = -1;																											//当前悬停的游戏功能按钮
int game_pressed_button = -1;																										//当前按压的游戏功能按钮


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
void Clear_Info_Background(int x, int y, int width, int height);				//按窗口坐标清理信息区背景
bool Is_InCirecle(int px, int py, int cx, int cy, int radius);					//是否在圆内判断函数
bool Is_InRect(int px, int py, int left, int top, int right, int bottom);		//是否在方框内判断函数

bool Input_Box();

//4、游戏界面与功能函数声明
void Draw_Board();	//五子棋盘绘制函数
void Draw_Info();	//信息区域绘制函数
void Draw_Game_Button(int button_index, int button_state);						//游戏功能按钮绘制函数
void Draw_All_Game_Buttons(int button_state);									//绘制全部游戏功能按钮
int Game_Hit_Button(int x, int y);												//游戏功能按钮命中判断函数
int Game_Button_Switch(ExMessage msg);											//游戏功能按钮交互函数
void Draw_Text_Page(const TCHAR* title, const TCHAR* content);					//通用文字页面绘制函数
void Draw_Instruction();														//游戏说明界面绘制函数
void Draw_Setting();															//游戏设置界面绘制函数
void Draw_Message_Page(const TCHAR* message);									//提示页面绘制函数
void Draw_Back_Button(int button_state, IMAGE* background_img);					//返回按钮绘制函数
int Back_Switch(ExMessage msg, IMAGE* background_img);							//返回按钮交互函数

void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]);				//绘制高光函数
void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]);							//清理高光函数（局部重绘提高效率）
void Player_Point(int player);														//玩家指向函数

void Turn_Timer_Start();														//回合计时初始化函数
void Turn_Timer_Update(int player);												//回合剩余时间计算函数
void Turn_Draw_Timer(int player);												//绘制倒计时函数

int Judge_Win_Chess(int Board[LINE_NUM][LINE_NUM], int x, int y, int player);	//判断游戏结束函数（这里的X，Y代表的是现在下的棋的位置，因为其实只有现在下的棋改变了棋盘的状态，提高搜索效率）
int Judge_Win_Timer(int player);													//时间判断游戏结束函数

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
bool Send_All(SOCKET s, const char* data, int len);															//使用可靠的TCP发送
bool Receive_Network_Message(SOCKET socket, NetworkMessage& msg);											//接收数据（服务端）
void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player, bool& isMyturn, bool& game_over);		//网络模式对战处理

//7、程序健壮性函数声明
bool Is_Valid_Move_Message(const NetworkMessage& msg);														//防止对方发送错误下棋信号

//=====================/函数具体内容/=====================

//1、基础功能函数
void Texture_Load() {

	//MENU界面素材
	loadimage(&img_headline, _T("./素材/headline.png"), HEADLINE_WIDTH, HEADLINE_HEIGHT);						//标题彩色图
	loadimage(&img_headline_opp, _T("./素材/headline1.png"), HEADLINE_WIDTH, HEADLINE_HEIGHT);					//标题掩码图
	loadimage(&img_menu_background, _T("./素材/menu_background1.png"), BOARD_SIZE + INFO_SIZE, BOARD_SIZE);		//封面背景图

	//PLAYING界面素材
	loadimage(&img_universal_background, _T("./素材/menu_background1.png"), BOARD_SIZE + INFO_SIZE, BOARD_SIZE);	//游戏开始背景
	loadimage(&img_game_background, _T("./素材/wood1.jpg"), BOARD_SIZE, BOARD_SIZE);							//游戏界面背景
	loadimage(&img_info_background, _T("./素材/info_background.jpg"), INFO_SIZE, BOARD_SIZE);					//信息框背景

	loadimage(&img_white, _T("./素材/white.png"), PIECE_SIZE, PIECE_SIZE);										//白棋彩色图
	loadimage(&img_white_opp, _T("./素材/white1.png"), PIECE_SIZE, PIECE_SIZE);									//白棋掩码图
	loadimage(&img_black, _T("./素材/black.png"), PIECE_SIZE, PIECE_SIZE);										//黑棋彩色图
	loadimage(&img_black_opp, _T("./素材/black1.png"), PIECE_SIZE, PIECE_SIZE);									//黑棋掩码图
}

void Game_Music_Control(ExMessage msg) {

	if (msg.message != WM_LBUTTONUP || Game_Hit_Button(msg.x, msg.y) != GAME_BUTTON_MUSIC) {
		return;
	}

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

	Draw_Game_Button(GAME_BUTTON_MUSIC, BUTTON_STATE_HOVER);
}

void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img) {

	putimage(x, y, mask, SRCAND);	//掩码图，黑色部分代表透明区域
	putimage(x, y, img, SRCPAINT);	//彩色图，保留掩码图黑色部分
}

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
	}
	else if (button_state == BUTTON_STATE_PRESSED) {
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

void Draw_Button_Text(int left, int top, int width, int height, const TCHAR* text, int button_state) {

	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FONT_WEIGHT;
	// 根据按钮高度调整字体大小
	if (height >= 64)
		fontStyle.lfHeight = 30;
	else if (height >= 50)
		fontStyle.lfHeight = 24;
	else
		fontStyle.lfHeight = 20;
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);

	int text_offset = 0;
	if (button_state == BUTTON_STATE_PRESSED) {
		text_offset = 2;
	}

	settextcolor(RGB(65, 38, 18));
	setbkmode(TRANSPARENT);
	outtextxy(
		left + (width - textwidth(text)) / 2 + text_offset,
		top + (height - textheight(text)) / 2 + text_offset,
		text
	);
}


void Draw_Back_Button(int button_state, IMAGE* background_img) {

	//局部刷新返回按钮背景，源坐标与窗口坐标保持一致
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

	Draw_Button_Frame(button_back_x, button_back_y, button_back_x + button_back_width, button_back_y + button_back_height, 18, button_state);
	Draw_Button_Text(button_back_x, button_back_y, button_back_width, button_back_height, _T("返回"), button_state);
}

int Back_Switch(ExMessage msg, IMAGE* background_img) {

	int current_button = -1;
	if (Is_InRect(msg.x, msg.y, button_back_x, button_back_y, button_back_x + button_back_width, button_back_y + button_back_height)) {
		current_button = MAIN_BACK;
	}

	if (msg.message == WM_MOUSEMOVE) {
		if (back_pressed_button != -1) {
			if (back_hover_button != current_button) {
				back_hover_button = current_button;
				Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_PRESSED : BUTTON_STATE_HOVER, background_img);
				FlushBatchDraw();
			}
		}
		else if (back_hover_button != current_button) {
			back_hover_button = current_button;
			Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL, background_img);
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONDOWN) {
		back_pressed_button = current_button;
		if (back_pressed_button == MAIN_BACK) {
			Draw_Back_Button(BUTTON_STATE_PRESSED, background_img);
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONUP) {
		int pressed_button = back_pressed_button;
		back_pressed_button = -1;

		if (pressed_button == MAIN_BACK) {
			Draw_Back_Button(current_button == MAIN_BACK ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL, background_img);
			FlushBatchDraw();
		}

		if (pressed_button == MAIN_BACK && current_button == MAIN_BACK) {
			printf("返回上一级页面\n");
			return MAIN_BACK;
		}
	}

	return -1;
}

//2、主界面与功能函数
void Draw_Menu_Button(int left, int top, const TCHAR* text, int button_state, IMAGE* background_img) {

	//局部刷新按钮所在背景，源坐标与窗口坐标保持一致，避免重绘时背景错位
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

void Draw_Main_Button(int button_index, int button_state) {

	switch (button_index) {
	case NETWORK_MODE_LOCAL:
		Draw_Menu_Button(button_mainall_x, button_singleplayer_y, _T("本地模式"), button_state, &img_menu_background);
		break;
	case NETWORK_MODE_MULTIPLAYER:
		Draw_Menu_Button(button_mainall_x, button_multiplayer_y, _T("多人模式"), button_state, &img_menu_background);
		break;
	case MAIN_INSTRUCTION:
		Draw_Menu_Button(button_mainall_x, button_instruction_y, _T("玩法介绍"), button_state, &img_menu_background);
		break;
	case MAIN_SETTING:
		Draw_Menu_Button(button_mainall_x, button_mainsetting_y, _T("游戏设置"), button_state, &img_menu_background);
		break;
	case MAIN_EXIT:
		Draw_Menu_Button(button_mainall_x, button_mainexit_y, _T("退出游戏"), button_state, &img_menu_background);
		break;
	}
}

int Main_Hit_Button(int x, int y) {

	if (Is_InRect(x, y, button_mainall_x, button_singleplayer_y, button_mainall_x + button_width, button_singleplayer_y + button_height)) {
		return NETWORK_MODE_LOCAL;
	}
	else if (Is_InRect(x, y, button_mainall_x, button_multiplayer_y, button_mainall_x + button_width, button_multiplayer_y + button_height)) {
		return NETWORK_MODE_MULTIPLAYER;
	}
	else if (Is_InRect(x, y, button_mainall_x, button_instruction_y, button_mainall_x + button_width, button_instruction_y + button_height)) {
		return MAIN_INSTRUCTION;
	}
	else if (Is_InRect(x, y, button_mainall_x, button_mainsetting_y, button_mainall_x + button_width, button_mainsetting_y + button_height)) {
		return MAIN_SETTING;
	}
	else if (Is_InRect(x, y, button_mainall_x, button_mainexit_y, button_mainall_x + button_width, button_mainexit_y + button_height)) {
		return MAIN_EXIT;
	}
	return -1;
}

void Draw_Main() {

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = 0;
	fontStyle.lfHeight = PIECE_SIZE / 2;
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);

	setbkcolor(RGB(255, 255, 255));
	cleardevice();

	setorigin(0, 0);
	putimage(0, 0, &img_menu_background);
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
		}
		else if (main_hover_button != current_button) {
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
	}
	else if (msg.message == WM_LBUTTONDOWN) {
		main_pressed_button = current_button;
		if (main_pressed_button != -1) {
			Draw_Main_Button(main_pressed_button, BUTTON_STATE_PRESSED);
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONUP) {
		int pressed_button = main_pressed_button;
		main_pressed_button = -1;

		if (pressed_button != -1) {
			Draw_Main_Button(pressed_button, current_button == pressed_button ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL);
			FlushBatchDraw();
		}

		if (pressed_button == current_button && current_button != -1) {
			if (current_button == NETWORK_MODE_LOCAL) printf("本地模式\n");
			else if (current_button == NETWORK_MODE_MULTIPLAYER) printf("多人模式\n");
			else if (current_button == MAIN_INSTRUCTION) printf("游戏说明\n");
			else if (current_button == MAIN_SETTING) printf("游戏设置\n");
			else if (current_button == MAIN_EXIT) printf("退出游戏\n");
			return current_button;
		}
	}

	return -1;
}


void Draw_Text_Page(const TCHAR* title, const TCHAR* content) {

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FONT_WEIGHT;
	fontStyle.lfHeight = PIECE_SIZE;
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);

	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

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

	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

	// ---------- 卡片参数 ----------
	int card_width = 600;
	int card_height = 420;   // 略微增高以容纳更大行距
	int card_left = (BOARD_SIZE + INFO_SIZE - card_width) / 2;
	int card_top = (BOARD_SIZE - card_height) / 2;
	int card_right = card_left + card_width;
	int card_bottom = card_top + card_height;

	// ---------- 绘制半透明白色卡片 ----------
	setlinestyle(PS_SOLID, 2);
	setlinecolor(RGB(200, 200, 200));
	setfillcolor(RGB(255, 255, 255));
	fillroundrect(card_left, card_top, card_right, card_bottom, 20, 20);
	roundrect(card_left, card_top, card_right, card_bottom, 20, 20);

	// ---------- 标题 ----------
	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FONT_WEIGHT;             // 加粗
	fontStyle.lfHeight = PIECE_SIZE * 0.85;       // 约34px
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);
	settextcolor(RGB(30, 30, 30));

	int title_x = card_left + (card_width - textwidth(_T("🎯 玩法介绍"))) / 2;
	int title_y = card_top + 30;
	outtextxy(title_x, title_y, _T("🎯 玩法介绍"));

	// 标题下划线
	setlinecolor(RGB(180, 180, 180));
	setlinestyle(PS_SOLID, 1);
	line(card_left + 40, title_y + 45, card_right - 40, title_y + 45);

	// ---------- 内容（列表） ----------
	fontStyle.lfWeight = FONT_WEIGHT;             // 正文加粗
	fontStyle.lfHeight = PIECE_SIZE * 0.55;       // 约22px
	settextstyle(&fontStyle);
	settextcolor(RGB(50, 50, 50));

	const TCHAR* lines[] = {
		_T("● 双击棋盘交叉点落子"),
		_T("● 黑棋先行，白棋后手"),
		_T("● 率先在横、竖、斜方向连成五子的一方获胜"),
		_T("● 每方总时间 10 分钟，单步限时 30 秒"),
		_T("● 点击右下角按钮可退出或重启游戏")
	};
	int line_count = sizeof(lines) / sizeof(lines[0]);
	int line_spacing = 40;   // 增大行距
	int start_y = title_y + 70;

	for (int i = 0; i < line_count; i++) {
		int line_x = card_left + 50;
		int line_y = start_y + i * line_spacing;
		outtextxy(line_x, line_y, lines[i]);
	}

	// ---------- 返回按钮 ----------
	back_hover_button = -1;
	back_pressed_button = -1;
	Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
}

void Draw_Setting() {

	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

	// ---------- 卡片参数 ----------
	int card_width = 580;   // 稍微加宽适应更大字号
	int card_height = 380;
	int card_left = (BOARD_SIZE + INFO_SIZE - card_width) / 2;
	int card_top = (BOARD_SIZE - card_height) / 2;
	int card_right = card_left + card_width;
	int card_bottom = card_top + card_height;

	// ---------- 半透明白色卡片 ----------
	setlinestyle(PS_SOLID, 2);
	setlinecolor(RGB(200, 200, 200));
	setfillcolor(RGB(255, 255, 255, 220));
	fillroundrect(card_left, card_top, card_right, card_bottom, 20, 20);
	roundrect(card_left, card_top, card_right, card_bottom, 20, 20);

	// ---------- 标题 ----------
	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FONT_WEIGHT;
	fontStyle.lfHeight = PIECE_SIZE * 0.85;       // 约34px
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);
	settextcolor(RGB(30, 30, 30));

	int title_x = card_left + (card_width - textwidth(_T("⚙ 游戏设置"))) / 2;
	int title_y = card_top + 30;
	outtextxy(title_x, title_y, _T("⚙ 游戏设置"));

	// 标题下划线
	setlinecolor(RGB(180, 180, 180));
	setlinestyle(PS_SOLID, 1);
	line(card_left + 40, title_y + 45, card_right - 40, title_y + 45);

	// ---------- 设置项 ----------
	fontStyle.lfWeight = FONT_WEIGHT;             // 加粗
	fontStyle.lfHeight = PIECE_SIZE * 0.55;       // 约22px
	settextstyle(&fontStyle);
	settextcolor(RGB(50, 50, 50));

	int start_y = title_y + 75;
	int line_spacing = 55;   // 增大行距

	// 第一项：音乐开关
	TCHAR music_status[50];
	if (music_flag == 1)
		wsprintf(music_status, _T("🎵 背景音乐：开启"));
	else
		wsprintf(music_status, _T("🔇 背景音乐：关闭"));
	outtextxy(card_left + 50, start_y, music_status);

	// 第二项：棋盘主题（占位）
	outtextxy(card_left + 50, start_y + line_spacing, _T("🎨 棋盘主题：木质经典"));

	// 第三项：计时器设置（占位）
	outtextxy(card_left + 50, start_y + 2 * line_spacing, _T("⏱ 总时间限制：10 分钟 / 单步：30 秒"));

	// 底部小提示（也加粗加大）
	fontStyle.lfHeight = PIECE_SIZE * 0.45;   // 约18px
	settextstyle(&fontStyle);
	settextcolor(RGB(150, 150, 150));
	outtextxy(card_left + 50, card_bottom - 45, _T("更多设置项将在后续版本中开放"));

	// ---------- 返回按钮 ----------
	back_hover_button = -1;
	back_pressed_button = -1;
	Draw_Back_Button(BUTTON_STATE_NORMAL, &img_universal_background);
}

void Draw_Message_Page(const TCHAR* message) {
	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FW_BOLD;          //加粗
	fontStyle.lfHeight = 50;               //放大字号
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);

	settextcolor(RGB(40, 40, 40));         //深灰色，更柔和
	int textW = textwidth(message);
	int textH = textheight(message);
	int x = (BOARD_SIZE + INFO_SIZE - textW) / 2;
	int y = (BOARD_SIZE - textH) / 2;
	outtextxy(x, y, message);

	FlushBatchDraw();
}

//3、多人界面与功能函数
void Draw_Multi_Button(int button_index, int button_state) {

	if (button_index == NETWORK_MODE_SERVER) {
		Draw_Menu_Button(button_multiall_x, button_createserver_y, _T("创建服务器"), button_state, &img_universal_background);
	}
	else if (button_index == NETWORK_MODE_CLIENT) {
		Draw_Menu_Button(button_multiall_x, button_joinserver_y, _T("加入服务器"), button_state, &img_universal_background);
	}
}

int Multi_Hit_Button(int x, int y) {

	if (Is_InRect(x, y, button_multiall_x, button_createserver_y, button_multiall_x + button_width, button_createserver_y + button_height)) {
		return NETWORK_MODE_SERVER;
	}
	else if (Is_InRect(x, y, button_multiall_x, button_joinserver_y, button_multiall_x + button_width, button_joinserver_y + button_height)) {
		return NETWORK_MODE_CLIENT;
	}
	return -1;
}

void Draw_Multi() {

	settextcolor(RGB(0, 0, 0));
	setbkmode(TRANSPARENT);
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = 0;
	fontStyle.lfHeight = PIECE_SIZE / 2;
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);

	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

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
	if (back_result == MAIN_BACK) {
		return MAIN_BACK;
	}

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
		}
		else if (multi_hover_button != current_button) {
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
	}
	else if (msg.message == WM_LBUTTONDOWN) {
		multi_pressed_button = current_button;
		if (multi_pressed_button != -1) {
			Draw_Multi_Button(multi_pressed_button, BUTTON_STATE_PRESSED);
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONUP) {
		int pressed_button = multi_pressed_button;
		multi_pressed_button = -1;

		if (pressed_button != -1) {
			Draw_Multi_Button(pressed_button, current_button == pressed_button ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL);
			FlushBatchDraw();
		}

		if (pressed_button == current_button && current_button != -1) {
			if (current_button == NETWORK_MODE_SERVER) printf("创建服务器\n");
			else if (current_button == NETWORK_MODE_CLIENT) printf("加入服务器\n");
			return current_button;
		}
	}

	return -1;
}

bool Input_Box() {

	//初始化填入框
	inputStr[0] = L'0';
	int index = 0;

	//输入框尺寸
	int inputBoxWidth = 400;
	int inputBoxHeight = 50;
	int inputBoxLeft = (BOARD_SIZE + INFO_SIZE - inputBoxWidth) / 2;
	int inputBoxTop = BOARD_SIZE / 2 + 20;
	int inputBoxRight = inputBoxLeft + inputBoxWidth;
	int inputBoxBottom = inputBoxTop + inputBoxHeight;

	//提示文字位置
	int promptY = inputBoxTop - 60;

	//取消按钮尺寸和位置
	int cancelWidth = 120;
	int cancelHeight = 45;
	int cancelLeft = (BOARD_SIZE + INFO_SIZE - cancelWidth) / 2;
	int cancelTop = inputBoxBottom + 25;
	int cancelRight = cancelLeft + cancelWidth;
	int cancelBottom = cancelTop + cancelHeight;

	//按钮状态
	bool cancelHover = false;
	bool cancelPressed = false;

	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	setorigin(0, 0);
	putimage(0, 0, &img_universal_background);

	//全局字体设置
	LOGFONT fontStyle;
	gettextstyle(&fontStyle);
	fontStyle.lfQuality = ANTIALIASED_QUALITY;
	fontStyle.lfWeight = FW_BOLD;
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
	settextstyle(&fontStyle);
	setbkmode(TRANSPARENT);

	while (true) {

		//处理外设消息
		ExMessage msg;
		while (peekmessage(&msg, -1)) {

			//鼠标信息
			bool hover = Is_InRect(msg.x, msg.y, cancelLeft, cancelTop, cancelRight, cancelBottom);

			if (msg.message == WM_MOUSEMOVE) {
				if (hover != cancelHover) {
					cancelHover = hover;
				}
			}
			else if (msg.message == WM_LBUTTONDOWN) {
				if (hover) {
					cancelPressed = true;
				}
			}
			else if (msg.message == WM_LBUTTONUP) {
				if (cancelPressed && hover) {
					//点击取消按钮
					inputStr[0] = _T('\0');
					return false;
				}
				cancelPressed = false;
			}

			//键盘信息
			if (msg.message == WM_KEYDOWN) {
				if (msg.vkcode == VK_RETURN) {							//回车确认
					return index > 0;									//防止空输入
				}
				else if (msg.vkcode == VK_ESCAPE) {						//ESC全部删除并退出
					inputStr[0] = L'\0';
					return false;
				}
				else if (msg.vkcode == VK_BACK && index > 0) {			//删除字符
					inputStr[--index] = _T('\0');
				}
			}
			else if (msg.message == WM_CHAR) {							//打印输入的字符
				wchar_t ch = (wchar_t)msg.ch;
				if (ch >= 32 && ch <= 126 && index < FONT_NUM - 1) {	//只允许打ASCII字符
					inputStr[index++] = ch;
					inputStr[index] = _T('\0');
				}
			}
		}

		//绘制整个界面
		cleardevice();
		putimage(0, 0, &img_universal_background);

		//提示文字
		fontStyle.lfHeight = 30;
		settextstyle(&fontStyle);
		settextcolor(RGB(50, 50, 50));
		int promptW = textwidth(_T("请输入服务器IP:端口，按回车确认"));
		outtextxy((BOARD_SIZE + INFO_SIZE - promptW) / 2, promptY, _T("请输入服务器IP:端口，按回车确认"));

		//输入框边框
		setlinestyle(PS_SOLID, 3);
		setlinecolor(RGB(30, 30, 30));
		rectangle(inputBoxLeft, inputBoxTop, inputBoxRight, inputBoxBottom);

		//输入文字（字号 26）
		fontStyle.lfHeight = 26;
		settextstyle(&fontStyle);
		settextcolor(RGB(20, 20, 20));
		int textY = inputBoxTop + (inputBoxHeight - textheight(_T("A"))) / 2;
		outtextxy(inputBoxLeft + 10, textY, inputStr);

		//光标闪烁
		if ((GetTickCount() / 500) % 2 == 0) {
			int caretX = inputBoxLeft + 10 + textwidth(inputStr);
			setlinecolor(RGB(0, 0, 0));
			setlinestyle(PS_SOLID, 2);
			line(caretX, textY + 2, caretX, textY + textheight(_T("A")) - 2);
		}

		//取消按钮
		int btnState = BUTTON_STATE_NORMAL;
		if (cancelPressed && cancelHover) btnState = BUTTON_STATE_PRESSED;
		else if (cancelHover) btnState = BUTTON_STATE_HOVER;
		//需要局部清除按钮区域背景
		int padding = 10;
		int refreshX = cancelLeft - padding;
		int refreshY = cancelTop - padding;
		int refreshW = cancelWidth + 2 * padding;
		int refreshH = cancelHeight + 2 * padding;
		if (refreshX < 0) { refreshW += refreshX; refreshX = 0; }
		if (refreshY < 0) { refreshH += refreshY; refreshY = 0; }
		if (refreshX + refreshW > BOARD_SIZE + INFO_SIZE) refreshW = BOARD_SIZE + INFO_SIZE - refreshX;
		if (refreshY + refreshH > BOARD_SIZE) refreshH = BOARD_SIZE - refreshY;
		if (refreshW > 0 && refreshH > 0) {
			putimage(refreshX, refreshY, refreshW, refreshH, &img_universal_background, refreshX, refreshY);
		}
		Draw_Button_Frame(cancelLeft, cancelTop, cancelRight, cancelBottom, 18, btnState);
		Draw_Button_Text(cancelLeft, cancelTop, cancelWidth, cancelHeight, _T("取消"), btnState);

		FlushBatchDraw();
		Sleep(10);
	}

	return true;
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
	Draw_All_Game_Buttons(BUTTON_STATE_NORMAL);

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

void Draw_Game_Button(int button_index, int button_state) {

	int center_x = 0;
	const TCHAR* button_text = _T("");

	switch (button_index) {
	case GAME_BUTTON_RESTART:
		center_x = button_restart_x;
		button_text = _T("重");
		break;
	case GAME_BUTTON_TAKEBACK:
		center_x = button_takeback_x;
		button_text = _T("悔");
		break;
	case GAME_BUTTON_MUSIC:
		center_x = button_music_x;
		button_text = music_flag == 1 ? _T("音") : _T("静");
		break;
	case GAME_BUTTON_SETTING:
		center_x = button_setting_x;
		button_text = _T("设");
		break;
	case GAME_BUTTON_EXIT:
		center_x = button_exit_x;
		button_text = _T("退");
		break;
	default:
		return;
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

	if (Is_InRect(x, y, button_restart_x - BUTTON_SIZE / 2, top, button_restart_x + BUTTON_SIZE / 2, bottom)) {
		return GAME_BUTTON_RESTART;
	}
	else if (Is_InRect(x, y, button_takeback_x - BUTTON_SIZE / 2, top, button_takeback_x + BUTTON_SIZE / 2, bottom)) {
		return GAME_BUTTON_TAKEBACK;
	}
	else if (Is_InRect(x, y, button_music_x - BUTTON_SIZE / 2, top, button_music_x + BUTTON_SIZE / 2, bottom)) {
		return GAME_BUTTON_MUSIC;
	}
	else if (Is_InRect(x, y, button_setting_x - BUTTON_SIZE / 2, top, button_setting_x + BUTTON_SIZE / 2, bottom)) {
		return GAME_BUTTON_SETTING;
	}
	else if (Is_InRect(x, y, button_exit_x - BUTTON_SIZE / 2, top, button_exit_x + BUTTON_SIZE / 2, bottom)) {
		return GAME_BUTTON_EXIT;
	}
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
		}
		else if (game_hover_button != current_button) {
			int old_hover = game_hover_button;
			game_hover_button = current_button;

			if (old_hover != -1) {
				Draw_Game_Button(old_hover, BUTTON_STATE_NORMAL);
			}
			if (game_hover_button != -1) {
				Draw_Game_Button(game_hover_button, BUTTON_STATE_HOVER);
			}
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONDOWN) {
		game_pressed_button = current_button;
		if (game_pressed_button != -1) {
			Draw_Game_Button(game_pressed_button, BUTTON_STATE_PRESSED);
			FlushBatchDraw();
		}
	}
	else if (msg.message == WM_LBUTTONUP) {
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

	Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);	//清空黑方回合计时区
	outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋重置倒计时文字
	Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);	//清空白方回合计时区
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
	Clear_Info_Background((int)timer_black_total_x, (int)timer_black_total_y, (int)total_time_sizex, (int)total_time_sizey);
	Clear_Info_Background((int)timer_white_total_x, (int)timer_white_total_y, (int)total_time_sizex, (int)total_time_sizey);

	settextcolor(BLACK);
	outtextxy(timer_black_total_x, timer_black_total_y, black_time_total_text);
	outtextxy(timer_white_total_x, timer_white_total_y, white_time_total_text);

	//回合剩余时间绘制
	if (player == BLACK) {

		setlinecolor(RED);
		roundrect(black_timearea_left, black_timearea_top, black_timearea_right, black_timearea_bottom, 15, 15);

		Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, time_text_round);	//绘制黑棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋倒计时文字
	}
	else if (player == WHITE) {

		setlinecolor(RED);
		roundrect(white_timearea_left, white_timearea_top, white_timearea_right, white_timearea_bottom, 15, 15);

		Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, time_text_round);	//绘制白棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋倒计时文字
	}

	//4、添加倒计时5秒闪烁提示
	static int flash_count = 0;	//闪烁节拍器，使用static保证函数调用后数值不变
	if (turn_remain_time <= 5 && turn_remain_time > 0) {
		flash_count++;
		if (flash_count % FLASH_CYCLE < FLASH_CYCLE / 2) {
			if (player == BLACK)	Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);	//清空黑方回合计时区
			else if (player == WHITE)	Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);	//清空白方回合计时区
		}
		else {
			settextcolor(RED);
			if (player == BLACK) {
				Clear_Info_Background((int)timer_black_x, (int)timer_black_y, (int)round_time_sizex, (int)round_time_sizey);	//清空黑方回合计时区
				outtextxy(timer_black_x, timer_black_y, time_text_round);	//绘制黑棋倒计时文字
			}
			else if (player == WHITE) {
				Clear_Info_Background((int)timer_white_x, (int)timer_white_y, (int)round_time_sizex, (int)round_time_sizey);	//清空白方回合计时区
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

	if (msg.message == WM_LBUTTONUP && Game_Hit_Button(msg.x, msg.y) == GAME_BUTTON_SETTING) {
		printf("跳转到设置面\n");
		return true;
	}
	return false;
}

bool Switch_To_Menu(ExMessage msg) {

	if (msg.message == WM_LBUTTONUP && Game_Hit_Button(msg.x, msg.y) == GAME_BUTTON_EXIT) {
		printf("跳转到主页面\n");
		return true;
	}
	return false;
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
	game_hover_button = -1;
	game_pressed_button = -1;

	FlushBatchDraw();
}

//6、网络功能函数
bool Parse_IPPort(const char* input, char* ip, int host_len, int& port) {

	//空输入驳回
	if (input == NULL || input[0] == '\0') {
		return false;
	}

	//查找冒号
	const char* colon = strchr(input, ':');
	if (colon == NULL) {
		//没有冒号，整个字符串为主机名，使用默认端口
		if (input[0] == '\0') return false;
		strncpy_s(ip, host_len, input, _TRUNCATE);
		port = PORT;  //默认端口
		return true;
	}

	//分离主机部分
	int host_part_len = colon - input;
	//冒号前为空驳回
	if (host_part_len <= 0 || host_part_len >= host_len) {
		return false;
	}
	if (host_part_len >= host_len) {
		return false;  // 主机名太长
	}
	strncpy_s(ip, host_len, input, host_part_len);
	ip[host_part_len] = '\0';

	//解析端口部分
	const char* port_str = colon + 1;
	char* endptr;
	long p = strtol(port_str, &endptr, 10);
	if (*endptr != '\0' || p <= 0 || p > 65535) {
		return false;  //端口无效
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

	//设置端口复用，允许立即重用 TIME_WAIT 状态的端口，防止第二次创建服务器失败
	int reuse = 1;
	if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == SOCKET_ERROR) {
		printf_s("设置端口复用失败：%d\n", WSAGetLastError());
	}

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
		printf_s("监听失败：%d\n", WSAGetLastError());
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
		if (remoteHost == NULL) {
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

	printf("客户端成功连接%s:%d，等待服务器接受...\n", ip, port);
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
	//关闭监听套接字，释放端口资源，避免后续干扰
	closesocket(ServerSocket);
	ServerSocket = INVALID_SOCKET;
	return true;
}

//使用可靠的TCP发送
bool Send_All(SOCKET s, const char* data, int len) {
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
			//错误的下棋信息不予下棋
			if (!Is_Valid_Move_Message(msg)) {
				break;
			}
			if (Board[msg.x][msg.y] != EMPTY) {
				break;
			}

			if (Board[msg.x][msg.y] == EMPTY) {

				Clear_Highlight(Board);

				int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * msg.x - OFFSET;	//计算图像棋盘对应位置
				int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * msg.y - OFFSET;

				if (msg.player == BLACK) {
					Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);	//绘制黑棋
					Board[msg.x][msg.y] = BLACK;

					if (Judge_Win_Chess(Board, msg.x, msg.y, msg.player) == msg.player) {
						choice = _getch();	//判断输赢
					}
					player = WHITE;	//棋方转换
				}
				else if (msg.player == WHITE) {
					Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
					Board[msg.x][msg.y] = WHITE;

					if (Judge_Win_Chess(Board, msg.x, msg.y, msg.player) == msg.player) {
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
			game_over = true;						//表示游戏结束，强制退出游戏
			break;
		}
		case MSG_TIMEOUT:
		{
			if (msg.player == player) {

				//执行与本地超时相同的换边逻辑
				player = BLACK + WHITE - player;
				isMyturn = !isMyturn;				 //翻转回合标志
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

//7、程序健壮性函数
bool Is_Valid_Move_Message(const NetworkMessage& msg) {
	return msg.msgtype == MSG_MOVE &&
		msg.x >= 0 && msg.x < LINE_NUM &&
		msg.y >= 0 && msg.y < LINE_NUM &&
		(msg.player == BLACK || msg.player == WHITE);
}


//=====================/主函数/=====================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	if (!Init_Winsock()) return -1;									//初始化Winsock

	Texture_Load();													//加载素材
	initgraph(BOARD_SIZE + INFO_SIZE, BOARD_SIZE, EX_DBLCLKS);		//创建画布，支持双击参数传入
	BeginBatchDraw();												//启动批量绘制 (采用双缓冲技术减少闪烁)

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);			//去除DPI放缩

	//HWND hConsole = GetConsoleWindow();							//展示控制台用于调试
	//ShowWindow(hConsole, SW_SHOW);

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
			while (currentState == MENU) {
				getmessage(&msg, EX_MOUSE);

				game_mode = Main_Switch(msg);
				if (game_mode == -1) continue;
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
			flushmessage();															//清空所有待处理消息
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
		case MULTIPLAYER:
		{
			//MULTIPLAYER绘制与功能函数

			flushmessage();          // 清空所有待处理消息
			//绘制创建服务器和加入服务器选择页面
			Draw_Multi();
			FlushBatchDraw();	//刷新显示

			//逻辑显示
			ExMessage msg;
			while (currentState == MULTIPLAYER) {

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
				else if (multi_mode == MAIN_BACK) {
					currentState = MENU;
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

				u_long mode = 1;
				ioctlsocket(ServerSocket, FIONBIO, &mode);	//将服务器套接字设置为非阻塞模式，在循环中检查accpet

				//绘制成功创建服务器提示画面
				Draw_Message_Page(_T("服务器创建成功，等待连接..."));

				// 定义取消按钮区域
				int cancel_width = 200;
				int cancel_height = 50;
				int cancel_x = (BOARD_SIZE + INFO_SIZE - cancel_width) / 2;
				int cancel_y = BOARD_SIZE / 2 + 80;
				int cancel_left = cancel_x, cancel_top = cancel_y;
				int cancel_right = cancel_x + cancel_width, cancel_bottom = cancel_y + cancel_height;
				bool cancel_hover = false;

				// 绘制取消按钮
				setbkmode(TRANSPARENT);
				settextcolor(RGB(0, 0, 0));
				LOGFONT fontStyle;
				gettextstyle(&fontStyle);
				fontStyle.lfQuality = ANTIALIASED_QUALITY;
				fontStyle.lfWeight = 0;
				fontStyle.lfHeight = PIECE_SIZE / 2;
				_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));
				settextstyle(&fontStyle);

				Draw_Button_Frame(cancel_x, cancel_y, cancel_right, cancel_bottom, 20, BUTTON_STATE_NORMAL);
				Draw_Button_Text(cancel_x, cancel_y, cancel_width, cancel_height, _T("取消"), BUTTON_STATE_NORMAL);
				FlushBatchDraw();


				bool waiting = true;	//正在等待服务器连接
				while (waiting) {

					//处理鼠标消息
					ExMessage msg;
					if (peekmessage(&msg, EX_MOUSE)) {
						bool hover = Is_InRect(msg.x, msg.y, cancel_left, cancel_top, cancel_right, cancel_bottom);
						if (msg.message == WM_MOUSEMOVE) {
							if (hover != cancel_hover) {
								cancel_hover = hover;
								//重绘整个等待界面
								Draw_Message_Page(_T("服务器创建成功，等待连接..."));
								//重新绘制取消按钮，根据悬停状态改变颜色
								int cancel_state = cancel_hover ? BUTTON_STATE_HOVER : BUTTON_STATE_NORMAL;
								Draw_Button_Frame(cancel_x, cancel_y, cancel_right, cancel_bottom, 20, cancel_state);
								Draw_Button_Text(cancel_x, cancel_y, cancel_width, cancel_height, _T("取消"), cancel_state);
								FlushBatchDraw();
							}
						}
						else if (msg.message == WM_LBUTTONUP && hover) {
							//取消服务器
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

					//非阻塞检查客户端连接
					sockaddr_in ClientAddr;
					int addrLen = sizeof(ClientAddr);
					SOCKET client = accept(ServerSocket, (sockaddr*)&ClientAddr, &addrLen);
					if (client != INVALID_SOCKET) {
						//连接成功
						ClientSocket = client;
						closesocket(ServerSocket);
						ServerSocket = INVALID_SOCKET;
						isConnected = true;
						isMyturn = true;          //服务端先手
						waiting = false;
						Draw_Message_Page(_T("客户端已连接，即将开始游戏"));
						FlushBatchDraw();
						Sleep(1500);
						currentState = PLAYING;
						break;
					}
					else {
						int err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK) {
							//accept 发生错误
							printf("accept error: %d\n", err);
							closesocket(ServerSocket);
							ServerSocket = INVALID_SOCKET;
							waiting = false;
							currentState = MULTIPLAYER;
							break;
						}
					}
				}
			}
			else {
				printf_s("服务器创建失败\n");

				//绘制创建失败提示
				Draw_Message_Page(_T("服务器创建失败，返回多人页面"));
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
			if (!Input_Box()) {													//ESC取消输入并返回多人页面
				currentState = MULTIPLAYER;
				break;
			}

			NetworkMode = NETWORK_MODE_CLIENT;
			printf_s("输入的服务器IP地址：%ls\n", inputStr);
			printf_s("正在连接中\n");

			char ipBuffer[FONT_NUM] = { 0 };
			WideCharToMultiByte(CP_ACP, 0, inputStr, -1, ipBuffer, sizeof(ipBuffer), NULL, NULL);	//使用 WideCharToMultiByte 确保正确转换

			char ip[256];
			int port;
			if (!Parse_IPPort(ipBuffer, ip, sizeof(ip), port)) {
				printf_s("输入的地址格式错误\n");
				//显示错误信息
				Draw_Message_Page(_T("地址格式错误，返回多人页面"));
				currentState = MULTIPLAYER;
				break;
			}

			if (Connect_Server(ip, port)) {
				isMyturn = false;				//我方是客户端，后下棋
				isConnected = true;				//成功连接
				printf_s("连接到服务器\n");

				//绘制成功连接页面
				Draw_Message_Page(_T("连接成功，即将开始游戏"));

				Sleep(1500);
				currentState = PLAYING;
			}
			else {
				printf_s("服务器连接失败\n");

				//绘制连接失败提示
				Draw_Message_Page(_T("服务器连接失败，返回多人页面"));
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
			//int movecount = 0;									//计算平局

			game_remain_time = time(NULL);						//初始化总游戏时间开始
			Turn_Timer_Start();									//初始化计时器

			//服务器强制黑棋先手，防止逻辑紊乱
			if (NetworkMode == NETWORK_MODE_SERVER) {
				isMyturn = true;
			}
			else if (NetworkMode == NETWORK_MODE_CLIENT) {
				isMyturn = false;
			}
			else {
				isMyturn = true;
			}

			flushmessage();										//清空所有待处理消息
			cleardevice();										//清空画面
			Draw_Board();										//绘制棋盘
			Draw_Info();										//绘制信息区
			FlushBatchDraw();

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

				//处理本地的鼠标信号
				ExMessage msg;
				if (peekmessage(&msg, EX_MOUSE, false)) {

					getmessage(&msg, EX_MOUSE);																//获取鼠标信息

					int game_button_result = Game_Button_Switch(msg);
					if (game_button_result == GAME_BUTTON_RESTART) {

						//网络模式下不准重启
						if (NetworkMode != NETWORK_MODE_LOCAL) {
							continue;
						}

						printf("重启游戏\n");
						Restart_Game(board, &player);																//重启游戏
						continue;
					}
					else if (game_button_result == GAME_BUTTON_TAKEBACK) {
						printf("悔棋\n");
						Take_Back_Move();
						continue;
					}
					else if (game_button_result == GAME_BUTTON_MUSIC) {
						Game_Music_Control(msg);															//音乐开关按键
						continue;
					}
					else if (game_button_result == GAME_BUTTON_SETTING) {
						printf("跳转到设置面\n");
						currentState = SETTING;
						break;
					}
					else if (game_button_result == GAME_BUTTON_EXIT) {
						printf("跳转到主页面\n");

						//通知对方退出
						if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
							NetworkMessage quitMsg;
							quitMsg.msgtype = MSG_QUIT;
							Send_Network_Message(ClientSocket, quitMsg);
						}

						//清理网络资源，释放端口
						if (ClientSocket != INVALID_SOCKET) {
							closesocket(ClientSocket);
							ClientSocket = INVALID_SOCKET;
						}
						if (ServerSocket != INVALID_SOCKET) {
							closesocket(ServerSocket);
							ServerSocket = INVALID_SOCKET;
						}
						isConnected = false;
						NetworkMode = NETWORK_MODE_LOCAL;   // 重置为本地模式，避免干扰后续操作

						currentState = MENU;																			//跳转至主页面
						Restart_Game(board, &player);
						break;
					}

					//限制下棋
					if (NetworkMode != NETWORK_MODE_LOCAL && !isMyturn) {											//网络模式检查是否是我方下棋（不是则循环跳过）
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

								//选择结束模式
								int over_choice;

								while (true) {

									if (peekmessage(&msg, EX_MOUSE, false)) {

										getmessage(&msg, EX_MOUSE);																		//获取鼠标信息

										int game_button_result = Game_Button_Switch(msg);
										if (game_button_result == GAME_BUTTON_RESTART) {

											//网络模式下不准重启
											if (NetworkMode != NETWORK_MODE_LOCAL) {
												continue;
											}

											printf("重启游戏\n");
											choice = 0;																					//表明要重启游戏
											Restart_Game(board, &player);																//重启游戏
											break;
										}
										else if (game_button_result == GAME_BUTTON_TAKEBACK) {											//游戏结束不允许悔棋
											continue;
										}
										else if (game_button_result == GAME_BUTTON_MUSIC) {
											Game_Music_Control(msg);																	//音乐开关按键
											continue;
										}
										else if (game_button_result == GAME_BUTTON_SETTING) {
											printf("跳转到设置面\n");
											currentState = SETTING;
											break;
										}
										else if (game_button_result == GAME_BUTTON_EXIT) {
											choice = 1;																					//表明要回到主界面
											printf("跳转到主页面\n");

											//清理网络资源，释放端口（游戏结束不需要对方强制退出）
											if (ClientSocket != INVALID_SOCKET) {
												closesocket(ClientSocket);
												ClientSocket = INVALID_SOCKET;
											}
											if (ServerSocket != INVALID_SOCKET) {
												closesocket(ServerSocket);
												ServerSocket = INVALID_SOCKET;
											}
											isConnected = false;
											NetworkMode = NETWORK_MODE_LOCAL;															//重置为本地模式，避免干扰后续操作

											currentState = MENU;																		//跳转至主页面
											Restart_Game(board, &player);
											break;
										}
									}

								}

								//选择结束模式
								if (choice == 0) {
									continue;										//重新开始下棋
								}
								else {
									break;											//再break一次，否则还是在棋盘界面
								}
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
			flushmessage();															//清空所有待处理消息
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
			}

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
