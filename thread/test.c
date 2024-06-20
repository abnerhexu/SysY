#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// 栈的大小
#define STACK_SIZE (1024 * 1024) // 1 MB

// 最大线程数
#define MAX_THREADS 4

// 栈空间和线程ID数组
char thread_stacks[MAX_THREADS][STACK_SIZE];
int thread_pids[MAX_THREADS];

int thread_create(int num_threads, int (*func)(void *)) {
    for (int i = 0; i < num_threads; i++) {
        thread_pids[i] = clone(func, thread_stacks[i] + STACK_SIZE, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, NULL);
        if (thread_pids[i] == -1) {
            return -1;
        }
    }
    for (int i = 0; i < num_threads; i++) {
        if (waitpid(thread_pids[i], NULL, 0) == -1) {
            return -1;
        }
    }
    return 0;
}

// 线程函数示例
int thread_func(void *arg) {
    printf("Hello from thread! PID: %d\n", getpid());
    return 0;
}

int main() {
    int num_threads = 4;
    if (thread_create(num_threads, thread_func) == -1) {
        fprintf(stderr, "Failed to create threads\n");
        return 1;
    }
    printf("All threads finished\n");
    return 0;
}
