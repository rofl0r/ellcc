typedef struct
{
 int i;
 struct
 {
   int j;
 } __value;  
} foo;


int main (foo *state)
{
    *state = ((foo){0, {0}});
}
