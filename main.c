#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

char *argv[20];
int argc;

void mygen()
{
		if (fork() == 0){
			execlp(argv[0],argv[0],NULL);
		}
		wait();
}

void inredirect()
{
		int fd;
		if (fork() == 0){
			fd = open(argv[2],O_RDONLY,0400 );
			close(0);
			dup(fd);
			execlp(argv[0],argv[0],NULL);
		}
		wait();
}

void outredirect()
{
		int fd;
		if (fork() == 0){
			fd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644 );
			close(1);
			dup(fd);
			execlp(argv[0],argv[0],NULL);
		}
		wait();
}

void cmdpipe()
{
		int pipe_fd[2];
		pid_t pid1,pid2;
		pipe(pipe_fd);
		if ((pid1=fork()) == 0){
				close(1);
				dup(pipe_fd[1]);
				execlp(argv[0],argv[0],NULL);
		} else
		{
				close(pipe_fd[1]);
				if  ((pid2=fork())==0){
						close(0);
						dup(pipe_fd[0]);
						execlp(argv[2],argv[2],NULL);
				}
		}
		wait();
}

void readcmd()
{
		int i = 0;
		char  cmd[20],temp[20];
		char *potoarr;
		int j = 0;
		int k = 0;
		potoarr = cmd;
		printf("aji@ice-desktop$ ");
		fgets(cmd, sizeof(cmd), stdin);
		while(*potoarr != '\0'){
			if (*potoarr != ' ' && *potoarr != '\n' ){
				temp[i++] = *potoarr++;
			}else{
				*potoarr++;
				temp[i] = '\0';
				argv[j++] = strdup(temp);
				i = 0;
			}
		}
		argc = j;
}

void callfunction(void)
{
	if (argc < 3)
		mygen();
	else if (argc == 3){
		if (*argv[1] == '>')
			outredirect();
		else if (*argv[1] == '<')
			inredirect();
		else if (*argv[1] == '|')
			cmdpipe();
	}
}

void main()
{
		while(1){
			readcmd();
			callfunction();
		}
}
