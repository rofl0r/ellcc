// is64 failure.
int main() 
{
    void (*fn) (void);
    void (*f[1])(void);  

    fn = f[1];
    fn();
}
