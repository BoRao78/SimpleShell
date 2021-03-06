# SimpleShell
CS3113: Customized simple shell in linux


Project 1c - myshell
CS3113 Project Exercise
Bo Rao 
113417729 rao0004
Nov./20/2017

Name

	myshell - A shell of course project written by C to interpret and execute command line.

Synopsis

	./1c [batchfile]

Description

	This is the final version of previous project 1a and 1b. As the primary imitation of the system shell, myshell can accept command line input and execute both Internal or External Commands/Aliases, can deal with indefinite parameters. It only supports very minimal syntaxes, basic terminal functions and may contain some known or unknown bugs.

Startup and Usage 

	Myshell has already been compiled and users can run 1c to use it. If there are some problems or users want to recompile, simply use "make" for there is a written makefile. Every time changed the 1c.c, use "make clean" to clean and use "make" to recompile. 

	Startup myshell without arguments will enter interactive shell mode, in which user input every line of command and myshell interpret and execute it. Myshell supports several internal commands and other external commands will be passed to OS to execute. Every line of command has indeterminate and unlimited arguments. Myshell will attempt to analyze the command name, parameters and other designators.

	Reversely, startup myshell with [batchfile] will enter batch file execution mode, which read and execute the batch file by lines and execute them like in the interactive shell mode. The batch file is assumed to be located in the shell startup directory.  

Internal Commands:
        
	echo - Print the current command line on the screen, won't be affected by I/O redirection.

	pause - Pause the shell and wait until the user presses "Enter".

	help - Show readme file, print on the screen or output to a file.

	cd - Change the working path to target directory, show the current and previous ones, will influence the process and environment variables.

	clr - Clear the screen, same with the external command "clear".

	dir <directory> - List the current directory contents in <directory>, same with external command "ls -al". <directory> can be absolute path or relative path.

	environ - List all the environment strings. For more about environment, see Extra Info.

	quit - Exit from the program, end myshell.

External Commands:
        
	other command - pass command to OS and execute, accept uncertain number of parameters, use fork-exec() instead of system(). If OS can't execute it, print the command.
        
Other Features:

	I/O redirection - All commands can realize I/O redirection forms as "command arg1 arg2 ... < inputfile > outputfile", except "echo". For more about I/O redirection, see Extra Info.
	
	background execution - If detected "&" in the end of a command line, the parent process will not wait for child process to finish and it returns to the command line prompt immediately, leaving the child process executing in background. For more about background execution, see Extra Info.

Known Bugs
	
	1. [Solved] Once used "&" to execute background, parent process will always not wait for child process, prompt will always appear before output of child process.
	Solution: child process created by exec() won't destroy itself, so the wait() of parent process will wait for the child process of last time, not this time. To solve it, parent process should wait() 2 times if don't wait() last time.

	2. Lack error check in some complex situation, unknown error may occur.

Polishing

	1. Check that arguments exist before accessing them. 
	Use a pointer to make and read every arguments of a command line in order, if the pointer points nothing, the arguments all have been read. Avoid using array locations to access arguments.
 
	2. What happens when Ctrl-C is pressed during a. command execution? b. user entry of commands? c.would SIGINT trapping control this better?
	a. End task and quit myshell.
	b. Leave current input, attached with "^C", then quit myshell.
	c. Better, trapping the Ctrl-C signal SIGINT can do something before exits, like cleanup and transfer return code.

	3. The code should check the existence/permissions of input redirection files?
	Should check them every time attempt to read or write a file, including input and output redirection files.

	4. Check makefile works
	Works well with "make" and "make clean".


Extra Info

	environment variables: 
	Each process has an environment associated with it. The environment strings are usually of the form: name=value. The value of an environmental variable can for example be the location of all executable files in the file system, the default editor that should be used, or the system locale settings. Users new to Linux may often find this way of managing settings a bit unmanageable. However, environment variables provide a simple way to share configuration settings between multiple applications and processes in Linux. In "cd" command, environment variables (strings) "PWD" and "OLDPWD" are changed.

	I/O redirection: 
	There are always three default files open, stdin (the keyboard), stdout (the screen), and stderr (error messages output to the screen). These, and any other open files, can be redirected. Redirection simply means capturing output or content of a file, command, program, script, or even code block within a script and sending it as input to another file, command, program, or script, or reversely assigning a file, etc. as output stream of other files, etc. 
	Command line "programname arg1 arg2 < inputfile > outputfile" will execute the program programname with arguments arg1 and arg2 , the stdin FILE stream replaced by inputfile and the stdout FILE stream replaced by outputfile. With output redirection, if the redirection character is ">" then the outputfile is created if it does not exist and truncated if it does. If the redirection token is ">>" then outputfile is created if it does not exist and appended to if it does.

	background execution:
	Every time myshell execute a task command, it create a child process to execute it and redirect the I/O if in need. Parent processes usually use wait() function to pause until child processes end to ensure that the child process ﬁnishes before the shell gets its next command line. Myshell also provides the option of 'background' operation. i.e. the child is started and then control by-passes the wait function to be immediately passed back to the shell to solicit the next command line. An ampersand ( & ) at the end of the command line indicates that the shell should return to the command line prompt immediately after launching that program.




