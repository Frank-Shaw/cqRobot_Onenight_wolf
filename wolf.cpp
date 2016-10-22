#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
//number of people from 3 to 12
//meaning of number is "wolf", "lackeys", "watchman", "seer", "robber", "troublemaker", "insomniac", "villager", "hunter", "cobbler"

const int character[13][10] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 2, 0, 0, 1, 1, 1, 0, 1, 0, 0 },
	{ 2, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
	{ 2, 0, 0, 1, 1, 1, 1, 1, 0, 1 },
	{ 2, 0, 0, 1, 1, 1, 1, 2, 0, 1 },
	{ 2, 1, 0, 1, 1, 1, 1, 2, 0, 1 },
	{ 2, 1, 0, 1, 1, 1, 1, 2, 1, 1 },
	{ 2, 1, 2, 1, 1, 1, 1, 1, 1, 1 },
	{ 2, 1, 2, 1, 1, 1, 1, 2, 1, 1 },
	{ 2, 1, 2, 1, 1, 1, 1, 3, 1, 1 },
	{ 2, 1, 2, 1, 1, 1, 1, 4, 1, 1 },
};

//use number to express character
//1 werewolf
//2 lackeys
//3 watchman
//4 seer
//5 robber
//6 troublemaker
//7 insomniac
//8 villager
//9 hunter
//10 cobbler
int player[13] = {0};

char *inf[11] = { "", "狼人", "爪牙", "守夜人", "预言家", "强盗", "捣蛋鬼", "失眠者", "村民", "猎人", "皮匠" };




void welcome_word()
{
	printf("欢迎开启一夜狼人游戏，我是这次游戏的上帝，小小萌神，请多指教。（感谢我的爸爸FrankShaw让我当上帝）\n请输入“#加入一夜狼人”,来参加这次一夜狼人游戏w\n");

}

int init()
{
	printf("Input the amount of people (3 - 12): ");
	int num = -1;
	scanf("%d", &num);
	return num;
}


void sendmessage(int ac, int64_t from, char* msg)
{
		printf("%s", msg);
}



int initgame(int number)
{
	int wolf = character[number[0];
	int lackeys = character[number][1];
	int watchman = character[number][2];
	int seer = character[number][3];
	int robber = character[number][4];
	int troublemaker = character[number][5];
	int insomniac = character[number][6];
	int villager = character[number][7];
	int hunter = character[number][8];
	int cobbler = character[number][9];


	char *msg;
	msg = (char *)malloc(sizeof(char)* 400);
	memset(msg, '\0', 400);

	char buf[30];

	strcpy(msg, "游戏开始！\n");

	sprintf(buf, "本轮游戏一共有%d个人", number);
	strcat(msg, buf);

	strcat(msg, "角色分配如下：\n");

	sprintf(buf, "狼人一共有%d个\n", wolf);
	strcat(msg, buf);

	if (lackeys)
	{
		sprintf(buf, "%d名爪牙\n", lackeys);
		strcat(msg, buf);
	}


	if (守夜人)
	{
		sprintf(buf, "%d名守夜人\n", watchman);
		strcat(msg, buf);
	}


	sprintf(buf, "%d名预言家\n", seer);
	strcat(msg, buf);

	sprintf(buf, "%d名强盗\n", robber);
	strcat(msg, buf);

	sprintf(buf, "%d名捣蛋鬼\n", troublemaker);
	strcat(msg, buf);

	if (insomniac)
	{
		sprintf(buf, "%d名失眠者\n", insomniac);
		strcat(msg, buf);
	}

	sprintf(buf, "%d名村民\n", villager);
	strcat(msg, buf);

	if (cobbler)
	{
		sprintf(buf, "%d名皮匠\n", cobbler);
		strcat(msg, buf);
	}

	if (hunter)
	{
		sprintf(buf, "%d名猎人\n", hunter);
		strcat(msg, buf);
	}

	return 0;
}

void deal_character(int num)
{
	//这里使用反向赋值，也就首先根据角色数量，生成一个带有角色数量和信息的序列，再将人的序列按顺序放置在序列当中
	int pre_character[13] = { 0 };
	int i;
	int pos = 1;


	//1 werewolf
	for (i = 0; i < character[num][0]; i++)
		pre_character[pos++] = 1;
	//2 lackeys
	for (i = 0; i < character[num][1]; i++)
		pre_character[pos++] = 2;
	//3 watchman
	for (i = 0; i < character[num][2]; i++)
		pre_character[pos++] = 3;
	//4 seer
	for (i = 0; i < character[num][3]; i++)
		pre_character[pos++] = 4;
	//5 robber
	for (i = 0; i < character[num][4]; i++)
		pre_character[pos++] = 5;
	//6 troublemaker
	for (i = 0; i < character[num][5]; i++)
		pre_character[pos++] = 6;
	//7 insomniac
	for (i = 0; i < character[num][6]; i++)
		pre_character[pos++] = 7;
	//8 villager
	for (i = 0; i < character[num][7]; i++)
		pre_character[pos++] = 8;
	//9 hunter
	for (i = 0; i < character[num][8]; i++)
		pre_character[pos++] = 9;
	//10 cobbler
	for (i = 0; i < character[num][8]; i++)
		pre_character[pos++] = 10;


	pos = 1;
	for (i = 0; i < num; i++)
	{
		int count = rand() % num + 1;
		while (count)
		{
			pos = (pos + 1) % num;
			if (!player[pos])
				count--;
		}
		player[pos] = pre_character[i+1];
	}


}


void print_player(int num)
{
	sendmessage(ac, fromGroup, "正在分配角色，请等待...\n");

	for (int i = 1; i <= playernum; i++)
	{
		char msg[80];
		sprintf(msg, "你是%d号玩家，你的身份是%s", i, inf[player[i]]);
		//CQ_sendPrivateMsg(ac, playerqq[i], msg);
		sendmessage(ac, playerqq[i], msg);
		Sleep(1000);
	}

	sendmessage(ac, fromGroup, "角色分配完毕，游戏开始。\n祝玩的愉快！");

}




int main()
{

	srand(time(NULL));

	int numberOfPeople;

	welcome_word();

	numberOfPeople = init();

	if (initgame(numberOfPeople))
		return -1;

	deal_character(numberOfPeople);

	print_player(numberOfPeople);


	system("pause\n");




}