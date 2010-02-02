/** @file
 * This file handles sending and receiving messages.
 *
 * @author Richard Pennington
 * @date January 28, 2009
 */

#include "message.h"

#if defined(__linux__)
  #include <sys/un.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #define LOCAL_DOMAIN_SOCKETS
  /** The name space in which to create local domain sockets.
   */
  #define LOCAL_SOCKET_PATH "/tmp/message_sockets"

  // Use the protocol local namespace.
  #define PROTOCOL_LOCAL_NAMESPACE
  #define SOCKET_NAME_OFFSET 1
#endif

// #define NDEBUG      // Remove to enable asserts.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>

/********************************************************************************************
 * OS interface.
 */

// Common definitions.
typedef void* OS_THREAD_DATA_T;
typedef OS_THREAD_DATA_T (*OS_THREAD_ENTRY)(OS_THREAD_DATA_T);
#ifndef ERROR
#define ERROR (-1)
#endif
#ifndef FALSE
#define FLASE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/** A cast for the argument pointer to an sockopt call.
 */
#define OS_SOCKOPT_CAST
/** A cast for the argument pointer to an ioctl call.
 */
#define OS_IOCTL_CAST

// Thread definitions.
#include <pthread.h>
/** The thread indentifier type.
 */
typedef  pthread_t OS_THREAD_T;
/** Get a thread's identifier.
 */
#define OS_THREAD_SELF() pthread_self()
/** Cancel a thread.
 * @param id The identifier of the thread to cancel.
 */
#define OS_THREAD_CANCEL(id) pthread_cancel(id)
/** Compare two thread identifiers.
 * @param id1 The first identifier to compare.
 * @param id2 The second identifier to compare.
 */
#define OS_THREAD_EQUAL(id1, id2) pthread_equal(id1, id2)
/** Exit from a thread.
 */
#define OS_THREAD_EXIT(arg) pthread_exit(arg)
/** Create a thread.
 */
static int OS_THREAD_CREATE(int noCreate,
                            OS_THREAD_T* id,
                            int priority,
                            size_t stack,
                            OS_THREAD_ENTRY dispatch,
                            OS_THREAD_DATA_T arg,
                            int detach)
{
    int status;
    pthread_attr_t attr;
    memset(&attr, 0, sizeof(attr));
    status = pthread_attr_init(&attr);
    if (status)
    {
        return ERROR;
    }
#if !defined(__linux__) && !defined(__sunos__)
    status = pthread_attr_setstacksize(&attr, stack);
    // Cannot set the stack size under Linux (there is no need to).
    if (status)
    {
        return ERROR;
    }
#endif

#if !defined(__CYGWIN__) && !defined(__sunos__)
    status = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (status)
    {
        return ERROR;
    }
#endif

    int value = detach ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
    status = pthread_attr_setdetachstate(&attr, value);
    if (status)
    {
        return ERROR;
    }
    if (noCreate)
    {
        // This thread is already active, get its id.
        *id = OS_THREAD_SELF();
    }
    else
    {
        // Create the thread.
        status = pthread_create(id, &attr, dispatch, arg);
        if (status)
        {
            return ERROR;
        }
    }

#if !defined(__sunos__)
    // Don't fiddle with scheduling under SunOS.

    int pval = sched_get_priority_max(SCHED_RR);
    if (priority > pval)
    {
        priority = pval;
    }
    else if (priority < (pval = sched_get_priority_min(SCHED_RR)))
    {
        priority = pval;
    }
    /* Set the thread priority.
     * Under Linux, this call will fail unless the program is suid root.
     * Therefore we will not check the error status.
     */
    struct sched_param param;
    param.sched_priority = priority;
    int policy = SCHED_RR;
    pthread_setschedparam(*id, policy, &param);
#endif

    if (noCreate)
    {
        // Call this thread's entry function.
        dispatch(arg);
    }
    return 0;
}

// Mutex definitions.
/** The mutex type.
 */
typedef pthread_mutex_t OS_MUTEX_T;
/** Initialize a mutex.
 * @param mutex The mutex to initialize.
 */
#define OS_MUTEX_INIT(mutex) pthread_mutex_init(&(mutex), NULL)
/** Lock a mutex.
 * @param mutex The mutex to lock.
 */
#define OS_MUTEX_LOCK(mutex) pthread_mutex_lock(&(mutex))
/** Unlock a mutex.
 * @param mutex The mutex to unlock.
 */
#define OS_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(&(mutex))
/** Destroy a mutex.
 * @param mutex The mutex to destroy.
 */
#define OS_MUTEX_DESTROY(mutex) pthread_mutex_destroy(&(mutex))

#include <semaphore.h>
#include <time.h>

/** The semaphore type.
 */
typedef sem_t OS_SEM_T;
/** Initialize a semaphore.
 * @param sem The semaphore to initialise.
 * @param value The initial semaphore value.
 */
#define OS_SEM_INIT(sem, value) sem_init(&(sem), 0, value)
#define OS_SEM_POST(sem) sem_post(&(sem))
#define OS_SEM_WAIT(sem) sem_wait(&(sem))
#define OS_SEM_TIMEDWAIT(sem, timeout) sem_timedwait(&(sem), timeout)
#define OS_SEM_DESTROY(sem) sem_destroy(&(sem, ))

