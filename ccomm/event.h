/** @file
 * This file defines the interface to the event handling library.
 *
 * @author Richard Pennington
 * @date January 28, 2009
 */

#ifndef _event_h_
#define _event_h_

#include <sys/types.h>

/** The (qpaque) connection type.
 */
typedef void* CONNECTION;

/** Initialize the event handling library.
 */
void initEvent(void);

/** Register a named local service.
 */
CONNECTION registerService(const char *name, int me);

/** Wait for the next event.
 */
unsigned char *getEvent(void);

/** Release a received buffer.
 */
void releaseBuffer(unsigned char* data);

/** Get a new buffer.
 */
unsigned char *getBuffer(size_t size);

/** Connect to a named service.
 */
CONNECTION connectTo(const char *to);

/** Set up a local domain listen socket for future connections.
 */
int localListen(const char *name);

/** Set up a TCP/IP listen socket for future connections.
 */
int tcpListen(const char *name, int port);

/** Send a buffer to a connection.
 */
CONNECTION sendBuffer(CONNECTION c, unsigned char *buffer);

/** Send a buffer to a named service.
 */
CONNECTION sendBufferTo(const char *to, unsigned char *buffer);

/** Send an event to a named service.
 */
CONNECTION sendEventTo(const char *to, unsigned char *event, size_t size);

/** Register a named non-local service.
 */
int registerRemoteService(const char *name);

#endif
