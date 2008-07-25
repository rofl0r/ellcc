// Fails for ARM.
struct foo {
 int i;  
}; 
 
extern struct foo f(void);

struct foo
main(void)
{
 return f();
}