// Once control.
/** The type of a once control structure.
 */
typedef pthread_once_t OS_THREAD_ONCE_T;
#ifdef __sunos__
// SunOS gives a warning about initializing with PTHREAD_ONCE_INIT. It is initialized to zero, anyway.
#define OS_THREAD_ONCE_INIT {}
#else
/** An initializer for a once control structure.
 */
#define OS_THREAD_ONCE_INIT PTHREAD_ONCE_INIT
#endif
/** Call a function once for multiple threads.
 * @param guard The once guard structure.
 * @param func The function to be called once.
 */
#define OS_THREAD_ONCE(guard, func) pthread_once(guard, func)

#define OS_SOCKLEN_T socklen_t

#if defined(__ECOS)
#include <pkgconf/nios2_hwconfig.h>

static int gethostname(char* name, int namelen)
{
    strncpy(name, GEHC_HOST_NAME, namelen);
    return 0;
}

#else

#include <unistd.h>		// For gethostname();

#endif

/** Signal handling.
 */
#define __USE_POSIX
#define __USE_POSIX199506
#include <signal.h>
#define WAKEUP_SIGNAL SIGUSR2           // The select breakout signal.

/********************************************************************************************
 * String handling.
 */

struct Str {
    struct info {
        struct Str* next;               // The next string in the hash table.
        unsigned references;            // The number of references to the string.
    } s;
    unsigned char data[];               // The string.
};

#define HASH_SIZE   511

static OS_MUTEX_T tableMutex;            // The hash table protection mutex.
static struct Str *table[HASH_SIZE];     // The hash table.

/** Calculate the hash value of a string.
 */
static int hash(const char* string)
{
    unsigned int sum = 5381;
    unsigned char* data = (unsigned char*)string;
    int c;
    while ((c = *data++))
    {
        sum += ((sum << 5) + sum) + c;
    }

    return sum % HASH_SIZE;
}

/** Initialize the string manager.
 */
static void initStr()
{
    OS_MUTEX_INIT(tableMutex);
    for (int i = 0; i < HASH_SIZE; ++i) {
        table[i] = NULL;
    }
}

/** Get a new string.
 */
static const char *getStr(const char *str)
{
    int bin = hash(str);
    OS_MUTEX_LOCK(tableMutex);
    struct Str *p;
    for (p = table[bin]; p; p = p->s.next) {
        if (   (const char*)(p->data) == str
            || strcmp(p->data, str) == 0) {
            // Have a match.
            ++p->s.references;
            OS_MUTEX_UNLOCK(tableMutex);
            return p->data;
        }
    }

    size_t size = strlen(str) + 1;
    p = malloc(size + sizeof(p->s));
    assert(p != NULL && "String malloc failed");
    strcpy(p->data, str);
    p->s.references = 1;
    p->s.next = table[bin];
    table[bin] = p;
    OS_MUTEX_UNLOCK(tableMutex);
    return p->data;   // Return the string pointer.
}

/** Release a string.
 */
static void releaseStr(const char* data)
{
    // Restore the string pointer.
    struct Str *str = (struct Str *)(data - offsetof(struct Str, data));
    if (str->s.references) {
        --str->s.references;
    }
    if (str->s.references) {
        return;
    }

    int bin = hash(data);
    OS_MUTEX_LOCK(tableMutex);
    struct Str *last, *current;
    assert(table[bin] != NULL && "No string in hash bin");
    for (last = NULL, current = table[bin]; current; last = current, current = current->s.next) {
        if (str == current) {
            if (last) {
                last->s.next = current->s.next;
            } else {
                table[bin] = current->s.next;
            }
            OS_MUTEX_UNLOCK(tableMutex);
            free(current);
            return;
        }
    }
    
    assert(1 && "String not found in hash table");
    OS_MUTEX_UNLOCK(tableMutex);
}

/********************************************************************************************
 * Buffer handling.
 */

struct Buffer {
    struct {
        struct Buffer *next;            // The pointer to the next buffer.
        unsigned char *start;           // The start of valid data.
        unsigned char *end;             // The end of valid data.
        size_t size;                    // The size of the buffer.
    } s;
    unsigned char data[];
};

/* The size of the buffer pool bins.
 * The last bin never has anything in it and is used only as
 * a marker for catch-all allocations.
 */
static const size_t bins[] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 0
};

// The the number of buffer pool bins.
#define POOL_SIZE (sizeof(bins) / sizeof(bins[0]))

static OS_MUTEX_T poolMutex;            // The pool protection mutex.
static struct Buffer *pool[POOL_SIZE];  // The buffer pool.

/** Find the apprpriate bin for a given size.
 * If the size is apprpriate for a fixed size bin,
 * it is adjusted appropriately.
 */
static int findBin(size_t *size)
{
    for (int i = 0; i < POOL_SIZE; ++i) {
        if (*size <= bins[i]) {
            // Fits, adjust the size.
            *size = bins[i];
            return i;
        }
    }

    // Keep the original size.
    return POOL_SIZE - 1;      // Not in any bin.
}

/** Initialize the buffer manager.
 */
static void initBuffers()
{
    OS_MUTEX_INIT(poolMutex);
    for (int i = 0; i < POOL_SIZE; ++i) {
        pool[i] = NULL;
    }
}

