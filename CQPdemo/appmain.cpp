
/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include "time.h"
using namespace std;


int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;




/*
* һҹ������Ϸ����ر�������
*/

//start ��ʾһҹ���˻��ڵ�״̬����0��ʾû����Ϸ��1��ʾ�տ�ʼ���ȴ������˱����μӣ�2��ʾ������Ա��λ����ʼ��Ϸ��3��ʾ��ʼ������ݣ�Ҳ�Ǵ����׶Σ���4��ʾ�ȴ�Ԥ�Լ��ж���5��ʾǿ���ж���6��ʾ�������ж���7��ʾ���ж���ϣ��ȴ�ͶƱ��8��ʾ�ȴ������ж���9��ʾ�������
int start = 0;
int playernum = 0; //�����μ�һҹ���˵��������

const int PLAY_NUMBER = 12;

int64_t playerqq[PLAY_NUMBER + 1];	//�����μ����˵����QQ
char playername[PLAY_NUMBER + 1][50];	//�����μ�������ҵ�����
int64_t start_playerqq;		//����������Ϸ��QQ


//ͶƱ��ر���
int player_voted[PLAY_NUMBER + 1];	//��ͶƱ�����Ʊ��
int player_vote[PLAY_NUMBER + 1];	//������Ƿ�Ͷ��Ʊ
int vote_player;

int64_t uniqueQQgroup;
int isGroup = 0;	//��ʾ��Ҫ�������鷢����Ϣ����Ⱥ���ͣ�0��ʾ�������鷢�ͣ�1��ʾ��Ⱥ����


//�ǳ���ؽṹ��
struct CQ_Type_GroupMember
{
	int64_t				GroupID;			// Ⱥ��
	int64_t				QQID;				// QQ��
	std::string			nick;				// QQ�ǳ�
	std::string			card;				// Ⱥ��Ƭ
	int					sex;				// �Ա� 0/�� 1/Ů
	int					age;				// ����
	std::string			area;				// ����
	int					jointime;			// ��Ⱥʱ��
	int					lastsent;			// �ϴη���ʱ��
	std::string			level_name;			// ͷ������
	int					permission;			// Ȩ�޵ȼ� 1/��Ա 2/����Ա 3/Ⱥ��
	bool				unfriendly;			// ������Ա��¼
	std::string			title;				// �Զ���ͷ��
	int					titleExpiretime;	// ͷ�ι���ʱ��
	bool				cardcanchange;		// ����Ա�Ƿ���Э������
};

struct CQ_TYPE_QQ
{
	int64_t				QQID;				//QQ��
	std::string			nick;				//�ǳ�
	int					sex;				//�Ա�
	int					age;				//����
};



//�����û��ǳƵ�

static const char reverse_table[128] =
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

unsigned char *base64_decode(unsigned char *bindata, size_t inlen, unsigned char **out, size_t *outlen)
{
	size_t _outlen = *outlen;
	unsigned char *_out = NULL;
	int bits_collected = 0;
	unsigned int accumulator = 0;
	size_t out_pos = 0;

	if (NULL == *out)
	{
		_outlen = inlen;
		_out = (unsigned char *)malloc(_outlen);
	}
	else
	{
		_outlen = *outlen;
		_out = *out;
	}

	int c = 0;
	for (int i = 0; i < inlen; i++)
	{
		c = bindata[i];
		if (isspace(c) || c == '=')
		{
			// Skip whitespace and padding. Be liberal in what you accept.
			continue;
		}
		if ((c > 127) || (c < 0) || (reverse_table[c] > 63))
		{
			return NULL;
		}
		accumulator = (accumulator << 6) | reverse_table[c];
		bits_collected += 6;
		if (bits_collected >= 8)
		{
			bits_collected -= 8;
			_out[out_pos++] = (char)((accumulator >> bits_collected) & 0xffu);
		}
	}

	*outlen = _outlen;
	*out = _out;
	return _out;
}


