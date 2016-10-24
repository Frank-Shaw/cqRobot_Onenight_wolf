
/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include "time.h"
using namespace std;


int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;




/*
* 一夜狼人游戏的相关变量参数
*/

//start 表示一夜狼人环节的状态机，0表示没有游戏，1表示刚开始，等待其他人报名参加，2表示所有人员就位，开始游戏。3表示开始分配身份（也是待命阶段），4表示等待预言家行动，5表示强盗行动，6表示捣蛋鬼行动，7表示都行动完毕，等待投票，8表示等待猎人行动，9表示公布结果
int start = 0;
int playernum = 0; //报名参加一夜狼人的玩家数量

const int PLAY_NUMBER = 12;

int64_t playerqq[PLAY_NUMBER + 1];	//报名参加狼人的玩家QQ
char playername[PLAY_NUMBER + 1][50];	//报名参加狼人玩家的名字
int64_t start_playerqq;		//发起狼人游戏者QQ


//投票相关变量
int player_voted[PLAY_NUMBER + 1];	//被投票的玩家票数
int player_vote[PLAY_NUMBER + 1];	//该玩家是否投过票
int vote_player;

int64_t uniqueQQgroup;
int isGroup = 0;	//表示需要向讨论组发送信息还是群发送，0表示向讨论组发送，1表示向群发送


//昵称相关结构体
struct CQ_Type_GroupMember
{
	int64_t				GroupID;			// 群号
	int64_t				QQID;				// QQ号
	std::string			nick;				// QQ昵称
	std::string			card;				// 群名片
	int					sex;				// 性别 0/男 1/女
	int					age;				// 年龄
	std::string			area;				// 地区
	int					jointime;			// 入群时间
	int					lastsent;			// 上次发言时间
	std::string			level_name;			// 头衔名字
	int					permission;			// 权限等级 1/成员 2/管理员 3/群主
	bool				unfriendly;			// 不良成员记录
	std::string			title;				// 自定义头衔
	int					titleExpiretime;	// 头衔过期时间
	bool				cardcanchange;		// 管理员是否能协助改名
};

struct CQ_TYPE_QQ
{
	int64_t				QQID;				//QQ号
	std::string			nick;				//昵称
	int					sex;				//性别
	int					age;				//年龄
};



//解析用户昵称的

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
void analysis_nickname(unsigned char* in, char** out, bool group)	//group = 1表示从QQ群过来的信息；0表示从讨论组
{
	size_t _len = 0;
	unsigned char *outtemp = 0;
	base64_decode(in, strlen((const char*)in), &outtemp, &_len);

	if (group)
	{
		//根据机器人的编码规则，进行读取中文字符
		unsigned char *infomation = outtemp;
		infomation += 16;	//得到昵称的长度位置
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到昵称长度
		infomation += 2;	//得到昵称位置
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//得到昵称名字



		infomation = infomation + nickname_len;	//得到群名片的长度位置

		unsigned short groupname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到群名片长度

		if (groupname_len != 0)
		{
			infomation += 2;	//得到群名片位置
			unsigned char *groupname;
			groupname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);
			for (unsigned int i = 0; i < groupname_len; i++)
			{
				groupname[i] = infomation[i];
			}

			groupname[groupname_len] = '\0';	//得到群名片名字


			*out = (char*)groupname;

		}
		else//表示没有设置群名片
		{
			*out = (char*)nickname;
		}
	}

	else  //从讨论组
	{
		//根据机器人的编码规则，进行读取中文字符
		unsigned char *infomation = outtemp;
		infomation += 8;	//得到昵称的长度位置
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到昵称长度
		infomation += 2;	//得到昵称位置
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//得到昵称名字


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

char *inf[11] = { "", "狼人", "爪牙", "守夜人", "预言家", "强盗", "捣蛋鬼", "失眠者", "村民", "猎人", "皮匠" };



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
		sendmessage(ac, fromGroup, "欢迎开启狼人游戏，我是这次游戏的上帝，小小萌神，请多指教。（感谢我的爸爸FrankShaw让我当上帝）\n请输入“#加入一夜狼人”,来参加这次狼人游戏");
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

	strcpy(msg, "游戏开始！\n");

	sprintf(buf, "本轮游戏一共有%d个人", number);
	strcat(msg, buf);

	strcat(msg, "角色分配如下：\n");

	sprintf(buf, "%d名狼人\n", wolf);
	strcat(msg, buf);

	if (lackeys)
	{
		sprintf(buf, "%d名爪牙\n", lackeys);
		strcat(msg, buf);
	}


	if (watchman)
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

	sendmessage(ac, fromGroup, msg);
	

	free(buf);
	free(msg);

	return 0;
}



