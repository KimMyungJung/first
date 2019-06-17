#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "GameServer.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<winsock2.h>
#include <Windows.h>
#include <mysql.h>

int compId(void *data1, void* data2)
{
	printf("user %s --- find %s\n", ((user*)data1)->id, data2);
	return strcmp(((user*)data1)->id, data2);
}

//�α��� 
user* login(void *sock) {

	//ID, PASSWORD Ȯ��
	char packetData[PACKETSIZE] = { 0, };
	char id[PACKETSIZE] = { 0, };
	char password[PACKETSIZE] = { 0, };

	//ID ����
	recv(*(SOCKET*)sock, id, PACKETSIZE, 0);
	printf("%s\n", id);


	//��й�ȣ ����
	recv(*(SOCKET*)sock, password, PACKETSIZE, 0);
	printf("%s\n", password);

	//�α��� ���� -- DB�� �������� ����� �ڵ� �ۼ� �ʿ� ///////////////	
	
	//�ϴ� 123 / 123 ���� �׽�Ʈ
	//if (!strcmp(id, "123") && !strcmp(password, "123"))
	//if(1)
	//if(strcmp(id, "") || strcmp(password, ""))
	if(server_login(id, password) && find_node(&userlist, id, compId))
	{
		//Ŭ���̾�Ʈ���� �α��� �����ߴٰ� �˸�
		packetData[0] = 1;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
		printf("Login Success\n");

		//user ��� ����		
		user* newUser = createUser(id, password);
		newUser->sock = sock;
		//userlist�� ��� �߰�
		push_back(&userlist, newUser);
		//���� ���� ���
		printUser(newUser);
		//printUser(get_idx(&userlist, 0));

		return newUser;
	}
	
	else {
		//�����ߴٰ� �˸�
		packetData[0] = 0;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
		printf("Login Fail\n");
		return NULL;
	}
}

void signUp(void *sock) {
	//ID, PASSWORD Ȯ��
	char packetData[PACKETSIZE] = { 0, };
	char id[PACKETSIZE] = { 0, };
	char password[PACKETSIZE] = { 0, };

	//ID ����
	recv(*(SOCKET*)sock, id, PACKETSIZE, 0);
	printf("%s\n", id);


	//��й�ȣ ����
	recv(*(SOCKET*)sock, password, PACKETSIZE, 0);
	printf("%s\n", password);

	
	if (server_signUp(id, password))
	{
		//Ŭ���̾�Ʈ���� ȸ������ �����ߴٰ� �˸�
		packetData[0] = 1;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
		printf("SignUp Success (%s, %s)\n", id, password);
	}
	else {
		//�����ߴٰ� �˸�
		packetData[0] = 0;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
		printf("SignUp Fail\n");
	}
}

void noLogin(void *sock)
{
	printf("NoLogin\n");
}

void characterSelect(void *sock, user* curUser)
{
	char packetData[PACKETSIZE] = { 0, };

	recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	curUser->character = (int)packetData[0];
	printf("%s�� ���� ĳ���� : %d\n", curUser->id, curUser->character);
}


room* roomMake(void *sock, user* curUser)
{
	printf("RoomMake\n");
	//��û�� ������ ���� ����
	room* newRoom = createRoom(curUser);
	//���� ���� �� ����Ʈ�� ����
	push_back(&roomlist, newRoom);
	//�� ���� ���
	printRoom(newRoom);
	
	return newRoom;
	
}

void roomList(void *sock)
{
	char packetData[PACKETSIZE] = { 0, };

	printf("RoomList\n");

	//�� ���� ����
	packetData[0] = roomlist.size;
	send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);

	for (int i = 0; i < roomlist.size; i++) {
		room* temp = get_idx(&roomlist, i);
		strcpy_s(packetData, sizeof(packetData), temp->owner->id);
		printf("%s\n", packetData);
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
		memset(packetData, 0, PACKETSIZE);
	}
}


