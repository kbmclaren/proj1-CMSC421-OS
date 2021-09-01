/*
    UMBC CMSC 421
    Spring 2021
    Project 1

    Due Date: 2/28/21 11:59:00 pm

    Author Name:
    Author email:
    Description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

typedef enum {false, true} bool;
const int UNIT = 1;
const int PAIR = 2;
/* const char PROC = 'proc';
const char EXIT = 'exit';  */


/*
    In this project, you are going to implement a number of functions to
    create a simple linux shell interface to perform basic linux commands.
    Please note that the below set of functions may be modified as you see fit.
    We are just giving the following as a suggestion. If you do use our
    suggested design, you *will* need to modify the function signatures (adding
    arguments and return types).
*/


//DEFINE FUNCTION PROTOTYPES (add arguments/return types as needed)
void user_prompt_loop();
char * get_user_command();
//char ** parse_command(char *, char *[]);
void parse_command(char *, char *[]);
// reevaluate
void execute_command( char *[]);
void flush_string( char *);
long returnMyInt( char * );

int main(int argc, char *argv[]) {

	if ( argc > UNIT){
		fprintf(stderr, "The command \"%s\" accepts no arguments.", argv[0]);
		exit(EXIT_FAILURE);
	} else { 
		fprintf(stdout, "The command %s is correctly formatted.\n", argv[0]);
		user_prompt_loop();
	}

	//printf("Do we get to here nemo");
    return 0;
}

/*
    user_prompt_loop():
    Get the user input using a loop until the user exits, prompting the user for
    a command. Gets command and sends it to a parser, then compares the first
    element to the two built-in commands ("proc", and "exit"). If it's none of
    the built-in commands, send it to the execute_command() function. If the
    user decides to exit, then exit 0 or exit with the user given value.
*/
void user_prompt_loop()
{
	char *command;
	bool flag = true;

	while (flag)
	{
		//Prompt for user
		printf("$ ");
		fflush(stdout);
		command = get_user_command();

		//Check for \n here is preferable to running through unescape().
		if( command[0] == '\n'){
				user_prompt_loop();
			}

		//Allocate appropriately sized array, include space for NULL pointer.
		int dam = count_spaces(command);
		char *holdArray[dam + UNIT];

		//Parsing is harder than expected.
		// I will make sure I understand the provided project code in the future.
		parse_command(command, holdArray);

		//no more need for command
		free(command);
		command = NULL;

		//since a token may have tripped shell error to stderr, how do I check for that
		// loop through holdArray and see if we have more than one NULL?
		int e =0;
		for( int i = 0; i < (dam + UNIT) ; i++)
		{
			//count the nulls
			if (holdArray[i] == NULL) {
				e++;
			}
		}

		//check count of nulls returned from parser
		//expect only one if command was correctly formatted
		if(e > UNIT){
			user_prompt_loop();
		}

		//check for proc and exit, else execute_command.
		if ( strcmp(holdArray[0], "proc") == 0)
		{
			//do what proc do
			exit(0);//good exit
		}
		else if ( strcmp(holdArray[0], "exit") == 0)
		{
			if ( dam == UNIT){
				exit(0);
			}
			//dam from count_spaces()
			//"exit ____ ____"
			else if(dam > UNIT)
			{
				if(dam == PAIR)
				{
					long val;
					//"exit ____"
					if(strcmp(holdArray[UNIT], "0") == 0) {
						exit(0);
					}
					else if( (val = returnMyInt(holdArray[UNIT]))  ){
						exit(val);
					}
					else
					{
						user_prompt_loop();
					}
				}
				//"exit ___ ___" just re-prompts.
				else{
					//to many entries after exit, try again.
					user_prompt_loop();
				}
			}

		} else {
			execute_command(holdArray);
		}

		//flag = false; // for dev only
	}
}

