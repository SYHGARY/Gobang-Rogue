//=====================/头文件/=====================

//预处理定义
#define _CRT_SECURE_NO_WARNINGS		//禁用CRT安全警告
#define _WINSOCK_DEPRECATED_NO_WARNINGS		//禁用Winsock弃用警告
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX	//避免min/max宏冲突

//Windows API
#include <winsock2.h>	//Winsock2 API
#include<ws2tcpip.h>	//Winsock2 API拓展
#include<windows.h>		//Windows API
#include<mmsystem.h>	//多媒体API

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

//=====================/常量设定/=====================

//棋盘绘制位置常量
#define LINE_NUM 15	//线条数
#define BOARD_SIZE 650	//棋盘大小
#define BOARD_MARGIN 280	//棋盘横线边缘
#define BOARD_MARGIN_REAL 45	//棋盘横线边缘真实距离  
#define OFFSET (BOARD_SIZE+PIECE_SIZE)/2	//鼠标坐标与绘图坐标偏移量
#define DOT_POS 160	//星位点位置
#define DOT_RADIUS 5	//星位点半径
#define BOARD_ORIGIN_X 325	//初始X坐标
#define BOARD_ORIGIN_Y 325	//初始Y坐标
#define PIECE_SIZE 40	//棋子大小 / 格子大小
#define FONT_POS 2.5 //字体位置

//信息区绘制位置常量
#define INFO_SIZE 400	//信息区大小

//棋盘状态常量
#define EMPTY 0	//无棋子
#define BLACK 1	//黑棋
#define WHITE 2	//白棋
#define START 1
#define END 0

//美工常量
#define FONT_WEIGHT 700	//字体粗细
#define BUTTON_SIZE 40	//按钮大小
#define BUTTON_GAP	10	//按钮间隔
#define BUTTON_POS 2.5	//按钮高度位置

//时间常量
#define TIME_LIMIT_TOTAL 600 //总时长限制10分钟
#define TIME_LIMIT 30 //单步限制30秒
#define TIME_WARN 10 //剩余10秒警告
#define FLASH_CYCLE 500	//闪烁周期为500个循环一闪

//网络模式常量（后续使用图形化界面替代）
#define PORT 8888				//默认网络端口
#define NETWORK_MODE_LOCAL	1	//本地对战
#define NETWORK_MODE_SERVER	2	//网络对战（服务端）
#define NETWORK_MODE_CLIENT	3	//网络对战（客户端）
#define NETWORK_MODE_AI	4		//人机对战

//网络状态常量
#define MSG_MOVE 0	//落子信息
#define MSG_QUIT 1	//退出信息
#define MSG_CHAT 2	//聊天信息

//=====================/结构体设定/=====================

//网络数据包
struct NetworkMessage {
	int msgtype;	//消息类型
	int x, y;		//棋盘坐标
	int player;		//玩家
	char data[256];	//其他数据
};

//=====================/公共变量设定/=====================

//初始设定公共变量
int highlightX = -1, highlightY = -1;	//记录高光坐标
time_t game_start_time;	//游戏开始时间
time_t turn_start_time;	//回合开始时间
int game_remain_time;	//游戏剩余时间
int turn_remain_time;	//回合剩余时间
bool game_time_running;	//计时器是否运行

//图片公共变量
IMAGE img_start_background, img_game_background, img_info_background;	//背景图片
IMAGE img_white, img_white_opp, img_black, img_black_opp;	//棋子图片

//游戏坐标公共变量
int button_restart_x = BOARD_SIZE + BUTTON_GAP + BUTTON_SIZE / 2;	//重新开始按钮X坐标
int button_takeback_x = BOARD_SIZE + BUTTON_GAP + BUTTON_SIZE + BUTTON_GAP + BUTTON_SIZE / 2;	//悔棋按钮X坐标
int button_exit_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;	//退出按钮X坐标
int button_setting_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;	//设置按钮X坐标
int button_music_x = BOARD_SIZE + INFO_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE - BUTTON_GAP - BUTTON_SIZE / 2;	//音乐按钮X坐标

//游戏功能相关公共变量
int music_flag = 1;	//音乐是否开启标识
char choice;	//用于让界面暂停

