struct htab {  
 int *dir;  
};

int f(struct htab *hashp)
{
  hashp->dir[0] = 0;
}