/** Get a new buffer.
 */
unsigned char *getBuffer(size_t size)
{
    size_t actual = size;
    int bin = findBin(&size);

    OS_MUTEX_LOCK(poolMutex);
    struct Buffer *buffer = pool[bin];
    if (buffer) {
        // Return a preallocated buffer.
        pool[bin] = buffer->s.next;
        OS_MUTEX_UNLOCK(poolMutex);
    } else {
        OS_MUTEX_UNLOCK(poolMutex);
        // Allocate a new buffer.
        buffer = malloc(size + sizeof(buffer->s));
        assert(buffer != NULL && "Cannot allocate buffer");
    }

    buffer->s.size = size;
    buffer->s.next = NULL;
    buffer->s.start = buffer->data;             // Set up valid data pointers.
    buffer->s.end = &buffer->data[actual];
    return buffer->data;   // Return the data pointer.
}

/** Release a buffer.
 */
void releaseBuffer(unsigned char* data)
{
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));
    int bin = findBin(&buffer->s.size);
    if (bin == POOL_SIZE - 1) {
        // Free big buffers.
        free(buffer);
    } else {
        OS_MUTEX_LOCK(poolMutex);
        buffer->s.next = pool[bin];
        pool[bin] = buffer;
        OS_MUTEX_UNLOCK(poolMutex);
    }
}

/** Set the next pointer of a buffer.
 */
static void setNext(unsigned char *data, unsigned char* next)
{
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));
    struct Buffer *nextBuffer = next ? (struct Buffer *)(next - offsetof(struct Buffer, data)) : NULL;
    buffer->s.next = nextBuffer;
}

/** Get the next pointer of a buffer.
 */
static unsigned char *getNext(unsigned char *data)
{
    assert(data != NULL && "NULL pointer passed to getNext");
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));
    return buffer->s.next ? buffer->s.next->data : NULL;
}

/** Expand a buffer.
 */
static unsigned char* expandBuffer(unsigned char *data, size_t size)
{
    assert(data != NULL && "NULL pointer passed to expandBuffer");
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));
    if (size <= buffer->s.size) {
        // No reallocation needed.
        return data;
    }

    unsigned char *newData = getBuffer(size);
    memcpy(newData, buffer->s.start, buffer->s.end - buffer->s.start);
    setNext(newData, getNext(data));
    releaseBuffer(data);
    return newData;
}

/** How much valid data is left in a buffer and where is it?
 */
static unsigned char *queryBuffer(unsigned char *data, size_t *size)
{
    assert(data != NULL && "NULL pointer passed to queryBuffer");
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));

    *size = buffer->s.end - buffer->s.start;
    return buffer->s.start;
}

/** Consume bytes from the beginning of the valid data in a buffer.
 */
static void consumeBuffer(unsigned char *data, size_t size)
{
    assert(data != NULL && "NULL pointer passed to consumeBuffer");
    // Restore the buffer pointer.
    struct Buffer *buffer = (struct Buffer *)(data - offsetof(struct Buffer, data));
    buffer->s.start += size;
}

/********************************************************************************************
 * Message handling.
 */
static OS_THREAD_ONCE_T once = OS_THREAD_ONCE_INIT;     // Initialize once.

struct SSD {                            // Service specific data.
    struct SSD *next;                   // The next service in the service list.
    const char *name;                   // The name of the service.
    OS_THREAD_T id;                     // The thread id, if any.
    OS_SEM_T sem;                       // The message semaphore.
    OS_MUTEX_T qmutex;                  // The input queue mutex.
    unsigned char *queue;               // The input queue.
    unsigned char *last;                // The last message in the input queue.
    fd_set wrfds;                       // Active write file descriptors.
    fd_set rdfds;                       // Active read file descriptors.
    int highest;                        // The highest active file descriptor.
    int localFd;                        // The local listen file descriptor.
    int tcpFd;                          // The tcp listen file descriptor.
    OS_THREAD_T fdid;                   // The file descriptor handling thread.
};
static pthread_key_t key;               // The key for thread specific data.

static OS_MUTEX_T SSDMutex;          // The thread list protection mutex.
static struct SSD *services;            // The global service list.

static OS_MUTEX_T fdMutex;              // The file descriptor table protection mutex.
static struct fd {                      // Per file descriptor information.
    struct SSD *from;                   // The thread owning this connection.
    const char *to;                     // The connection name.
    OS_MUTEX_T qmutex;                  // The output queue mutex.
    unsigned char *queue;               // The next buffer to write.
    unsigned char *last;                // The last buffer in the list.
    unsigned char *read;                // The current read buffer.
    size_t bytesRead;                   // The number of bytes read.
    size_t bytesNeeded;                 // The number of bytes needed.
    union {
        uint32_t size;                  // The size buffer.
        unsigned char array[sizeof(uint32_t)];
    } u;
} *fds[FD_SETSIZE];                     // The array of file descriptor information.

/** The thread cleanup function.
 */