//网络相关公共变量
SOCKET ServerSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
int NetworkMode = NETWORK_MODE_LOCAL;	//网络状态默认本地
char ServerIP[20] = "127.0.0.1";		//默认为回送地址
bool isConnected = false;				//检测是否连接成功
bool isMyturn = true;					//防止玩家在对手回合下棋


//=====================/总体功能函数声明/=====================

//1、基础功能函数声明
void Texture_Load();	//素材导入函数
void Game_Music_Control();	//游戏音乐控制函数
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img);	//透明图片载入函数
bool Is_InCirecle(int px, int py, int cx, int cy, int radius);	//是否在圆内判断函数

//2、游戏界面与功能函数声明
void Draw_Board();	//五子棋盘绘制函数
void Draw_Info();	//信息区域绘制函数

void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]);	//绘制高光函数
void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]);	//清理高光函数（局部重绘提高效率）
void Player_Point(int player);	//玩家指向函数

void Turn_Timer_Start();	//回合计时初始化函数
void Turn_Timer_Update();	//回合剩余时间计算函数
void Turn_Draw_Timer(int player);	//绘制倒计时函数

void Draw_Gaming_Elements();	//绘制棋盘其他元素
int Judge_Win(int Board[LINE_NUM][LINE_NUM], int x, int y, int player);	//判断游戏结束函数（这里的X，Y代表的是现在下的棋的位置，因为其实只有现在下的棋改变了棋盘的状态，提高搜索效率）

//3、游戏玩法函数声明
void Take_Back_Move();

//4、网络功能函数声明
bool Init_Winsock();				//初始化Winsock
bool Create_Server();			//创建服务器（服务端）
bool Connect_Server(const char* ip);			//连接服务器（客户端）
bool Accept_Connection();		//同意连接（服务端）
bool Send_Network_Message();		//传输数据（客户端）
bool Receive_Network_Message();	//接收数据（服务端）
void Network_Mode_Event();		//网络模式对战处理

//=====================/函数具体内容/=====================

//1、基础功能函数
void Texture_Load() {

	loadimage(&img_start_background, _T("./素材/background1.jpg"), BOARD_SIZE, BOARD_SIZE);	//游戏开始背景
	loadimage(&img_game_background, _T("./素材/wood1.jpg"));	//游戏界面背景
	loadimage(&img_info_background, _T("./素材/info_background.jpg"), INFO_SIZE, BOARD_SIZE);	//信息框背景

	loadimage(&img_white, _T("./素材/white.png"), PIECE_SIZE, PIECE_SIZE);	//白棋彩色图
	loadimage(&img_white_opp, _T("./素材/white1.png"), PIECE_SIZE, PIECE_SIZE);	//白棋掩码图
	loadimage(&img_black, _T("./素材/black.png"), PIECE_SIZE, PIECE_SIZE);	//黑棋彩色图
	loadimage(&img_black_opp, _T("./素材/black1.png"), PIECE_SIZE, PIECE_SIZE);	//黑棋掩码图
}

void Game_Music_Control(ExMessage msg) {

	if (Is_InCirecle(msg.x, msg.y, button_music_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE/2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN && music_flag == 0) {
		mciSendString(_T("play ./素材/music1.mp3 repeat"), NULL, 0, NULL);    //重复播放音乐
		music_flag = 1;
		printf_s("开启音乐");
	}
	else if (Is_InCirecle(msg.x, msg.y, button_music_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE / 2, BUTTON_SIZE / 2) && msg.message == WM_LBUTTONDOWN && music_flag == 1) {
		mciSendString(_T("stop bgm"), NULL, 0, NULL);    //关闭播放音乐
		music_flag = 0;
		printf_s("关闭音乐");
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

//2、游戏界面与功能函数
void Draw_Board() {

	//1、放置木质背景
	setorigin(0, 0);	//设置初始点
	putimage(0, 0, &img_game_background);	//放置游戏背景

	//2、绘制网格
	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
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

	//1、防止信息背景
	setorigin(0, 0);	//设置初始点
	putimage(BOARD_SIZE, 0, &img_info_background);	//放置信息背景
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
	if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);	//左上角星位点
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);	//右上角星位点
	else if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);	//左下角星位点
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);	//右下角星位点
	else if (centerX == BOARD_ORIGIN_X && centerY == BOARD_ORIGIN_Y) solidcircle(BOARD_ORIGIN_X, BOARD_ORIGIN_Y, DOT_RADIUS);	//中心星位点

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
	game_time_running = true;	//设置计时器状态为开启

	//2、清理上次计时区域
	setorigin(0, 0);	//设置坐标原点
	int round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	int round_time_sizey = PIECE_SIZE / 2;
	int timer_black_x = BOARD_SIZE + FONT_POS * 2 + PIECE_SIZE + 5;	//加5略微靠右
	int timer_black_y = PIECE_SIZE + FONT_POS + round_time_sizey;
	int timer_white_x = BOARD_SIZE + INFO_SIZE - FONT_POS * 2 - PIECE_SIZE - round_time_sizex;
	int timer_white_y = PIECE_SIZE + FONT_POS + round_time_sizey;

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