void deal_character(int num)
{
	srand((unsigned int)time(NULL));
	//这里使用反向赋值，也就首先根据角色数量，生成一个带有角色数量和信息的序列，再将人的序列按顺序放置在序列当中
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


	pos = 1;	//首先定位1
	for (i = 1; i <= num; i++)
	{
		int count = rand() % (num + 3) + 1;	//随机一个数字，表示从当前位置往后推动这个数字位，下一个分配角色
		while (count)	//往后推动无角色的位置位数
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
	sendmessage(ac, fromGroup, "正在分配角色，请等待...\n");

	for (int i = 1; i <= playernum; i++)
	{
		char msg[80];
		sprintf(msg, "你是%d号玩家，你的身份是%s", i, inf[player[i]]);
		CQ_sendPrivateMsg(ac, playerqq[i], msg);
		
		Sleep(1000);
	}

	sendmessage(ac, fromGroup, "角色分配完毕，游戏开始。\n祝玩的愉快！\n现在请等待夜间角色行动...\n");

}





void wolf_startgame(int64_t fromGroup, int64_t fromQQ)
{
	init();
	welcome_word(fromGroup);
}

void wolf_time(int64_t fromGroup)
{
	//检索狼人数
	int wolf_count = 0;	//既作为狼人数统计，也作为狼人位置的下标
	int64_t wolf_player[2];
	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 1)	//如果是狼人
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
		sprintf(t, "这局游戏，你是狼人，这局游戏只有你一位狼人，要小心的隐藏好自己\n三张底牌其中一张为：%s", inf[bottom[b]]);
		CQ_sendPrivateMsg(ac, wolf_player[0], t);

	}
	else if (wolf_count == 2)
	{
		char boddy[120] = "你的狼同伴是：\n";
		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)
			{
				char cnum[30];
				sprintf(cnum, "%d号狼人%s\n", i, playername[i]);
				strcat(boddy, cnum);
			}

		}

		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)
			{
				char msg[50];
				sprintf(msg, "你作为%d号狼人。", i);
				CQ_sendPrivateMsg(ac, playerqq[i], msg);
				Sleep(500);
				CQ_sendPrivateMsg(ac, playerqq[i], boddy);
			}
		}
	}
	else
	{
		sendmessage(ac, fromGroup, "游戏出现未知错误！\n");
	}
}


void lackeys_time(int64_t fromGroup)
{
	int lackeys;
	for (lackeys = 1; lackeys <= playernum; lackeys++)
	{
		if (player[lackeys] == 2)	//如果是爪牙
			break;
	}

	if (lackeys <= playernum)	//表示场上有爪牙
	{
		//检索狼人数
		int wolf_count = 0;	//既作为狼人数统计，也作为狼人位置的下标
		int64_t wolf_player[2];

		char boddy[200] = "你是爪牙，你需要保护的狼是（如果没有，表示场上没有狼）：\n";

		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 1)	//如果是狼人
			{
				wolf_player[wolf_count++] = playerqq[i];
				char buf[50];
				sprintf(buf, "%d号玩家%s\n", i, playername[i]);
				strcat(boddy, buf);
			}

		}

		CQ_sendPrivateMsg(ac, playerqq[lackeys], boddy);

	}


}

