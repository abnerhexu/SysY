#include <linux/sched.h>    /* Definition of struct clone_args */
#include <sched.h>          /* Definition of CLONE_* constants */
#include <sys/syscall.h>    /* Definition of SYS_* constants */
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

#define NUM_THREADS 4
#define STACK_SIZE 4096
char stacks[NUM_THREADS][STACK_SIZE];

int thread_create(int num_threads, void *func) {
    int n = num_threads;
    while (n > 0) {
        void *stack_top = stacks[n-1] + STACK_SIZE;
        int pid = clone(func, stack_top, CLONE_VM|CLONE_VFORK, 0);
        n--;
    }
    n = 0;
    while (n < num_threads) {
        wait(NULL);
    }
}

void thread_join(int i, int n) {
    if (i != n) {
        waitid(P_ALL, 0, NULL, WEXITED);
    }
    if (i != 0) {
        _exit(0);
    }
}