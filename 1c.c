/*
CS 3113 Project 1c
Bo Rao   113417729
Nov./16/2017

Final version of project 1a and 1b.
Accept command line input and execute both Internal or External Commands/Aliases.
Can deal with indefinite parameters.

Internal Commands:
	
	echo - always print the current command line on the screen. 
	pause - pause down and wait until user presses "Enter".
	help - show readme file.
	cd - change the directory of process and environment variables.
	clr - clears the screen
	dir <directory> - list the current directory centents in <directory>
	environ - list all the environment strings
	quit - exits from the program

External Commands:
	
	other command - pass command to OS and execute, accepte uncertain number of parameters, use fork-exec() instead of system(). 
	
	read batch file - read and execute the batch file by lines, use as "./1c <filename>".
	
Other Features:

	All commands can realize I/O redirection forms as "command arg1 arg2 ... < inputfile > outputfile",except "echo". 
	
	If detected "&" in the end of command line, the parent process will not wait for child process to finish and it returns to the command line prompt immediately, leaving the child process executing in background.

To konw more detials, see readme file.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER 1024         //max line buffer
#define MAX_FILENAME 256	//max length of filename
#define MAX_ARGS 64             //max # args
#define SEPARATORS " \t\n"      //token sparators

extern char **environ;		//environment array
char * args[MAX_ARGS];
char ** arg;	
char prompt[MAX_ARGS] = "==>";
char readme_path[MAX_BUFFER] = "readme";
int last_time_wait = 1;

void readBatchFile(char filename[MAX_FILENAME]);
void execcommand();
int checkWait();

int main(int argc, char ** argv){
	
	char buf[MAX_BUFFER];
	//get readme file fullpath
	char *current_dir = (char *)malloc(MAX_ARGS);
	if(!getcwd(current_dir, MAX_ARGS)){
		fprintf(stderr, "Can't get current directory!\n");
		exit(0);
	}   
	strcat(current_dir,"/readme");       
	strcpy(readme_path, current_dir);
	free(current_dir);

	//check and execute batch file
	if(argc > 2) {					// only one batchfile can be read
		fprintf(stderr, "Too many arguments: only one batch file name\n");
	} else if(argc == 2) {
		readBatchFile(argv[1]);			// execute the batchfile
	}

/* keep reading input until "quit" command or eof of redirected input */

	while(!feof(stdin)){

// get command line from input

	
	fputs(prompt, stdout);
	if(fgets(buf, MAX_BUFFER, stdin)){
		
// tokenize the input into args array
		
		arg = args;
		*arg++ = strtok(buf, SEPARATORS);	//tokenize input
		while(*arg++ = strtok(NULL, SEPARATORS));
							//last entry will be NULL
		execcommand();
		
		}
	}
}


