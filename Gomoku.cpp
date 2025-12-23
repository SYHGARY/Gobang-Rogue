#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<easyx.h>
#include<conio.h>
#include<time.h>
#include<random>

#pragma comment(lib, "winmm.lib")	//连接Windows媒体库，用于播放音乐

//位置常量
#define LINE_NUM 15	//线条数
#define BOARD_SIZE 650	//棋盘大小
#define BOARD_MARGIN 280	//棋盘横线边缘
#define BOARD_MARGIN_REAL (BOARD_SIZE-BOARD_MARGIN*2)/2	//棋盘横线边缘真实距离  
#define OFFSET (BOARD_SIZE+PIECE_SIZE)/2	//鼠标坐标与绘图坐标偏移量
#define DOT_POS 160	//星位点位置
#define DOT_RADIUS 5	//星位点半径
#define BOARD_ORIGIN_X 325	//初始X坐标
#define BOARD_ORIGIN_Y 325	//初始Y坐标
#define PIECE_SIZE 40	//棋子大小 / 格子大小
#define FONT_POS (BOARD_MARGIN_REAL-PIECE_SIZE)/2 //字体位置

//棋盘状态常量
#define EMPTY 0
#define BLACK 1
#define WHITE 2
//游戏状态常量
#define START 1
#define END 0

//字体常量
#define FONT_WEIGHT 700	//字体粗细

//时间常量
#define TIME_LIMIT_TOTAL 600 //总时长限制10分钟
#define TIME_LIMIT 30 //单步限制30秒
#define TIME_WARN 10 //剩余10秒警告
#define FLASH_CYCLE 500	//闪烁周期为500个循环一闪

//公共变量
int highlightX = -1, highlightY = -1;	//记录高光坐标

time_t game_start_time;	//游戏开始时间
time_t turn_start_time;	//回合开始时间
int game_remain_time;	//游戏剩余时间
int turn_remain_time;	//回合剩余时间
bool game_time_running;	//计时器是否运行

//素材导入函数
IMAGE img_start_background, img_game_background, img_white, img_white_opp, img_black, img_black_opp;
void Texture_Load() {

	loadimage(&img_start_background, _T("./素材/background1.jpg"), BOARD_SIZE, BOARD_SIZE);	//游戏开始背景
	loadimage(&img_game_background, _T("./素材/wood.jpg"));	//游戏界面背景
	loadimage(&img_white, _T("./素材/white.png"), PIECE_SIZE, PIECE_SIZE);	//白棋彩色图
	loadimage(&img_white_opp, _T("./素材/white1.png"), PIECE_SIZE, PIECE_SIZE);	//白棋掩码图
	loadimage(&img_black, _T("./素材/black.png"), PIECE_SIZE, PIECE_SIZE);	//黑棋彩色图
	loadimage(&img_black_opp, _T("./素材/black1.png"), PIECE_SIZE, PIECE_SIZE);	//黑棋掩码图
}

//游戏音乐函数
void Game_Music_Load() {

	mciSendString(_T("open ./素材/music1.mp3"), NULL, 0, NULL);	//打开音乐
	mciSendString(_T("play ./素材/music1.mp3 repeat"), NULL, 0, NULL);	//重复播放音乐
}

//五子棋盘绘制函数
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

//透明图片载入函数
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img) {

	putimage(x, y, mask, SRCAND);	//掩码图，黑色部分代表透明区域
	putimage(x, y, img, SRCPAINT);	//彩色图，保留掩码图黑色部分
}

//绘制高光函数
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

//清理高光函数（局部重绘提高效率）
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

//玩家指向函数
void Player_Point(int player) {

	//1、清理上次的文字（字体高宽比约0.75）
	setorigin(0, 0);	//设置坐标中心点
	putimage(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4 * 0.75, PIECE_SIZE, &img_game_background, BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS);

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
		outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("黑棋落子"));
	}
	else if (player == BLACK) {
		//printf_s("白棋落子\n");
		outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("白棋落子"));
	}
}