room* roomEnter(void *sock, user* curUser)
{
	char packetData[PACKETSIZE] = { 0, };
	int roomidx;


	//�� ��ȣ ����
	recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	roomidx = packetData[0];

	//printf("room Enter %d\n", roomidx);
	//���޹��� �� ��� �޾ƿ�
	room* target = get_idx(&roomlist, roomidx);
	

	//���޹��� ���� �Խ�Ʈ ����Ʈ�� ��û�� ���� �߰��Ѵ�.
	//push_back(&(target->guestlist), curUser);
	//������ �� ���� ���
	printRoom(target);

	/*
	�濡 ������ �������� ���� ������ �Ѱ��־�� �Ѵ�.
	���� ������ ��� ǥ������ ���ӷ����� ǥ���ȴ�.
	�� �����غ����� ����.
	*/

	struct sockaddr_in addr;
	char name[256];
	int addr_size = sizeof(struct sockaddr_in);

	getpeername(*(target->owner->sock), (struct sockaddr *)&addr, &addr_size);
	strncpy_s(packetData, 16, inet_ntoa(addr.sin_addr), 16);
	printf("Owner IP Send : %s", packetData);

	//�� ���� IP ����
	send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);


	return target;
}

void save(void *sock, user* curUser )
{
	char packetData[PACKETSIZE] = { 0, };
	int dataLen = 0;
	int idx; 
	int scrshotLen = 0; 

	//[data]
	recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	dataLen = *((int *)packetData);

	printf("recv data length %d\n", dataLen);

	char *saveData = malloc(dataLen); //db table�� id�� �α��� ���� data�� savedata�� �����Ͱ� ����-
	// ������ ���� => Id, data ��� �ľ� �����ϹǷ� ���������� db�� �����Ͽ� ����
	int result = recv(*(SOCKET*)sock, saveData, dataLen, 0);
	if (result == dataLen)
	{
		printf("recv Success %s\n", saveData);
	}

	//[idx]
	recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	idx = *((int *)packetData);
	memset(packetData, 0, PACKETSIZE);

	printf("recv idx value %d\n", idx);

	////[scrshot]
	//recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	//scrshotLen = *((int *)packetData);

	//printf("recv screenshot length %d\n", scrshotLen);

	//char *scrshot = malloc(scrshotLen);

	//result = recv(*(SOCKET*)sock, scrshot, scrshotLen, 0);
	//if (result == scrshotLen)
	//{
	//	printf("recv Success %s\n", scrshot);
	//}
	//printf("call data_save function\n");
	//������ �߼�
	data_save(curUser->id, saveData, dataLen, idx);
	
	
	//�������� �׿����� �� �״��� �𸣰���
	//free(saveData);
}

void load(void *sock, user* curUser, int idx)
{
	char packetData[PACKETSIZE] = { 0, };
	memset(packetData, 0, PACKETSIZE);

	char result[100001];
	recv(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	idx = *((int *)packetData); 
	data_load(curUser->id, result, idx);

	int len;

	printf("%s", result);
	

	if (result != NULL) {
		len = strlen(result);
		printf("%d", len);
		*((int *)packetData) = len;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);

		send(*(SOCKET*)sock, result, len, 0);
	}
	else {
		len = -1;
		*((int *)packetData) = len;
		send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
	}
}

void labelload(void *sock, user* curUser) 
{
	char packetData[PACKETSIZE] = { 0, };
	memset(packetData, 0, PACKETSIZE);

	for (int i = 1; i < 5; i++) {

		label_load(curUser->id, packetData, i);

		int len;

		if (strlen(packetData) != 0) {
			send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
			printf("s  = %s\n", packetData);

		}
		else {
			
			send(*(SOCKET*)sock, packetData, PACKETSIZE, 0);
			printf("f  = %s\n", packetData);
		}memset(packetData, 0, PACKETSIZE);
	}
}

void play(void *sock, user* curUser, room* curRoom)
{
	char packetData[PACKETSIZE] = { 0, };
	int recvDone = 0;
	int recvCnt = 0;
	int recvIndex = 0;


	while (1)
	{
		byte* buf = malloc(10240);
		recvDone = 0;
		recvCnt = 0;
		recvIndex = 0;

		printf("Wait Receive\n");

		//recv
		while (!recvDone) 
		{
			recvCnt = recv(*((SOCKET *)sock), &buf[recvIndex], PACKETSIZE, 0);
			if (recvCnt) {
				recvIndex += recvCnt;
			}
			else {
				recvDone = 1;
			}
		}
		printf("recvIndex : %d\n", recvIndex);
		//recvCnt = recv(*((SOCKET *)sock), &buf[recvIndex], sizeof(buf), 0);


		//send
		//node* cur = curRoom->guestlist.head->next;
		//while (cur) {
		//	send((*((user*)cur->value)->sock), buf, PACKETSIZE, 0);
		//	cur = cur->next;
		//}

		Sleep(1000);

	}

}