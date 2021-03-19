#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>



/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;
int loggedIn = 0;
int GameOn=0;           //devine 1 cand se da comanda play si incepe jocul
int board[7][8];
int color;              // valoarea aleasa de fiecare jucator care reprezinta culoarea
int SbWon;              //devine 1 in momentul in care castiga icneva
int scor;
int turn=0;

struct vecini{          //structura in care updatam pozitiile pentru a verifica cine castiga
    int oriz;
    int vert;
    int diagP;  //diagonala principala
    int diagS;  //diagonala secundara
}vec[6][7];



void start_game()
{

    for(int i=0; i<6; i++) {
        for (int j = 0; j <7; j++) {
            board[i][j] = 0;

        }

    }
    GameOn=1;

}
int GetTurn()     //verific daca e randul jucatorului
{
    FILE *fptr;
    int num;
    fptr = fopen("Turn.txt", "r");

    if (fptr == NULL) {
        // exit status for OS that an error occurred
        exit(1);
    }


    if((num = getw(fptr)) != EOF) {
        fclose(fptr);
        return num;
    }
    else
    {
        fclose(fptr);
        return 0;
    }



}

void PutTurn(int turnn)
{
    char s;
    int num, fd;
    FILE * fptr;
    fptr = fopen("Turn.txt", "w");
    if (fptr == NULL) {
        // exit status for OS that an error occurred
        exit(1);
    }

    putw(turnn, fptr);
    fclose(fptr);

}

int GetSbWon (){
    FILE *fptr;
    int num;
    fptr = fopen("Winner.txt", "r");

    if (fptr == NULL) {
        exit(1);
    }
    if( (num = getw(fptr)) != EOF)
        SbWon= num;

    fclose(fptr);
}

int PutSbWon (int winner){
    FILE *fptr;
    int num;
    fptr = fopen("Winner.txt", "w");

    if (fptr == NULL) {
        exit(1);
    }
    if( (num = getw(fptr)) != EOF)
        SbWon= num;
    putw(winner, fptr);

    fclose(fptr);
}


int Win(int row, int col)
{
    int i= row;
    int j=col;
    int castig=0;
    vec[i][j].oriz=1;
    vec[i][j].vert=1;
    vec[i][j].diagP=1;
    vec[i][j].diagS=1;


    //verificam orizontala
    if(j>0 && vec[i][j-1].oriz >=1)
    {
        vec[i][j].oriz += vec[i][j-1].oriz;
        vec[i][j-1].oriz++;
    }
    if(j<7 && vec[i][j+1].oriz >=1)
    {
        vec[i][j].oriz += vec[i][j+1].oriz;
        vec[i][j+1].oriz++;
    }

    //verificam verticala
    if(i>0 && vec[i-1][j].vert >=1)
    {
        vec[i][j].vert += vec[i-1][j].vert;
        vec[i-1][j].vert++;
    }
    if(i<6 && vec[i+1][j].vert >= 1)
    {
        vec[i][j].vert += vec[i+1][j].vert;
        vec[i+1][j].vert++;
    }
    /*verificam diagonala principala / */
    if((j>0) && (i> 0) && (vec[i-1][j-1].diagP >=1))
    {
        vec[i][j].diagP += vec[i-1][j-1].diagP;
        vec[i-1][j-1].diagP++;
    }
    if((i<6) && (j<7) && vec[i+1][j+1].diagP >=1)
    {
        vec[i][j].diagP += vec[i+1][j+1].diagP;
        vec[i+1][j+1].diagP++;
    }

    //verificam diagonala secundara /
    if((i>0) && (j<7) && vec[i-1][j+1].diagS >= 1)
    {
        vec[i][j].diagS += vec[i-1][j+1].diagS;
        vec[i-1][j+1].diagS++;
    }
    if((i<6) && (j>0) && vec[i+1][j-1].diagS >=1)
    {
        vec[i][j].diagS += vec[i+1][j-1].diagS;
        vec[i+1][j-1].diagS++;
    }

    if(vec[i][j].oriz == 4 || vec[i][j].vert == 4 || vec[i][j].diagP == 4 || vec[i][j].diagS == 4 )
    {
        castig=1;


        scor++;
    }

    printf("Scorul este: %d \n", scor);

    return castig;

}


