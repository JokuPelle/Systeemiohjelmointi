//	File: wish.c
//	Date: 16.7.2019
//    Course: CT30A3370 - Käyttöjärjestelmät ja systeemiohjelmointi
//	Desc: This is a simple Unix Shell (Wisconsin Shell). This shell can be used in
//	      interactive mode or batch mode, if an output file is provided.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// Function declarations:
void fixline(char* line);
char **lineToArgs(char* line);
void execute(char** args);
void wish_cd(char** args);
void wish_path(char**args);
void command(char** args);
int numOfCommands(char **args);
char **getComArgs(char **all);
char *fileOutput(char** args);

//******************************************************************
// Main-function
// Purpose: Turns on batch mode, reads user's input or batch file's lines, turns them into arguments and executes them.
// Uses: fixline, lineToArgs, execute
int main(int argc, char **argv) {
	char error_message0[] = "Error: Reading input failed.\n";
	char error_message1[] = "Error: Too many arguments.\n";
	char error_message2[] = "Error: File couldn't be opened.\n";
	char *line;
	size_t size = 0;
	char** args;
	FILE *file;
	ssize_t read;
	bool batch_mode = false;
	
	// Too many arguments
	if (argc > 2) {
		write(fileno(stderr), error_message1, strlen(error_message1));
		exit(1);
	// Inputfile
	} else if (argc == 2) {
		if ((file = fopen(argv[1], "r")) == NULL) {
			write(fileno(stderr), error_message2, strlen(error_message2));
			exit(1);
		} else batch_mode = true;
	}
	
	setenv("PATH", "/bin", 1);
	while(1) {
		if (batch_mode) {
			if ((read = getline(&line,&size,file)) == -1) {
				return 0;
			}
		} else {
			printf("wish> ");
			if ((read = getline(&line,&size,stdin)) == -1) {
				write(fileno(stderr), error_message0, strlen(error_message0));
			}
		}
		strtok(line,"\n");
		fixline(line);	
		args = lineToArgs(line);
		execute(args);
		free(args);
	}
	return 0;
}

//******************************************************************
// fixline
// Purpose: Adds spaces around redirection and parallel command operators and removes tabs from the input-line.
// Used by: main
void fixline(char* line) {
	int end;
	for (int i=0; line[i] != '\0'; i++) {
		if (line[i] == '\t') line[i] = ' ';
		end = i+1;
	}
	for (int i=0; line[i] != '\0'; i++) {
		if (line[i] == '>' || line[i] == '&'){
			end += 2;
			for (int j=end; j > i+2; j--) {
				line[j] = line[j-2];
			}
			if (line[i] == '>') {
				line[i] = ' ';
				line[i+1] = '>';
			} else {
				line[i] = ' ';
				line[i+1] = '&'; }
			line[i+2] = ' ';
			i += 2;
		}
	}
}

