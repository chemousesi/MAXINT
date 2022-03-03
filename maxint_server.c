/*
DESCRIPTION DU PROTOCOL 'maxint'
serveur
qui accepte des clients
    - le client se connecte
    - le client envoie <pseudo>
    - il attent la réponse du serveur qui sera HELLO <pseudo>
    CLIENT :
    le client peut envoyé deux types de messages
     - INT <val> val entier envoyé en big endian
     - MAX pour demander le max
     il affiche le max int, l'ip de celui qui a donné le max

    SERVEUR :
    - il renvoie INTOK quand il reçoit le message
    - s'il reçoit un message de type MAX il envoie REP<pseudo><ip><val> pseudo, im codée sur 4 octets et la val du max
    - si le serveur n'a pas recu de max il envoie NOP
    - si plusieurs clients envoient la val max on revoie le dernier client qui a envoyé


*/

/*
TODO : all done
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_NAME 10

void *comm(void *arg);

/* this structure is made to pass arguments to the thread function*/
struct arg_struct
{
    int *sock;
    struct sockaddr_in caller;
};

/* Global variables */
pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;
short have_max = 0;
uint16_t max_int;
struct sockaddr_in max_ip_adress;
char pseudo_max[20];

int main(int argc, char const *argv[])
{
    int port = 3535;
    if (argc == 2)
        /* we can modify the port by adding it to the command line */
        port = atoi(argv[1]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address_sock;
    address_sock.sin_family = AF_INET;
    address_sock.sin_port = htons(port);
    inet_aton("127.0.0.1", &address_sock.sin_addr);

    int r = bind(sock, (struct sockaddr *)&address_sock, sizeof(struct sockaddr_in));

    max_ip_adress.sin_family = AF_INET;

    if (r == 0)
    {
        printf("[SERVER STARTED][PORT %d]...\n", port);

        r = listen(sock, 5);
        while (1)
        {
            struct sockaddr_in caller;
            socklen_t size = sizeof(caller);

            int *sock_client = (int *)malloc(sizeof(int));
            *sock_client = accept(sock, (struct sockaddr *)&caller, &size);
            if (*sock_client < 0)
            {
                perror("error while accepting client");
                exit(EXIT_FAILURE);
            }

            /* threads to be added here */
            pthread_t th;
            struct arg_struct args;

            args.sock = sock_client;
            args.caller = caller;
            if (pthread_create(&th, NULL, comm, (void *)&args) != 0)
            {
                perror("error while making thread\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

void *comm(void *argument)
{
    /*declarations*/

    char temp_pseudo[MAX_NAME + 1];
    char rep[50];
    char buf[50];
    uint32_t ip;

    struct arg_struct *args = argument;

    int sock_client = *args->sock;
    struct sockaddr_in temp_ip = args->caller;

    char *ip_str = inet_ntoa(temp_ip.sin_addr);

    /* server recieves the pseudo */
    int rec = recv(sock_client, temp_pseudo, 10, 0);
    if (rec < 0)
    {
        perror("error while recieving pseudo\n");
        exit(EXIT_FAILURE);
    }
    temp_pseudo[10] = '\0';

    printf("[SERVER] [ACCEPT] Client:  %s  \tSOCKET: %d \t ip: %s\n", temp_pseudo, sock_client, ip_str);

    /* send back hello */
    sprintf(rep, "HELLO %s", temp_pseudo);

    int s = send(sock_client, rep, 16, 0); // 15 est la taille du message
    if (s < 0)
    {
        perror("error while sending hello \n");
        exit(EXIT_FAILURE);
    }

    /* recieve request */
    rec = recv(sock_client, buf, 49, 0);
    if (rec < 0)
    {
        perror("error while recieving command from client\n");
        exit(EXIT_FAILURE);
    }
    buf[rec] = '\0';

    if (buf[0] == 'I')
    /* INT <val> */
    {

        uint16_t tmp = (uint16_t)atoi(&buf[4]);

        if (rec < 0)
        {
            perror("error while recieving int from client\n");
            exit(EXIT_FAILURE);
        }

        printf("[CLIENT %s] int: %u\n", temp_pseudo, tmp);
        pthread_mutex_lock(&verrou);
        if (have_max)
        {

            if (tmp >= max_int)
            /* the new int is greater than the old one */
            {
                /*chaging int, ip, and pseudo of the max*/
                max_int = tmp;
                max_ip_adress.sin_addr.s_addr = temp_ip.sin_addr.s_addr;
                strcpy(pseudo_max, temp_pseudo);
            }
        }
        else
        /* no max existing yet */
        {
            have_max = 1;
            max_int = tmp;
            max_ip_adress.sin_addr.s_addr = temp_ip.sin_addr.s_addr;
            strcpy(pseudo_max, temp_pseudo);
        }
        pthread_mutex_unlock(&verrou);

        sprintf(rep, "INTOK");
        int s = send(sock_client, rep, 5, 0);
        if (s < 0)
        {
            perror("error while sending INTOK to client");
            exit(EXIT_FAILURE);
        }
    }
    else
    /* MAX */
    {

        printf("[CLIENT %s]: %s\n", temp_pseudo, buf);

        if (have_max)
        {
            // on la max il suffit juste de l'envoyer avec les coordonnées de celui qui l'a donné

            ip = max_ip_adress.sin_addr.s_addr;
            sprintf(rep, "REP%s%08x%04x", pseudo_max, ip, max_int);
            printf("[SERVER] Answer SENT : %s\n", rep);

            int s = send(sock_client, rep, strlen(rep), 0);
            if (s < 0)
            {
                perror("error while sending max int to client\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        { // on a pas de max
            sprintf(rep, "NOP");
            int s = send(sock_client, rep, 3, 0);
            if (s < 0)
            {
                perror("error while sending NOP to client");
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;
}