int Check(int col)
{
    int answer=0;
    int i;
    for(i=5; i>=0; i--)
    {
        if(board[i][col-1]==0)
        {
            board[i][col-1]=color;
            break;
        }
    }
    //printf("this is i: %d \n", i);
    answer= Win(i, col-1);

    return answer;

}
void ClearBoard()
{
    FILE *fptr;
    fptr = fopen("BoardDisplay.txt", "w");

    if (fptr == NULL) {
        // exit status for OS that an error occurred
        exit(1);
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            // printf("%d", matrice[i][j]);
            putw(0, fptr);
        }
    }
    // close connection
    fclose(fptr);

}

void GetBoard()
{       //copiez boardul celuilalt jucator
    FILE *fptr;
    int num;
    fptr = fopen("BoardDisplay.txt", "r");

    if (fptr == NULL) {
        // exit status for OS that an error occurred
        exit(1);
    }

    int i=0;
    int j=0;
    while((num = getw(fptr)) != EOF) {

        if(j==7 && i<6) {
            i++;
            j=0;
        }

        board[i][j] = num;
        j++;

    }


    // close connection
    fclose(fptr);
}

void UpdateBoard()
{
    FILE *fptr;
    fptr = fopen("BoardDisplay.txt", "w");

    if (fptr == NULL) {
        // exit status for OS that an error occurred
        exit(1);
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            // printf("%d", matrice[i][j]);
            putw(board[i][j], fptr);
        }
    }
    // close connection
    fclose(fptr);
}

int login(char* user)
{
    FILE *fp;
    int  ok=0;
    char filecontent[100];

    strcpy(user, user+6);   //extrag doar numele de utilizator din comanda

    if((fp= fopen("usernames.txt", "r"))== NULL) perror("Eroare la deschiderea fisierului");    //deschid fisierul cu useri


    while (fgets(filecontent, 100, fp) != NULL)     //citesc linie cu linie din fisier
    {

        filecontent[strlen(filecontent)-1]=0;       //inlocuiesc ultimul caracter luat de pe linia din fisier(care e \n) cu \0

        if(strcmp(filecontent, user)==0)            //caut username-ul
        {
            ok=1;
            loggedIn=1;
            break;
        }
    }
    fclose(fp);

    return ok;

}

int signUp(char* username)
{
    FILE *fread;
    char filecontent[100];
    int ok=0;
    int already_exists=0;

    strcpy(username, username+8);

    if((fread= fopen("usernames.txt", "r"))== NULL) perror("Eroare la deschiderea fisierului");    //deschid fisierul cu useri


    while (fgets(filecontent, 100, fread) != NULL)     //citesc linie cu linie din fisier
    {

        filecontent[strlen(filecontent)-1]=0;       //inlocuiesc ultimul caracter luat de pe linia din fisier(care e \n) cu \0

        if(strcmp(filecontent, username)==0)            //caut username-ul
        {
            already_exists=1;
            break;
        }
    }
    fclose(fread);

    if(already_exists==0) {
        FILE *fapp = fopen("usernames.txt", "a");
        if (fapp == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }

        // printf("user: ..%s..\n", username);
        fprintf(fapp, "%s\n", username);
        ok = 1;


        fclose(fapp);
        loggedIn = 1;
    }
    return ok;
}


