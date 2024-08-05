#include <kernel/thread.h>
#include <kernel/cpu/cpu.h>
#include <kernel/spinlock.h>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* waitpid */
#include <stdio.h>     /* printf, perror */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* exit, fork */
#include <pthread.h>

static volatile int shared_counter = 0;
static spinlock_t lock = SPIN_STATIC_UNLOCKED;

static void *thread_func(void *arg) {
    int iterations = 10;
    for (int i = 0; i < iterations; i++) {
        spin_lock(&lock);
        int local_counter = ++shared_counter;
        spin_unlock(&lock);

        printf("CPU %d, iteration %d, shared_counter: %d\n", cpu_get_id(), i, local_counter);
    }
    return NULL;
}

void start_thread_on_cpu(int cpu_id) {
    struct thread *t = thread_create(THREAD_FLAG_SUSPENDED, thread_func, NULL);
    if (!t) {
        printf("Failed to create thread on CPU %d\n", cpu_id);
        return;
    }

    cpu_bind(cpu_id, t);
    thread_launch(t);
}

int count;
char b = '\n';
void *
thd_1(void *arg){
	char c = '1';
	for(int i = 0; ; i++){
		if((i % 10000000) == 0){
			count++;
			if(count % 70 == 0) write(2,&b,1);

			write(2,&c,1);
		}
	}
}

void *
thd_2(void * arg){
	char c = '2';
	for(int i = 0; ; i++){
		if((i % 10000000) == 0){
			count++;
			if(count % 70 == 0) write(2,&b,1);

			write(2,&c,1);
		}
	}
}

void *
thd_3(void * arg){
	char c = '3';
	for(int i = 0; ; i++){
		if((i % 10000000) == 0){
			count++;
			if(count % 70 == 0) write(2,&b,1);

			write(2,&c,1);
		}
	}
}
int main(void) {
	struct thread * tid1, *tid2, *tid3;
	
	sched_lock();
	tid1 = thread_create(0,thd_1,0);
	tid2 = thread_create(0,thd_2,0);
	tid3 = thread_create(0,thd_3,0);

	//sched_affinity_set(&tid1->schedee.affinity,1);
	//sched_affinity_set(&tid2->schedee.affinity,1);
	//sched_affinity_set(&tid3->schedee.affinity,1);

	thread_launch(tid1);
	thread_launch(tid2);
	thread_launch(tid3);

	sched_unlock();

	thread_join(tid1,NULL);
	thread_join(tid2,NULL);
	thread_join(tid3,NULL);
}
