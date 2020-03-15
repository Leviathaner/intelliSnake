/**
*┌──────────────────────────────────────────────────────────────┐
*│　Description：A simple game                                                    
*│　Author：Leviathan                                              
*│　Version：1.3                                                
*│　Date：2017-12-21                        
*└──────────────────────────────────────────────────────────────┘
*/



#pragma warning(disable:4996)
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include <conio.h>

/*保存地图数据*/
#define x_length   100
#define y_length   30

/*保存按键ansi码*/
#define UP  72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define ESC 27
#define SPACE 32

/*用于记录自动移动的方向*/
#define ups  1
#define downs 2
#define rights 3
#define lefts 4

/*蛇节点的结构体*/
typedef struct SNAKE
{
	int x, y;
	struct SNAKE* next;
}snake;

/*读取文件排行榜的数据缓冲区*/
struct MEMBER
{
	char aname[20];
	int  grades;
}member[100];

/*存档时蛇的信息*/
typedef struct SSNAKE
{
	int x;
	int y;
}saveSnake;


/*全局变量*/

int Count = 1;//统计步数，每20步产生一个毒草
int Length = 2;//统计蛇的长度；
int Levelup = 1;
int PoisonCount = 3;//毒草个数
double SleepTime = 500;
int Score = 0;//统计得分；
int SingleAddScore = 10;//单个食物得分
int EndgameStatus = 0;//游戏结束的情况：1撞到墙2咬到自己3被毒草毒死4主动退出游戏
char IsNew = 0;//记录玩家选择，n表示新游戏,r表示读取存档；
int Items[x_length][y_length] = { 0 };//1记录食物2毒草坐标3炸弹坐标4墙坐标



snake* Head;//指向蛇头的指针
snake* Q;//遍历蛇的指针
char Name[20];//读取名字
char Str[60];
int Direction;//小蛇移动方向



int SaveLevelup;//记录关卡
int SaveDirection;//记录小蛇移动方向的数字
double SaveSleepTime;//控制运动速度
int SaveScore;
int SaveSingleAddScore;
char SaveName[20];
int SaveLength;//记录小蛇的长度
saveSnake SaveSnakePosition[100];//存储蛇的位置



void Pos(int, int);//移动到指定坐标
void HideCursor();//隐藏光标函数



void init();//初始化游戏
void startWindow();//游戏开始界面
void createHelpInfo();//游戏开始时的帮助
void createGameGelpInfo();
void createGame(int, int, int, int);
void createMap(int );//根据关卡产生地图包括墙和帮助
void createFirstWall();
void createSecondWall();
void createThirdWall();//产生墙
void createFood();//产生食物
void createBomb();//产生炸弹，踩到炸掉一半
void createSnake(int, int);//创建小蛇

void createPoison();//产生毒草
void deletePoison();//删除毒草
void updatePoison(int );//更新毒草

void updateSnake();//遍历蛇身左边是否与各个物品重叠，更新小蛇和游戏状态
void endGame();//结束游戏
void snakeMove(int );//小蛇的移动和检查
void upMove();
void downMove();
void rightMove();
void leftMove();
void autoMove();//判断小蛇无按键时自动移动的方向
int isBiteSelf(snake* s);//判断是否咬到了自己
void decreaseSnakeBody();//尾减一节
void increaseSnakeBody();//头加一节
int hasFood();
void changeColor(const int );//控制字体的颜色
int isOverlay(int, int);//检查食物毒草是否与蛇重叠
void inputName();//输入用户名
void selectHTStart();//选择新游戏或读取存档
void showGrade();//显示排行榜
void saveGame();//存档
int hasBomb();//检查地图上还有无炸弹
void checkKey();//检查小蛇运动时的按键
void saveGrade();//保存成绩
void choseDif();//选择难度
int passLevel(int);
void poisonTwinkle(int);


int main()
{
	system("mode con cols=150 lines=40");
	init();//初始化欢迎界面
	if (Levelup == 1) {
		createGame(1, 4, 2, 500);
		Levelup = passLevel(1);
	}
	if (Levelup == 2) {
		createGame(2, 5, 2, 400);
		Levelup = passLevel(2);
	}
	if (Levelup == 3) {
		createGame(3, 6, 2, 300);
	}
	return 0;
}