static void onexit(void *data)
{
    struct SSD *SSD = data;             // Get the service specific data.
    // Mark the service as inactive.
    SSD->id = 0;

#if RICH        // Keep defunct service info around. Remove this code.
    // RICH: Should there be a removeService() function?
    // Remove the service from the service list.
    OS_MUTEX_LOCK(SSDMutex);
    struct SSD *lSSD, *nSSD;
    int found = 0;
    for (lSSD = NULL, nSSD = services; nSSD; lSSD = nSSD, nSSD = nSSD->next) {
        if (nSSD == SSD) {
            found = 1;
            if (lSSD) {
                lSSD->next = nSSD->next;
            } else {
                services = nSSD->next;
            }
            break;
        }
    }
    OS_MUTEX_UNLOCK(SSDMutex);
    assert(found && "Service specific data not found");

    // Clean up service specific data.
    releaseStr(SSD->name);
    // Deallocate the input queue.
    unsigned char *p;
    while ((p = SSD->queue)) {
        SSD->queue = getNext(p);
        releaseBuffer(p);
    }
    free(SSD);
#endif
}

/** Initialize the message manager.
 */
static void init(void)
{
    initStr();                          // Initialize the string manager.
    initBuffers();                      // Initialize the buffer manager.
    pthread_key_create(&key, onexit);
    OS_MUTEX_INIT(SSDMutex);
    OS_MUTEX_INIT(fdMutex);
    for (int i = 0; i < FD_SETSIZE; ++i) {
        fds[i] = NULL;
    }
}

void initMessage(void)
{
    OS_THREAD_ONCE(&once, init);
}

/** Create new service specific data.
 */
static struct SSD *newSSD(const char *name)
{
    struct SSD *SSD = malloc(sizeof(struct SSD));
    assert(SSD != NULL && "Can't allocate service specific data");
    SSD->name = name;
    SSD->id = 0;
    OS_SEM_INIT(SSD->sem, 0);
    OS_MUTEX_INIT(SSD->qmutex);
    SSD->queue = NULL;          // No input messages.
    SSD->last = NULL;           // No input messages.
    FD_ZERO(&SSD->wrfds);
    FD_ZERO(&SSD->rdfds);
    SSD->highest = 0;
    SSD->localFd = -1;
    SSD->tcpFd = -1;
    SSD->fdid = 0;
    return SSD;
}

/** Register a local service.
 * This function gives a thread a name and creates the
 * thread specific data needed to manage messages for the thread,
 * if necessary.
 */
CONNECTION registerService(const char *name, int me)
{
    name = getStr(name);
    OS_MUTEX_LOCK(SSDMutex);
    struct SSD *SSD;
    for (SSD = services; SSD; SSD = SSD->next) {
        if (SSD->name == name) {
            // This service is already registered.
            break;
        }
    }

    if (SSD == NULL) {
        SSD = newSSD(name);
        SSD->next = services;
        services = SSD;
    }

    OS_MUTEX_UNLOCK(SSDMutex);

    if (me) {
        // Take ownership of the service.
        pthread_setspecific(key, SSD);
        SSD->id = OS_THREAD_SELF();
    }
    return SSD;
}

/** Wait for the next message.
 */
unsigned char *getMessage(void)
{
    struct SSD *SSD = (struct SSD*)pthread_getspecific(key);
    assert(SSD != NULL && "getMessage() called by an unregistered thread");

    // Wait for activity.
    OS_SEM_WAIT(SSD->sem);
    OS_MUTEX_LOCK(SSD->qmutex);
    unsigned char *data = SSD->queue;
    assert(data != NULL && "No data in the queue after wakeup");
    SSD->queue = getNext(data);
    setNext(data, NULL);
    if (SSD->queue == NULL) {
        SSD->last = NULL;
    }
    OS_MUTEX_UNLOCK(SSD->qmutex);
    return data;
}

/** Define an empty signal handler.
 */
static void handler(int arg)
{
}

/** Handle signals in the file descriptor handling threads.
 */
static void catchWakeup()
{
    // Set up the wakeup signal handler.
    struct sigaction action;
     
    action.sa_handler = handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction (WAKEUP_SIGNAL, &action, NULL);
    sigaction (SIGPIPE, &action, NULL);
}

/** Have a new fd, check if highest needs a change.
 */
static void setHighest(struct SSD *SSD, int fd)
{
    if (fd < SSD->highest) {
        return;
    }

    SSD->highest = fd + 1;
}

/** Closed a fd, check if highest needs a change.
 */
static void checkHighest(struct SSD *SSD, int fd)
{
    FD_CLR(fd, &SSD->wrfds);
    FD_CLR(fd, &SSD->rdfds);
    if (fd < SSD->highest - 1) {
        return;
    }

    for (fd = SSD->highest; fd; --fd) {
        if (FD_ISSET(fd, &SSD->rdfds) || FD_ISSET(fd, &SSD->wrfds)) {
            SSD->highest = fd + 1;
            return;
        }
    }
    assert(1 && "Closed the last file descriptor held by this service");
}

/** Write a buffer to an active write file descriptor.
 */
