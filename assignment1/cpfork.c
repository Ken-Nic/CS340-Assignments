#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, const char *argv[]) {
	pid_t pid;

	const char *srcfilename = argv[1];
	const char *tgtfilename = argv[2];
	const char *tgt2filename = argv[3];

	
//  Calling fork to generate a new process, we then set it to a variable so that the output of the new child can be accessed.
	pid = fork();
	
//   conditonal covering when the fork fails to execute/something abnormal from the child else we continue on
	if(pid < 0){
		fprintf(stderr,"Fork failed");
		return 1;
	} else if(pid == 0){
//		Setting a variable to hold the return of excelp if there is any
		int ret = execlp("./cpfile.exe","cpfile.exe",srcfilename,tgtfilename,NULL);
		
//		If execlp fails then it will return a -1, capturing that case within the next conditional
		if(ret == -1 ){
//			Print out error and retunr "3" as a errocode for execlp failing to run
			fprintf(stderr,"exceplp failed to execture properly [3]");
			return 3;
		}
	} else {
		wait(NULL);
		printf("Child complete");
	}
	return 0;
}