//analysis_nickname
void analysis_nickname(unsigned char* in, char** out, bool group)	//group = 1��ʾ��QQȺ��������Ϣ��0��ʾ��������
{
	size_t _len = 0;
	unsigned char *outtemp = 0;
	base64_decode(in, strlen((const char*)in), &outtemp, &_len);

	if (group)
	{
		//���ݻ����˵ı�����򣬽��ж�ȡ�����ַ�
		unsigned char *infomation = outtemp;
		infomation += 16;	//�õ��ǳƵĳ���λ��
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ��ǳƳ���
		infomation += 2;	//�õ��ǳ�λ��
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//�õ��ǳ�����



		infomation = infomation + nickname_len;	//�õ�Ⱥ��Ƭ�ĳ���λ��

		unsigned short groupname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ�Ⱥ��Ƭ����

		if (groupname_len != 0)
		{
			infomation += 2;	//�õ�Ⱥ��Ƭλ��
			unsigned char *groupname;
			groupname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);
			for (unsigned int i = 0; i < groupname_len; i++)
			{
				groupname[i] = infomation[i];
			}

			groupname[groupname_len] = '\0';	//�õ�Ⱥ��Ƭ����


			*out = (char*)groupname;

		}
		else//��ʾû������Ⱥ��Ƭ
		{
			*out = (char*)nickname;
		}
	}

	else  //��������
	{
		//���ݻ����˵ı�����򣬽��ж�ȡ�����ַ�
		unsigned char *infomation = outtemp;
		infomation += 8;	//�õ��ǳƵĳ���λ��
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ��ǳƳ���
		infomation += 2;	//�õ��ǳ�λ��
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//�õ��ǳ�����


		*out = (char*)nickname;

	}
}


//number of people from 3 to 12
//meaning of number is "wolf", "lackeys", "watchman", "seer", "robber", "troublemaker", "insomniac", "villager", "hunter", "cobbler"

const int character[PLAY_NUMBER + 1][10] =
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
int player[PLAY_NUMBER + 1] = { 0 };
int bottom[4] = { 0 };

char *inf[11] = { "", "����", "צ��", "��ҹ��", "Ԥ�Լ�", "ǿ��", "������", "ʧ����", "����", "����", "Ƥ��" };



