/*
Client1
- this program will create 5 clients clients:
     connect to server and send a random number between 0 and 65535
     diconnect

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define MAX_NAME 10 // limite max des caractères pour le nom

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        perror("argument problem");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);

    struct sockaddr_in adress_sock;
    adress_sock.sin_family = AF_INET;
    adress_sock.sin_port = htons(port);
    inet_aton("127.0.0.1", &adress_sock.sin_addr);

    uint16_t r;
    char buff[100];
    char rep[50];
    srand(getpid());

    char pseudo[MAX_NAME + 1];
    int res, s;

    for (short i = 0; i < 5; i++)
    {
        int serv = socket(PF_INET, SOCK_STREAM, 0);

        res = connect(serv, (struct sockaddr *)&adress_sock,
                      sizeof(struct sockaddr_in));
        if (res == -1)
        {
            perror("error while connecting client");
            exit(EXIT_FAILURE);
        }

        sprintf(pseudo, "clientnum%d", i);

        s = send(serv, pseudo, 10, 0);
        if (s == -1)
        {
            perror("error while sending pseudo client");
            exit(EXIT_FAILURE);
        }

        s = recv(serv, buff, 99, 0);
        if (s == -1)
        {
            perror("error while recieving from server");
            exit(EXIT_FAILURE);
        }
        buff[s] = '\0';
        printf("[SERVER]: %s\n", buff);

        /*send random number between 0 and 65535*/
        r = rand() % 65535;
        sprintf(rep, "INT %u", r);
        s = send(serv, rep, strlen(rep), 0);
        if (s == -1)
        {
            perror("error while sending int from client");
            exit(EXIT_FAILURE);
        }
        printf("[CLIENT %s]: %s (SENT SUCCEFULLY)\n", pseudo, rep);

        /*recieve OK*/
        s = recv(serv, buff, 99, 0);
        if (s == -1)
        {
            perror("error while recieving from server");
            exit(EXIT_FAILURE);
        }
        buff[s] = '\0';
        printf("[SERVER]: %s\n", buff);
        close(serv);
    }

    return 0;
}