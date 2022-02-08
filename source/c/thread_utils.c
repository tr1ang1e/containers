#include <errno.h>

#include "../../include/c/thread_utils.h"
#include "../../include/c/logging.h"

static void trace_last_error(const char* pMessage)
{
	int errCode = errno;
	ERR("%s (%d - %s)", pMessage, errCode, strerror(errCode));
}


bool mutex_init(MUTEX_TYPE* mutex)
{
	if (INIT_MUTEX(*mutex) != 0)
	{
		trace_last_error("Failed to init mutex");
		return false;
	}

	return true;
}

bool mutex_destroy(MUTEX_TYPE* mutex)
{
	if (DESTROY_MUTEX(*mutex) != 0)
	{
		trace_last_error("Failed to destroy mutex");
		return false;
	}

	return true;
}

bool mutex_lock(MUTEX_TYPE* mutex)
{
	if (LOCK_MUTEX(*mutex) != 0)
	{
		trace_last_error("Failed to lock mutex");
		return false;
	}

	return true;
}

bool mutex_unlock(MUTEX_TYPE* mutex)
{
	if (UNLOCK_MUTEX(*mutex) != 0)
	{
		trace_last_error("Failed to unlock mutex");
		return false;
	}

	return true;
}


/* TEMPORARILY UNUSED

bool condition_init(CONDITION_TYPE* cond)
{
	if (INIT_CONDITION(*cond) != 0)
	{
		trace_last_error("Failed to init condition");
		return false;
	}

	return true;
}

bool condition_destroy(CONDITION_TYPE* cond)
{
	if (DESTROY_CONDITION(*cond) != 0)
	{
		trace_last_error("Failed to destroy condition");
		return false;
	}

	return true;
}

bool condition_signal(CONDITION_TYPE* cond)
{
	if (SIGNAL_CONDITION(*cond) != 0)
	{
		trace_last_error("Failed to signal condition");
		return false;
	}

	return true;
}

bool condition_broadcast(CONDITION_TYPE* cond)
{
	if (BROADCAST_CONDITION(*cond) != 0)
	{
		trace_last_error("Failed to broadcast condition");
		return false;
	}

	return true;
}

bool condition_wait(CONDITION_TYPE* cond, MUTEX_TYPE* mutex, unsigned int timeoutMs)
{
	bool received = false;

	GENERATE_TIMESPEC(timeoutMs);
	int waitResult = WAIT_CONDITION(*cond, mutex, ts);
	received = waitResult == 0;

	return received;
}

*/