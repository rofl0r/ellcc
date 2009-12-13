bool EStop = false;                             // A global condition.
__event__ evEStop();                            // A global event declaration.

//__active__
 class Switch {
   __initial__ __state__ Off {
       __onentry__ power = false;
       case evOn(): if (!EStop) { turnOn(); goto On; }
   }
   __state__ On {
       __onentry__ {
           power = true;   // Entry actions.
           timer = 0;
       }
       case evEStop():                         // Handle evEStop also.
       case evOff(): { turnOff(); goto Off; }  // Applies to every nested state.
       // A nested state machine. This could be replaced with, e.g.
       // __state__ DoBlink;
       // which could contain these states and could be defined elsewhere.
       __initial__ __state__ BlinkOn {
           __timer__ 500;                      // Set a .5 second timeout.
           __onentry__ indicator = true;
           __timeout__: goto BlinkOff;
       }
       __state__ BlinkOff {
           __timer__ 500;
           __onentry__ indicator = false;
           __timeout__: goto BlinkOn;
       }
       __onexit__ indicator = false;           // Exit action.
   } and {                                     // The ever favourite and state.
       __timer__ 1000;                         // Count power on time.
       __timeout__: ++timer;                   // Internal transition, use
                                               // __timeout__: ++timer; goto On;
                                               // for an external transition.
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

