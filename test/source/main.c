void _exit(int status)
{
    for ( ;; ) ;
}
void exit(int status)
{
    _exit(status);
}
int main(int argc, char** argv)
{
}