void watchman_time(int64_t fromGroup)
{
	//检索守夜人数
	int watchman_count = 0;	//既作为守夜人数统计，也作为守夜人位置的下标
	int64_t watchman_player[2] = { 0, 0 };

	char boddy[120] = "你是守夜人，你的守夜人同伴是：\n";

	for (int i = 1; i <= playernum; i++)
	{
		if (player[i] == 3)	//如果是守夜人
		{
			watchman_player[watchman_count++] = playerqq[i];
			char cnum[30];
			sprintf(cnum, "%d号守夜人%s\n", i, playername[i]);
			strcat(boddy, cnum);
		}

	}
	if (watchman_count > 2)
	{
		sendmessage(ac, fromGroup, "游戏出现未知错误！\n");
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
		if (player[i] == 4)	//如果是预言家
		{
			have_seer = true;
			char boddy[200] = "你是预言家，你可以发动两个技能之一：回复一个序号表示查看一个玩家身份；或者回复0查看三张底牌当中的任意两张。\n";
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
		if (player[i] == 5)	//如果是强盗
		{
			have_robber = true;
			char boddy[120] = "你是强盗，请回复一个玩家的序号，查看身份并交换，如果不想交换请回复0。\n";
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
		if (player[i] == 6)	//如果是捣蛋鬼
		{
			have_troublemaker = true;
			char boddy[120] = "你是捣蛋鬼，请回复两个玩家的序号，用空格分隔，例如（3 5），将他们的身份互换查看身份并交换。\n";
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
		if (player[i] == 7)	//如果是失眠者
		{
			have_insomniac = true;
			char boddy[120];
			sprintf(boddy, "你是失眠者，你最后的身份是：%s", inf[player[i]]);
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
	sendmessage(ac, uniqueQQgroup, "好啦，昨晚上所有身份都已经行动完毕，现在开始进入无限制讨论，如果想投票，请私聊回复序号即可，记住，一旦投票不可更改。");
	start = 7;
}



void show_all()
{
	char mm[1000] = "本局游戏结束~所有人身份如下：\n";
	for (int i = 1; i <= playernum; i++)
	{
		char buf[50];
		sprintf(buf, "%d号玩家身份是%s\n", i, inf[player[i]]);
		strcat(mm, buf);
	}
	strcat(mm, "本次游戏结束，感谢大家~\n");
	sendmessage(ac, uniqueQQgroup, mm);
	start = 0;
}

void all_end()
{
	sendmessage(ac, uniqueQQgroup, "好啦，所有玩家已经投票完毕，现在开始统计票数...");
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
		sprintf(buf, "%d号玩家得到的票数为%d\n", i, player_voted[i]);
		strcat(buff, buf);
	}

	if (max_ticket == 1)	//都投了自己，平票，没有人死亡
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
			sendmessage(ac, uniqueQQgroup, "场上没有狼人，这局游戏村民胜利！");
		}
		else
		{
			sendmessage(ac, uniqueQQgroup, "场上还有隐藏的狼人喔，这局游戏狼人胜利！");
		}
		
	}
	else
	{

		int have_hunter = false;

		char dead_player[500] = "昨晚票数最高的几名玩家是：\n";
		char dead_player_inf[100] = "他们的身份是：\n";
		for (int i = 1; i < playernum; i++)
		{
			if (player_voted[i] == max_ticket)
			{
				char buf[30];
				sprintf(buf, "%d号玩家%s\n", i, playername[i]);
				strcat(dead_player, buf);

				memset(buf, '\0', 30);
				sprintf(buf, "%d号玩家%s\n", i, inf[player[i]]);
				strcat(dead_player_inf, buf);
				strcat(dead_player_inf, "请等待5秒，检查死亡者当中是否有猎人，如果没有，5秒后公布所有人身份。");
				if (player[i] == 9)	//表示死亡玩家有猎人
					have_hunter = true;

			}
			
		}

		strcat(dead_player, dead_player_inf);
		sendmessage(ac, uniqueQQgroup, dead_player);
		
		Sleep(5000);

		if (have_hunter)
		{
			sendmessage(ac, uniqueQQgroup, "死亡者当中有猎人，请猎人私聊我你想要带走的人，若不带走，请回复0，其他人请等待...");
			start = 8;
		}
		else
			show_all();

	}


	
	
}




/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息

	if (start == 4)	//预言家行动回合
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 4)	//这个人是预言家
			{
				int ansofseer = atoi(msg);
				if (ansofseer == 0)
				{
					char b[200];
					sprintf(b, "你选择了查看底牌，其中两张底牌为 %s 和 %s 。", inf[bottom[1]], inf[bottom[2]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;
					robber_time();
				}
				else if (ansofseer > 0 && ansofseer <= playernum)
				{
					char b[200];
					sprintf(b, "你选择了查看一个玩家的身份，%s他的身份是%s 。", playername[ansofseer], inf[player[ansofseer]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;
					robber_time();
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "输入错误，请重新输入！");

			}
		}
	}

	else if (start == 5)	//强盗行动回合
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 5)	//这个人是预言家
			{
				int ansofrobber = atoi(msg);
				if (ansofrobber == 0)
				{
					CQ_sendPrivateMsg(ac, fromQQ, "你选择了不换牌");
					start = 3;
					troublemaker_time();
				}
				else if (ansofrobber > 0 && ansofrobber <= playernum)
				{
					char b[200];
					sprintf(b, "你选择了与%s玩家交换身份，他的身份是%s。", playername[ansofrobber], inf[player[ansofrobber]]);
					CQ_sendPrivateMsg(ac, fromQQ, b);
					start = 3;


					//交换强盗和被换的人
					int temp = player[ansofrobber];
					player[ansofrobber] = player[i];
					player[i] = temp;

					troublemaker_time();
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "输入错误，请重新输入！");

			}
		}
	}
	

	else if (start == 6)	//捣蛋鬼行动回合
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 6)	//这个人是捣蛋鬼
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
					CQ_sendPrivateMsg(ac, fromQQ, "捣蛋鬼不能选择自己进行交换，请重新输入！");
				}
				else
				{

					//交换捣蛋鬼选择的人
					int temp = player[x];
					player[x] = player[y];
					player[y] = temp;

					start = 7;

					char tt[200];
					sprintf(tt, "你选择了%d号玩家%s和%d号玩家%s交换", x, playername[x], y, playername[y]);
					CQ_sendPrivateMsg(ac, fromQQ, tt);

					insomniac_time();
				}
				
			}
		}
	}

	else if (start == 7)	//接受各位玩家的投票
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (playerqq[i] == fromQQ)	//这个人是玩家，并且没有投过票
			{
				if (player_vote[i] == 0)
				{
					int v = atoi(msg);
					player_voted[v]++;
					player_vote[i] = 1;

					char b[200];
					sprintf(b, "你选择了投给%d号玩家%s", v, playername[v]);
					CQ_sendPrivateMsg(ac, fromQQ, b);

					vote_player++;
					memset(b, '\0', 200);
					sprintf(b, "%d号玩家%s已经投票！还剩%d位玩家没有投票", i, playername[i], (playernum - vote_player));

					sendmessage(ac, uniqueQQgroup, b);

					if (vote_player == playernum)
					{
						start = 9;
						all_end();
					}

				}
				else
				{
					CQ_sendPrivateMsg(ac, fromQQ, "你已经投过票了！");
				}
			}
		}
	}
	else if (start == 8)	//猎人死后行动
	{
		for (int i = 1; i <= playernum; i++)
		{
			if (player[i] == 9)	//这个人是猎人
			{
				int ansofhunter = atoi(msg);
				if (ansofhunter == 0)
					sendmessage(ac, uniqueQQgroup, "猎人选择了不带人！");
				else if (ansofhunter > 0 && ansofhunter <= playernum)
				{
					char buf[100];
					sprintf(buf, "猎人选择了带走%d号玩家%s，他的身份是%s！", ansofhunter, playername[ansofhunter], inf[player[ansofhunter]]);
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
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	if (strcmp(msg, "#开始一夜狼人") == 0)
	{
		//if (fromQQ == 452434701)	//设置创建者权限
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
				sendmessage(ac, fromGroup, "一夜狼人游戏已经开始！");

		//}
		//else
			//sendmessage(ac, fromGroup, "你不是我爸爸，不给你开一夜狼人游戏");

	}


	else if (strcmp(msg, "#结束一夜狼人") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromGroup, "结束一夜狼人游戏！");
				start = 0;
			}
			else
				sendmessage(ac, fromGroup, "你不是一夜狼人游戏的发起者，不能结束一夜狼人游戏！");
		}
		else
			sendmessage(ac, fromGroup, "并没有开始一夜狼人游戏！");
	}



	else if (strcmp(msg, "#加入一夜狼人") == 0)
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
				if (playerqq[i] == fromQQ)	//检测重复加入的函数，前期开发先去掉
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
				sprintf(buf, "%s已经加入了一夜狼人，不能重复加入", playername[i]);
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
				sprintf(buf, "%s加入成功\n当前人数为%d人:\n", nickname, playernum);
				for (int i = 1; i <= playernum; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d号玩家：", i);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromGroup, buf);
			}


		}
		else
			sendmessage(ac, fromGroup, "没有正在进行的一夜狼人游戏，请输入指令“开始一夜狼人”，来开启一场一夜狼人游戏");
	}

	else if (strcmp(msg, "#开启一夜狼人游戏") == 0)
	{
		if (start == 1)
			if (playernum >= 3 && playernum <= PLAY_NUMBER)
			{
				initgame(playernum, fromGroup);
				start = 2;
				deal_character(playernum);
				print_player(fromGroup, playernum);
				start = 3;	//进入角色确认阶段
				character_move_stage(fromGroup);
				
			}
			else
				sendmessage(ac, fromGroup, "人数不足，无法开启游戏");
		else
			sendmessage(ac, fromGroup, "无法开启，游戏不在正确状态");

	}


	if (strcmp(msg, "#查看一夜状态") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromGroup, m);
	}


	if (strcmp(msg, "#查看一夜玩家") == 0)
	{
		char buf[300] = "现在的玩家为：\n";
		for (int i = 1; i <= playernum; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d号玩家：", i);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromGroup, buf);
	}


	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {



	if (strcmp(msg, "#开始一夜狼人") == 0)
	{
		//if (fromQQ == 452434701)	设置创建者权限
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
				sendmessage(ac, fromDiscuss, "一夜狼人游戏已经开始！");

		//}
		//else
			//sendmessage(ac, fromDiscuss, "你不是我爸爸，不给你开一夜狼人游戏");

	}


	else if (strcmp(msg, "#结束一夜狼人") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromDiscuss, "结束一夜狼人游戏！");
				start = 0;
			}
			else
				sendmessage(ac, fromDiscuss, "你不是一夜狼人游戏的发起者，不能结束一夜狼人游戏！");
		}
		else
			sendmessage(ac, fromDiscuss, "并没有开始一夜狼人游戏！");
	}



	else if (strcmp(msg, "#加入一夜狼人") == 0)
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
				if (playerqq[i] == fromQQ)	//检测重复加入的函数，前期开发先去掉
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
				sprintf(buf, "%s已经加入了一夜狼人，不能重复加入", playername[i]);
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
				sprintf(buf, "%s加入成功\n当前人数为%d人:\n", nickname, playernum);
				for (int i = 1; i <= playernum; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d号玩家：", i);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromDiscuss, buf);
			}


		}
		else
			sendmessage(ac, fromDiscuss, "没有正在进行的一夜狼人游戏，请输入指令“开始一夜狼人”，来开启一场一夜狼人游戏");
	}

	else if (strcmp(msg, "#开启一夜狼人游戏") == 0)
	{
		if (start == 1)
		if (playernum >= 3 && playernum <= PLAY_NUMBER)
		{
			initgame(playernum, fromDiscuss);
			start = 2;
			deal_character(playernum);
			print_player(fromDiscuss, playernum);
			start = 3;	//进入角色确认阶段
			character_move_stage(fromDiscuss);

		}
		else
			sendmessage(ac, fromDiscuss, "人数不足，无法开启游戏");
		else
			sendmessage(ac, fromDiscuss, "无法开启，游戏不在正确状态");

	}


	if (strcmp(msg, "#查看一夜状态") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromDiscuss, m);
	}


	if (strcmp(msg, "#查看一夜玩家") == 0)
	{
		char buf[300] = "现在的玩家为：\n";
		for (int i = 1; i <= playernum; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d号玩家：", i);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromDiscuss, buf);
	}

	
	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}