//回合计时初始化函数
void Turn_Timer_Start() {

	//1、重置回合时间
	turn_start_time = time(NULL);	//记录回合开始时间
	turn_remain_time = TIME_LIMIT;	//重置回合剩余时间
	game_time_running = true;	//设置计时器状态为开启

	//2、清理上次计时区域
	setorigin(0, 0);	//设置坐标原点
	int round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	int round_time_sizey = PIECE_SIZE / 2;
	int timer_black_x = FONT_POS * 2 + PIECE_SIZE + 5;	//加5略微靠右
	int timer_black_y = FONT_POS + round_time_sizey;
	int timer_white_x = BOARD_SIZE - FONT_POS * 2 - PIECE_SIZE - round_time_sizex;
	int timer_white_y = FONT_POS + round_time_sizey;

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

	putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_game_background, timer_black_x, timer_black_y);	//清空黑方回合计时区
	outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋重置倒计时文字
	putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_game_background, timer_white_x, timer_white_y);	//清空白方回合计时区
	outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋重置倒计时文字
}

//回合剩余时间计算函数
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

//绘制倒计时函数
void Turn_Draw_Timer(int player) {

	//1、清理上次计时区域
	setorigin(0, 0);	//设置坐标原点
	int round_time_sizex = PIECE_SIZE * 8 * 0.75 / 2;	//回合计时区大小
	int round_time_sizey = PIECE_SIZE / 2;
	int timer_black_x = FONT_POS * 2 + PIECE_SIZE + 5;	//加5略微靠右
	int timer_black_y = FONT_POS + round_time_sizey;
	int timer_white_x = BOARD_SIZE - FONT_POS * 2 - PIECE_SIZE - round_time_sizex;
	int timer_white_y = FONT_POS + round_time_sizey;

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
		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_game_background, timer_black_x, timer_black_y);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, time_text);	//绘制黑棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_game_background, timer_white_x, timer_white_y);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, _T("回合剩余：30秒"));	//绘制白棋倒计时文字
	}
	else if (player == WHITE) {
		putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_game_background, timer_white_x, timer_white_y);	//清空白方回合计时区
		outtextxy(timer_white_x, timer_white_y, time_text);	//绘制白棋倒计时文字
		settextcolor(BLACK);	//保证静止区域为黑色
		putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_game_background, timer_black_x, timer_black_y);	//清空黑方回合计时区
		outtextxy(timer_black_x, timer_black_y, _T("回合剩余：30秒"));	//绘制黑棋倒计时文字
	}

	//4、添加倒计时5秒闪烁提示
	static int flash_count = 0;	//闪烁节拍器，使用static保证函数调用后数值不变
	if (turn_remain_time <= 5 && turn_remain_time > 0) {
		flash_count++;
		if (flash_count % FLASH_CYCLE < FLASH_CYCLE / 2) {
			if(player==BLACK)	putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_game_background, timer_black_x, timer_black_y);	//清空黑方回合计时区
			else if(player==WHITE)	putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_game_background, timer_white_x, timer_white_y);	//清空白方回合计时区
		}
		else {
			settextcolor(RED);
			if (player == BLACK) {
				putimage(timer_black_x, timer_black_y, round_time_sizex, round_time_sizey, &img_game_background, timer_black_x, timer_black_y);	//清空黑方回合计时区
				outtextxy(timer_black_x, timer_black_y, time_text);	//绘制黑棋倒计时文字
			}
			else if (player == WHITE) {
				putimage(timer_white_x, timer_white_y, round_time_sizex, round_time_sizey, &img_game_background, timer_white_x, timer_white_y);	//清空白方回合计时区
				outtextxy(timer_white_x, timer_white_y, time_text);	//绘制白棋倒计时文字
			}
		}
	}
	else {
		flash_count = 0;
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点
}

//绘制棋盘其他元素
void Draw_Gaming_Elements() {


	//1、计时区设置
	setorigin(0, 0);	//设置坐标原点

	int black_piece_x = 2 * FONT_POS;	//黑棋X坐标
	int black_piece_y = FONT_POS;	//黑棋Y坐标
	int white_piece_x = BOARD_SIZE - 2 * FONT_POS - PIECE_SIZE;	//白棋X坐标
	int white_piece_y = FONT_POS;	//白棋y坐标
	Put_Transparent_Image(black_piece_x, black_piece_y, &img_black_opp, &img_black);	//黑棋计时区
	Put_Transparent_Image(white_piece_x, white_piece_y, &img_white_opp, &img_white);	//白棋计时区

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标原点

}

//判断游戏结束函数（这里的X，Y代表的是现在下的棋的位置，因为其实只有现在下的棋改变了棋盘的状态，提高搜索效率）
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
			putimage(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4, PIECE_SIZE, &img_game_background, BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS);	//清理上次的文字（字体高宽比约0.75）

			if (player == BLACK) {
				printf_s("黑方胜利");
				outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
				FlushBatchDraw();	//刷新显示
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y); //恢复坐标中心
				return BLACK;
			}
			else if (player == WHITE) {
				printf_s("白方胜利");
				outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
				FlushBatchDraw();	//刷新显示
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
				return WHITE;
			}
		}
	}

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);	//恢复坐标中心
	return EMPTY;
}

