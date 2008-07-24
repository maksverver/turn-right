#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

void report(FILE *fp, char *what, rlim_t id)
{
    struct rlimit rlim;
    fprintf(fp, "%s (%d): ", what, (int)id);
    if(getrlimit(id, &rlim) != 0)
        fprintf(fp, "getrlimit() failed!\n");
    else
        fprintf(fp, "soft: %d; hard: %d.\n", (int)rlim.rlim_cur, (int)rlim.rlim_max);
}

int main()
{
    report(stderr, "Virtual memory (b)", RLIMIT_AS);
    report(stderr, "CPU time (s)", RLIMIT_CPU);
    report(stderr, "RSS (b)", RLIMIT_RSS);
    report(stderr, "Data limit (s)", RLIMIT_DATA);
    report(stderr, "Stack limit (s)", RLIMIT_STACK);
    return 0;
}