int main () {
    struct sockaddr_in server;    // structura folosita de server
    struct sockaddr_in from;
    char msg[100];        //mesajul primit de la client
    char msgrasp[100] = " ";        //mesaj de raspuns pentru client
    int sd;            //descriptorul de socket
    int count=0;

    int client;

    int length = sizeof(from);

    ClearBoard();
    PutTurn(turn);
    PutSbWon(SbWon);

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 1) == -1) {
        perror("[server]Eroare la listen().\n");
        return errno;
    } else
        printf("Listening...\n");

    /* servim in mod concurent clientii... */
    while (1) {

        /* acceptam un client */
        client = accept(sd, (struct sockaddr *) &from, &length);


        if(client<0)
        {
            exit(1);
        }

        count++;

        printf("Connection accepted from client %d\n", count);

        int pid;
        if ((pid = fork()) == -1) {
            close(client);
            continue;
        } else
        if (pid > 0) {
            // parinte
            close(client);
            while(waitpid(-1,NULL,WNOHANG));
            continue;
        } else
        if (pid == 0) {
            // copil
            close(sd);

            while (1) {

                bzero(msg, sizeof(msg));
                printf ("[server]Asteptam mesajul...\n");
                fflush (stdout);


                /* citirea mesajului */
                if (read (client, msg, 100) <= 0)

                {
                    perror ("[server]Eroare la read() de la client.\n");
                    close (client);	/* inchidem conexiunea cu clientul */
                    continue;		/* continuam sa ascultam */
                }
               printf("Pesaj primit: %s \n", msg);

                if (strncmp(msg, "login ", 6) == 0) {
                    //printf("Sunt in login \n");
                    //preiau usernameul de la client
                    //apoi ii transmit clientului daca s-a reusit logarea sau nu
                   // printf("start: %d\n", loggedIn);
                    int answer;

                    if(loggedIn == 1)       //daca e deja logat, nu mai poate sa faca login iar
                    {
                        strcpy(msg, "You are already logged in! \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));

                    }
                    else {
                        answer = login(msg);
                        if (answer == 1) {
                            strcpy(msg, "You are logged in! \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));
                        } else {
                            strcpy(msg, "User not found, please sign up! \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));
                        }
                    }
                }
                else
                if (strncmp(msg, "sign up ", 8) == 0){
                    //ii cere clientului numele de utilizator pe care vrea sa il foloseasca
                    //apelul functiei care va adauga un nou utilizator
                    //trimit raspuns catre client
                    int user;

                    if(loggedIn == 1)       //daca e deja logat, nu mai poate sa faca sign up iar
                    {
                        strcpy(msg, "You are already logged in! \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));

                    }
                    else {
                        user = signUp(msg);
                        if (user == 0) {
                            strcpy(msg, "Username already exists! \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));

                        } else {
                            strcpy(msg, "Account created, you are now logged in! \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));

                        }
                    }
                }
                else
                if (strcmp(msg, "play") == 0){
                    if( loggedIn ==0)
                    {
                        strcpy(msg, "You need to be logged in to play! \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));
                    }
                    else {

                        //printf("am intrat in play \n");
                        start_game();      //initiem jocul

                        strcpy(msg, "Game started! \n Please choose: colour 1 or 2 \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));
                    }
                }
                else
                if(strncmp(msg, "colour", 6) == 0)
                {
                    if(GameOn == 1) {
                        char colour[2];
                        strcpy(colour, msg + 7);
                        strcpy(msg, msg + 7);         //ii trimit clientului culoarea aleasa

                        color = atoi(colour);
                        GameOn++;
                      //  printf("This is your color: %d", color);


                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));

                    }
                    else{
                        strcpy(msg, "The game hasn't started yet! Select play \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));
                    }


                }
                else
                if(strncmp(msg, "move", 4) == 0)
                {
                    turn=GetTurn();
                    if((GameOn==2) && ((count % 2== 0 && turn % 2 ==0) || (count % 2== 1 && turn % 2 == 1) ))
                    {
                        char move[2];
                        int imove;
                        int CheckAnswer;
                        //int Turn;

                        turn++;

                        PutTurn(turn);


                        strcpy(move, msg+5);     //conversie char* to char
                        imove=atoi(move);       //conversie char to int


                        GetBoard();
                        CheckAnswer=Check(imove);       //apel functie care creaza boardul
                        UpdateBoard();

                        GetSbWon();
                        if(SbWon == 1)
                        {
                            GameOn=0;
                            strcpy(msg, "lose \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));

                        }
                        else
                        if(CheckAnswer == 1)
                        {
                            SbWon=1;
                            PutSbWon(SbWon);
                            GameOn=0;

                            strcpy(msg, "win \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));
                        }
                        else
                        {
                            strcpy(msg, "continue \n");
                            write(client, msg, strlen(msg));
                            bzero(msg, sizeof(msg));
                        }


                    }
                    else
                    {
                        strcpy(msg, "It is not your turn yet, please wait and try again \n");
                        write(client, msg, strlen(msg));
                        bzero(msg, sizeof(msg));
                    }


                }
                else
                if (strcmp(msg, "exit") == 0) {     //comanda de exit=> clientul doreste sa se deconecteze de la server
                    printf("Client %d has disconnected\n",count);
                    break;
                }
                else {     //comanda incorecta


                    strcpy(msg, "Incorrect Command \n");
                    write(client, msg, strlen(msg));
                    bzero(msg, sizeof(msg));
                }
            }


        }

    }/* while */

    close(client);

    return 0;
}			/* main */