static void writeBuffer(struct SSD *SSD, int fd)
{
    // Send as much of the buffer as possible.
    OS_MUTEX_LOCK(fds[fd]->qmutex);
    if (fds[fd]->queue) {
        // There is more data to write.
        // RICH: TODO Check send result, adjust buffer.
        char *buffer = fds[fd]->queue;
        fds[fd]->queue = getNext(fds[fd]->queue);
        size_t size;
        char *rest = queryBuffer(buffer, &size);
        size_t sent = write(fd, &size, sizeof(uint32_t));
        assert(sent == sizeof(uint32_t) && "Short write");
        sent = write(fd, rest, size);
        assert(sent == size && "Short write");
        releaseBuffer(buffer);
    }
    
    if (fds[fd]->queue == NULL) {
        // There is no more data to write.
        OS_MUTEX_LOCK(fds[fd]->from->qmutex);
        FD_CLR(fd, &fds[fd]->from->wrfds);
        OS_MUTEX_UNLOCK(fds[fd]->from->qmutex);
    } 
    OS_MUTEX_UNLOCK(fds[fd]->qmutex);
}

static void *fdHandler(void *arg);

/** Create or wake up the fd thread.
 */
static void fdThread(struct SSD *SSD)
{
    if (SSD->fdid) {
        // The thread has been created, signal it.
        pthread_kill(SSD->fdid, WAKEUP_SIGNAL);
    } else {
        OS_THREAD_CREATE(0, /* No create */
                         &SSD->fdid, 
                         0, /* RICH: Better choice? priority */
                         4096, /* stack */
                         fdHandler,
                         SSD,
                         1);    /* detach */
    }
}

/** Set up a file descriptor.
 */
static void fdSetup(struct SSD *SSD, const char *to, int fd)
{
    if (fds[fd] == NULL) {
        fds[fd] = malloc(sizeof(*fds[fd]));
        assert(fds[fd] != NULL && "Can't allocate a new connection");
    }

    fds[fd]->from = SSD;
    fds[fd]->to = to;
    OS_MUTEX_INIT(fds[fd]->qmutex);
    fds[fd]->queue = NULL;
    fds[fd]->last = NULL;
    fds[fd]->read = NULL;               // No read buffer.
    fds[fd]->bytesRead = 0;             // No bytes read.
    fds[fd]->bytesNeeded = sizeof(uint32_t);
    fds[fd]->u.size = 0;
    OS_MUTEX_LOCK(fds[fd]->qmutex);
    // Set the appropriate file descriptors.
    OS_MUTEX_LOCK(SSD->qmutex);
    FD_SET(fd, &SSD->rdfds);
    setHighest(SSD, fd);
    // Create or wake up the fd thread.
    fdThread(SSD);
    OS_MUTEX_UNLOCK(SSD->qmutex);
    OS_MUTEX_UNLOCK(fds[fd]->qmutex);
}

/** Read a buffer from an active read file descriptor.
 */
static void readBuffer(struct SSD *SSD, int fd)
{
    // Read data from the file descriptor, send a complete buffer to the service.
    // check for a closed connection, discard partially written buffers.
    if (fd == SSD->localFd || fd == SSD->tcpFd) {
        // Someone is making a connection, accept it.
        struct sockaddr_in address;
        OS_SOCKLEN_T addrlen = sizeof(address);
        int newFd = accept(fd, (struct sockaddr *)&address, &addrlen);
        assert(newFd >= 0 && "Accept of connection failed");
        in_addr_t addr = ntohl(address.sin_addr.s_addr);

        int arg;
        arg = 1;
        arg = ioctl(newFd, FIONBIO, OS_IOCTL_CAST&arg);
        assert (arg != -1 && "can't set connecting socket FIONBIO");
        // Set up the new file descriptor.
        fdSetup(SSD, NULL, newFd);

        // Turn off the Nagle algorithm.
        int flag = 1;
        setsockopt(newFd, IPPROTO_TCP, TCP_NODELAY, OS_SOCKOPT_CAST&flag, sizeof(int));
        return;
    }

    struct fd *fp = fds[fd];
    size_t size;
    unsigned char *buffer;
    if (fp->read == NULL) {
        // Reading the size.
        size = sizeof(uint32_t) - fp->bytesRead;
        buffer =  &fp->u.array[fp->bytesRead];
    } else {
        size = fp->bytesNeeded - fp->bytesRead;
        buffer = &fp->read[fp->bytesRead];
    }

    // Get the next data available.
    ssize_t rsize = read(fd, buffer, size);
    if (rsize <= 0) {
        // Either an error occured or the connection closed.
        OS_MUTEX_LOCK(SSD->qmutex);
        checkHighest(SSD, fd);
        OS_MUTEX_UNLOCK(SSD->qmutex);

        // Break the connection.
        OS_MUTEX_LOCK(fp->qmutex);
        fp->from = NULL;
        fp->to = NULL;
        // Get ready for the next message.
        if (fp->read) {
            // Release a partially filled buffer.
            releaseBuffer(fp->read);
        }
        // Release any queued output.
        while (fp->queue) {
            unsigned char *buffer = fp->queue;
            fp->queue = getNext(fp->queue);
        }
        fp->last = NULL;
        fp->bytesRead = 0;
        fp->bytesNeeded = sizeof(uint32_t);
        fp->read = NULL;
        OS_MUTEX_UNLOCK(fp->qmutex);
        close(fd);
        return;
    }

    fp->bytesRead += size;
    if (fp->bytesRead >= fp->bytesNeeded) {
        if (fp->read == NULL) {
            // We have a size, get a buffer for the body.
            fp->read = getBuffer(fp->u.size);
            fp->bytesRead = 0;
            fp->bytesNeeded = fp->u.size;
            return;
        }
        
        // The buffer is filled, pass it on.
        OS_MUTEX_LOCK(SSD->qmutex);
        if (SSD->last) {
            setNext(SSD->last, buffer);
        } else {
            SSD->queue = buffer;
        }
        SSD->last = buffer;
        OS_MUTEX_UNLOCK(SSD->qmutex);
        OS_SEM_POST(SSD->sem);
        
        // Get ready for the next message.
        fp->bytesRead = 0;
        fp->bytesNeeded = sizeof(uint32_t);
        fp->read = NULL;
    }
}