void init()
{
	playernum = 0;
	vote_player = 0;
	for (int i = 0; i < PLAY_NUMBER + 1; i++)
	{
		player[i] = 0;
		playerqq[i] = 0;
		player_vote[i] = 0;
		player_voted[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		player[i] = 0;
		playerqq[i] = 0;
	}
	
}

void sendmessage(int ac, int64_t from, char* msg)
{
	if (isGroup)
		CQ_sendGroupMsg(ac, from, msg);
	else
		CQ_sendDiscussMsg(ac, from, msg);
}



void welcome_word(int64_t fromGroup)
{
		sendmessage(ac, fromGroup, "��ӭ����������Ϸ�����������Ϸ���ϵۣ�СС�������ָ�̡�����л�ҵİְ�FrankShaw���ҵ��ϵۣ�\n�����롰#����һҹ���ˡ�,���μ����������Ϸ");
}



int initgame(int number, int64_t fromGroup)
{
	int wolf = character[number][0];
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

	sprintf(buf, "%d������\n", wolf);
	strcat(msg, buf);

	if (lackeys)
	{
		sprintf(buf, "%d��צ��\n", lackeys);
		strcat(msg, buf);
	}


	if (watchman)
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

	sendmessage(ac, fromGroup, msg);
	

	free(buf);
	free(msg);

	return 0;
}



void deal_character(int num)
{
	srand((unsigned int)time(NULL));
	//����ʹ�÷���ֵ��Ҳ�����ȸ��ݽ�ɫ����������һ�����н�ɫ��������Ϣ�����У��ٽ��˵����а�˳����������е���
	int pre_character[PLAY_NUMBER + 1] = { 0 };
	int i;
	int pos = 1;


	//1 werewolf
	for (i = 1; i <= character[num][0]; i++)
		pre_character[pos++] = 1;
	//2 lackeys
	for (i = 1; i <= character[num][1]; i++)
		pre_character[pos++] = 2;
	//3 watchman
	for (i = 1; i <= character[num][2]; i++)
		pre_character[pos++] = 3;
	//4 seer
	for (i = 1; i <= character[num][3]; i++)
		pre_character[pos++] = 4;
	//5 robber
	for (i = 1; i <= character[num][4]; i++)
		pre_character[pos++] = 5;
	//6 troublemaker
	for (i = 1; i <= character[num][5]; i++)
		pre_character[pos++] = 6;
	//7 insomniac
	for (i = 1; i <= character[num][6]; i++)
		pre_character[pos++] = 7;
	//8 villager
	for (i = 1; i <= character[num][7]; i++)
		pre_character[pos++] = 8;
	//9 hunter
	for (i = 1; i <= character[num][8]; i++)
		pre_character[pos++] = 9;
	//10 cobbler
	for (i = 1; i <= character[num][8]; i++)
		pre_character[pos++] = 10;


	pos = 1;	//���ȶ�λ1
	for (i = 1; i <= num; i++)
	{
		int count = rand() % (num + 3) + 1;	//���һ�����֣���ʾ�ӵ�ǰλ�������ƶ��������λ����һ�������ɫ
		while (count)	//�����ƶ��޽�ɫ��λ��λ��
		{
			pos++;
			if (pos > num + 3)
				pos = 1;
			if (!!pre_character[pos])
				count--;
		}
		player[i] = pre_character[pos];
		


		pre_character[pos] = 0;
	}

	pos = 1;
	for (i = 1; i <= num + 3; i++)
	{
		if (pre_character[i] != 0)
		{
			bottom[pos++] = pre_character[i];
		}
	}


}




void print_player(int64_t fromGroup, int num)
{
	sendmessage(ac, fromGroup, "���ڷ����ɫ����ȴ�...\n");

	for (int i = 1; i <= playernum; i++)
	{
		char msg[80];
		sprintf(msg, "����%d����ң���������%s", i, inf[player[i]]);
		CQ_sendPrivateMsg(ac, playerqq[i], msg);
		
		Sleep(1000);
	}

	sendmessage(ac, fromGroup, "��ɫ������ϣ���Ϸ��ʼ��\nף�����죡\n������ȴ�ҹ���ɫ�ж�...\n");

}





void wolf_startgame(int64_t fromGroup, int64_t fromQQ)
{
	init();
	welcome_word(fromGroup);
}

void wolf_time(int64_t fromGroup)
{
	//����������
	int wolf_count = 0;	//����Ϊ������ͳ�ƣ�Ҳ��Ϊ����λ�õ��±�
	int64_t wolf_player[2];
	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 1)	//���������
		{
			wolf_player[wolf_count++] = playerqq[i];
		}
			
	}
	if (wolf_count == 0)
	{
		return;
	}
	else if (wolf_count == 1)
	{
		int b = rand() % 3 + 1;
		char t[100];
		sprintf(t, "�����Ϸ���������ˣ������Ϸֻ����һλ���ˣ�ҪС�ĵ����غ��Լ�\n���ŵ�������һ��Ϊ��%s", inf[bottom[b]]);
		CQ_sendPrivateMsg(ac, wolf_player[0], t);

	}
	else if (wolf_count == 2)
	{
		char boddy[120] = "�����ͬ���ǣ�\n";
		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)
			{
				char cnum[30];
				sprintf(cnum, "%d������%s\n", i, playername[i]);
				strcat(boddy, cnum);
			}

		}

		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)
			{
				char msg[50];
				sprintf(msg, "����Ϊ%d�����ˡ�", i);
				CQ_sendPrivateMsg(ac, playerqq[i], msg);
				Sleep(500);
				CQ_sendPrivateMsg(ac, playerqq[i], boddy);
			}
		}
	}
	else
	{
		sendmessage(ac, fromGroup, "��Ϸ����δ֪����\n");
	}
}


void lackeys_time(int64_t fromGroup)
{
	int lackeys;
	for (lackeys = 1; lackeys <= playernum; lackeys++)
	{
		if (player[lackeys] == 2)	//�����צ��
			break;
	}

	if (lackeys <= playernum)	//��ʾ������צ��
	{
		//����������
		int wolf_count = 0;	//����Ϊ������ͳ�ƣ�Ҳ��Ϊ����λ�õ��±�
		int64_t wolf_player[2];

		char boddy[200] = "����צ��������Ҫ���������ǣ����û�У���ʾ����û���ǣ���\n";

		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)	//���������
			{
				wolf_player[wolf_count++] = playerqq[i];
				char buf[50];
				sprintf(buf, "%d�����%s\n", i, playername[i]);
				strcat(boddy, buf);
			}

		}

		CQ_sendPrivateMsg(ac, playerqq[lackeys], boddy);

	}


}

