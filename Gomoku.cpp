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

//公共变量
int highlightX = -1, highlightY = -1;	//记录高光坐标

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

	//防止木质背景
	setorigin(0, 0);	//设置初始点
	putimage(0, 0, &img_game_background);	//放置游戏背景

	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
	setlinecolor(BLACK);
	setfillcolor(BLACK);

	//绘制竖线
	for (int i = 0; i < LINE_NUM; i++) {
		line(-BOARD_MARGIN + PIECE_SIZE * i, BOARD_MARGIN, -BOARD_MARGIN + PIECE_SIZE * i, -BOARD_MARGIN);
	}
	//绘制横线
	for (int i = 0; i < LINE_NUM; i++) {
		line(-BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i, BOARD_MARGIN, BOARD_MARGIN - PIECE_SIZE * i);
	}
	//绘制点
	solidcircle(0, 0, DOT_RADIUS);
	solidcircle(-DOT_POS, DOT_POS, DOT_RADIUS);
	solidcircle(DOT_POS, -DOT_POS, DOT_RADIUS);
	solidcircle(-DOT_POS, -DOT_POS, DOT_RADIUS);
	solidcircle(DOT_POS, DOT_POS, DOT_RADIUS);

}

//透明图片载入函数
void Put_Transparent_Image(int x, int y, const IMAGE* mask, const IMAGE* img) {

	putimage(x, y, mask, SRCAND);	//掩码图，黑色部分代表透明区域
	putimage(x, y, img, SRCPAINT);	//彩色图，保留掩码图黑色部分

}

//绘制高光函数
void Draw_Highlight(int x, int y, int Board[LINE_NUM][LINE_NUM]) {
	
	//记录高光位置（对应二维棋盘坐标）
	highlightX = x;
	highlightY = y;

	//保存原画笔设置
	COLORREF origin_line_color = getlinecolor();	//线条颜色
	LINESTYLE origin_line_style;
	getlinestyle(&origin_line_style);	//线条形态

	//设置现画笔图样（高光样式）
	setlinecolor(RED);	//红色
	setlinestyle(PS_DASH);	//虚线

	setorigin(0, 0);

	//图像棋盘对应位置（putimage需要图片左上角位置，circle需要圆心位置，因此不需要OFFSET）
	int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
	int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;

	//绘制高光
	circle(centerX, centerY, PIECE_SIZE / 2);

	//恢复原画笔
	setlinecolor(origin_line_color);
	setlinestyle(&origin_line_style);
}

//清理高光函数（局部重绘提高效率）
void Clear_Highlight(int Board[LINE_NUM][LINE_NUM]) {  

	if (highlightX == -1 || highlightY == -1) return;	//没有高光提早退出

	//保存原画笔设置
	COLORREF origin_line_color = getlinecolor();	//线条颜色
	LINESTYLE origin_line_style;
	getlinestyle(&origin_line_style);	//线条形态
	//现画笔设置
	setlinecolor(BLACK);
	setfillcolor(BLACK);

	//设置新坐标原点
	setorigin(0, 0);

	//获得高光中心坐标
	int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * highlightX;
	int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * highlightY;

	//获得背景重绘区域（比高光微大些）
	int left = centerX - PIECE_SIZE / 2;
	int top = centerY - PIECE_SIZE / 2;
	
	//重绘背景(后面两个left与top代表原图片的x,y坐标)
	putimage(left, top, PIECE_SIZE + 1, PIECE_SIZE + 1, &img_game_background, left, top);

	//绘制网格线
	// 横线
	int startX = left;
	int endX = left + PIECE_SIZE;
	if (highlightX == 0) startX = centerX;
	if (highlightX == LINE_NUM - 1) endX = centerX;
	line(startX, centerY, endX, centerY);

	// 竖线
	int startY = top;
	int endY = top + PIECE_SIZE;
	if (highlightY == 0) startY = centerY;
	if (highlightY == LINE_NUM - 1) endY = centerY;
	line(centerX, startY, centerX, endY);

	//重新绘制星位点
	//左上角星位点
	if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);
	//右上角星位点
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y - DOT_POS) solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y - DOT_POS, DOT_RADIUS);
	//左下角星位点
	else if (centerX == BOARD_ORIGIN_X - DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) solidcircle(BOARD_ORIGIN_X - DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);
	//右下角星位点
	else if (centerX == BOARD_ORIGIN_X + DOT_POS && centerY == BOARD_ORIGIN_Y + DOT_POS) solidcircle(BOARD_ORIGIN_X + DOT_POS, BOARD_ORIGIN_Y + DOT_POS, DOT_RADIUS);
	//中心星位点
	else if (centerX == BOARD_ORIGIN_X && centerY == BOARD_ORIGIN_Y) solidcircle(BOARD_ORIGIN_X, BOARD_ORIGIN_Y, DOT_RADIUS);

	//恢复原有坐标
	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);

	// 恢复画笔设置
	setlinecolor(origin_line_color);
	setlinestyle(&origin_line_style);

	//重置高光坐标
	highlightX = -1;
	highlightY = -1;
}

//玩家指向函数
void Player_Point(int player) {

	//设置坐标中心点
	setorigin(0, 0);

	//清理上次的文字
	putimage(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4, PIECE_SIZE, &img_game_background, BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS);

	//字母结构调整
	setbkmode(TRANSPARENT);	//文本背景透明
	settextcolor(BLACK);	//文字颜色黑色

	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置

	//字体属性调整
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//根据下棋方绘制文字
	if (player == WHITE) {
		printf_s("黑棋落子\n");
		outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("黑棋落子"));
	}
	else if (player == BLACK) {
		printf_s("白棋落子\n");
		outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("白棋落子"));
	}
}

//计时初始化

