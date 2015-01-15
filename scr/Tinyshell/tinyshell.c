#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void strstd(char *str) //chuan hoa xau
{
	int j = 0, i;
	for (i = 0; i < strlen (str); i++)
		{
			if (str[i] != ' ' || (str[i] == ' ' && str[i-1] != ' ' && i>0)) //xoa cac ki tu trang dau va giua xau
				{
					str[j] = str[i];
					j++;
				}
		}
	if (str[j - 1] == ' ' || str[i] == '\n') //xoa ki tu trang cuoi xau
		str[j-1] = 0;
	else
		str[j] = 0;
}

char **convert(char *cmd) //Ham tao tham so cho execvp
{
	char *tmp = cmd;
	char **p = (char**) malloc(30 * sizeof (char*));
	int i, t=0, length = strlen (cmd);
	for (i = 0; i <= length; i++)
		{
			if (cmd[i] == ' ') //Chia xau thanh cac tu
				{
					cmd[i] = 0;
					p[t++] = tmp;
					tmp = cmd + i + 1;
				}
			else if (cmd[i] == 0) //Neu den cuoi xau
				{
					p[t++] = tmp;
					p[t] = NULL;
				}
		}
	return p;
}

pid_t spawn (char *cmd)
{
	char **arglist;
	int background = 0, status =0;
	pid_t childPid;
	if (cmd[strlen (cmd) -1] == '&')
	{
		background = 1;
		cmd[strlen (cmd) -1] = 0;
	}
	arglist = convert (cmd);
	childPid = fork ();
	if (childPid == 0) //child process
		{
			execvp (arglist[0], arglist);
			//exec return only if an error occurs
			printf ("An error occurs\n");
			abort ();
		}
	else //main process
		{
			if (background == 1) //background process
				printf ("\nBackground process have created \t %s \n", cmd); 
			else //foreground process
				{
					printf ("\nWating a process ... \t %s \n ", cmd);
					waitpid (childPid, &status, 1);
				}
		}
	return childPid;
}

void batch(char *script)
{
	FILE *fp;
	static char tmp[200];
	if (!(fp = fopen (script, "r")))
		return;
	while (!feof (fp))
	{
		printf("\ntinyshell$ ");
		bzero(tmp,200);
		fgets(tmp,200,fp);
		tmp[strlen(tmp) -1] = 0;
		strstd(tmp);
		if (strcmp (tmp, ""))
		  spawn (tmp);
	}
	fclose (fp);
}

char* isbatchscr (char *cmd)
{
	static char tmp[200];
	int i = 0;
	while (cmd[i] != ' ')
		{
			tmp[i] = cmd[i];
			i++;
		}
	tmp[i] = 0;
	if (!strcmp (tmp, "tinyshell"))
		{
			bzero(tmp, 200);
			for (i = 10; i<strlen (cmd); i++)
				tmp[i-10] = cmd[i];
			return tmp;
		}
	return NULL;
}

int main(void)
{
	printf("\n               TINY SHELL               \n\n");
	char cmd[200];
	while (1)
		{
			printf("tinyshell$ ");
			fgets (cmd, 200, stdin);
			cmd[strlen (cmd) -1] = 0;
			strstd (cmd);
			if (isbatchscr (cmd))
				batch (isbatchscr (cmd));
			else if (strcmp (cmd, ""))
				spawn (cmd);
		}
}