long returnMyInt(char * mixedUp)
{
	char holdThis[strlen(mixedUp)+UNIT];
	char * p;
	p = mixedUp;
	int help = 0;
	for(long unsigned i = 0; i < strlen(mixedUp); i++){
		if(isdigit(p[0])){
			//found a number
			holdThis[help] = p[0];
			help++;
		}
		p++;
	}

	//now holdThis holds all the numbers in mixedUp anD nothing else
	//atoi is not encouraged
	p = holdThis;
	return strtol(p, NULL, 10);
}
/*
      loop:
        1. prompt the user to type command by printing "$ " (don't forget the
           space)
        2. get the user input using get_user_command() function
        3. parse the user input using parse_command() function
        Example:
            user input: "ls -la"
            parsed output: ["ls", "-la", NULL]
        4. compare the first element of the parsed output to "proc"and "exit"
        5. if the first element is "proc" then you have to use the /proc file
           system as described in part 2 of the project
            i) concat the full command:
                Ex: user input $ proc /process_id_no/status
                    concated output: /proc/process_id_no/status
            ii) read from the file line by line. you may user fopen() and
                getline() functions
            iii) display the following information according to the user input
                 from /proc
                a) Get the cpu information if the input is /proc/cpuinfo
                - Cpu Mhz
                - Cache size
                - Cpu cores
                - Address sizes
                b) Get the number of currently running processes from
                   /proc/loadavg
                c) Get how many seconds your box has been up, and how many
                   seconds it has been idle from /proc/uptime
                d) Get the following information from /proc/process_id_no/status
                - the vm size of the virtual memory allocated the vbox
                - the most memory used vmpeak
                - the process state
                - the parent pid
                - the number of threads
                - number of voluntary context switches
                - number of involuntary context switches
                e) display the list of environment variables from
                   /proc/process_id_no/environ
                f) display the performance information if the user input is
                   /proc/process_id_no/sched
        6. if the first element is "exit" the use the exit() function to
           terminate the program
        7. otherwise pass the parsed command to execute_command() function
        8. free the allocated memory using the free() function
    */

    /*
        Functions you may find useful here (not an exhaustive list):
        get_user_command(), parse_command(), execute_command(), strcmp(),
        strcat(), strlen(), strncmp(), fopen(), fclose(), getline(), isdigit(),
        atoi(), fgetc()
    */


/*
    get_user_command():
    Take input of arbitrary size from the user and return to the
    user_prompt_loop()
*/
char * get_user_command() {
	//FILE * fp; 
	char *listlessLine = NULL;
	size_t len = 0; 
	//"signed size_t" may hold the number -1
	//ssize_t whyRead;

	//getline allocates a buffer for storing the line when I set up
	// listlessLine as Null and len as 0.
	if( getline(&listlessLine, &len, stdin) != -(UNIT)){
		//printf("%zi\n", whyRead);	
	} else {
		perror("get_user_command() -> line 152");
		exit(EXIT_FAILURE);
	}

	//return the buffer with input
	return listlessLine;

	/*
        Functions you may find useful (not an exhaustive list):
        malloc(), realloc(), getline(), fgetc()
    */
}

void flush_string( char * cp){
	char * temp; 
	if( (temp = strchr(cp, '\n')) != NULL)
    *temp = '\0';
}

/*******************************************************************/
/*
    parse_command():
    Take command input read from the user and parse appropriately.
    Example:
        user input: "ls -la"
        parsed output: ["ls", "-la", NULL]
    Example:
        user input: "echo     hello                     world  "
        parsed output: ["echo", "hello", "world", NULL]
*/
/* Improved parse_command */
void parse_command(char * unparsed, char * tokenArray[] )
{
	int count = 0;
	flush_string(unparsed);

	//should be before unescape
	int check = first_unquoted_space(unparsed);

	//printf("%d", check);

	while (check != -(UNIT)) {
		char *cp;
		cp = unparsed;

		//reassign unparsed to point to space between inputs, and change to null terminator.
		unparsed = &unparsed[check];
		*unparsed = '\0';

		//now that cp points to a c string, assign to tokenArray
		tokenArray[count] = cp;
		count++;

		//now cp points to complete c string at start of input
		//cp now pointe to null terminator at the end of lead string.
		//iterate unparsed to start of what is left of input
		unparsed++;
		check = first_unquoted_space(unparsed);
	}

	//ALL other spaces in user input are protected by quotations.
	// or no spaces left in string.
	// or "\0" is the only thing in input - want to check for return early
	if (check == -(UNIT)) {
		tokenArray[count] = unparsed;
		count++;
	}

	//add last token "NULL"
	tokenArray[count] = NULL;

	//tokenizing is finished
	//check each token for illegal escape sequence
	//OR check for illegal escapes back in prompt loop
	int i = 0;
	while (tokenArray[i] != NULL) {
		tokenArray[i] = unescape(tokenArray[i], stderr);
		/*if (tokenArray[i] == NULL) {
 			//I would like to interrupt checking each token
			break;
		}*/
		i++;
	}
}

/*
    execute_command():
    Execute the parsed command if the commands are neither proc nor exit;
    fork a process and execute the parsed command inside the child process
*/
 void execute_command(char * tokenArray[]) {
	 pid_t pid;
	 int status;

	 if( (pid = fork()) == 0 ){
		 //child executes command
		 execvp(tokenArray[0], tokenArray);

	 }
	 else if (pid > 0 ){
		 //parent waits
		 waitpid(pid, &status, 0);

	 }
	 else if (pid == -(UNIT)){
		 fprintf(stderr, "Fork error in execute_command()");
		 return;
	 }

	 /*
	    Functions you may find useful (not an exhaustive list):
	    fork(), execvp(), waitpid(), wait()
	*/
}


