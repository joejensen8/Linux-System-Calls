
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>i
#include <time.h>
#include <string.h>

// stores the string "WHALE=x" - Must be global so decrement_whale doesn't destroy the string at function completion
char whale[64];

// this method will input the current WHALE value, and return the new value of whale
void decrement_whale() {
	
	// get integer value of WHALE
	int temp = atoi(getenv("WHALE"));
	// error check atoi
	if (temp == NULL) {
		perror("atoi or getenv did not convert properly.");
		exit(errno);
	}
	temp = temp - 1; // decrease the value of WHALE
	strcpy(whale, "WHALE="); // note: there is no error variable returned by strcpy
	char num[10];

	if (sprintf(num, "%d", temp) < 0) { // copy the WHALE int value into a char array: negative value returned on error
		perror("putenv error occured in decrement whale.");		
		exit(errno);
	}
	// concat whale and num to get "WHALE=x"
	strcat(whale, num); // note: there is no error variable returned by strcat
	
	if (putenv(whale) == 0) { // putenv worked properly.
		
		fflush(stdout);
		printf("(WHALE environment variable is now %s)\n", getenv("WHALE"));
		fflush(stdout);
	
	} else { // sprintf errors..
		perror("sprintf errror occured.");
		exit(errno);
	}
}

void print_info () {
	char hostname[128];
	time_t cur_time;
	time(&cur_time);
	
	fflush(stdout);
	printf("\nProcess ID: %d\n", getpid()); // no error check, always successful
	printf("Parent ID: %d\n", getppid()); // no error check, always successful
	
	fflush(stdout);
		
	if (gethostname(hostname, sizeof hostname) == -1) {
		perror("gethostname failed");
		exit(errno);
	}
	fflush(stdout);
	printf("Host name: %s\n", hostname);
	fflush(stdout);
	
	char *login = getlogin();
	if (login == NULL) {
		perror("getlogin failed.");
		exit(errno);
	}
	fflush(stdout);
	printf("User name: %s\n", login);
	fflush(stdout);
	
	char *time = ctime(&cur_time);
	if (time == NULL) {
		perror("ctime failed.");
		exit(errno);
	}
	fflush(stdout);
	printf("Time: %s", time);	
	fflush(stdout);
	
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL) {
		perror("getcwd failed.");
		exit(errno);
	}
	fflush(stdout);
	printf("Working directory %s\n", getcwd(NULL, 0));
	printf("This is the parent process!\n\n");	
	fflush(stdout);
}

int main (void) {	

	// this call prints all general information.
	print_info();	
	
	// intitialize WHALE to 7 and error check.
	if (putenv("WHALE=7") != 0) {
		perror("WHALE initialize error occured.");
		exit(errno);
	} 
	
	// fork to create C1
	int c1_pid = fork();

	if (c1_pid == -1) {
		perror("fork returned error: ");	
	} else if (c1_pid == 0) { // this is C1
		// print process ID and parent's process ID
		fflush(stdout);
		printf("C1's Process ID: %d\n", getpid());
		printf("C1's Parent ID: %d\n\n", getppid());
		fflush(stdout);
		
		sleep(2);

		fflush(stdout);
		printf("C1: 6 shrimp ");
		fflush(stdout);
		decrement_whale();
		
		sleep(3);

		fflush(stdout);
		printf("C1: 3 shrimp ");
		fflush(stdout);
		decrement_whale();

		sleep(3);
		
		// change directory to root
		if (chdir("/") == -1) {
			perror("changing directory failed.");
			exit(errno);
		}
		// change address space to execute "ls -la" command
		if (execlp("ls", "ls", "-la", NULL) == -1) {
			perror("exec error occured.");
			exit(errno);
		}

	} else { // this is the parent and C2
		// fork again to create second child
		int c2_pid = fork();
		
		if (c2_pid == -1) {
			perror("fork returned error: ");
		} else if (c2_pid == 0) { // this is C2
			fflush(stdout);
			printf("C2's Process ID: %d\n", getpid());
			printf("C2's Parent ID: %d\n\n", getppid());
			fflush(stdout);

			sleep(3);

			fflush(stdout);
			printf("C2: 5 shrimp ");
			fflush(stdout);
			decrement_whale();

			sleep(3);
			
			fflush(stdout);
			printf("C2: 2 shrimp ");
			fflush(stdout);
			decrement_whale();

			sleep(3);

			// print current working directory
			char *cwd;
			cwd = getcwd(NULL, 0);
			if (cwd == NULL) {
				perror("getcwd failed.");
			} else {
				fflush(stdout);
				printf("\nC2 Working Directory: %s\n", cwd);
				fflush(stdout);
			}

		} else { // this is the parent
			sleep(1); // sleep to let C1, C2 print their info
			
			char *env;
			env = getenv("WHALE");
			if (env == NULL) {
				perror("getenv failed.");
				exit(errno);
			}
			printf("P0: 7 shrimp (WHALE environment variable is now %s)\n", env);
			sleep(3);

			fflush(stdout);
			printf("P0: 4 shrimp ");
			fflush(stdout);
			decrement_whale();

			sleep(3);

			fflush(stdout);
			printf("P0: 1 shrimp ");
			fflush(stdout);
			decrement_whale();
	
			fflush(stdout);
			printf("\n");
			fflush(stdout);
	
			// wait for children to compete, then decrement whale one final time.
			wait(NULL);
			wait(NULL); // wait twice so both children terminate
			
			// decrement one last time
			fflush(stdout);
			printf("P0: LAST DECREMENT ");
			fflush(stdout);
			decrement_whale();
		}
	}
	return 0;
}

// TO DO:
// make sure to error check and flush all statements

