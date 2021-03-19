
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;
/* portul de conectare la server*/
int port;
int board[6][7];
//void showBoard(char* moves)
//{
//
//    char try[2];
//    //char up;
//    int imoves;
//    strcpy(try, moves);     //conversie char* to char
//    imoves=atoi(try);       //conversie char to int
//
//    printf("Here, here: %s\n", try);
//    printf("Here, here int: ..%d..\n", imoves);
//    for(int i=0; i<6;i++)               //verific pozitia unde trebuie pusa mutarea
//    {
//        if(board[i][imoves] == 0)
//        {
//            board[i][imoves]=1;
//        }
//    }
//}

void  ShowBoard()
        {
        FILE *fptr;
            fptr = fopen("BoardDisplay.txt", "r");

//            if (fptr != NULL) {
//                printf("File created successfully!\n");
//            } else {
//                printf("Failed to create the file.\n");
//                // exit status for OS that an error occurred
//                exit(1);
//            }

            int num;
            int col = 0;

            while ((num = getw(fptr)) != EOF) {
                col++;
                printf("%d  ", num);
                if (col == 7) {
                    printf("\n");
                    col = 0;
                }
            }
        }


int main (int argc, char *argv[])
{
    int sd;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare
    char msg[100];		// mesajul trimis

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons (port);

    /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }
    printf("Welcome to Connect4! \t \n");
    printf("Optiuni: login username \n");
    printf("\t Sing Up: signUp username \n ");
    printf("\t Start Game: play \n ");

    while(1){

        printf("Introduceti comanda dorita \t");
        scanf("%[^\n]%*c", &msg[0]);       //comanda primita de la client
        printf("Client: Mesajul citit este: %s \n", msg);
        //trimiterea mesajului catre server
        if (write (sd, msg, 100) <= 0)
        {
            perror ("[client]Eroare la write() spre server.\n");
            return errno;
        }


        if(strcmp(msg, "exit") == 0)
        {
            close(sd);
            printf("[-]Disconnected from the server. \n");
            exit(1);
        }

        bzero(msg, sizeof(msg));
        /* citirea raspunsului dat de server */
        if (read (sd, msg, sizeof(msg)) < 0)
        {
            perror ("[client]Eroare la read() de la server.\n");
            //return errno;
        }
        /* afisam mesajul primit */
        printf("[-]Mesajul primit este: \t%s\n", msg);

        if (strcmp(msg,"1") == 0)
        {
            printf("Hello, player 1!\n Make your move \n");

        }
        else
        if (strcmp(msg,"2") == 0)
        {
            printf("Hello, player 2!\n Make your move \n");

        }

        if (strncmp(msg,"lose", 4) == 0)
        {
            ShowBoard();
            printf("You lost the game \n Options: \n Play again? \n exit\n");

        }
        else
        if (strncmp(msg,"win", 3) == 0)
        {
            ShowBoard();
            printf("Congratulations! You won! \n Options: \n Play again? \n exit \n");

        }
        else
            if(strncmp(msg,"continue", 8) == 0)
            {
                ShowBoard();
                printf("Make your next move \n");
            }



    }
}