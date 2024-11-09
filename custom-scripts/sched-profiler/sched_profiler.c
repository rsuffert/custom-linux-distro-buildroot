#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>
#include <stdbool.h>

// ======================================= CONSTANTS =======================================
/*
 * Low idle scheduling policy ID. This has been defined in the kernel.
 */
#define SCHED_LOW_IDLE 7

/*
 * Number of letters in the alphabet.
 */
#define MAX_CHARS 26

// ======================================= PROTOTYPES =======================================
/*
 * Function which is executed by each thread. It writes its ID (char) to the global buffer until it is full.
 * @param data the data passed to the thread.
 */
void* run(void *data);

/*
 * Prints the scheduling policy from its ID, and returns whether or not the scheduling policy is recognized.
 * @param policy the ID of the policy.
 * @return true if the policy is recognized; false otherwise.
 */
bool print_validate_sched(int policy);

/*
 * Sets the policy with the given ID to the given thread.
 * @param thr the thread whose policy is to be set.
 * @param newpolicy the ID of the policy to set to the thread.
 * @return true if the policy was set successfully; false otherwise.
 */
bool set_sched_policy(pthread_t *thr, int newpolicy);

/*
 * Prints the given buffer without any processing.
 * @param buf the buffer to be printed.
 * @param size the size of the buffer.
 */
void print_raw_buffer(char* buf, int size);

/*
 * Prints the given buffer with post-processing. It trims substrings of the same character to only one character,
 * prints it, and then also prints how many times each character appeared in the trimmed substring.
 * @param buf the buffer to be processed.
 * @size the size of the buffer.
 */
void print_processed_buffer(char* buf, int size);

/*
 * Releases any resources globally allocated in the program.
 */
void cleanup();

// ======================================= GLOBAL VARS =======================================

/*
 * Variables received from the command line.
 * The size of the global buffer the threads will write to, the number of threads, and the scheduling policy ID.
 */
int buffer_size, n_threads, sched_policy_id;

/*
 * The global buffer that the threads will write to.
 */
char* buffer;

/*
 * The number of characters written to the buffer (next available index).
 */
int buffer_count;

/*
 * The mutex used to synchronize access to the buffer.
 */
pthread_mutex_t mutex;

/*
 * The woker threads that will write to the buffer.
 */
pthread_t* threads;

/*
 * A list of the data passed to each thread. Each element is is a char representing the ID of the thread.
 */
char* threads_data;

// ======================================= FUNCTIONS =======================================

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
	if (n_threads > MAX_CHARS)
	{
		printf("error: maximum number of threads is %d\n", MAX_CHARS);
		cleanup();
		return 1;
	}

	// print user selected information
	printf("\nAllocating buffer of size %d for %d threads to write to\n", buffer_size, n_threads);
	printf("Scheduling policy:\n");
	if (!print_validate_sched(sched_policy_id))
	{
		printf("error: scheduling policy %d is invalid\n", sched_policy_id);
		printf(
			"valid scheduling policies are: %d (LOW IDLE), %d (IDLE), %d (RR), and %d (FIFO)\n",
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
	threads_data = (char*)malloc(n_threads * sizeof(char));

	// initialize mutex
	pthread_mutex_init(&mutex, NULL);

	// start threads
	for (int i=0; i<n_threads; i++)
	{
		threads_data[i] = 'A' + i; // 'A' for first thread, 'B' for second etc.
		pthread_create(&threads[i], NULL, run, &threads_data[i]);
		if (!set_sched_policy(&threads[i], sched_policy_id))
		{
			printf("error: failed to set scheduling policy for thread %d\n", i);
			cleanup();
			return 1;
		}
	}

	// wait for all threads to finish
	for (int i=0; i<n_threads; i++)
		pthread_join(threads[i], NULL);
	
	// show results
	printf("\nThreads finished executing!\n");
	print_raw_buffer(buffer, buffer_count); // TODO: why are other threads writing at the very end for the SCHED_LOW_IDLE policy?
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

bool print_validate_sched(int policy)
{
	bool valid = true;

	switch(policy)
	{
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

bool set_sched_policy(pthread_t* thr, int newpolicy)
{
	int policy, ret;
	struct sched_param param;

	// get the current sched policy and params of the thread
	ret = pthread_getschedparam(*thr, &policy, &param);
	if (ret != 0)
	{
		perror("pthread_getschedparam: ");
		return false;
	}

	printf("current policy:");
	print_validate_sched(policy);

	printf("setting to policy:");
	print_validate_sched(newpolicy);

	// set the sched priority based on the new policy
	param.sched_priority = (newpolicy == SCHED_LOW_IDLE || newpolicy == SCHED_IDLE) ? 0 : 1;

	// set the new sched policy and params for the thread
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
	{
		perror("pthread_setschedparam: ");
		return false;
	}

	pthread_getschedparam(*thr, &policy, &param); // TODO: why isn't the policy being correctly set?
	printf("new policy:");
	print_validate_sched(policy);
	
	return true;
}

void print_raw_buffer(char* buf, int size)
{
	for (int i=0; i<size; i++)
		printf("%c", buf[i]);
	printf("\n");
}

void print_processed_buffer(char* buf, int size)
{
	char* trimmed_buf;
	int trimmed_count;

	// trim substrings of equal characters to only one char and print the result
	trimmed_buf = (char*)malloc(size);
	trimmed_count = 0;
	for (int i=0; i<size; i++)
	{
		char c = buf[i];
		trimmed_buf[trimmed_count++] = c;
		while ((buf[i+1] == c) && (i+1 < size))
			i++;
	}
	print_raw_buffer(trimmed_buf, trimmed_count);

	// print the number of occurences of each character in the trimmed buffer
	int occurences[MAX_CHARS] = {0};
	for (int i=0; i<trimmed_count; i++)
	{
		char c = trimmed_buf[i];
		occurences[c - 'A']++;
	}
	for (int i=0; i<n_threads; i++)
		printf("%c: %d\n", 'A' + i, occurences[i]);

	free(trimmed_buf);
}

void cleanup()
{
	pthread_mutex_destroy(&mutex);
	free(buffer);
	free(threads);
	free(threads_data);
}