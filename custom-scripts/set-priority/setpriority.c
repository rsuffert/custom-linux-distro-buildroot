#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>
#include <stdbool.h>

#define SCHED_LOW_IDLE 7

char* buffer;
int buffer_count;
int buffer_size;
pthread_mutex_t mutex;

void *run(void *data)
{
	char id = *((char*)data);
	bool exit = false;
	while (!exit)
	{
		pthread_mutex_lock(&mutex);

		// write the id to the buffer until it is full
		if (buffer_count < buffer_size)
			buffer[buffer_count++] = id;
		else
			exit = true;

		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void print_sched(int policy)
{
	switch(policy){
		case SCHED_DEADLINE: printf("SCHED_DEADLINE");   break;
		case SCHED_FIFO:     printf("SCHED_FIFO");       break;
		case SCHED_RR:       printf("SCHED_RR");         break;
		case SCHED_NORMAL:   printf("SCHED_OTHER");      break;
		case SCHED_BATCH:    printf("SCHED_BATCH");      break;
		case SCHED_IDLE:     printf("SCHED_IDLE");       break;
		case SCHED_LOW_IDLE: printf("SCHED_LOW_IDLE\n"); break;
		default:             printf("unknown\n");        break;
	}
	printf(
		"PRI_MIN: %d | PRI_MAX: %d\n",
		sched_get_priority_min(policy),
		sched_get_priority_max(policy)
	);
}

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		printf("Invalid priority: MIN: %d, MAX: %d\n", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));

		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);
	printf("current: ");
	print_sched(policy);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
	printf("new: ");
	print_sched(policy);

	return 0;
}

int main(int argc, char **argv)
{
	int n_threads, sched_policy_id;
	pthread_t *threads;
	char* thread_data;

	// get command line arguments
	if (argc != 4)
	{
		printf("usage: %s <buffer_size> <n_threads> <sched_policy_id>\n", argv[0]);
		return 0;
	}
	buffer_size = atoi(argv[1]);
	n_threads = atoi(argv[2]);
	sched_policy_id = atoi(argv[3]);

	// print user selected information
	printf("Allocating buffer of size %d for %d threads to write to\n", buffer_size, n_threads);
	printf("Scheduling policy:\n");
	print_sched(sched_policy_id);

	// allocate memory for the buffer
	buffer_count = 0;
	buffer = (char*)malloc(buffer_size);

	// allocate memory for the threads and the thread data
	threads = (pthread_t*)malloc(n_threads * sizeof(pthread_t));
	thread_data = (char*)malloc(n_threads * sizeof(char));

	// initialize mutex
	pthread_mutex_init(&mutex, NULL);

	// start threads
	for (int i=0; i<n_threads; i++)
	{
		thread_data[i] = 'A' + i; // 'A' for first thread, 'B' for second etc.
		pthread_create(&threads[i], NULL, run, &thread_data[i]);
		setpriority(&threads[i], sched_policy_id, 0); // TODO: prioridade pode ser sempre zero ou deve ser recebida por parâmetro?
	}

	// wait for all threads to finish
	for (int i=0; i<n_threads; i++)
		pthread_join(threads[i], NULL);
	
	// TODO: printar o buffer antes e depois do pós-processamento
	
	// release allocated resources
	pthread_mutex_destroy(&mutex);
	free(buffer);
	free(threads);
	free(thread_data);

	return 0;
}