void init()
{
	FILE* fp;
	startWindow();
	selectHTStart();
	if (IsNew == 'n') {
		inputName();
		choseDif();
		system("pause");
		system("cls");
	}
	else if (IsNew == 'r') {
		fp = fopen("关卡状态.txt", "r+");
		fread(&SaveLevelup, sizeof(int), 1, fp);
		fread(&SaveScore, sizeof(int), 1, fp);
		fread(&SaveSingleAddScore, sizeof(int), 1, fp);
		fread(SaveName, sizeof(Name), 1, fp);
		fclose(fp);
		Levelup = SaveLevelup;
		Score = SaveScore;
		SingleAddScore = SaveSingleAddScore;
		strcpy(Name, SaveName);
	}
	createHelpInfo();
}

void createGame(int levelup, int poisonCount, int snakeLength, int sleepTime) {
	PoisonCount = poisonCount;
	int x, y;
	for (x = 0; x < x_length; x++)
		for (y = 0; y < y_length; y++)
			Items[x][y] = 0;
	Length = snakeLength;
	SleepTime = sleepTime;
	createMap(levelup);
	switch (levelup)
	{
	case 1:
	case 2:createSnake(12, 12); snakeMove(1); break;
	case 3:createSnake(36, 12); snakeMove(0); break;
	default: break;
	}
}

int passLevel(int levelup) {
	system("cls");
	Pos(60, 13);
	printf("恭喜您通过第%d关，即将进入下一关", levelup);
	system("pause");
	system("cls");
	levelup += 1;
	return levelup;
}

void createMap(int levelup)//产生地图
{
	switch (levelup)
	{
	case 1:createFirstWall(); break;
	case 2:createSecondWall(); break;
	case 3:createThirdWall(); break;
	default: break;
	}
	createGameGelpInfo();
}

void createSnake(int xPosition, int yPosition)
{
	FILE* fp;
	if (IsNew == 'n')
	{
		snake* tail;
		Head = (snake*)malloc(sizeof(snake));//从蛇头开始，尾插法，以x,y设定开始的位置
		if (Head != NULL) {
			Head->x = xPosition * 2;
			Head->y = yPosition;
			Head->next = NULL;
			Q = Head;
		}
		tail = (snake*)malloc(sizeof(snake));
		if (tail != NULL) {
			tail->x = Head->x - 2;
			tail->y = Head->y;
			Q->next = tail;
			tail->next = NULL;
		}
		Q = Head;
	}
	else if (IsNew == 'r')
	{
		Q = NULL;
		snake* ptemp = NULL;
		fp = fopen("蛇体情况.txt", "r+");
		fread(&SaveLength, sizeof(int), 1, fp);
		Head = (snake*)malloc(sizeof(snake));
		fread(SaveSnakePosition, sizeof(SaveSnakePosition), 1, fp);
		Length = SaveLength;
		Head->x = SaveSnakePosition[0].x;
		Head->y = SaveSnakePosition[0].y;
		Q = (snake*)malloc(sizeof(snake));
		Head->next = Q;
		for (int i = 1; i < SaveLength; i++)
		{
			if (i != 1)
				Q = Q->next;
			Q->x = SaveSnakePosition[i].x;
			Q->y = SaveSnakePosition[i].y;
			ptemp = (snake*)malloc(sizeof(snake));
			ptemp->next = NULL;
			Q->next = ptemp;
		}
		Q->next = NULL;
		fclose(fp);
		free(ptemp);
		Q = Head;
	}
	while (Q != NULL)//从头到为，输出蛇身
	{
		Pos(Q->x, Q->y);
		printf("■");
		Q = Q->next;
	}
}

