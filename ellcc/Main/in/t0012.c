// A function like macro expansion when it shouldn't.
#define isnan(x) int
#define P(x) (x)
extern int isnan P(float);

int main()
{
}

