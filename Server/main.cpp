#pragma comment(lib, "Ws2_32.lib") //библиотека сокетов
#include <WinSock2.h> // сами сокеты
#include <iostream> //для вывода информации в консоль
#include <WS2tcpip.h> //для получения информации о сервере

SOCKET Connect; // информаци о подключаемом в данный момент сокете
SOCKET * Connections; // все сокеты, массив сокетов
SOCKET Listen; //сокет для подключения

int ClientCount=0; //количество подключенных пользователей

//функция, которая будет рассылать все сообщения пользователям
void SendMessageToClient(int ID)
{
	char*buffer=new char[1024];//создаем буфер, где будет храниться передаваемое сообщение 
	for(;;Sleep(75))
	{
		memset(buffer,0,sizeof(buffer)); //подчищаем буфер
		if (recv(Connections[ID],buffer,1024,NULL)) //получаем сообщение от клиента и передаем в буфер, 1024-"длина сообщения", но так как мы ее не знаем, просто задаем максимальное
		{
			printf(buffer); //выводим сообщение
			printf("\n");
			for (int i=0; i<=ClientCount; i++) 
			{
				send(Connections[i],buffer,strlen(buffer),NULL); //передаем сообщение всем остальным пользователям
			}
		}
	}
	delete buffer;
}

int main()
{
	setlocale(LC_ALL, "russian"); //подключение русского языка
	WSAData data;
	WORD version=MAKEWORD(2,2); // версия сокетов
	int res = WSAStartup(version, &data); // храним результат того, инициализировался ли сокет
	if (res!=0)
	{
		return 0;
	}

	struct addrinfo hints;
	struct addrinfo * result;

	Connections = (SOCKET*)calloc(64,sizeof(SOCKET)); //выделяем память под массив сокетов

	ZeroMemory(&hints,sizeof(hints)); // "чистим" структуру 

	//настройки сокета
	hints.ai_family=AF_INET; //тип сокета
	hints.ai_flags=AI_PASSIVE; //флаг
	hints.ai_socktype=SOCK_STREAM; //сам тип сокета, задаем TCP, а не UDP
	hints.ai_protocol=IPPROTO_TCP;

	//получаем информацию о хосте
	getaddrinfo(NULL,"7770",&hints,&result);
	
	//инициализируем сам сокет
	Listen=socket(result->ai_family,result->ai_socktype,result->ai_protocol);
	bind(Listen,result->ai_addr, result->ai_addrlen); //объявили сам сервер, теперь он может принимать подключения
	listen(Listen,SOMAXCONN); //задаем максимальное количество подключений

	freeaddrinfo(result);

	printf("Сервер начал работу....\n");
	for (;;Sleep(75)) //бесконечный цикл и замедляем его работу
	{
		if (Connect=accept(Listen,NULL,NULL))// сервер ждет подключений, как оно происходит, сразу обрабатывает
		{
			printf("Клиент подключен...\n");
			Connections[ClientCount]=Connect; //сохраняем подключенного пользователя в наш массив сокетов
			ClientCount++;
			CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)SendMessageToClient,(LPVOID)(ClientCount-1),NULL,NULL); //создаем поток, запускаем в потоке функцию SendMessageToClient, передаем ей Id 
		}
	}

	return 1;
}