void Turn_Timer_Update() {

	if (!game_time_running)	return;	//检查计时器是否启动，没启动提早返回

	//1、获取剩余时间
	time_t current_time = time(NULL);	//记录现在时间
	turn_remain_time = TIME_LIMIT - (int)difftime(current_time, turn_start_time);	//difftime(time1,time2)，time1为较晚时间，time2为较早时间，返回的是double类型，所以要int

	//2、判断是否关闭计时器
	if (turn_remain_time < 0) {
		turn_remain_time = 0;	//重置剩余时间
		game_time_running = false;	//关闭计时器，防止溢出
	}
}

void Turn_Draw_Timer(int player) {

	//1、清理上次计时区域
	setorigin(0, 0);	//设置坐标原点
	int round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	int round_time_sizey = PIECE_SIZE / 2;
	int timer_black_x = BOARD_SIZE + FONT_POS * 2 + PIECE_SIZE + 5;	//加5略微靠右
	int timer_black_y = PIECE_SIZE + FONT_POS + round_time_sizey;
	int timer_white_x = BOARD_SIZE + INFO_SIZE - FONT_POS * 2 - PIECE_SIZE - round_time_sizex;
	int timer_white_y = PIECE_SIZE + FONT_POS + round_time_sizey;

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
	wchar_t time_text[20];
	if (turn_remain_time >= 0) {
		wsprintf(time_text, _T("回合剩余：%02d秒"), turn_remain_time);	//wsprintf()的作用是将拼接字符串放入缓冲区
	}
	else {
		wsprintf(time_text, _T("超时"));
	}

	if (player == BLACK) {
		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, time_text);	//绘制黑棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋倒计时文字
	}
	else if (player == WHITE) {
		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, time_text);	//绘制白棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋倒计时文字
	}

	//4、添加倒计时5秒闪烁提示
	static int flash_count = 0;	//闪烁节拍器，使用static保证函数调用后数值不变
	if (turn_remain_time <= 5 && turn_remain_time > 0) {
		flash_count++;
		if (flash_count % FLASH_CYCLE < FLASH_CYCLE / 2) {
			if(player==BLACK)	putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
			else if(player==WHITE)	putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
		}
		else {
			settextcolor(RED);
			if (player == BLACK) {
				putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空黑方回合计时区
				outtextxy(timer_black_x, timer_black_y, time_text);	//绘制黑棋倒计时文字
			}
			else if (player == WHITE) {
				putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_info_background, round_time_sizex, round_time_sizey);	//清空白方回合计时区
				outtextxy(timer_white_x, timer_white_y, time_text);	//绘制白棋倒计时文字
			}
		}
	}
	else {
		flash_count = 0;
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点
}

