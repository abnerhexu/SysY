#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int t4_create() {
    int pid1, pid2, pid3, pid4;
    // Parent is pid0
    pid1 = fork();
    if (pid1 == 0) {
        // thread 1
        pid2 = fork();
        if (pid2 == 0) {
            // thread 2
        }
        else {
            // thread 1
        }
    }
    else {
        // thread 0
        pid3 = fork();
    }
}

int t_wait() {

}

int t_join() {

}