/** The file descriptor handling thread.
 */
static void *fdHandler(void *arg)
{
    struct SSD *SSD = arg;              // Get the thread specific data.

    // RICH: TODO Find any writeable file descriptors and start writing.

    for ( ;; ) {
        // Grab the interesting file descriptors.
        OS_MUTEX_LOCK(SSD->qmutex);
        int highest = SSD->highest;
        fd_set wfds = SSD->wrfds;
        fd_set rfds = SSD->rdfds;
        OS_MUTEX_UNLOCK(SSD->qmutex);

        assert(highest > 0 && "No active file descriptors for select()");
        // Wait for a file descriptor or signal.
        int active = select(highest, &rfds, &wfds, NULL, NULL);
        if (active < 0) {
            if (errno == EINTR) {
                // The handler was signaled, re-read file descriptors.
                continue;
            }

            fprintf(stderr, "select failed(%s) %d, %08X %08X: %s\n",
                    SSD->name, highest,
                    *(unsigned long *)&rfds, *(unsigned long *)&wfds,
                    strerror(errno));
            abort();
        }

        // At least one file descriptor is active.
        for (int fd = 0; active > 0 && fd < highest; ++fd) {
            // Check all active file descriptors.
            if (FD_ISSET(fd, &wfds)) {
                writeBuffer(SSD, fd);
                --active;
            }
            if (FD_ISSET(fd, &rfds)) {
                readBuffer(SSD, fd);
                --active;
            }
        }
    }
}

/** Send a buffer to a connection.
 */
CONNECTION sendBuffer(CONNECTION c, unsigned char *buffer)
{
    if ((struct fd **)c >= &fds[0] && (struct fd **)c < &fds[FD_SETSIZE]) {
        // This is a file descriptor based connection.
        struct fd *fd = *(struct fd **)c;       // Get the descriptor table entry.
        if (fd == NULL) {
            // The connection was lost.
            return NULL;
        }
        
        // Send to a remote service.
        OS_MUTEX_LOCK(fd->qmutex);
        if (fd->last) {
            setNext(fd->last, buffer);
        } else {
            fd->queue = buffer;
        }
        fd->last = buffer;
        // Set the appropriate file descriptors.
        OS_MUTEX_LOCK(fd->from->qmutex);
        int nfd = (struct fd **)c - &fds[0];
        FD_SET(nfd, &fd->from->wrfds);
        FD_SET(nfd, &fd->from->rdfds);
        setHighest(fd->from, nfd);
        fdThread(fd->from);
        OS_MUTEX_UNLOCK(fd->from->qmutex);
        // Create or wake up the fd thread.
        OS_MUTEX_UNLOCK(fd->qmutex);
        return fd;
    } else {
        // Send to a local service.
        struct SSD *SSD = c;
        OS_MUTEX_LOCK(SSDMutex);
        OS_MUTEX_LOCK(SSD->qmutex);
        if (SSD->last) {
            setNext(SSD->last, buffer);
        } else {
            SSD->queue = buffer;
        }
        SSD->last = buffer;
        OS_MUTEX_UNLOCK(SSD->qmutex);
        OS_SEM_POST(SSD->sem);
        OS_MUTEX_UNLOCK(SSDMutex);
        return SSD;
    }
}

#ifdef LOCAL_DOMAIN_SOCKETS
/** Make the name of a local domain socket.
 */
static void makeSocketName(char *path, const char *name/* , char* protocol */)
{
#ifdef PROTOCOL_LOCAL_NAMESPACE
    *path++ = '\0';	// Use the protocol abstract name space.
#endif
    strcpy(path, LOCAL_SOCKET_PATH);
    strcat(path, "/");
    strcat(path, name);
#if RICH
    strcat(path, "_");
    strcat(path, protocol);
#endif
}
#endif

/** Connect to a local domain socket.
 */
static int localConnect(const char *name)
{
#ifdef LOCAL_DOMAIN_SOCKETS
    int fd;
    /* Attempt to connect a LOCAL domain socket.
     * This will succeed if the thread is on the local system and has created
     * a LOCAL domain listen socket.
     */

    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        // No local listener.
        return -1;
    }

    // Set to non-blocking mode.
    int arg = 1;
    if (ioctl(fd, FIONBIO, OS_IOCTL_CAST&arg) == -1) {
        return -1; 
    }

    // Set up the socket address.
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    makeSocketName(address.sun_path, name /* , protocol */);
    if (connect(fd, (struct sockaddr*)&address,
                strlen(address.sun_path + SOCKET_NAME_OFFSET)
                + 1 + sizeof(address.sun_family)) >= 0) {
        // The connection has been established.
        return fd;
    }
    
    // Can't connect.
    close(fd);