void Draw_Gaming_Elements() {

	//1、计时区元素
	setorigin(0, 0);	//设置坐标原点

	int black_piece_x = BOARD_SIZE + 2 * FONT_POS;	//黑棋X坐标
	int black_piece_y = PIECE_SIZE + FONT_POS;	//黑棋Y坐标
	int white_piece_x = BOARD_SIZE + INFO_SIZE - 2 * FONT_POS - PIECE_SIZE;	//白棋X坐标
	int white_piece_y = PIECE_SIZE + FONT_POS;	//白棋y坐标
	Put_Transparent_Image(black_piece_x, black_piece_y, &img_black_opp, &img_black);	//黑棋计时区
	Put_Transparent_Image(white_piece_x, white_piece_y, &img_white_opp, &img_white);	//白棋计时区

	//2、游戏功能区元素

	//试验区
	solidcircle(button_restart_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE, BUTTON_SIZE / 2);
	solidcircle(button_takeback_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE, BUTTON_SIZE / 2);
	solidcircle(button_exit_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE, BUTTON_SIZE / 2);
	solidcircle(button_setting_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE, BUTTON_SIZE / 2);
	solidcircle(button_music_x, BOARD_SIZE - BUTTON_POS - BUTTON_SIZE, BUTTON_SIZE / 2);

	//3、游戏联机区元素

	//4、游戏卡牌区元素

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点
}

