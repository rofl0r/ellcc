/** @file
 * This file defines a Rhapsody/Message bridge.
 *
 * @author Richard Pennington
 * @date February 4, 2009
 */

#ifndef _rhapbridge_h_
#define _rhapbridge_h_

#include "message.h"
#include "OMEvent.h"
#include "PCLEvent.h"

unsigned char *rhapToMsg(OMEvent *event, size_t size, uint32_t& code, size_t& psize);

OMEvent *msgToRhap(unsigned char *message, size_t size, uint32_t code);

unsigned char *pclToMsg(PCLEvent *event, size_t size, uint32_t& code, size_t& psize);

PCLEvent *msgToPCL(unsigned char *message, size_t size, uint32_t code);

#endif