#endif
    return -1;
}

/** Connect to a TCP/IP socket.
 */
static int tcpConnect(const char *name)
{
    // RICH: TODO Look up service, make connection.
    return -1;
}

/** Connect to a named service.
 */
CONNECTION connectTo(const char *to)
{
    to = getStr(to);

    // Check for a local service.
    for (struct SSD *SSD = services; SSD; SSD = SSD->next) {
        if (SSD->name == to) {
            // Have a matching local service.
            return SSD;
        }
    }
    
    // Check for an existing connection.
    struct SSD *SSD = (struct SSD*)pthread_getspecific(key);
    assert(SSD != NULL && "A non-service is attempting to make a connection");
    for (int i = 0; i < FD_SETSIZE; ++i) {
        if (   fds[i] != NULL
            && fds[i]->to == to
            && fds[i]->from == SSD) {
            // A connection is open already.
            return &fds[i];
        }
    }

    int fd;
    // Try to open a local domain socket.
    fd = localConnect(to);              // Check for a local domain listener.
    if (fd < 0) {
        // We don't have a local listener, try to open a TCP/IP socket.
        fd = tcpConnect(to);
    }

    if (fd >= 0) {
        // Set up the connection.
        fdSetup(SSD, to, fd);
        return &fds[fd];
    }

    // This thread is not active yet, register it and try again.
    return registerService(to, 0);
}

/** Set up a local domain listen socket for future connections.
 */
void localListen(void)
{
#ifdef LOCAL_DOMAIN_SOCKETS
    struct SSD *SSD = (struct SSD*)pthread_getspecific(key);

    if (SSD->localFd > 0) {
        // Already listening.
        return;
    }

    struct sockaddr_un address;
    int bindResult;
    int count;
    int fd;		// The listen socket.

    // Create a LOCAL domain listen socket.
    fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    assert (fd >= 0 && "can't create a local domain socket");

    /* Bind listen socket to listen port.  First set various fields in
     * the address structure, then call bind().
     */
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    makeSocketName(address.sun_path, SSD->name /* , protocol */);

    // Set the reuse address socket option
    count = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &count, sizeof(count));

    bindResult = bind(fd, (struct sockaddr *) &address,
                      strlen(address.sun_path + SOCKET_NAME_OFFSET) + 1 + sizeof(address.sun_family));
    if (bindResult >= 0)
    {
        int arg;
        arg = 1;
        arg = ioctl(fd, FIONBIO, OS_IOCTL_CAST&arg);
        assert (arg != -1 && "can't set listen socket FIONBIO");

        // Listen for connections.
        listen(fd, 5);

        // Wake up on new connections.
        fdSetup(SSD, NULL, fd);
        SSD->localFd = fd;
    }
    else
    {
        // If the address is already in use, we can still use it.
        assert(errno == EADDRINUSE && "Error opening local listen socket");
        // No local domain socket available, this is a noop.
    }

#else
    // No local domain sockets available, this is a noop.
#endif
}

/** Set up a TCP/IP listen socket for future connections.
 */
void tcpListen(int port)
{
    struct SSD *SSD = (struct SSD*)pthread_getspecific(key);

    if (SSD->localFd > 0) {
        // Already listening.
        return;
    }

    struct sockaddr_in address;
    int bindResult;
    int count;
    int fd;		// The listen socket.

    // Create a TCP listen socket.
    fd = socket(AF_INET, SOCK_STREAM, 0);
    assert (fd >= 0 && "can't create a TCP/IP socket");

    /* Bind listen socket to listen port.  First set various fields in
     * the address structure, then call bind().
     */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
#if defined(__ECOS) || defined(_VXWORKS_)
    address.sin_len = sizeof(address);
#endif

    // Set the reuse address socket option
    count = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, OS_SOCKOPT_CAST&count, sizeof(count));

    // Wait forever to get a connection socket.
    for (count = 0; ; ++count)
    {
        bindResult = bind(fd, (struct sockaddr *) &address, sizeof(address));

        if (bindResult >= 0)
        {
            int arg;
            arg = 1;
            arg = ioctl(fd, FIONBIO, OS_IOCTL_CAST&arg);
            assert (arg != -1 && "can't set listen socket FIONBIO");

            // Listen for connections.
            listen(fd, 5);

            // Wake up on new connections.
            fdSetup(SSD, NULL, fd);
            SSD->localFd = fd;
            break;
        }
        else
        {
            printf("bind failure on TCP port %d, attempt %d: %s\n", port, count, strerror(errno));
            sleep(1);
        }
    }
}

/** Send a buffer to a named service.
 */
CONNECTION sendBufferTo(const char *to, unsigned char *buffer)
{
    to = getStr(to);
    // Search for local services.
    OS_MUTEX_LOCK(SSDMutex);
    for (struct SSD *SSD = services; SSD; SSD = SSD->next) {
        if (SSD->name == to) {
            OS_MUTEX_LOCK(SSD->qmutex);
            if (SSD->last) {
                setNext(SSD->last, buffer);
            } else {
                SSD->queue = buffer;
            }
            SSD->last = buffer;
            OS_MUTEX_UNLOCK(SSD->qmutex);
            OS_SEM_POST(SSD->sem);
            OS_MUTEX_UNLOCK(SSDMutex);
            return SSD;
        }
    }
        
    OS_MUTEX_UNLOCK(SSDMutex);

    // Search for services in other processes or remote systems.
    CONNECTION c = connectTo(to);
    if (c) {
        // We have a connection, send the message.
        c = sendBuffer(c, buffer);
    }
    if (c == 0) {
        // The send failed. Release the buffer.
        releaseBuffer(buffer);
    }
        
    return c;
}

