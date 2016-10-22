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

char *inf[11] = { "", "����", "צ��", "��ҹ��", "Ԥ�Լ�", "ǿ��", "������", "ʧ����", "����", "����", "Ƥ��" };




void welcome_word()
{
	printf("��ӭ����һҹ������Ϸ�����������Ϸ���ϵۣ�СС�������ָ�̡�����л�ҵİְ�FrankShaw���ҵ��ϵۣ�\n�����롰#����һҹ���ˡ�,���μ����һҹ������Ϸw\n");

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

	strcpy(msg, "��Ϸ��ʼ��\n");

	sprintf(buf, "������Ϸһ����%d����", number);
	strcat(msg, buf);

	strcat(msg, "��ɫ�������£�\n");

	sprintf(buf, "����һ����%d��\n", wolf);
	strcat(msg, buf);

	if (lackeys)
	{
		sprintf(buf, "%d��צ��\n", lackeys);
		strcat(msg, buf);
	}


	if (��ҹ��)
	{
		sprintf(buf, "%d����ҹ��\n", watchman);
		strcat(msg, buf);
	}


	sprintf(buf, "%d��Ԥ�Լ�\n", seer);
	strcat(msg, buf);

	sprintf(buf, "%d��ǿ��\n", robber);
	strcat(msg, buf);

	sprintf(buf, "%d��������\n", troublemaker);
	strcat(msg, buf);

	if (insomniac)
	{
		sprintf(buf, "%d��ʧ����\n", insomniac);
		strcat(msg, buf);
	}

	sprintf(buf, "%d������\n", villager);
	strcat(msg, buf);

	if (cobbler)
	{
		sprintf(buf, "%d��Ƥ��\n", cobbler);
		strcat(msg, buf);
	}

	if (hunter)
	{
		sprintf(buf, "%d������\n", hunter);
		strcat(msg, buf);
	}

	return 0;
}

void deal_character(int num)
{
	//����ʹ�÷���ֵ��Ҳ�����ȸ��ݽ�ɫ����������һ�����н�ɫ��������Ϣ�����У��ٽ��˵����а�˳����������е���
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
	sendmessage(ac, fromGroup, "���ڷ����ɫ����ȴ�...\n");

	for (int i = 1; i <= playernum; i++)
	{
		char msg[80];
		sprintf(msg, "����%d����ң���������%s", i, inf[player[i]]);
		//CQ_sendPrivateMsg(ac, playerqq[i], msg);
		sendmessage(ac, playerqq[i], msg);
		Sleep(1000);
	}

	sendmessage(ac, fromGroup, "��ɫ������ϣ���Ϸ��ʼ��\nף�����죡");

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