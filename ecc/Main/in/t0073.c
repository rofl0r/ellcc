struct foo {
  const char c;
};

extern const char  fix(const char);
struct foo s;

int f()
{
  s.c = fix(s.c);
}