void watchman_time(int64_t fromGroup)
{
	//������ҹ����
	int watchman_count = 0;	//����Ϊ��ҹ����ͳ�ƣ�Ҳ��Ϊ��ҹ��λ�õ��±�
	int64_t watchman_player[2] = { 0, 0 };

	char boddy[120] = "������ҹ�ˣ������ҹ��ͬ���ǣ�\n";

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 3)	//�������ҹ��
		{
			watchman_player[watchman_count++] = playerqq[i];
			char cnum[30];
			sprintf(cnum, "%d����ҹ��%s\n", i, playername[i]);
			strcat(boddy, cnum);
		}

	}
	if (watchman_count > 2)
	{
		sendmessage(ac, fromGroup, "��Ϸ����δ֪����\n");
	}
	else
	{
		for (int i = 0; i < watchman_count; i++)
		{
			CQ_sendPrivateMsg(ac, watchman_player[i], boddy);
		}
	}

}

void seer_time(int64_t fromGroup)
{

	bool have_seer = false;

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 4)	//�����Ԥ�Լ�
		{
			have_seer = true;
			char boddy[200] = "����Ԥ�Լң�����Է�����������֮һ���ظ�һ����ű�ʾ�鿴һ�������ݣ����߻ظ�0�鿴���ŵ��Ƶ��е��������š�\n";
			start = 4;
			CQ_sendPrivateMsg(ac, playerqq[i], boddy);
		}

	}

	if (!have_seer)
	{
		robber_time();
	}

}


void robber_time()
{

	bool have_robber = false;

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 5)	//�����ǿ��
		{
			have_robber = true;
			char boddy[120] = "����ǿ������ظ�һ����ҵ���ţ��鿴��ݲ�������������뽻����ظ�0��\n";
			start = 5;
			CQ_sendPrivateMsg(ac, playerqq[i], boddy);
		}

	}

	if (!have_robber)
		troublemaker_time();

}

void troublemaker_time()
{
	bool have_troublemaker = false;

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 6)	//����ǵ�����
		{
			have_troublemaker = true;
			char boddy[120] = "���ǵ�������ظ�������ҵ���ţ��ÿո�ָ������磨3 5���������ǵ���ݻ����鿴��ݲ�������\n";
			start = 6;
			CQ_sendPrivateMsg(ac, playerqq[i], boddy);
		}

	}

	if (!have_troublemaker)
		insomniac_time();

}


void insomniac_time()
{

	bool have_insomniac = false;

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 7)	//�����ʧ����
		{
			have_insomniac = true;
			char boddy[120];
			sprintf(boddy, "����ʧ���ߣ�����������ǣ�%s", inf[player[i]]);
			start = 7;
			CQ_sendPrivateMsg(ac, playerqq[i], boddy);
		}

	}

	if (!have_insomniac)
		sunrise();
}


void character_move_stage(int64_t fromGroup)
{
	if (character[playernum][0])
		wolf_time(fromGroup);
	if (character[playernum][1])
		lackeys_time(fromGroup);
	if (character[playernum][2])
		watchman_time(fromGroup);
	if (character[playernum][3])
		seer_time(fromGroup);
	
}


void sunrise()
{
	sendmessage(ac, uniqueQQgroup, "������������������ݶ��Ѿ��ж���ϣ����ڿ�ʼ�������������ۣ������ͶƱ����˽�Ļظ���ż��ɣ���ס��һ��ͶƱ���ɸ��ġ�");
	start = 7;
}



void show_all()
{
	char mm[1000] = "������Ϸ����~������������£�\n";
	for (int i = 1; i <= playernum; i++)
	{
		char buf[50];
		sprintf(buf, "%d����������%s\n", i, inf[player[i]]);
		strcat(mm, buf);
	}
	strcat(mm, "������Ϸ��������л���~\n");
	sendmessage(ac, uniqueQQgroup, mm);
	start = 0;
}