void execcommand(){
		if(!last_time_wait)
			wait(NULL); //release zombie child process(if don't wait() last time)
		int dont_wait = checkWait();
		char * argvs[MAX_ARGS];

		if(args[0]){				//if there's anything there
			
// check for internal/external command
			if(!strcmp(args[0], "echo")){	//"echo" command
				arg = args;
				*arg++;
				while(*arg){
					fprintf(stdout, "%s ",*arg++);
				}
				fprintf(stdout, "\n");
				return;
			}
			
			if(!strcmp(args[0], "pause")){	//"pause" command
				getpass("Press Enter to continue...\n");
				return;
			}
		
			if(!strcmp(args[0], "help")){	//"help" command
				argvs[0] = "more";
				argvs[1] = readme_path;
				argvs[2] = NULL;
					
				arg = args;
				int flagr = 0;
				int flagw = 0;
				char *rfile = (char *)malloc(MAX_ARGS);
				char *wfile = (char *)malloc(MAX_ARGS); 
				*arg++;
				while(*arg){
					if(!strcmp(*arg, "<")){
						*arg++;
						//if(!*arg) break;
						flagr = 1;
						strcpy(rfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">")){
						*arg++;
						//if(!*arg) break;
						flagw = 1;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">>")){
						*arg++;
						//if(!*arg) break;
						flagw = 2;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					}				
				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						if(flagr)
							if(!freopen(rfile, "r", stdin))
								fprintf(stdout, "%s can't be open for input redirection.\n",rfile);
						if(flagw == 1)
							if(!freopen(wfile, "w", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						if(flagw == 2) 
							if(!freopen(wfile, "a", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						execvp(argvs[0], argvs);
						fprintf(stderr, "exec error\n");
					default:		// parent
						if (!dont_wait){
							last_time_wait = 1;
							wait(NULL);
						}
						else
							last_time_wait = 0;
				}
				free(rfile);free(wfile);
				return;

			}			
	
			if(!strcmp(args[0], "quit"))	//"quit" command
				exit(0);						

			if(!strcmp(args[0], "clr")){	//"clr" command
				argvs[0] = "clear";
				argvs[1] = NULL;
				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						execvp("clear", argvs);
						fprintf(stderr, "exec error\n");
					default:		// parent
						if (!dont_wait){
							last_time_wait = 1;
							wait(NULL);
						}
						else
							last_time_wait = 0;
				}
				return;
			}
// change current directory and set and change environmrnt strings

			if(!strcmp(args[0], "cd")){	//"cd" command

				//check and change directory
				arg = args;	
				char *current_dir = (char *)malloc(MAX_ARGS);
				char *dir = (char *)malloc(MAX_ARGS);
				if(!getcwd(current_dir, MAX_ARGS)){
					fprintf(stdout, "Error Current Directory!\n");
					return;
					}
				fprintf(stdout, "Previous Directory: %s\n",current_dir);
				*arg++;
				dir = strcpy(dir, *arg);
				if(chdir(dir) == -1){
					fprintf(stdout, "Error Current Directory!\n");
					return;
					}
				fprintf(stdout, "Current Directory: %s\n",getcwd(dir, MAX_ARGS));
				
				//change PWD environ
				char *OLDPWD = (char *)malloc(MAX_ARGS);
				strcpy(OLDPWD, getenv("PWD"));
				setenv("OLDPWD", OLDPWD, 1);	
				setenv("PWD", dir, 1);			
	
				//put name in the shell prompt
				strcpy(prompt, "==>");
				strcat(prompt, dir);
				strcat(prompt, " $ ");
	
				free(current_dir);free(dir);free(OLDPWD);
				return;
			}
			if(!strcmp(args[0], "dir")){	//"dir" command
				arg = args;
				int flagr = 0;
				int flagw = 0;
				int flagdir = 0;
				char *rfile = (char *)malloc(MAX_ARGS);
				char *wfile = (char *)malloc(MAX_ARGS); 
				argvs[0] = "ls";
				argvs[1] = "-al";			
				
				*arg++;
				while(*arg){
					if(!strcmp(*arg, "<")){
						*arg++;
						flagr = 1;
						strcpy(rfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">")){
						*arg++;
						flagw = 1;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">>")){
						*arg++;
						flagw = 2;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					
					argvs[2] = *arg;
					argvs[3] = NULL;
					flagdir = 1;
					*arg++;
					}				


				if(!flagdir)				
					argvs[2] = NULL;

				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						if(flagr)
							if(!freopen(rfile, "r", stdin))
								fprintf(stdout, "%s can't be open for input redirection.\n",rfile);
						if(flagw == 1)
							if(!freopen(wfile, "w", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						if(flagw == 2) 
							if(!freopen(wfile, "a", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);

						execvp("ls", argvs);
						fprintf(stderr, "exec error\n");
					default:		// parent
						if (!dont_wait){
							last_time_wait = 1;
							wait(NULL);
						}
						else
							last_time_wait = 0;

				}
				free(rfile);free(wfile);
				return;
			}
			
			if(!strcmp(args[0], "environ")){	//"environ" command
				arg = args;
				int flagr = 0;
				int flagw = 0;
				char *rfile = (char *)malloc(MAX_ARGS);
				char *wfile = (char *)malloc(MAX_ARGS); 
				*arg++;
				while(*arg){
					if(!strcmp(*arg, "<")){
						*arg++;
						//if(!*arg) break;
						flagr = 1;
						strcpy(rfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">")){
						*arg++;
						//if(!*arg) break;
						flagw = 1;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">>")){
						*arg++;
						//if(!*arg) break;
						flagw = 2;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					}				
				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						if(flagr)
							if(!freopen(rfile, "r", stdin))
								fprintf(stdout, "%s can't be open for input redirection.\n",rfile);
						if(flagw == 1)
							if(!freopen(wfile, "w", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						if(flagw == 2) 
							if(!freopen(wfile, "a", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						char ** env = environ;
						while(*env) printf("%s\n", *env++);//step through environment
						exit(0);
					default:		// parent
						if (!dont_wait){
							last_time_wait = 1;
							wait(NULL);
						}
						else
							last_time_wait = 0;
				}
			
				free(rfile);free(wfile);
				return;
			}

			// else pass command onto OS(if failed to execute, print them out)
			arg = args;
			int flagr = 0;
			int flagw = 0;
			char *rfile = (char *)malloc(MAX_ARGS);
			char *wfile = (char *)malloc(MAX_ARGS); 
			arg++;
			while(*arg){
				if(!strcmp(*arg, "<")){
					arg++;
					flagr = 1;
					strcpy(rfile, *arg);
					}
				if(!strcmp(*arg, ">")){
					arg++;
					flagw = 1;
					strcpy(wfile, *arg);
					}
				if(!strcmp(*arg, ">>")){
					arg++;
					flagw = 2;
					strcpy(wfile, *arg);
					}
				arg++;
				}		
			arg = args;
			int n = 0;	//number of input
			while(*arg++) n++;
			if(flagr && flagw) args[n - 4] = NULL;
			if(flagr || flagw) args[n - 2] = NULL;
			pid_t pid = 0;	
			switch (pid = fork ()) {
				case -1:
					fprintf(stderr, "fork error\n");
				case 0:			// child
						if(flagr)
							if(!freopen(rfile, "r", stdin))
								fprintf(stdout, "%s can't be open for input redirection.\n",rfile);
						if(flagw == 1)
							if(!freopen(wfile, "w", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
						if(flagw == 2) 
							if(!freopen(wfile, "a", stdout))
								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
					
					execvp(args[0], args);
					fprintf(stderr, "exec error\n");//error to execute
					arg = args;
					while(*arg) fprintf(stdout, "%s ", *arg++);
					fputs("\n", stdout);
					exit(0);		
				default:		// parent
					if (!dont_wait){
						last_time_wait = 1;
						wait(NULL);
					}
					else
						last_time_wait = 0;
				}
			free(rfile);free(wfile);
	}
}


void readBatchFile(char filename[MAX_FILENAME]) {
	FILE *fp;
	int lineNo = 1;
	char lineContent[MAX_BUFFER];

	fp = fopen(filename, "r");			// open the batch file
	if(fp == NULL)
		fprintf(stderr, "Batch file does not exists or not readable\n");
	
	while (fgets (lineContent, MAX_BUFFER, fp )) {	// read a line
		printf("line %d: %s", lineNo++, lineContent);		// print the command number
		arg = args;
		*arg++ = strtok(lineContent, SEPARATORS);
		while(*arg++ = strtok(NULL, SEPARATORS));		// tokenize the line
		execcommand();					// run command
		putchar('\n');
	}

	fclose(fp);					// close the file
	exit(0);					// exit when at the end of the file
}

int checkWait(){
	arg = args;
	*arg++;
	char ** arg2;
	while(*arg){
		if(!strcmp(*arg, "&")){
			arg2 = arg;
			*arg2++;
			if(!*arg2){
				*arg = NULL;
				return 1;
			}
			else
				continue;
		}	
		*arg++;
	}
	return 0;
}
