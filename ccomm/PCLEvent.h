#ifndef _PCLEvent_h_
#define _PCLEvent_h_

/** A PCL event as seen on the wire.
 */
class PCLWireEvent {
    /** The event code.
     */
    uint32_t code;
    /** The event size.
     * The total size is the size of the event header plus the size of the
     * event payload.
     */
    uint32_t size;
    /** Options.
     */
    uint32_t options;
    /** Pad to provide 8-byte alignment for 64bit field.
     */
    uint32_t pad;
    /** The time the event was sent.
     * This time is in nanoseconds from Jan. 1, 1970.
     */
    int64_t sent;

public:
    PCLWireEvent(uint32_t code = 0)
        : code(code), size(0), options(0), pad(0), sent(0) { }
    uint32_t getId() { return code; }
};

/** A PCL event as seen in memory.
 */
class PCLEvent : public PCLWireEvent {
    /** The time the event was received.
     * This time is in nanoseconds from Jan. 1, 1970.
     */
    int64_t received;
    /** The time the event processing was completed.
     * This time is in nanoseconds from Jan. 1, 1970.
     */
    int64_t executed;
    /** The sender's name.
     */
    const char *sid;
    /** The receiver's name.
     */
    const char *rid;

public:
    PCLEvent(uint32_t code = 0)
        : received(0), executed(0), sid(0), rid(0), PCLWireEvent(code) { }
};

#endif