void all_end()
{
	sendmessage(ac, uniqueQQgroup, "��������������Ѿ�ͶƱ��ϣ����ڿ�ʼͳ��Ʊ��...");
	Sleep(500);


	int max_ticket = 0;

	char buff[500] = "";
	for (int i = 1; i <= playernum; i++)
	{
		if (player_voted[i] > max_ticket)
		{
			max_ticket = player_voted[i];
		}
		char buf[50];
		sprintf(buf, "%d����ҵõ���Ʊ��Ϊ%d\n", i, player_voted[i]);
		strcat(buff, buf);
	}

	if (max_ticket == 1)	//��Ͷ���Լ���ƽƱ��û��������
	{
		bool victor = true;
		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)
			{
				victor = false;
				break;
			}
		}
		if (victor)
		{
			sendmessage(ac, uniqueQQgroup, "����û�����ˣ������Ϸ����ʤ����");
		}
		else
		{
			sendmessage(ac, uniqueQQgroup, "���ϻ������ص�����ร������Ϸ����ʤ����");
		}
		
	}
	else
	{

		int have_hunter = false;

		char dead_player[500] = "����Ʊ����ߵļ�������ǣ�\n";
		char dead_player_inf[100] = "���ǵ�����ǣ�\n";
		for (int i = 1; i < playernum; i++)
		{
			if (player_voted[i] == max_ticket)
			{
				char buf[30];
				sprintf(buf, "%d�����%s\n", i, playername[i]);
				strcat(dead_player, buf);

				memset(buf, '\0', 30);
				sprintf(buf, "%d�����%s\n", i, inf[player[i]]);
				strcat(dead_player_inf, buf);
				strcat(dead_player_inf, "��ȴ�5�룬��������ߵ����Ƿ������ˣ����û�У�5��󹫲���������ݡ�");
				if (player[i] == 9)	//��ʾ�������������
					have_hunter = true;

			}
			
		}

		strcat(dead_player, dead_player_inf);
		sendmessage(ac, uniqueQQgroup, dead_player);
		
		Sleep(5000);

		if (have_hunter)
		{
			sendmessage(ac, uniqueQQgroup, "�����ߵ��������ˣ�������˽��������Ҫ���ߵ��ˣ��������ߣ���ظ�0����������ȴ�...");
			start = 8;
		}
		else
			show_all();

	}


	
	
}




