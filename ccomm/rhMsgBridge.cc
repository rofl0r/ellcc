/** @file
 * This file implements a Rhapsody/Message bridge.
 *
 * @author Richard Pennington
 * @date February 4, 2009
 */

#include "message.h"

unsigned char *rhapToMsg(OMEvent *event, size_t size, uint32_t code)
{
}

OMEvent *msgToRhap(unsigned char *message, size_t size, uint32_t code)
{
    // Get enough space for the entire event.
    void *buffer = new unsigned char[sizeof(OMEvent) + size];
    (void)new(buffer) OMEvent(static_cast<IOxfEvent::ID>(code));
    memcpy(buffer + sizeof(OMEvent), message, size);
    return buffer;
}