void snakeMove(int isPass)
{
	if (IsNew == 'n') {
		Direction = rights;
	}
	else if (IsNew == 'r') {
		FILE* fp;
		fp = fopen("运动状态.txt", "r+");
		fread(&SaveDirection, sizeof(int), 1, fp);
		fread(&SaveSleepTime, sizeof(double), 1, fp);
		Direction = SaveDirection;
		SleepTime = SaveSleepTime;
		fclose(fp);
	}
	IsNew = 'n';
	while (1)
	{
		Q = Head;
		Head = (snake*)malloc(sizeof(snake));
		Head->next = Q;
		checkKey();
		autoMove();
		updateSnake();
		Count++;
		updatePoison(Count);
		if (!hasFood()) createFood();
		if (!hasBomb()) createBomb();
		Pos(110, 11);
		printf("您当前得分为%d", Score);
		Sleep(SleepTime);
		poisonTwinkle(Count);
		if ((Length > 5) && (isPass == 1) )
			break;
	}
}

void checkKey()
{
	while (_kbhit())
	{
		int c;
		c = _getch();
		if (c == UP && Direction != downs)
			Direction = ups;
		else if (c == DOWN && Direction != ups)
			Direction = downs;
		else if (c == RIGHT && Direction != lefts)
			Direction = rights;
		else if (c == LEFT && Direction != rights)
			Direction = lefts;
		else if (c == ESC)
		{
			EndgameStatus = 4;
			endGame();
		}
		else if (c == SPACE)
		{
			while (1)
			{
				Sleep(500);
				if (_getch() == SPACE)
					break;
			}
		}
	}
}

void autoMove()
{
	if (Direction == ups)
		upMove();
	else if (Direction == downs)
		downMove();
	else if (Direction == lefts)
		leftMove();
	else if (Direction == rights)
		rightMove();
}

void updateSnake()
{
	int i, j;
	Q = Head;
	i = Q->x;
	j = Q->y;
	if (Items[i][j] == 0)
	{
		decreaseSnakeBody();
		increaseSnakeBody();
	}
	else if (Items[i][j] == 1)
	{
		increaseSnakeBody();
		Items[i][j] = 0;
		//createFood();
		SleepTime *= 0.8;
		Score += SingleAddScore;
		SingleAddScore += 5;
		Length++;
	}
	else if (Items[i][j] == 2 && Q->next != NULL)
	{
		decreaseSnakeBody();
		decreaseSnakeBody();
		increaseSnakeBody();
		Items[i][j] = 0;
		Score -= 5;
		SleepTime *= 1.2;
		Length--;
	}
	else if (Items[i][j] == 3)
	{
		int ll = Length;
		while (ll > (Length / 2))
		{
			decreaseSnakeBody();
			ll--;
			SleepTime *= 1.2;
		}
		decreaseSnakeBody();
		increaseSnakeBody();
		Items[i][j] = 0;
		//createBomb();
		Length = ll;
	}
	if (Q->next == NULL)
	{
		EndgameStatus = 3;
		endGame();
	}
	if (Items[i][j] == 4)
	{
		EndgameStatus = 1;
		endGame();
	}
	else if (isBiteSelf(Head))
	{
		EndgameStatus = 2;
		endGame();
	}
}

void updatePoison(int count) {
	if (count % 20 == 0)
	{
		deletePoison();
		createPoison();
	}
}

