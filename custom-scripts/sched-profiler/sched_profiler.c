#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>
#include <stdbool.h>

#define SCHED_LOW_IDLE 7

#define MAX_CHAR 26 // number of characters in the alphabet

/*
 * Function which is executed by each thread. It writes its ID (char) to the global buffer until it is full.
 * @param data the data passed to the thread.
 */
void* run(void *data);

/*
 * Prints the scheduling policy from its ID, and returns whether or not the thread is recognized.
 * @param policy the ID of the policy.
 * @return true if the policy is recognized; false otherwise.
 */
bool print_sched(int policy);

/*
 * Sets the policy with the given ID to the given thread.
 * @param thr the thread whose policy is to be set.
 * @param newpolicy the ID of the policy to set to the thread.
 */
void set_policy(pthread_t *thr, int newpolicy);

/*
 * Prints the given buffer.
 * @param buf the buffer to be printed.
 * @param size the size of the buffer.
 */
void print_buffer(char* buf, int size);

/*
 * Processes and prints the given buffer. It trims substrings of the same character to only one character, prints it,
 * and then also prints how many times each character appeared in the trimmed substring.
 * @param buf the buffer to be processed.
 * @size the size of the buffer.
 */
void print_processed_buffer(char* buf, int size);

/*
 * Releases any resources globally allocated in the program.
 */
void cleanup();

int buffer_size, n_threads, sched_policy_id;

char* buffer;
int buffer_count;

pthread_mutex_t mutex;
pthread_t* threads;
char* thread_data;

int main(int argc, char** argv)
{
	// get command line arguments
	if (argc != 4)
	{
		printf("usage: %s <buffer_size> <n_threads> <sched_policy_id>\n", argv[0]);
		cleanup();
		return 1;
	}
	buffer_size = atoi(argv[1]);
	n_threads = atoi(argv[2]);
	sched_policy_id = atoi(argv[3]);

	// validate number of threads
	if (n_threads > MAX_CHAR)
	{
		printf("error: maximum number of threads is %d\n", MAX_CHAR);
		cleanup();
		return 1;
	}

	// print user selected information
	printf("Allocating buffer of size %d for %d threads to write to\n", buffer_size, n_threads);
	printf("Scheduling policy:\n");
	if (!print_sched(sched_policy_id))
	{
		printf("error: scheduling policy %d is invalid\n", sched_policy_id);
		printf(
			"valid scheduling policies are: %d (LOW IDLE), %d (IDLE), %d (RR), %d (FIFO)\n",
			SCHED_LOW_IDLE, SCHED_IDLE, SCHED_RR, SCHED_FIFO
		);
		cleanup();
		return 1;
	}

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
		set_policy(&threads[i], sched_policy_id);
	}

	// wait for all threads to finish
	for (int i=0; i<n_threads; i++)
		pthread_join(threads[i], NULL);
	
	// show results
	printf("\nThreads finished executing!\n");
	print_buffer(buffer, buffer_count);
	print_processed_buffer(buffer, buffer_count);
	
	cleanup();
	return 0;
}

void* run(void *data)
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

bool print_sched(int policy)
{
	bool valid = true;
	switch(policy){
		case SCHED_FIFO:     printf("\tSCHED_FIFO\n");     break;
		case SCHED_RR:       printf("\tSCHED_RR\n");       break;
		case SCHED_IDLE:     printf("\tSCHED_IDLE\n");     break;
		case SCHED_LOW_IDLE: printf("\tSCHED_LOW_IDLE\n"); break;
		default:
			printf("\tunknown\n");
			valid = false;
	}
	return valid;
}

void set_policy(pthread_t* thr, int newpolicy)
{
	int policy, ret, newpriority;
	struct sched_param param;

	if (newpolicy == SCHED_LOW_IDLE || newpolicy == SCHED_IDLE)
		newpriority = 0;
	else
		newpriority = 1;

	pthread_getschedparam(*thr, &policy, &param);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
}

void print_buffer(char* buf, int size)
{
	for (int i=0; i<size; i++)
		printf("%c", buf[i]);
	printf("\n");
}

void print_processed_buffer(char* buf, int size)
{
	char* trimmed_buf;
	int trimmed_count;

	// step 1: trim substrings of equal characters to only one char and print the result
	trimmed_buf = (char*)malloc(size);
	trimmed_count = 0;
	for (int i=0; i<size; i++)
	{
		char c = buf[i];
		trimmed_buf[trimmed_count++] = c;
		while ((buf[i+1] == c) && (i+1 < size))
			i++;
	}
	print_buffer(trimmed_buf, trimmed_count);

	// step 2: print the number of occurences of each character in the trimmed buffer
	int occurences[MAX_CHAR] = {0};
	for (int i=0; i<trimmed_count; i++)
	{
		char c = trimmed_buf[i];
		occurences[c - 'A']++;
	}
	for (int i=0; i<n_threads; i++)
		printf("%c: %d\n", 'A' + i, occurences[i]);

	// step 3: free allocated resources
	free(trimmed_buf);
}

void cleanup()
{
	pthread_mutex_destroy(&mutex);
	free(buffer);
	free(threads);
	free(thread_data);
}