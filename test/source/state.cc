bool EStop = false;                             // A global condition.
__event__ evEStop();                            // A global event declaration.

__active__ class Switch {
   -> __state__ Off {
       -> power = false;
       case evOn(): if (!EStop) { turnOn(); -> On; }
   }
   __state__ On {
       -> {
           power = true;                        // Entry actions.
           timer = 0;
       }
       case evEStop():                          // Handle evEStop also.
       case evOff(): { turnOff(); -> Off; }     // Applies to every nested state.
       // A nested state machine. This could be replaced with, e.g.
       // __state__ DoBlink;
       // which could contain these states and could be defined elsewhere.
       -> __state__ BlinkOn {
           -> [500] -> BlinkOff;                // Set a .5 second timeout.
           -> indicator = true;
       }
       __state__ BlinkOff {
           -> [500] -> BlinkOn;
           -> indicator = false;
       }
       ->* indicator = false;                   // Exit action.
   } and {                                      // The ever favourite and state.
       __state__ Timer {
           -> [1000] ++timer;                   /* Count power on time, internal transition.
                                                 * Use -> [1000] { ++timer; -> Timer; }
                                                 * for an external transition.
                                                 */
       }
   }
   Switch() { power = false; indicator = false; timer = 0; }
private:
   bool power;
   bool indicator;
   int timer;
   void turnOn() { power = true; }
   void turnOff() { power = false; }
};

int main(int argc, char** argv)
{
   Switch sw;                  // sw becomes active here.
   // Send targeted events.
   sw.evOn();                  // Queue an event.
   sw.evOff();
                               //
   evEStop();                  // Global event.
                               // sw is destroyed here, including its thread.
}

__state__ Init {
    -> evCall(1, 2, 3);
    case evCallResult(int result): { answer = result; return; }
}
