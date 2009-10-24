 // A function pointer.
 char getpagesize ();
 char (*f) () = getpagesize;
 
 int
 main ()
 {
 return f != getpagesize;
   ;
   return 0;
 }

