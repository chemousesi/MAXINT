/*
client2
    - connects to server
    - send MAX to get the pseudo of the pseudo, ip, val of the max int

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

    int serv = socket(PF_INET, SOCK_STREAM, 0);

    char buff[100];
    char rep[50]; // utilisé comme buffer de réponse
    srand(getpid());

    char pseudo[MAX_NAME + 1];
    memset(pseudo, '\0', MAX_NAME + 1);
    int res, s;

    res = connect(serv, (struct sockaddr *)&adress_sock,
                  sizeof(struct sockaddr_in));
    if (res == -1)
    {
        perror("error while connecting client");
        exit(EXIT_FAILURE);
    }

    strncpy(pseudo, "client_max", 11);

    s = send(serv, pseudo, 10, 0);
    // len is always 10
    if (s == -1)
    {
        perror("error while sending pseudo client");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT]: %s <pseudo>\n", pseudo);
    res = recv(serv, buff, 49, 0);
    if (res == -1)
    {
        perror("error while recieving hello");
        exit(EXIT_FAILURE);
    }
    buff[res] = '\0';
    printf("[SERVER]: %s\n", buff);

    sprintf(rep, "MAX");
    s = send(serv, rep, strlen(rep), 0);
    if (s == -1)
    {
        perror("error while sending int from client max");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT]: MAX\n");
    s = recv(serv, buff, 49, 0);
    // traitement d'exception
    if (s == -1)
    {
        perror("error while recieving from server");
        exit(EXIT_FAILURE);
    }
    buff[s] = '\0';

    if (buff[0] == 'N')
    {
        printf("[SERVER]: %s\n", buff);
        printf("There is no max yet \n");
    }
    else
    {
        // on a REP<pseudo><ip><val>

        strncpy(pseudo, buff + 3, 10);
        printf("[SERVER]: max from : %s \t", pseudo);

        char s[9];
        strncpy(s, buff + 13, 8);
        s[9] = '\0';

        uint32_t ip_int = strtoul(s, NULL, 16);

        uint16_t val = (uint16_t)strtoul(&buff[21], NULL, 16);

        struct sockaddr_in max_ip_adress;
        max_ip_adress.sin_addr.s_addr = ip_int;
        char *ip_str = inet_ntoa(max_ip_adress.sin_addr);
        printf("%s \t %u \t\n", ip_str, val);
    }

    close(serv);
    return 0;
}