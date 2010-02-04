/** @file
 * This file tests a Rhapsody/Message bridge.
 *
 * @author Richard Pennington
 * @date February 4, 2009
 */

#include <stdio.h>
#include "rhapbridge.h"

#if 1

/********************************************************************************************
 * Test program.
 */
class evTest : public OMEvent {
public:
    evTest(int i, float f) : i(i), f(f), OMEvent(0x1234) {}

    int i;
    float f;
};

class EvTest : public PCLEvent {
public:
    EvTest(int i, float f) : i(i), f(f), PCLEvent(0x1234) {}

    int i;
    float f;
};

int main(int argc, char **argv)
{
    char *p = strrchr(argv[0], '/');
    if (p == NULL) {
        p = argv[0];
    } else {
        ++p;
    }

    char name[100];
    strcpy(name, p);
    char *e = name + strlen(name);

    initMessage();
    registerService(name, 1);
    localListen();

    evTest *ev = new evTest(1, 3.14);
    uint32_t id;
    size_t size;

    unsigned char *message = rhapToMsg(ev, sizeof(*ev), id, size);
    delete ev;
    ev = static_cast<evTest*>(msgToRhap(message, size, id));
    printf("OMEvent i = %d, f = %g, id = 0x%x\n", ev->i, ev->f, ev->getId());
    EvTest *pev = static_cast<EvTest*>(msgToPCL(message, size, id));
    printf("PCLEvent i = %d, f = %g, id = 0x%x\n", pev->i, pev->f, pev->getId());
    releaseBuffer(message);
    delete ev;
    delete pev;
}

#else

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
                         (OS_THREAD_DATA_T)getStr(name),
                         0);    /* detach */
        sendBufferTo(name, getBuffer(100));
        OS_THREAD_T id2;
        strcpy(e, "thread2");
        OS_THREAD_CREATE(0, /* No create */
                         &id2, 
                         0, /* priority */
                         10000, /* stack */
                         thread,
                         (OS_THREAD_DATA_T)getStr(name),
                         0);    /* detach */
        void *data;
        pthread_join(id1, &data);
        pthread_join(id2, &data);
        fprintf(stderr, "iteration %d\n", ++i);
    } while (1);
}

#endif