//主函数
int main() {

	//=====================/一、游戏初始化阶段/=====================

	//1、导入游戏素材
	Texture_Load();	//图片素材导入
	//Game_Music_Load();	//音乐素材导入

	//2、初始化图形界面
	initgraph(BOARD_SIZE, BOARD_SIZE, EX_DBLCLKS);	//创建画布，支持双击参数传入
	HWND hConsole = GetConsoleWindow();	//展示控制台用于调试
	ShowWindow(hConsole, SW_SHOW);

	//3、启用双缓冲批量绘制棋盘
	BeginBatchDraw();	//启动批量绘制 (采用双缓冲技术减少闪烁)
	Draw_Board();	//绘制棋盘

	//4、游戏数据初始化
	int board[LINE_NUM][LINE_NUM] = { {EMPTY} };	//棋盘二维数组表示，并初始化
	int player = BLACK;	//标记下棋手
	int start = START;	//游戏状态标记
	char choice;	//用于让界面暂停
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

		//2、处理鼠标信息
		ExMessage msg;
		if (peekmessage(&msg, EX_MOUSE, false)) {	//检测是否有鼠标信息，有再进入有阻塞的鼠标信息提取//必须使用if，使用while会因为系统或硬件原因容易进入死循环

			//2.1、计算鼠标坐标与棋盘坐标对应关系
			getmessage(&msg, EX_MOUSE);
			int boardX = (msg.x - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;	//计算二维数组棋盘对应位置
			int boardY = (msg.y - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;	//添加20用于四舍五入提高用户体验
			int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * boardX - OFFSET;	//计算图像棋盘对应位置
			int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * boardY - OFFSET;
			//printf("对应棋盘X=%d，Y=%d\n", boardX, boardY);	//棋盘对应位置测试代码


			//2.2、鼠标移动时绘制高亮提示下棋位置
			if (msg.message == WM_MOUSEMOVE && board[boardX][boardY] == EMPTY) {
				if ((boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {
					Clear_Highlight(board);	//清理原有高光
					Draw_Highlight(boardX, boardY, board);	//绘制高光
					FlushBatchDraw();	//刷新显示
				}
				else {
					Clear_Highlight(board);	//若不符合条件也进行清理操作
					FlushBatchDraw();	//刷新显示
				}
			}

			//2.3、执行下棋机制与绘制棋子操作（为防止误触双击下棋）
			if (board[boardX][boardY] == 0 && (boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {
				if (msg.message == WM_LBUTTONDBLCLK && player == BLACK) {

					Clear_Highlight(board);	//清理高光
					Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);	//绘制黑棋
					board[boardX][boardY] = BLACK;

					if (Judge_Win(board, boardX, boardY, player) == BLACK)	choice = _getch();	//判断输赢
					player = WHITE;	//棋方转换

					Turn_Timer_Start();	//初始化计时器
					FlushBatchDraw();	//刷新显示
					continue;
				}

				if (msg.message == WM_LBUTTONDBLCLK && player == WHITE) {

					Clear_Highlight(board);	//清理高光
					Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);	//绘制白棋
					board[boardX][boardY] = WHITE;

					if (Judge_Win(board, boardX, boardY, player) == WHITE)	choice = _getch();	//判断输赢
					player = BLACK;	//棋方转换

					Turn_Timer_Start();	//初始化计时器
					FlushBatchDraw();	//刷新显示
					continue;
				}
			}

			//2.4、立即更新计时器
			Turn_Timer_Update();	//计算剩余时间
			Turn_Draw_Timer(player);	//绘制倒计时
			FlushBatchDraw();	//刷新显示

		}

		//3、倒计时结束强制换边
		if (!game_time_running) {
			player = BLACK + WHITE - player;	//强制换边
			Turn_Timer_Start();	//初始化计时器
			FlushBatchDraw();	//刷新显示
			continue;
		}

		//4、刷新计时器显示
		FlushBatchDraw();	//刷新显示
		//Sleep(10);	//短暂休眠减少CPU占用
	}

	//=====================/三、游戏结束资源清理阶段/=====================

	//1、结束游戏
	EndBatchDraw();	//结束批量绘制
	closegraph();	//关闭画布
	return 0;
}

