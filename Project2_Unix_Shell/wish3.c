#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void wish_cd(char** args) {
	char error_message[30] = "An error has occured\n";
	if (args[1] == NULL || args[2] != NULL) {
		write(STDERR_FILENO, error_message, strlen(error_message));
	} else {
		if (chdir(args[1]) != 0) {
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
	}
}

void wish_path(char** args) {
	char pathname[30] = "";
	char* p = pathname;
	for (int i=1; args[i] != NULL; i++) {
		strcat(pathname, args[i]);
		strcat(pathname, ":");
	}
	if (args[1] != NULL) {
		pathname[strlen(pathname)-1] = '\0';
	}
	setenv("PATH", pathname, 1);
}

char *stripOutput(char** args) {
	char *o = NULL;
	for (int i=0; args[i] != NULL; i++) {
		//printf("%d. %s.\n",i, args[i]);
		if (strcmp(args[i], ">") == 0) {
			//printf("!1!\n");
			if (args[i+1] != NULL && args[i+2] == NULL) {
				//printf("!2!\n");
				if (strcmp(args[i+1], ">") == 0) return NULL; 
				o = args[i+1];
				for (i=i;args[i] != NULL; i++) {
					args[i] = NULL;
				}			
				break;
			} else return NULL;
		}
	} return o;
}
//echo hello > text.txt & echo ...
char **separateCom(char **all) {
	char **tokens = malloc(12 * sizeof(char*));
	int i=0;
	while (all[0] != NULL) {
		//printf("single command arg%d: %s.\n",i,all[0]);
		if (strcmp(all[0],"&") == 0) {
			for (int j=0;all[j] != NULL; j++) {
				all[j] = all[j+1];
			}
			break;
		} else {
			tokens[i] = all[0];
			for (int j=0;all[j] != NULL; j++) {
				all[j] = all[j+1];
			}
		}
		i++;
	}
	return tokens;
}

int numOfCommands(char **args) {
	int num = 1;
	for (int i=0; args[i] != NULL; i++) {
		if (strcmp(args[i], "&") == 0) num++;
	}
	return num;
}

//TODO aERRORS
void command(char** args) {
	char error_message[30] = "An error has! occured\n";
	pid_t childpid, wpid;
	int numOfComs = numOfCommands(args);
	//for (int j=0;args[j] != NULL;j++) printf("all arguments %d. %s.\n",j,args[j]);
	for (int i=0; i<numOfComs; i++) {
		char **comargs = separateCom(args);
		//for (int j=0;comargs[j] != NULL;j++) printf("input command arg%d. %s.\n",j,comargs[j]);
		char *redirectFile = stripOutput(comargs);
		childpid = fork();
		if (childpid == -1) printf("There was an error while calling fork()!\n");
		else if (childpid == 0) {
			//-----------fork file redirection//
			if (redirectFile != NULL) {
				// WRONLY = Write-only, CREAT = create, TRUNC = clear
				int fd = open(redirectFile, O_WRONLY | O_CREAT | O_TRUNC);
				dup2(fd, fileno(stdout));
				dup2(fd, fileno(stderr));
				close(fd);
			}
			//-----------running the command inside the fork-----------//
			if (execvp(comargs[0], comargs) == -1) {
				write(fileno(stderr), error_message, strlen(error_message));
				exit(0); //close fork if error
			}
		}
	}
	while ((wpid = wait(0)) > 0);
	printf("ended\n");
}

void execute(char** args) {
	if (strcmp(args[0], "exit") == 0) {
		exit(0);
	} else if (strcmp(args[0], "cd") == 0) {
		wish_cd(args);
	} else if (strcmp(args[0], "path") == 0) {
		wish_path(args);
	} else {
		command(args);
	}
}

char **split_line(char * line){
	char **tokens = malloc(16 * sizeof(char*));
	char *token;
	int position = 0;

	token = strtok(line, " ");
	while (token != NULL) {
		tokens[position] = token;
		position++;
		token = strtok(NULL, " ");
	}
	tokens[position] = NULL;
	return tokens;
}

int main(int argc, char **argv) {
	char error_message[30] = "An error has... occured\n";
	char line[32];
	char *b = line;
	int size = 1;
	char** args;
	FILE *file;
	ssize_t read;
	bool batch_mode = false;
	
	// Too many arguments
	if (argc > 2) {
		printf("Too many arguments.\n");
		exit(0);
	// Inputfile
	} else if (argc == 2) {
		if ((file = fopen(argv[1], "r")) == NULL) {
			printf("Can't find file.\n");
				return 0;
		} else batch_mode = true;
	}
	
	setenv("PATH", "/bin", 1);
	while(1) {
		if (batch_mode) {
			if ((read = getline(&b,&size,file)) == -1) {
				return 0;
			}
		} else {
			printf("wish> ");
			getline(&b,&size,stdin);
		}
		strtok(line,"\n");
		args = split_line(line); //Split lines into argument array
		execute(args);
		free(args);
	}
	return 0;
}
