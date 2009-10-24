// A struct assignment.
struct foo {
}; 

static struct foo current_foo;

int main()
{
 struct foo local_foo;

 local_foo = current_foo;
}