int Judge_Win(int Board[LINE_NUM][LINE_NUM], int x, int y, int player) {

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

//3、游戏玩法函数
//悔棋函数
void Take_Back_Move() {
	return;
}

//4、网络功能函数
bool Init_Winsock() {

	WSADATA wsaData;	//WSADATA用于存储Winsock初始化信息与版本数据
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);	//请求Winsock2.2版本，成功返回0
	if (result != 0) {
		printf_s("Winsock初始化失败：%d\n", result);
		return false;
	}
	else {
		printf_s("Winsock初始化成功\n");
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

bool Connect_Server(const char* ip) {

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
	ServerAddr.sin_addr.s_addr = inet_addr(ip);	//连接特定ip
	ServerAddr.sin_port = htons(PORT);			//监听端口

	//3、创建连接
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

void Network_Mode_Event(int Board[LINE_NUM][LINE_NUM], int& player) {

	if (!isConnected)	return;	//没有连接成功提前退出

	setorigin(BOARD_SIZE / 2, BOARD_SIZE / 2);	//设置初始坐标

	//设置成非阻塞状态，防止因等待网络通信而导致计时器等元素卡顿
	u_long mode = 1;	//1为非阻塞
	ioctlsocket(ClientSocket, FIONBIO, &mode);

	NetworkMessage msg;
	if (Receive_Network_Message(ClientSocket, msg)) {	//使用ServerSocket监听，使用ClientSocket双向通信
		switch (msg.msgtype) {
		case MSG_MOVE:
			if (Board[msg.x][msg.y] == EMPTY) {

				Clear_Highlight(Board);

				int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * msg.x - OFFSET;	//计算图像棋盘对应位置
				int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * msg.y - OFFSET;

				if (msg.player == BLACK) {
					Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);	//绘制黑棋
					Board[msg.x][msg.y] = BLACK;

					if (Judge_Win(Board, msg.x, msg.y, player) == BLACK) {
						choice = _getch();	//判断输赢
					}
					player = WHITE;	//棋方转换
				}
				else if (msg.player == WHITE) {
					Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
					Board[msg.x][msg.y] = WHITE;

					if (Judge_Win(Board, msg.x, msg.y, player) == WHITE) {
						choice = _getch();	//判断输赢
					}
					player = BLACK;	//棋方转换
				}

				isMyturn = true;	//接受完信息后，轮到我端下棋
				Turn_Timer_Start();	//初始化计时器
				FlushBatchDraw();	//刷新显示
			}
			break;
		case MSG_QUIT:
			printf_s("对方断开连接\n");
			isConnected = false;
			break;
		}

		//恢复为阻塞状态
		mode = 0;	//0为阻塞
		ioctlsocket(ClientSocket, FIONBIO, &mode);

		setorigin(0, 0);	//恢复初始坐标
	}



}

//=====================/主函数/=====================
int main() {

	//初始化Winsock
	if (!Init_Winsock()) {
		return -1;
	}

	//选择游戏模式
	printf_s("=====================================\n");
	printf_s("        欢迎游玩Gobang-Rogue！       \n");
	printf_s("=====================================\n");
	printf_s("请选择游玩模式：1、本地 2、创建服务器 3、连接服务器：");
	int game_mode;
	if (scanf_s("%d", &game_mode) != 1) {
		printf("输入错误\n");
		return -1;
	}

	switch (game_mode) {
	case NETWORK_MODE_LOCAL:
		NetworkMode = NETWORK_MODE_LOCAL;
		printf_s("本地对战模式\n");
		isMyturn = true;	//本地对战始终本端下棋
		break;
	case NETWORK_MODE_SERVER:
		NetworkMode = NETWORK_MODE_SERVER;
		printf_s("创建服务器\n");
		if (Create_Server()) {
			printf_s("服务器创建成功，等待客户端连接\n");

			if (Accept_Connection()) {
				isMyturn = true;			//我方是服务器，先下棋
				isConnected = true;			//成功连接
				printf_s("客户端已连接\n");
			}
		}
		else {
			printf_s("服务器创建失败\n");
			NetworkMode = NETWORK_MODE_LOCAL;
		}
		break;
	case NETWORK_MODE_CLIENT:
		NetworkMode = NETWORK_MODE_CLIENT;
		printf_s("输入服务器IP地址：");
		scanf_s("%s", ServerIP, (unsigned)_countof(ServerIP));
		printf_s("正在连接中\n");
		if (Connect_Server(ServerIP)) {
			isMyturn = false;				//我方是客户端，后下棋
			isConnected = true;				//成功连接
			printf_s("连接到服务器\n");
		}
		else {
			printf_s("服务器连接失败\n");
			NetworkMode = NETWORK_MODE_LOCAL;
		}
		break;
	default:
		printf_s("无效选择，默认本地模式\n");
		NetworkMode = NETWORK_MODE_LOCAL;
		isMyturn = true;					//本地均是本端下棋
		break;
	}



	//=====================/一、游戏初始化阶段/=====================

	//1、导入游戏素材
	Texture_Load();	//图片素材导入

	//mciSendString(_T("open ./素材/music1.mp3"), NULL, 0, NULL);    //打开音乐
	//mciSendString(_T("play ./素材/music1.mp3 repeat"), NULL, 0, NULL);    //重复播放音乐

	//2、初始化图形界面
	initgraph(BOARD_SIZE + INFO_SIZE, BOARD_SIZE, EX_DBLCLKS);	//创建画布，支持双击参数传入
	HWND hConsole = GetConsoleWindow();	//展示控制台用于调试
	ShowWindow(hConsole, SW_SHOW);

	//3、启用双缓冲批量绘制棋盘
	BeginBatchDraw();	//启动批量绘制 (采用双缓冲技术减少闪烁)
	Draw_Board();	//绘制棋盘
	Draw_Info();	//绘制信息区

	//4、游戏数据初始化
	int board[LINE_NUM][LINE_NUM] = { {EMPTY} };	//棋盘二维数组表示，并初始化
	int player = BLACK;	//标记下棋手
	int start = START;	//游戏状态标记
	game_remain_time = time(NULL);	//初始化总游戏时间开始
	Turn_Timer_Start();	//初始化计时器

	//=====================/二、游戏主循环阶段/=====================

	while (true) {

		//1、更新计时器和游戏状态
		Turn_Timer_Update();	//计算剩余时间
		Turn_Draw_Timer(player);	//绘制倒计时

		Draw_Gaming_Elements();	//绘制游戏界面元素
		Player_Point(BLACK + WHITE - player);	//显示玩家指向
		FlushBatchDraw();	//刷新显示

		//2、处理网络事件
		if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
			Network_Mode_Event(board, player);
		}

		//3、处理鼠标信息
		ExMessage msg;
		if (peekmessage(&msg, EX_MOUSE, false)) {	//检测是否有鼠标信息，有再进入有阻塞的鼠标信息提取//必须使用if，使用while会因为系统或硬件原因容易进入死循环

			//3.1、计算鼠标坐标与棋盘坐标对应关系
			getmessage(&msg, EX_MOUSE);

			Game_Music_Control(msg);	//音乐素材导入

			int boardX = (msg.x - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;	//计算二维数组棋盘对应位置
			int boardY = (msg.y - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;	//添加20用于四舍五入提高用户体验
			int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * boardX - OFFSET;	//计算图像棋盘对应位置
			int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * boardY - OFFSET;
			//printf("对应棋盘X=%d，Y=%d\n", boardX, boardY);	//棋盘对应位置测试代码


			//3.2、鼠标移动时绘制高亮提示下棋位置
			if ((boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {
				if (msg.message == WM_MOUSEMOVE && board[boardX][boardY] == EMPTY) {
					Clear_Highlight(board);	//清理原有高光
					Draw_Highlight(boardX, boardY, board);	//绘制高光
					FlushBatchDraw();	//刷新显示
				}
			}
			else {
				Clear_Highlight(board);	//若不符合条件也进行清理操作
				FlushBatchDraw();	//刷新显示
			}

			//3.3、执行下棋机制与绘制棋子操作（为防止误触双击下棋）
			if (msg.message == WM_LBUTTONDBLCLK && board[boardX][boardY] == 0 && (boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {

				//网络模式下，检查是否是我方下棋，不是则循环跳过
				if (NetworkMode != NETWORK_MODE_LOCAL && !isMyturn) {
					continue;
				}

				if (player == BLACK) {

					Clear_Highlight(board);	//清理高光
					Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);	//绘制黑棋
					board[boardX][boardY] = BLACK;

					//发送网络数据包
					if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
						//设定回合信息
						NetworkMessage turnMsg;
						turnMsg.msgtype = MSG_MOVE;	//消息类型为下棋
						turnMsg.x = boardX;
						turnMsg.y = boardY;
						turnMsg.player = BLACK;

						//发送数据包
						if (Send_Network_Message(ClientSocket, turnMsg)) {
							printf("发送黑棋落子：(%d , %d)\n", turnMsg.x, turnMsg.y);
						}

						isMyturn = false;		//发送完信息后，我端不下棋
					}

					if (Judge_Win(board, boardX, boardY, player) == BLACK) {
						choice = _getch();	//判断输赢
					}
					player = WHITE;	//棋方转换

					Turn_Timer_Start();	//初始化计时器
				}

				else if (player == WHITE) {

					Clear_Highlight(board);	//清理高光
					Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);	//绘制白棋
					board[boardX][boardY] = WHITE;

					//发送网络数据包
					if (NetworkMode != NETWORK_MODE_LOCAL && isConnected) {
						//设定回合信息
						NetworkMessage turnMsg;
						turnMsg.msgtype = MSG_MOVE;	//消息类型为下棋
						turnMsg.x = boardX;
						turnMsg.y = boardY;
						turnMsg.player = WHITE;

						//发送数据包
						if (Send_Network_Message(ClientSocket, turnMsg)) {
							printf("发送白棋落子：(%d , %d)\n", turnMsg.x, turnMsg.y);
						}

						isMyturn = false;		//发送完信息后，我端不下棋
					}

					if (Judge_Win(board, boardX, boardY, player) == WHITE) {
						choice = _getch();	//判断输赢
					}
					player = BLACK;	//棋方转换

					Turn_Timer_Start();	//初始化计时器
				}

				FlushBatchDraw();	//刷新显示
			}

			//3.4、立即更新计时器
			Turn_Timer_Update();	//计算剩余时间
			Turn_Draw_Timer(player);	//绘制倒计时
			FlushBatchDraw();	//刷新显示

		}

		//4、倒计时结束强制换边
		if (!game_time_running) {
			player = BLACK + WHITE - player;	//强制换边
			if (NetworkMode != NETWORK_MODE_LOCAL && isMyturn == true) {
				isMyturn = false;
			}
			else if (NetworkMode != NETWORK_MODE_LOCAL && isMyturn == false) {
				isMyturn = true;
			}
			Turn_Timer_Start();	//初始化计时器
			FlushBatchDraw();	//刷新显示
		}

		//5、刷新计时器显示
		FlushBatchDraw();	//刷新显示
		//Sleep(10);	//短暂休眠减少CPU占用
	}

	//=====================/三、游戏结束资源清理阶段/=====================

	//1、清理游戏资源
	EndBatchDraw();	//结束批量绘制
	closegraph();	//关闭画布

	//2、清理网络资源
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
