/*---------------------------------------------------------------------------*\
 *  execute-process.cpp                                                      *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This implementation for execute-process relies on fork(), execv(),       *
 *    wait3(), dup2(), gettimeofday(), and setrlimit().  
\*---------------------------------------------------------------------------*/
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "execute-process.h"
using namespace std;

ProgramInfo execute_process(string name, char *argv[],
                  FILE *input, FILE *output, FILE *errput,
                  unsigned max_cpu_time)
{
    pid_t child_id;
    timeval before, after;
    rlimit time_limit;
    rusage time_taken;
    ProgramInfo r_val;

    gettimeofday(&before, NULL);
    if ((child_id = fork())) {
        if (child_id < 0) throw string("could not open process");
        wait3(&r_val.exit_code, 0, &time_taken);
        gettimeofday(&after, NULL);
        r_val.user_sec  = time_taken.ru_utime.tv_sec;
        r_val.user_usec = time_taken.ru_utime.tv_usec;
        r_val.sys_sec   = time_taken.ru_stime.tv_sec;
        r_val.sys_usec  = time_taken.ru_stime.tv_usec;
        r_val.wall_sec  = after.tv_sec - before.tv_sec;
        if (before.tv_usec > after.tv_usec) {
            --(r_val.wall_sec);
            r_val.wall_usec = 1000000 + after.tv_usec - before.tv_usec;
        } else {
            r_val.wall_usec = after.tv_usec - before.tv_usec;
        }
    } else {
        if (input != NULL) {
            dup2(fileno(input), STDIN_FILENO);
        }
        if (output != NULL) {
            dup2(fileno(output), STDOUT_FILENO);
        }
        if (errput != NULL) {
            dup2(fileno(errput), STDERR_FILENO);
        }
        time_limit.rlim_cur = time_limit.rlim_max = max_cpu_time;
        if (max_cpu_time && setrlimit(RLIMIT_CPU, &time_limit)) {
            throw string("failed to set time limit");
        }
        execv(name.c_str(), argv);
        throw string("failed to execute process");
    }
    return r_val;
}

ProgramInfo execute_process(string name, char *argv[],
                  string input, string output, string errput,
                  unsigned max_cpu_time)
{
    FILE *in = fopen(input.c_str(), "r");
    FILE *out = fopen(output.c_str(), "w");
    FILE *err = fopen(errput.c_str(), "w");
    ProgramInfo r_val = execute_process(name, argv,
                                        in, out, err, max_cpu_time);
    if (in != NULL)  fclose(in);
    if (out != NULL) fclose(out);
    if (err != NULL) fclose(err);
    return r_val;
}
