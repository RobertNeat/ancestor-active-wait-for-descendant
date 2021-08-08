#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

//fork
#include <sys/types.h>

//wait
#include <sys/wait.h>

pid_t available;
int childExitStatus;

void giveBack(pid_t child_pid){
    available=child_pid;
}

void sig_chld_service(int sig){
    if(available!=0){
        waitpid(available,&childExitStatus,0);
        printf("Child exit status reading:%d\n",childExitStatus);
        exit(1);
    }
    if(available==0){
        //child proces ends, forward child exit status to parent
        exit(0);
    }
}

int main(){
    pid_t child_pid;
    child_pid = fork();

if(child_pid<0){
    printf("Something bad!!\n");
}

    giveBack(child_pid);//return pid for public use
    signal(SIGCHLD,sig_chld_service);

    printf("Signals are blocked.\n");
    sigset_t blocked, occured;
    sigemptyset(&blocked);
    sigaddset(&blocked,SIGCHLD);
    sigprocmask(SIG_BLOCK,&blocked,NULL);
    
    int counter=0;
    while(1){
        //printf("My pid:%d\n",getpid());

        if(child_pid!=0 && child_pid>0){
        printf("I wait for child to end <my pid:%d>\n",getpid());
        sleep(1);
        }

        if(child_pid==0){
            printf("Child pid:%d\n",getpid());
            sleep(1);
        }

        counter++;
        if(counter==8){//15
            sigpending(&occured);
            if(sigismember(&occured,SIGCHLD))
                printf("You tried close using SIGCHLD\n");
            sigprocmask(SIG_UNBLOCK,&blocked,NULL);
            printf("Signals unlocked\n");
        }
    }
    return 0;
}