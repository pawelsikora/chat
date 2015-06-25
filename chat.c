#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#define COMMAND_TUX "tux"

void parse_command(char * command);

int main(int argc, char *argv[])
{
    int host_port;
    int user_port;
    int host_sockfd, user_sockfd;
    int host_address_size, user_address_size;
    int host_read_sockfd, user_write_sockfd;
    int connect_res;
    struct sockaddr_in host_address, user_address;
    fd_set read_set, test_set;

    char char_buffer;
    char user_input[1024];
    int cmd = 0;
    char *cmd_buf = (char *)malloc(200);

    // struct sockaddr_in address;

    user_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (user_sockfd < 0)
    {
        perror("nie moge utworzyc user soketa");
        exit(-1);
    }

    host_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (host_sockfd < 0)
    {
        perror("nie moge utworzyc host soketa");
        exit(-1);
    }

    host_port = atoi(argv[1]);
    printf("Port HOSTA=%d\n", host_port);

    user_port = atoi(argv[2]);
    printf("Port UŻYTKOWNIKA=%d\n", user_port);

    memset(&host_address, 0x00, sizeof(host_address));
    host_address.sin_addr.s_addr = INADDR_ANY;
    host_address.sin_family = AF_INET;
    host_address.sin_port = htons(host_port);

    memset(&user_address, 0x00, sizeof(user_address));
    inet_aton(argv[3], &user_address.sin_addr.s_addr);
    user_address.sin_family = AF_INET;
    user_address.sin_port = htons(user_port);

    if (bind(host_sockfd, (struct sockaddr *) &host_address, sizeof(host_address)) < 0)
    {
        perror("Nie udała się operacja bind na deksryptorze hosta\n");
        exit(-1);
    }

    host_address_size = sizeof(user_address);
    listen(host_sockfd, 10);

    FD_ZERO(&read_set);
    FD_SET(host_sockfd, &read_set);
    FD_SET(0, &read_set);


    do
    {
        connect_res = connect(user_sockfd, (struct sockaddr *) &user_address, sizeof(user_address));
        sleep(1);
    } while(connect_res < 0);

    host_read_sockfd = accept(host_sockfd, (struct sockaddr *)&user_address, &user_address_size);
    if (host_read_sockfd < 0)
    {
        perror("Nie udało się zaakceptować połączenia z deksryptorem hosta");
        exit(-1);
    }

    int flags = fcntl(host_read_sockfd, F_GETFL, 0);
    fcntl(host_read_sockfd, F_SETFL, flags | O_NONBLOCK | O_SYNC);

    flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK | O_SYNC);
    
    flags = fcntl(1, F_GETFL, 0);
    fcntl(1, F_SETFL, flags | O_SYNC);

//		initscr();
//		raw();
//		keypad(stdscr, TRUE);
    while(1)
    {
        int n = 0;
        test_set = read_set;
        select(0xFFFFFFFF, &test_set, NULL, NULL, NULL);

        if (FD_ISSET(host_sockfd, &test_set))
        {
            do
            {
		n = read(host_read_sockfd, &char_buffer, 1);    
		

                if (n == 1)
                {
                    write(1, &char_buffer, 1);

		  if (cmd == 1)
		  {
		  	if (char_buffer == ')'){
				cmd = 0;
				write(1, "ha ha, dostałem komendę do wykonania. ciekawe co powinienem z nią zrobić? :)", 76);
				parse_command(cmd_buf);
				*cmd_buf = '\0';
				
			}
			else
			{
		  		strncat(cmd_buf, (const char *)&char_buffer, 1);  
			}

		}
		    if ( char_buffer == '/')
		    	{
		    	write(1, "\nOtrzymalem '/'!", 16);
			cmd = 1;
			}
		}
		
	    } while(n > 0);
        }

        if (FD_ISSET(0, &test_set))
        {
            do
            {
                n = read(0, user_input, 1);
                if (n > 0)
                {
                    if (user_input[0] == '/')
		    	printf("\nwyslano '/' \n\n");

		    write(user_sockfd, user_input, 1);
                }
            } while(n > 0);
        }
    }

    return 0;
}

void parse_command(char *buffer)
{
	const char cmd1[]= "tux";
	const char cmd2[]= "dir ";
	const char cmd3[]= "nowyplik ";
	int which_cmd = 0;
	char cmd_1[] = "mkdir ";
	char cmd_2[] = "touch ";

	write(1, "\n buffer = ", 11);
	write(1, buffer, strlen(buffer));

	if (strncmp(buffer, cmd2, 4) == 0)
		which_cmd = 2;
	else if (strncmp(buffer, cmd3, 9) == 0)
		which_cmd = 3;
	else if (strncmp(buffer, cmd1, 3) == 0)
		which_cmd = 1;
	

	//int n = strncmp(buffer, cmd1, 3);
	
	if (which_cmd == 1)
		write(1, "\nHa! rozpoznałem komendę tux!\n", 31);
	else if (which_cmd == 2)
		{
		write(1, "\nZrob nowy katalog: ", 21);
		buffer += 4;
		write(1, buffer, strlen(buffer));
		
		strcat(cmd_1, buffer);
		strcat(cmd_1, "/");
		write(1, cmd_1, strlen(cmd_1));
		system(cmd_1);
		}
	else if (which_cmd == 3)
		{
		write(1, "\nStworz nowy plik: ", 20);
		buffer += 9;
		write(1, buffer, strlen(buffer));
		
		strcat(cmd_2, buffer);
		write(1, cmd_2, strlen(cmd_2));
		system(cmd_2);
		}
	else
		write(1, "\nPierdol się :P nie znam takiej komendy\n", 40);
}
