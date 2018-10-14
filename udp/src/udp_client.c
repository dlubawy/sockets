/* A simple UDP client */
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define SERVER_UDP_PORT 5000
#define MAXLEN 4096
#define DEFLEN 64

long delay(struct timeval t1, struct timeval t2);

int main(int argc, char *argv[])
{
    int data_size = DEFLEN, port = SERVER_UDP_PORT;
    int i, j, sd;
    unsigned int server_len;
    char *pname, *host, rbuf[MAXLEN], sbuf[MAXLEN];
    struct hostent *hp;
    struct sockaddr_in server;
    struct timeval start, end;
    void close();

    pname = argv[0];
    argc--;
    argv++;
    if (argc > 0 && (strcmp(*argv, "-s") == 0)) {
        if (--argc > 0 && (data_size = atoi(*++argv))) {
            argc--;
            argv++;
        }
        else {
            fprintf(stderr, "Usage: %s [-s data_size] host [port]\n", pname);
            exit(1);
        }
    }
    if (argc > 0) {
        host = *argv;
        if (--argc > 0) {
            port = atoi(*++argv);
        }
    }
    else {
        fprintf(stderr, "Usage: %s [-s data_size] host [port]\n", pname);
        exit(1);
    }

    /* Create a datagram socket */
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr, "Can't create a socket\n");
        exit(1);
    } 

    /* Store server's information */
    memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(host)) == NULL) {
        fprintf(stderr, "Can't get server's IP address\n");
        exit(1);
    }
    memcpy(hp->h_addr_list[0], (char *)&server.sin_addr, hp->h_length);
    if (data_size > MAXLEN) {
        fprintf(stderr, "Data is too big\n");
        exit(1);
    }
    /* data is a, b, c,..., z, a, b,... */
    for (i = 0; i < data_size; i++) {
        j = (i < 26) ? i : i % 26;
        sbuf[i] = 'a' + j;
    }

    gettimeofday(&start, NULL);

    /* transmit data */
    server_len = sizeof(server);
    if (sendto(sd, sbuf, data_size, 0, (struct sockaddr *)&server, server_len) == -1) {
        fprintf(stderr, "sendto error\n");
        exit(1);
    }

    /* receive data */
    if (recvfrom(sd, rbuf, MAXLEN, 0, (struct sockaddr *)&server, &server_len) < 0) {
        fprintf(stderr, "recvfrom error\n");
        exit(1);
    }

    gettimeofday(&end, NULL);

    printf("Round-trip delay = %ld ms.\n", delay(start, end));

    if (strncmp(sbuf, rbuf, data_size) != 0) {
        printf("Data is corrupted\n");
    } 

    close(sd);
    return 0;
}

/*
 *  Compute the delay between t1 and t2 in milliseconds
 */
long delay (struct timeval t1, struct timeval t2)
{
    long d;

    d = (t2.tv_sec - t1.tv_sec) * 1000;
    d += ((t2.tv_usec - t1.tv_usec + 500) / 1000);
    return (d);
}
