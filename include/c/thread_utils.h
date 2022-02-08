#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

#include <pthread.h>
#include <stdbool.h>

#define MUTEX_TYPE			  pthread_mutex_t
#define CONDITION_TYPE		pthread_cond_t
#define THREAD_TYPE			  pthread_t

// mutex
#define INIT_MUTEX(mutex) pthread_mutex_init(&(mutex), NULL)
#define DESTROY_MUTEX(mutex) pthread_mutex_destroy(&(mutex))
#define LOCK_MUTEX(mutex) pthread_mutex_lock(&(mutex))
#define UNLOCK_MUTEX(mutex) pthread_mutex_unlock(&(mutex))


/* TEMPORARILY UNUSED

// condition
#define INIT_CONDITION(cond) pthread_cond_init(&(cond), NULL)
#define DESTROY_CONDITION(cond) pthread_cond_destroy(&(cond))
#define WAIT_CONDITION(cond, mutex, timevalue) pthread_cond_timedwait(&(cond), &(mutex), &(timevalue))
#define SIGNAL_CONDITION(cond) pthread_cond_signal(&(cond))
#define BROADCAST_CONDITION(cond) pthread_cond_broadcast(&(cond))

// thread
#define THREAD_ROUTINE(func_name, pArg) void* func_name(void* pArg)
#define THREAD_ROUTINE_RET_CODE() NULL
#define CREATE_THREAD(thread, pFunc, pArg) pthread_create(&thread, NULL, pFunc, (void*)(pArg))
#define JOIN_THREAD(thread)	pthread_join(thread, NULL)

// clang-format off
#define GENERATE_TIMESPEC(deltaMs)                        							\
		struct timespec ts = { 0 }; 																				\
		clock_gettime(CLOCK_REALTIME, &ts); 																\
		unsigned long long TARGET_NS = ts.tv_nsec + deltaMs * NS_PER_MS; 		\
		ts.tv_sec += TARGET_NS / NS_PER_SEC; 																\
		ts.tv_nsec = TARGET_NS % NS_PER_SEC;
// clang-format on

*/


#ifdef __cplusplus
extern "C" {
#endif

	bool mutex_init(MUTEX_TYPE* mutex);
	bool mutex_destroy(MUTEX_TYPE* mutex);
	bool mutex_lock(MUTEX_TYPE* mutex);
	bool mutex_unlock(MUTEX_TYPE* mutex);

/* TEMPORARILY UNUSED 

	bool condition_init(CONDITION_TYPE* cond);
	bool condition_destroy(CONDITION_TYPE* cond);
	bool condition_signal(CONDITION_TYPE* cond);
	bool condition_broadcast(CONDITION_TYPE* cond);
	bool condition_wait(CONDITION_TYPE* cond, MUTEX_TYPE* mutex, unsigned int timeoutMs);

*/

#ifdef __cplusplus
}
#endif

#endif