/** Send an message to a named service.
 */
CONNECTION sendMessageTo(const char *to, unsigned char *message, size_t size)
{
    unsigned char *buffer = getBuffer(size);
    memcpy(buffer, message, size);
    CONNECTION c = sendBufferTo(to, buffer);
    if (c == 0) {
        // The send failed. Release the buffer.
        releaseBuffer(buffer);
    }
    return c;
}

/** Send an message to connection.
 */
CONNECTION sendMessage(CONNECTION c, unsigned char *message, size_t size)
{
    unsigned char *buffer = getBuffer(size);
    memcpy(buffer, message, size);
    c = sendBuffer(c, buffer);
    if (c == 0) {
        // The send failed. Release the buffer.
        releaseBuffer(buffer);
    }
    return c;
}

/********************************************************************************************
 * Test program.
 */

#ifdef TEST
static void *thread(void *data)
{
    registerService(data, 1);
    const char *me = getStr(data);
    localListen();
    unsigned char *buffer = getBuffer(strlen("Hello world") + 1);
    strcpy(buffer, "Hello world");
    size_t count;
    queryBuffer(buffer, &count);
    printf("buffer size = %u count = %u\n", *(size_t *)(buffer - sizeof(size_t)), count);
    buffer = expandBuffer(buffer, 200);
    printf("buffer size = %u, contains %s\n", *(size_t *)(buffer - sizeof(size_t)), buffer);
    char *nbuffer = getBuffer(30);
    printf("nbuffer size = %u, contains %s\n", *(size_t *)(nbuffer - sizeof(size_t)), nbuffer);
    consumeBuffer(nbuffer, 6);
    buffer = queryBuffer(nbuffer, &count);
    printf("count = %u, contains %s\n", count, buffer);

    const char* s1 = getStr("hi");
    printf("s1 = %s, references = %u\n", s1, *(size_t *)(s1 - sizeof(size_t)));
    const char* s2 = getStr("hi");
    printf("s1 = %s, references = %u\n", s1, *(size_t *)(s1 - sizeof(size_t)));
    printf("s2 = %s, references = %u\n", s2, *(size_t *)(s2 - sizeof(size_t)));
    releaseStr(s2);
    printf("s1 = %s, references = %u\n", s1, *(size_t *)(s1 - sizeof(size_t)));
    releaseStr(s1);
    printf("s1 = %s, references = %u\n", s1, *(size_t *)(s1 - sizeof(size_t)));

    unsigned char *p1 = getBuffer(16);
    unsigned char *p2 = getBuffer(33);
    setNext(p1, p2);
    unsigned char *p3 = getBuffer(129);
    setNext(p2, p3);
    
    unsigned char *p = p1;
    while (p) {
        printf("p size = %u\n", *(size_t *)(p - sizeof(size_t)));
        p = getNext(p);
    }

    printf("%s waiting\n", me);
    char* message = getMessage();
    printf("%s got %p\n", me, message);
    releaseBuffer(message);
    if (me[strlen(me) - 1] == '1') {
        char name[100];
        strcpy(name, me);
        name[strlen(name) - 1] = '2';
        printf("%s sending to %s\n", me, name);
        sendBufferTo(name, getBuffer(30));
    }
}

int main(int argc, char **argv)
{
    catchWakeup();                      // Catch the wakeup signal.
    initMessage();
    char *p = strrchr(argv[0], '/');
    if (p == NULL) {
        p = argv[0];
    } else {
        ++p;
    }

    char name[100];
    strcpy(name, p);
    char *e = name + strlen(name);

    registerService(name, 1);
    struct SSD *SSD = (struct SSD*)pthread_getspecific(key);
    assert(SSD != NULL && "The SSD of main is not set");
    localListen();
    if (strcmp(name, "main") != 0) {
        // Not main, send a message to it.
        sendMessageTo("main", "this is a message", strlen("this is a message") + 1);
    } else {
        char *buffer = getMessage();
        size_t size;
        queryBuffer(buffer, &size);
        printf("got: '%s'\n", buffer);
        releaseBuffer(buffer);
        exit(0);
    }
    int i = 0;
    do {
        OS_THREAD_T id1;
        strcpy(e, "thread1");
        OS_THREAD_CREATE(0, /* No create */
                         &id1, 
                         0, /* priority */
                         10000, /* stack */
                         thread,
                         name,
                         0);    /* detach */
        sendBufferTo(name, getBuffer(100));
        OS_THREAD_T id2;
        strcpy(e, "thread2");
        OS_THREAD_CREATE(0, /* No create */
                         &id2, 
                         0, /* priority */
                         10000, /* stack */
                         thread,
                         name,
                         0);    /* detach */
        void *data;
        pthread_join(id1, &data);
        pthread_join(id2, &data);
        fprintf(stderr, "iteration %d\n", ++i);
    } while (1);
}
#endif
