// Compile and run for every target.
// XFAIL: *
// RUN: %ecc -g -o %t %s && %t
// RUN: %armecc -g -o %t %s && %armrun %t
// RUN: %armebecc -g -o %t %s && %armebrun %t
// RUN: %i386ecc -g -o %t %s && %i386run %t
// RUN: %microblazeecc -g -o %t %s && %microblazerun %t
// RUN: %mipsecc -g -o %t %s && %mipsrun %t
// RUN: %mipselecc -g -o %t %s && %mipselrun %t
// RUN: %ppcecc -g -o %t %s && %ppcrun %t
// FAIL: %ppc64ecc -g -o %t %s && %ppc64run %t
// RUN: %x86_64ecc -g -o %t %s && %x86_64run %t

int comm;
int main()
{
    comm = 6;
}