//判断游戏结束函数
//这里的X，Y代表的是现在下的棋的位置，因为其实只有现在下的棋改变了棋盘的状态，提高搜索效率
int Judge_Win(int Board[LINE_NUM][LINE_NUM], int x, int y, int player) {

	//重设坐标原点
	setorigin(0, 0);

	//字母结构调整
	setbkmode(TRANSPARENT);	//文本背景透明
	settextcolor(BLACK);	//文字颜色黑色

	LOGFONT fontStyle;	//创建字体结构体
	gettextstyle(&fontStyle);	//获取当前字体设置

	//字体属性调整
	fontStyle.lfQuality = ANTIALIASED_QUALITY;	//启用抗锯齿
	fontStyle.lfWeight = FONT_WEIGHT;	//设置字体粗细
	fontStyle.lfHeight = PIECE_SIZE;	//设置字体高度
	_tcscpy_s(fontStyle.lfFaceName, _T("微软雅黑"));

	settextstyle(&fontStyle);	//应用新字体设置

	//四个方向：右、上、右上、右下
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
	//实时位置
	int cur_x = x, cur_y = y;
	//提前退出
	if (player == EMPTY) return EMPTY;

	for (int i = 0; i < 4; i++) {
		int count = 1;
		//同一个方向和其反向累加
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
		//一个大方向上存在5个连续则输出胜方
		if (count >= 5) {

			//清理上次的文字
			putimage(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, PIECE_SIZE * 4, PIECE_SIZE, &img_game_background, BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS);

			if (player == BLACK) {
				printf_s("黑方胜利");
				outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("黑方胜利"));
				FlushBatchDraw();	//刷新显示
				//恢复坐标中心
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
				return BLACK;
			}
			else if (player == WHITE) {
				printf_s("白方胜利");
				outtextxy(BOARD_ORIGIN_X - 1.5 * PIECE_SIZE, FONT_POS, _T("白方胜利"));
				FlushBatchDraw();	//刷新显示
				//恢复坐标中心
				setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
				return WHITE;
			}
		}
	}

	//恢复坐标中心
	setorigin(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
	return EMPTY;
}

int main() {

	//导入游戏素材
	Texture_Load();
	Game_Music_Load();

	//绘制棋盘
	initgraph(BOARD_SIZE, BOARD_SIZE, EX_DBLCLKS);	//创建画布，支持双击参数传入

	//展示控制台用于调试
	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_SHOW);

	//采用双缓冲技术减少闪烁
	BeginBatchDraw();	//启动批量绘制

	Draw_Board();	//绘制棋盘

	//棋盘二维数组表示，并初始化
	int board[LINE_NUM][LINE_NUM] = { {EMPTY} };

	//游戏主体
	int player = BLACK;	//标记下棋手
	int start = START;	//游戏状态标记
	char choice;	//用于让界面暂停

	while (true) {

		//处理鼠标信息
		ExMessage msg;
		msg = getmessage(EX_MOUSE);

		//二维数组棋盘对应位置
		int boardX = (msg.x - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;	//添加20用于四舍五入提高用户体验
		int boardY = (msg.y - BOARD_MARGIN_REAL + PIECE_SIZE / 2) / PIECE_SIZE;
		//图像棋盘对应位置
		int centerX = BOARD_MARGIN_REAL + PIECE_SIZE * boardX - OFFSET;
		int centerY = BOARD_MARGIN_REAL + PIECE_SIZE * boardY - OFFSET;
		
		//printf("对应棋盘X=%d，Y=%d\n", boardX, boardY);	//棋盘对应位置测试代码

		//绘制高光区（跟随鼠标）
		if (msg.message == WM_MOUSEMOVE && board[boardX][boardY]==EMPTY) {
			if ((boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {
				//清理原有高光
				Clear_Highlight(board);
				//绘制高光
				Draw_Highlight(boardX, boardY, board);
				FlushBatchDraw();	//刷新显示
			}
			else {
				//若不符合条件也进行清理操作
				Clear_Highlight(board);
				FlushBatchDraw();	//刷新显示
			}
		}

		//游戏开始添加玩家指向
		if (start == START) {
			Player_Point(WHITE);
			FlushBatchDraw();	//刷新显示
			start = END;
		}

		//下棋机制与绘制棋子
		if (board[boardX][boardY] == 0 && (boardX >= 0 && boardX <= LINE_NUM - 1) && (boardY >= 0 && boardY <= LINE_NUM - 1)) {
			if (msg.message == WM_LBUTTONDBLCLK && player == BLACK) {
				//指向黑方
				Player_Point(player);
				//清理高光
				Clear_Highlight(board);
				//绘制黑棋
				Put_Transparent_Image(centerX, centerY, &img_black_opp, &img_black);
				board[boardX][boardY] = BLACK;
				//判断输赢
				if (Judge_Win(board, boardX, boardY, player) == BLACK)	choice = _getch();
				player = WHITE;	//棋方转换
				FlushBatchDraw();	//刷新显示
				continue;
			}
			if (msg.message == WM_LBUTTONDBLCLK && player == WHITE) {
				//指向白方
				Player_Point(player);
				//清理高光
				Clear_Highlight(board);
				//绘制白棋
				Put_Transparent_Image(centerX, centerY, &img_white_opp, &img_white);
				board[boardX][boardY] = WHITE;
				//判断输赢
				if (Judge_Win(board, boardX, boardY, player) == WHITE)	choice = _getch();
				player = BLACK;	//棋方转换
				FlushBatchDraw();	//刷新显示
				continue;
			}
		}

		//短暂休眠减少CPU占用
		Sleep(1);
	}

	//结束批量绘制
	EndBatchDraw();

	//关闭画布
	closegraph();
	return 0;
}

