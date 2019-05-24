#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_LEN_LINE    30	
#define LEN_HOSTNAME	30


int main(void)
{
    char command[MAX_LEN_LINE]; 
    char *args[] = {command, NULL}; 
    int ret, status; 
    pid_t pid, cpid;

    printf("%c[1;36m", 27);  
    printf("\nMy shell을 시작합니다 !\n");
    
    while (true) {
        char *s; 
	char *program_name[10] = { NULL, };      
	int len;
	int program_count=0;

        char hostname[LEN_HOSTNAME + 1];   
    	memset(hostname, 0x00, sizeof(hostname));
    	gethostname(hostname, LEN_HOSTNAME); 
	
	printf("%c[1;32m", 27);		/* username@hostname$ 형식의 shell prompt */
	printf("\n%s@%s$ ",getpwuid(getuid())->pw_name, hostname); 
	printf("%c[1;0m", 27); 

	s = fgets(command, MAX_LEN_LINE, stdin);
 	
        if (s == NULL) { 
            fprintf(stderr, "fgets failed\n"); 
            exit(1); 
	}
	if (strcmp(command, "exit\n")==0) {		/* exit 입력시 shell 종료 */
		printf("%c[1;31m", 27);  
		printf("exit가 입력되었습니다 ! shell을 종료합니다.\n\n");
		return 0;
	}

	len = strlen(command);
        printf("저장된 길이 : %d\n", len); 
        if (command[len - 1] == '\n') {   
            command[len - 1] = '\0'; 	
        }
        printf("[입력 내용 : %s]\n", command); 
	
	char *filename = strtok(command, ";");	/* ";"을 기준으로 여러개의 프로그램 실행 */
	while (filename != NULL) {  
	    program_name[program_count] = filename;
	    program_count++;
	    filename = strtok(NULL, ";"); 	
	}

	printf("< 실행할 프로그램 : "); 
	int i=0; 
	for (i; i<program_count; i++) {
	    printf("%s ", program_name[i]);
	}
	printf(">\n");

	for (int i=0; i<program_count; i++) {
	    printf("%c[1;34m", 27);  
	    printf("\n>>>");
            printf("%c[1;0m", 27);  
	    printf(" %s 프로그램을 실행합니다.\n", program_name[i]);

            pid = fork(); 
            if (pid < 0) {
                fprintf(stderr, "fork failed\n");
                exit(1);
            } 
            if (pid != 0) {  /* parent */
                cpid = waitpid(pid, &status, 0);  
                if (cpid != pid) { 
                    fprintf(stderr, "waitpid failed\n");        
                }
                printf("Child process terminated\n"); 
                if (WIFEXITED(status)) { 
                    printf("Exit status is %d\n", WEXITSTATUS(status)); 
                }
            }   

           else {  /* child */  
	        ret = execve(program_name[i], args, NULL); 
                if (ret < 0) {
                    fprintf(stderr, "execve failed\n");   
                    return 1; 
                }
	   }
        }
    }
    return 0;
}
