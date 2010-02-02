/** @file
 * This file defines the interface to the message handling library.
 *
 * @author Richard Pennington
 * @date January 28, 2009
 */

#ifndef _message_h_
#define _message_h_

#include <sys/types.h>

/** The (qpaque) connection type.
 */
typedef void* CONNECTION;

/** Initialize the message handling library.
 * This function must be called in an application before
 * any other part of the message library is used.
 * This function may be called any number of times.
 */
void initMessage(void);

/** Register a named local service.
 * This function registers a service that is capable of receiving
 * messages and making external connections.
 */
CONNECTION registerService(const char *name, int me);

/** Wait for the next message.
 * This function returns when the next complete message has been placed
 * in the service's receive queue.
 */
unsigned char *getMessage(void);

/** Release a received buffer.
 * All buffers returned my get message should be released if they
 * are to be reused.
 */
void releaseBuffer(unsigned char* data);

/** Get a new buffer.
 * This function returns a buffer of the given size, suitable
 * to use as a parameter to sendBuffer() and sendBufferTo().
 */
unsigned char *getBuffer(size_t size);

/** Connect to a named service.
 * This function connects to a service, local or remote.
 */
CONNECTION connectTo(const char *to);

/** Set up a local domain listen socket for future connections.
 */
void localListen(void);

/** Set up a TCP/IP listen socket for future connections.
 */
void tcpListen(int port);

/** Send a buffer to a named service.
 * This function sends a buffer, allocated by getBuffer()
 * to the specified named service.
 */
CONNECTION sendBufferTo(const char *to, unsigned char *buffer);

/** Send a buffer to a connection.
 * This function sends a buffer, allocated by getBuffer()
 * to the specified previously opened connection.
 */
CONNECTION sendBuffer(CONNECTION c, unsigned char *buffer);

/** Send an message to a named service.
 * This function sends a message to the specified named service.
 */
CONNECTION sendMessageTo(const char *to, unsigned char *message, size_t size);

/** Send an message to connection.
 * This function sends a message to the specified connection.
 */
CONNECTION sendMessage(CONNECTION c, unsigned char *message, size_t size);

/** Register a named non-local service.
 */
int registerRemoteService(const char *name);

#endif
