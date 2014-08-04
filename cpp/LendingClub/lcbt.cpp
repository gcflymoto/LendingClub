#ifndef _GNU_SOURCE
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#endif
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define handle_error_en(en, msg)                                        \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


extern int lcmain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    int s = 0;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);

    pthread_t thread = pthread_self();

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0) {
        handle_error_en(s, "pthread_getaffinity_np");
    }
    
    return lcmain(argc, argv);
}