/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ

	if (start == 4)	//Ԥ�Լ��ж��غ�
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 4)	//�������Ԥ�Լ�
			{
				int ansofseer = atoi(msg);
				if (ansofseer == 0)
				{
					char b[200];
					sprintf(b, "��ѡ���˲鿴���ƣ��������ŵ���Ϊ %s �� %s ��", inf[bottom[1]], inf[bottom[2]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;
					robber_time();
				}
				else if (ansofseer > 0 && ansofseer <= playernum)
				{
					char b[200];
					sprintf(b, "��ѡ���˲鿴һ����ҵ���ݣ�%s���������%s ��", playername[ansofseer], inf[player[ansofseer]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;
					robber_time();
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "����������������룡");

			}
		}
	}

	else if (start == 5)	//ǿ���ж��غ�
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 5)	//�������Ԥ�Լ�
			{
				int ansofrobber = atoi(msg);
				if (ansofrobber == 0)
				{
					CQ_sendPrivateMsg(ac, fromQQ, "��ѡ���˲�����");
					start = 3;
					troublemaker_time();
				}
				else if (ansofrobber > 0 && ansofrobber <= playernum)
				{
					char b[200];
					sprintf(b, "��ѡ������%s��ҽ�����ݣ����������%s��", playername[ansofrobber], inf[player[ansofrobber]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;


					//����ǿ���ͱ�������
					int temp = player[ansofrobber];
					player[ansofrobber] = player[i];
					player[i] = temp;

					troublemaker_time();
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "����������������룡");

			}
		}
	}
	

	else if (start == 6)	//�������ж��غ�
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 6)	//������ǵ�����
			{
				
				char* b;
				b = strchr((char *)msg, ' ');
				b++;
				char a[10];
				strcpy(a, msg);
				a[b - msg - 1] = '\0';
				
				int x = atoi(a);
				int y = atoi(b);


				if (x == i || y == i)
				{
					CQ_sendPrivateMsg(ac, fromQQ, "��������ѡ���Լ����н��������������룡");
				}
				else
				{

					//����������ѡ�����
					int temp = player[x];
					player[x] = player[y];
					player[y] = temp;

					start = 7;

					char tt[200];
					sprintf(tt, "��ѡ����%d�����%s��%d�����%s����", x, playername[x], y, playername[y]);
					CQ_sendPrivateMsg(ac, fromQQ, tt);

					insomniac_time();
				}
				
			}
		}
	}

	else if (start == 7)	//���ܸ�λ��ҵ�ͶƱ
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ)	//���������ң�����û��Ͷ��Ʊ
			{
				if (player_vote[i] == 0)
				{
					int v = atoi(msg);
					player_voted[v]++;
					player_vote[i] = 1;

					char b[200];
					sprintf(b, "��ѡ����Ͷ��%d�����%s", v, playername[v]);
					CQ_sendPrivateMsg(ac, fromQQ, b);

					vote_player++;
					memset(b, '\0', 200);
					sprintf(b, "%d�����%s�Ѿ�ͶƱ����ʣ%dλ���û��ͶƱ", i, playername[i], (playernum - vote_player));

					sendmessage(ac, uniqueQQgroup, b);

					if (vote_player == playernum)
					{
						start = 9;
						all_end();
					}

				}
				else
				{
					CQ_sendPrivateMsg(ac, fromQQ, "���Ѿ�Ͷ��Ʊ�ˣ�");
				}
			}
		}
	}
	else if (start == 8)	//���������ж�
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 9)	//�����������
			{
				int ansofhunter = atoi(msg);
				if (ansofhunter == 0)
					sendmessage(ac, uniqueQQgroup, "����ѡ���˲����ˣ�");
				else if (ansofhunter > 0 && ansofhunter <= playernum)
				{
					char buf[100];
					sprintf(buf, "����ѡ���˴���%d�����%s�����������%s��", ansofhunter, playername[ansofhunter], inf[player[ansofhunter]]);
					sendmessage(ac, uniqueQQgroup, buf);
				}
				start = 9;
				show_all();
			}
		}
	}


	return EVENT_BLOCK;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	if (strcmp(msg, "#��ʼһҹ����") == 0)
	{
		//if (fromQQ == 452434701)	//���ô�����Ȩ��
		//{
			if (start == 0)
			{
				start_playerqq = fromQQ;
				isGroup = 1;
				uniqueQQgroup = fromGroup;
				wolf_startgame(fromGroup, fromQQ);
				start = 1;
			}
			else
				sendmessage(ac, fromGroup, "һҹ������Ϸ�Ѿ���ʼ��");

		//}
		//else
			//sendmessage(ac, fromGroup, "�㲻���Ұְ֣������㿪һҹ������Ϸ");

	}


	else if (strcmp(msg, "#����һҹ����") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromGroup, "����һҹ������Ϸ��");
				start = 0;
			}
			else
				sendmessage(ac, fromGroup, "�㲻��һҹ������Ϸ�ķ����ߣ����ܽ���һҹ������Ϸ��");
		}
		else
			sendmessage(ac, fromGroup, "��û�п�ʼһҹ������Ϸ��");
	}



	else if (strcmp(msg, "#����һҹ����") == 0)
	{
		if (start == 1)
		{
			int joined = 0;
			int set = 1;
			int i;
			for (i = 1; i <= PLAY_NUMBER; i++)
			{
				if (playerqq[i] == 0)
					set = i;
				if (playerqq[i] == fromQQ)	//����ظ�����ĺ�����ǰ�ڿ�����ȥ��
				{
					joined = 1;
					break;
				}
				if (playerqq[i] == 0)
					break;
			}
			if (joined)
			{
				char buf[50];
				sprintf(buf, "%s�Ѿ�������һҹ���ˣ������ظ�����", playername[i]);
				sendmessage(ac, fromGroup, buf);
			}
			else
			{

				unsigned char* member = (unsigned char *)CQ_getGroupMemberInfoV2(ac, fromGroup, fromQQ, 1);


				char *nickname;

				nickname = (char *)malloc(sizeof(char)* 50);

				analysis_nickname(member, &nickname, 1);

				strcpy(playername[set], nickname);
				playerqq[set] = fromQQ;
				playernum++;
				char buf[300];
				sprintf(buf, "%s����ɹ�\n��ǰ����Ϊ%d��:\n", nickname, playernum);
				for (int i = 1; i <= playernum; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d����ң�", i);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromGroup, buf);
			}


		}
		else
			sendmessage(ac, fromGroup, "û�����ڽ��е�һҹ������Ϸ��������ָ���ʼһҹ���ˡ���������һ��һҹ������Ϸ");
	}

	else if (strcmp(msg, "#����һҹ������Ϸ") == 0)
	{
		if (start == 1)
			if (playernum >= 3 && playernum <= PLAY_NUMBER)
			{
				initgame(playernum, fromGroup);
				start = 2;
				deal_character(playernum);
				print_player(fromGroup, playernum);
				start = 3;	//�����ɫȷ�Ͻ׶�
				character_move_stage(fromGroup);
				
			}
			else
				sendmessage(ac, fromGroup, "�������㣬�޷�������Ϸ");
		else
			sendmessage(ac, fromGroup, "�޷���������Ϸ������ȷ״̬");

	}


	if (strcmp(msg, "#�鿴һҹ״̬") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromGroup, m);
	}


	if (strcmp(msg, "#�鿴һҹ���") == 0)
	{
		char buf[300] = "���ڵ����Ϊ��\n";
		for (int i = 1; i <= playernum; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d����ң�", i);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromGroup, buf);
	}


	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {



	if (strcmp(msg, "#��ʼһҹ����") == 0)
	{
		//if (fromQQ == 452434701)	���ô�����Ȩ��
		//{
			if (start == 0)
			{
				start_playerqq = fromQQ;
				isGroup = 0;
				uniqueQQgroup = fromDiscuss;
				wolf_startgame(fromDiscuss, fromQQ);
				start = 1;
			}
			else
				sendmessage(ac, fromDiscuss, "һҹ������Ϸ�Ѿ���ʼ��");

		//}
		//else
			//sendmessage(ac, fromDiscuss, "�㲻���Ұְ֣������㿪һҹ������Ϸ");

	}


	else if (strcmp(msg, "#����һҹ����") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromDiscuss, "����һҹ������Ϸ��");
				start = 0;
			}
			else
				sendmessage(ac, fromDiscuss, "�㲻��һҹ������Ϸ�ķ����ߣ����ܽ���һҹ������Ϸ��");
		}
		else
			sendmessage(ac, fromDiscuss, "��û�п�ʼһҹ������Ϸ��");
	}



	else if (strcmp(msg, "#����һҹ����") == 0)
	{
		if (start == 1)
		{
			int joined = 0;
			int set = 1;
			int i;
			for (i = 1; i <= PLAY_NUMBER; i++)
			{
				if (playerqq[i] == 0)
					set = i;
				if (playerqq[i] == fromQQ)	//����ظ�����ĺ�����ǰ�ڿ�����ȥ��
				{
					joined = 1;
					break;
				}
				if (playerqq[i] == 0)
					break;
			}
			if (joined)
			{
				char buf[50];
				sprintf(buf, "%s�Ѿ�������һҹ���ˣ������ظ�����", playername[i]);
				sendmessage(ac, fromDiscuss, buf);
			}
			else
			{

				unsigned char* member = (unsigned char *)CQ_getStrangerInfo(ac, fromQQ, 1);


				char *nickname;

				nickname = (char *)malloc(sizeof(char)* 50);

				analysis_nickname(member, &nickname, 0);

				strcpy(playername[set], nickname);
				playerqq[set] = fromQQ;
				playernum++;
				char buf[300];
				sprintf(buf, "%s����ɹ�\n��ǰ����Ϊ%d��:\n", nickname, playernum);
				for (int i = 1; i <= playernum; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d����ң�", i);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromDiscuss, buf);
			}


		}
		else
			sendmessage(ac, fromDiscuss, "û�����ڽ��е�һҹ������Ϸ��������ָ���ʼһҹ���ˡ���������һ��һҹ������Ϸ");
	}

	else if (strcmp(msg, "#����һҹ������Ϸ") == 0)
	{
		if (start == 1)
		if (playernum >= 3 && playernum <= PLAY_NUMBER)
		{
			initgame(playernum, fromDiscuss);
			start = 2;
			deal_character(playernum);
			print_player(fromDiscuss, playernum);
			start = 3;	//�����ɫȷ�Ͻ׶�
			character_move_stage(fromDiscuss);

		}
		else
			sendmessage(ac, fromDiscuss, "�������㣬�޷�������Ϸ");
		else
			sendmessage(ac, fromDiscuss, "�޷���������Ϸ������ȷ״̬");

	}


	if (strcmp(msg, "#�鿴һҹ״̬") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromDiscuss, m);
	}


	if (strcmp(msg, "#�鿴һҹ���") == 0)
	{
		char buf[300] = "���ڵ����Ϊ��\n";
		for (int i = 1; i <= playernum; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d����ң�", i);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromDiscuss, buf);
	}

	
	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}
