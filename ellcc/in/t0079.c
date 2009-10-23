struct demangle_component
{

  union
  {

    struct
    {


      const char *s;
      int len;
    } s_name;


    struct
    {

      const struct demangle_operator_info *op;
    } s_operator;


    struct
    {

      int args;

      struct demangle_component *name;
    } s_extended_operator;


    struct
    {

      enum gnu_v3_ctor_kinds kind;

      struct demangle_component *name;
    } s_ctor;


    struct
    {

      enum gnu_v3_dtor_kinds kind;

      struct demangle_component *name;
    } s_dtor;


    struct
    {

      const struct demangle_builtin_type_info *type;
    } s_builtin;


    struct
    {

      const char* string;

      int len;
    } s_string;


    struct
    {

      long number;
    } s_number;


    struct
    {

      struct demangle_component *left;

      struct demangle_component *right;
    } s_binary;

  } u;
};
struct d_standard_sub_info
{

  char code;

  const char *simple_expansion;

  int simple_len;


  const char *full_expansion;

  int full_len;



  const char *set_last_name;

  int set_last_name_len;
};

static const struct d_standard_sub_info standard_subs[] =
{
  { 't', "std", (sizeof "std") - 1,
    "std", (sizeof "std") - 1,
    0, 0 },
  { 'a', "std::allocator", (sizeof "std::allocator") - 1,
    "std::allocator", (sizeof "std::allocator") - 1,
    "allocator", (sizeof "allocator") - 1 },
  { 'b', "std::basic_string", (sizeof "std::basic_string") - 1,
    "std::basic_string", (sizeof "std::basic_string") - 1,
    "basic_string", (sizeof "basic_string") - 1 },
  { 's', "std::string", (sizeof "std::string") - 1,
    "std::basic_string<char, std::char_traits<char>, std::allocator<char> >", (sizeof "std::basic_string<char, std::char_traits<char>, std::allocator<char> >") - 1,
    "basic_string", (sizeof "basic_string") - 1 },
  { 'i', "std::istream", (sizeof "std::istream") - 1,
    "std::basic_istream<char, std::char_traits<char> >", (sizeof "std::basic_istream<char, std::char_traits<char> >") - 1,
    "basic_istream", (sizeof "basic_istream") - 1 },
  { 'o', "std::ostream", (sizeof "std::ostream") - 1,
    "std::basic_ostream<char, std::char_traits<char> >", (sizeof "std::basic_ostream<char, std::char_traits<char> >") - 1,
    "basic_ostream", (sizeof "basic_ostream") - 1 },
  { 'd', "std::iostream", (sizeof "std::iostream") - 1,
    "std::basic_iostream<char, std::char_traits<char> >", (sizeof "std::basic_iostream<char, std::char_traits<char> >") - 1,
    "basic_iostream", (sizeof "basic_iostream") - 1 }
};

struct d_info
{

  const char *s;

  const char *send;

  int options;

  const char *n;

  struct demangle_component *comps;

  int next_comp;

  int num_comps;

  struct demangle_component **subs;

  int next_sub;

  int num_subs;



  int did_subs;

  struct demangle_component *last_name;



  int expansion;
};

static struct demangle_component *
d_substitution (struct d_info *di, int prefix)
{
  char c;

  if (! ((*((di)->n)) == 'S' ? ((di)->n++, 1) : 0))
    return 0;

  c = ((*((di)->n)) == '\0' ? '\0' : *((di)->n++));
  if (c == '_' || ((c) >= '0' && (c) <= '9') || ((c) >= 'A' && (c) <= 'Z'))
    {
      int id;

      id = 0;
      if (c != '_')
 {
   do
     {
       if (((c) >= '0' && (c) <= '9'))
  id = id * 36 + c - '0';
       else if (((c) >= 'A' && (c) <= 'Z'))
  id = id * 36 + c - 'A' + 10;
       else
  return 0;
       if (id < 0)
  return 0;
       c = ((*((di)->n)) == '\0' ? '\0' : *((di)->n++));
     }
   while (c != '_');

   ++id;
 }

      if (id >= di->next_sub)
 return 0;

      ++di->did_subs;

      return di->subs[id];
    }
  else
    {
      int verbose;
      const struct d_standard_sub_info *p;
      const struct d_standard_sub_info *pend;

      verbose = (di->options & (1 << 3)) != 0;
      if (! verbose && prefix)
 {
   char peek;

   peek = (*((di)->n));
   if (peek == 'C' || peek == 'D')
     verbose = 1;
 }

      pend = (&standard_subs[0]
       + sizeof standard_subs / sizeof standard_subs[0]);
      for (p = &standard_subs[0]; p < pend; ++p)
 {
   if (c == p->code)
     {
       const char *s;
       int len;

       if (p->set_last_name != 0)
  di->last_name = d_make_sub (di, p->set_last_name,
         p->set_last_name_len);
       if (verbose)
  {
    s = p->full_expansion;
    len = p->full_len;
  }
       else
  {
    s = p->simple_expansion;
    len = p->simple_len;
  }
       di->expansion += len;
       return d_make_sub (di, s, len);
     }
 }

      return 0;
    }
}
