/** @file
 * This file implements a Rhapsody/Message bridge.
 *
 * @author Richard Pennington
 * @date February 4, 2009
 */

#include "rhapbridge.h"

unsigned char *rhapToMsg(OMEvent *event, size_t size, uint32_t& code, size_t& psize)
{
    code = event->getId();
    psize = size - sizeof(OMEvent);
    unsigned char *buffer = getBuffer(psize);
    memcpy(buffer, event + 1, psize);
    return buffer;
}

OMEvent *msgToRhap(unsigned char *message, size_t size, uint32_t code)
{
    // Get enough space for the entire event.
    OMEvent *event = reinterpret_cast<OMEvent*>(new unsigned char[size + sizeof(OMEvent)]);
    (void)new(event) OMEvent(static_cast<IOxfEvent::ID>(code));
    memcpy(event + 1, message, size);
    releaseBuffer(message);
    return event;
}

unsigned char *pclToMsg(PCLEvent *event, size_t size, uint32_t& code, size_t& psize)
{
    code = event->getId();
    psize = size - sizeof(PCLEvent);
    unsigned char *buffer = getBuffer(psize);
    memcpy(buffer, event + 1, psize);
    return buffer;
}

PCLEvent *msgToPCL(unsigned char *message, size_t size, uint32_t code)
{
    // Get enough space for the entire event.
    PCLEvent *event = reinterpret_cast<PCLEvent*>(new unsigned char[size + sizeof(PCLEvent)]);
    (void)new(event) PCLEvent(code);
    memcpy(event + 1, message, size);
    releaseBuffer(message);
    return event;
}