//******************************************************************
// lineToArgs
// Purpose: Converts fixed input-line into arguments.
// Used by: main
char **lineToArgs(char* line) {
	char error_message8[] = "Error: Memory allocation failed.\n";
	char **tokens;
	if ((tokens = malloc(100 * sizeof(char*))) == NULL) {
		write(fileno(stderr), error_message8, strlen(error_message8));
		exit(1);
	}
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

//******************************************************************
// execute
// Purpose: Calls the functions for built-in-commands and other commands based on given arguments.
// Used by: main
// Uses: wish_cd, wish_path, command
void execute(char** args) {
	if (strcmp(args[0], "exit") == 0) {
		free(args);
		exit(0);
	} else if (strcmp(args[0], "cd") == 0) {
		wish_cd(args);
	} else if (strcmp(args[0], "path") == 0) {
		wish_path(args);
	} else {
		command(args);
	}
}

//******************************************************************
// wish_cd
// Purpose: Executes the cd-function.
// Used by: execute
void wish_cd(char** args) {
	char error_message3[] = "Error: cd requires exactly one argument.\n";
	char error_message4[] = "Error: Directory couldn't be found.\n";
	if (args[1] == NULL || args[2] != NULL) {
		write(STDERR_FILENO, error_message3, strlen(error_message3));
	} else {
		if (chdir(args[1]) != 0) {
			write(STDERR_FILENO, error_message4, strlen(error_message4));
		}
	}
}

//******************************************************************
// wish_path
// Purpose: Executes the path-function.
// Used by: execute
void wish_path(char** args) {
	char pathname[30] = "";
	for (int i=1; args[i] != NULL; i++) {
		strcat(pathname, args[i]);
		strcat(pathname, ":");
	}
	if (args[1] != NULL) {
		pathname[strlen(pathname)-1] = '\0';
	}
	setenv("PATH", pathname, 1);
}

//******************************************************************
// command
// Purpose: Executes non-built-in-commands. Formats arguments into proper commands. Creates a child process for each parallel command given.
// Used by: execute
// Uses: numOfCommands, getComArgs, fileOutput
void command(char** args) {
	char error_message5[] = "Error: Child process creation failed.\n";
	char error_message6[] = "Error: Redirection file could't be opened.\n";
	char error_message7[] = "Error: Command is incorrect or couldn't be executed.\n";
	pid_t childpid, wpid;
	//get number of commands
	int numOfComs = numOfCommands(args);
	for (int i=0; i<numOfComs; i++) {
		//separate args into command arguments, get redirection file, create child process
		char **comargs = getComArgs(args); 
		char *redirectFile = fileOutput(comargs);
		childpid = fork();
		if (childpid == -1) {
			write(fileno(stderr), error_message5, strlen(error_message5));
		} else if (childpid == 0) {
			//redirection
			if (redirectFile != NULL) {
				int fd;
				// WRONLY = Write-only, CREAT = create, TRUNC = clear, IRUSR & IWUSR = rights
				if ((fd = open(redirectFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
					write(fileno(stderr), error_message6, strlen(error_message6));
				} else {
					dup2(fd, fileno(stdout));
					dup2(fd, fileno(stderr));
					close(fd);
				}
			}
			//execute the command inside fork
			if (execvp(comargs[0], comargs) == -1) {
				write(fileno(stderr), error_message7, strlen(error_message7));
				exit(0);
			}
		}
	}
	while ((wpid = wait(0)) > 0); //wait for all child processes to complete.
}

//******************************************************************
// numOfCommands
// Purpose: Counts the number of parallel commands.
// Used by: command
int numOfCommands(char **args) {
	int num = 1;
	for (int i=0; args[i] != NULL; i++) {
		if (strcmp(args[i], "&") == 0) num++;
	}
	return num;
}

//******************************************************************
// getComArgs
// Purpose: Separates arguments into different command calls and removes the '&' arguments.
// Used by: command
char **getComArgs(char **all) {
	char **tokens = malloc(12 * sizeof(char*));
	int i=0;
	while (all[0] != NULL) {
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

//******************************************************************
// fileOutput
// Purpose: Returns the redirection file from command.
// Used by: command
char *fileOutput(char** args) {
	char error_message8[] = "Error: Multiple redirection symbols in one command.\n";
	char error_message9[] = "Error: Too many redirection arguments.\n";
	char *o = NULL;
	for (int i=0; args[i] != NULL; i++) {
		if (strcmp(args[i], ">") == 0) {
			if (args[i+1] != NULL && args[i+2] == NULL) {
				if (strcmp(args[i+1], ">") == 0) {
					write(STDERR_FILENO, error_message8, strlen(error_message8));
					return NULL; }
				o = args[i+1];
				for (i=i;args[i] != NULL; i++) {
					args[i] = NULL;
				}			
				break;
			} else {
				write(STDERR_FILENO, error_message9, strlen(error_message9));
				return NULL; }
		}
	} return o;
}

//------------------------------------------------------
//-EOF-