void Pos(int x, int y)//将光标移动到指定位置
{
	COORD pos;
	HANDLE hOutput;
	pos.X = x;
	pos.Y = y;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
	HideCursor();
}
void HideCursor()//隐藏光标函数
{
	CONSOLE_CURSOR_INFO cursor_info = { 1,0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void createFirstWall()//产生墙
{
	int x, y;//x,y为横纵坐标
	for (y = 0; y < y_length; y++)
	{
		for (x = 0; x < x_length; x++)
		{
			if (y == 0 || y == y_length - 1 || x == 0 || x == x_length - 2)
			{
				printf("#");
				Items[x][y] = 4;
			}
			else printf(" ");
		}
		printf("\n");
	}
}
void createSecondWall()
{
	int x, y;//x,y为横纵坐标
	for (y = 0; y < y_length; y++)
	{
		for (x = 0; x < x_length; x++)
		{
			if (y == 0 || y == y_length - 1 || x == 0 || x == x_length - 2)
			{
				printf("#");
				Items[x][y] = 4;
			}
			else if (y < (y_length / 2) && x < (x_length - 2) && x % 20 == 0)
			{
				printf("#");
				Items[x][y] = 4;
			}
			else if (y > (y_length / 2) && x < (x_length - 2) && x % 14 == 0)
			{
				printf("#");
				Items[x][y] = 4;
			}
			else printf(" ");
		}
		printf("\n");
	}

}
void createThirdWall()
{
	int x, y;//x,y为横纵坐标
	for (y = 0; y < y_length; y++)
	{
		for (x = 0; x < x_length; x++)
		{
			if ((y < y_length / 2 && x == x_length / 2 - 10) || (y == 0 && x > x_length / 2 - 10 && x < x_length - 2) || (x == x_length - 2 && y < y_length / 2))
			{
				printf("#");
				Items[x][y] = 4;
			}
			else if (y > y_length / 2 && x == x_length / 2 + 10 || y == y_length - 1 && x<x_length / 2 + 10 || x == 0 && y>y_length / 2)
			{
				printf("#");
				Items[x][y] = 4;
			}
			else if (y == y_length / 2 && x <= x_length / 2 - 10 || y == y_length / 2 && x >= x_length / 2 + 10 && x <= x_length - 2)
			{
				{printf("#");
				Items[x][y] = 4; }
			}
			else printf(" ");
		}
		printf("\n");
	}
	for (x = 0; x <= x_length / 2 - 10; x++)
		for (y = 0; y < y_length / 2; y++)
			Items[x][y] = 4;
	for (x = x_length / 2 + 10; x <= x_length - 2; x++)
		for (y = y_length / 2; y <= y_length - 1; y++)
			Items[x][y] = 4;
}
void createGameGelpInfo()
{
	Pos(110, 12);
	printf("小蛇长度越长，速度越快");
	Pos(110, 13);
	printf("▲代表毒草；★代表食物,●代表炸弹");
	Pos(110, 14);
	printf("不能穿墙，不能咬到自己");
	Pos(110, 15);
	printf("踩到毒草减一节");
	Pos(110, 16);
	printf("吃到食物加一节");
	Pos(110, 17);
	printf("踩到炸弹减一半");
	Pos(110, 18);
	printf("用↑.↓.←.→分别控制蛇的移动.");
	Pos(110, 19);
	printf("ESC ：退出游戏，space：暂停游戏.");
}

void createFood()
{
	int x, y;//i记录个数，x记录列，y记录行
	do {
		srand((unsigned int)time(NULL) * 100);
		x = (rand() % (x_length / 2 - 2)) * 2 + 2;
		y = rand() % (y_length - 3) + 1;
	} while (isOverlay(x, y));
	Items[x][y] = 1;
	Pos(x, y);
	changeColor(12);
	printf("★");
	changeColor(7);

}
void createPoison()
{
	int i, x, y;//i记录个数，x记录行，y记录列
	srand((unsigned int)time(NULL));
	for (i = 0; i < PoisonCount;)
	{
		do {
			x = (rand() % (x_length / 2 - 2)) * 2 + 2;
			y = rand() % (y_length - 3) + 1;
		} while (isOverlay(x, y));
		Items[x][y] = 2;
		i++;
	}
	for (x = 0; x < x_length; x++)
		for (y = 0; y < y_length; y++)
		{
			if (Items[x][y] == 2)
			{
				Pos(x, y);
				changeColor(2);
				printf("▲");
				changeColor(7);
			}
		}
}
void deletePoison()
{
	int x, y;
	for (x = 0; x < x_length; x++)
		for (y = 0; y < y_length; y++)
		{
			if (Items[x][y] == 2)
			{
				Pos(x, y);
				printf("  ");
				Items[x][y] = 0;
			}
		}
}

void startWindow()
{
	system("title 聪明的小蛇");
	Pos(50, 15);
	printf("欢迎来到贪吃蛇游戏");
	system("pause");
	system("cls");
}
void createHelpInfo()
{
	Pos(65, 15);
	printf("用↑.↓.←.→分别控制蛇的移动\n");
	Pos(65, 16);
	printf("蛇身越长，速度就越快。\n");
	system("pause");
	system("cls");
}

void upMove()
{
	Head->x = Q->x;
	Head->y = Q->y - 1;
	Direction = ups;
}
void downMove()
{
	Head->x = Q->x;
	Head->y = Q->y + 1;
	Direction = downs;
}
void leftMove()
{
	Head->x = Q->x - 2;
	Head->y = Q->y;
	Direction = lefts;
}
void rightMove()
{
	Head->x = Q->x + 2;
	Head->y = Q->y;
	Direction = rights;
}

int isBiteSelf(snake* s)
{
	Q = s->next;
	while (Q != NULL)
	{
		if (Q->x == s->x && Q->y == s->y)
			return 1;
		else Q = Q->next;
	}
	return 0;
}
void decreaseSnakeBody()
{
	Q = Head;
	while (Q->next->next != NULL)
		Q = Q->next;
	Pos(Q->next->x, Q->next->y);
	printf("  ");
	free(Q->next);
	Q->next = NULL;

}
void increaseSnakeBody()
{
	Q = Head;
	while (Q != NULL)
	{
		Pos(Q->x, Q->y);
		changeColor(9);
		printf("■");
		changeColor(7);
		Q = Q->next;
	}
	Q = Head;
}

void endGame()
{
	system("cls");
	Pos(70, 12);
	if (EndgameStatus == 1)
	{
		printf("对不起，您撞到墙了。游戏结束.  ");
		saveGrade();
		showGrade();
	}
	else if (EndgameStatus == 2)
	{
		printf("对不起，您咬到自己了。游戏结束.  ");
		saveGrade();
		showGrade();
	}
	else if (EndgameStatus == 3)
	{
		printf("对不起，您已死亡。游戏结束  ");
		saveGrade();
		showGrade();
	}
	else if (EndgameStatus == 4)
	{
		printf("您已主动结束游戏  游戏已存档");
		saveGame();
	}
	system("pause");
	system("cls");
	exit(0);
}
int hasFood()
{
	int x, y;
	for (x = 0; x < x_length; x++)
		for (y = 0; y < y_length; y++)
		{
			if (Items[x][y] == 1)
				return 1;
		}
	return 0;
}
void changeColor(const int color)//控制字体的颜色
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int  isOverlay(int a, int b)
{
	Q = Head;
	while (Q != NULL)
	{
		if (Q->x == a && Q->y == b)
			return 1;
		Q = Q->next;
	}
	if (Items[a][b] != 0)
		return 1;
	Q = Head;
	return 0;
}
void inputName()
{
	int i;
	Pos(55, 14);
	printf("请输入您的用户名（20个字符以内");
	Pos(65, 16);
	fgets(Name, 20, stdin);
	for (i = 0; i < 19; i++)
	{
		if (Name[i] == '\n')
			Name[i] = '\0';
	}
	system("pause");
	system("cls");
}
void showGrade()
{
	FILE* pf;
	if ((pf = fopen("排行榜.txt", "rb+")) == NULL)
	{
		printf("Can't open the text.");
		exit(0);
	}
	char ch;
	int i = 0;
	ch = getc(pf);
	while (!feof(pf))
	{
		if (ch == ':')
			i++;
		ch = getc(pf);
	}
	rewind(pf);
	int k, j;
	for (k = 0; k < i; k++)
		fscanf(pf, "%s%d", &member[k].aname, &member[k].grades);
	struct MEMBER temp;
	for (j = 0; j < i - 1; j++)
		for (k = 0; k < i - 1 - j; k++)
		{
			if (member[k].grades < member[k + 1].grades)
			{
				temp = member[k];
				member[k] = member[k + 1];
				member[k + 1] = temp;
			}
		}
	changeColor(10);
	Pos(2, 1);
	printf("排行榜\n");
	printf("姓名  成绩\n");
	int m;
	for (m = 0; m < i; m++)
		printf("%s   %d\n", member[m].aname, member[m].grades);
}
void selectHTStart()
{
	Pos(60, 14);
	printf("继续上次游戏请按r键\n");
	Pos(60, 15);
	printf("开始新游戏请按n键\n");
	IsNew = _getch();
	while (IsNew != 'n' && IsNew != 'r')
		IsNew = _getch();
	system("pause");
	system("cls");
}
void saveGame()
{
	FILE* fpSnakeStatus;
	FILE* fpMoveStatus;
	FILE* fpEnvStatus;
	Q = Head->next;
	SaveLength = 0;
	while (Q != NULL)
	{
		SaveSnakePosition[SaveLength].x = Q->x;
		SaveSnakePosition[SaveLength].y = Q->y;
		Q = Q->next;
		SaveLength++;
	}
	SaveSingleAddScore = SingleAddScore;
	SaveScore = Score;
	SaveDirection = Direction;
	SaveSleepTime = SleepTime;
	SaveLevelup = Levelup;
	strcpy(SaveName, Name);
	if ((fpSnakeStatus = fopen("蛇体情况.txt", "w+")) == NULL 
		|| (fpMoveStatus = fopen("运动状态.txt", "w+")) == NULL 
		|| (fpEnvStatus = fopen("关卡状态.txt", "w+")) == NULL)
	{
		printf("Can't open the storefiles");
		exit(0);
	}

	fwrite(&SaveLength, sizeof(int), 1, fpSnakeStatus);
	fwrite(SaveSnakePosition, sizeof(SaveSnakePosition), 1, fpSnakeStatus);
	fclose(fpSnakeStatus);

	fwrite(&SaveDirection, sizeof(int), 1, fpMoveStatus);
	fwrite(&SleepTime, sizeof(double), 1, fpMoveStatus);
	fclose(fpMoveStatus);


	fwrite(&SaveLevelup, sizeof(int), 1, fpEnvStatus);
	fwrite(&SaveScore, sizeof(int), 1, fpEnvStatus);
	fwrite(&SaveSingleAddScore, sizeof(int), 1, fpEnvStatus);
	fwrite(SaveName, sizeof(Name), 1, fpEnvStatus);
	fclose(fpEnvStatus);
}
void createBomb()
{
	int x, y;//i记录个数，x记录列，y记录行
	do {
		srand((unsigned int)time(NULL) * 10000);
		x = (rand() % (x_length / 2 - 2)) * 2 + 2;
		y = rand() % (y_length - 3) + 1;
	} while (isOverlay(x, y));
	Items[x][y] = 3;
	Pos(x, y);
	changeColor(12);
	printf("●");
	changeColor(7);
}
int hasBomb()
{
	int x, y;
	for (x = 0; x < x_length; x++)
		for (y = 0; y < y_length; y++)
		{
			if (Items[x][y] == 3)
				return 1;
		}
	return 0;
}
void saveGrade()
{
	FILE* fp;
	sprintf(Str, "%s%s%d%c", Name, ": ", Score, ' ');
	if ((fp = fopen("排行榜.txt", "ab+")) == NULL)
	{
		printf("Can't open the text");
		exit(0);
	}
	fputs(Str, fp);
	fclose(fp);
}
void choseDif()
{
	Pos(50, 13);
	printf("请选择关卡");
	Pos(55, 14);
	printf("1.沙漠模式");
	Pos(55, 15);
	printf("2.荒野模式");
	Pos(55, 16);
	printf("3.无限模式\n");
	char c = _getch();
	while (c != '1' && c != '2' && c != '3')
		c = _getch();
	if (c == '1') Levelup = 1;
	else if (c == '2') Levelup = 2;
	else if (c == '3') Levelup = 3;
}
void poisonTwinkle(int count) {
	int x, y;
	for (x = 0; x <= x_length - 1; x++)
		for (y = 0; y <= y_length - 1; y++){
			if (Items[x][y] == 2){
				if (count % 2 == 0){
					Pos(x, y);
					changeColor(2);
					printf("▲");
					changeColor(7);
				}else {
					Pos(x, y);
					printf("  ");
				}
			}
		}

}
