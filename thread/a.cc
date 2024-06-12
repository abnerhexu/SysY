#include <pthread.h>

void* thread_foo1(void* tid) { int a = 0; a++; }
void* thread_foo2(void* tid) { int b = 1; b--;}
int main() {
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_foo1, NULL);
    pthread_create(&thread2, NULL, thread_foo2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
