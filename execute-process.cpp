/*---------------------------------------------------------------------------*\
 *  execute-process.cpp                                                      *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This implementation for execute-process relies on fork(), execv(),       *
 *    wait3(), dup2(), gettimeofday(), and setrlimit().                      *
 *    It also uses the boost::lexical_cast library to aid in producing       *
 *    exception messages.                                                    *
 *                                                                           *
 *  TO DO:                                                                   *
 *   - Schedule an alarm signal for the parent based on max_cpu_time.  This  *
 *     parameter can then be used for the maximum _real_ time - ie. when     *
 *     that much real time has passed, the child is killed.  This prevents   *
 *     a child blocking for a long time, taking up real time but not cpu time*
\*---------------------------------------------------------------------------*/
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <boost/lexical_cast.hpp>
#include "execute-process.h"
using namespace std;


static pid_t child_id = 0;

static struct sigaction sact;

static void signal_handler(int signum)
{
    if (child_id && !kill(child_id, signum)){
        sigaction(signum, &sact, NULL);
    } else {
        exit(signum);
    }
}

static void set_signal_handler()
{
    sact.sa_handler = signal_handler;
    sact.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sact, NULL);
    sigaction(SIGINT, &sact, NULL);
}



ProgramInfo execute_process(string name, char *argv[],
                  FILE *input, FILE *output, FILE *errput,
                  unsigned max_cpu_time)
{
    int exit_status;
    timeval before, after;
    rlimit time_limit;
    rusage time_taken;
    ProgramInfo r_val;
    set_signal_handler();

    gettimeofday(&before, NULL);
    if ((child_id = fork())) {
        if (child_id < 0) throw string("could not open process");
        wait3(&exit_status, 0, &time_taken);
        gettimeofday(&after, NULL);
        if (WIFSIGNALED(exit_status)) {
            throw string("process terminated by signal number ")
                + boost::lexical_cast<string>(WTERMSIG(exit_status));
        }
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
    FILE *in, *out, *err;
    if (input == "") in = stdin;
    else in = fopen(input.c_str(), "r");
    if (output == "") out = stdout;
    else out = fopen(output.c_str(), "w");
    if (errput == "") err = stderr;
    else err = fopen(errput.c_str(), "w");

    ProgramInfo r_val = execute_process(name, argv,
                                        in, out, err, max_cpu_time);
    if (in != NULL)  fclose(in);
    if (out != NULL) fclose(out);
    if (err != NULL) fclose(err);
    return r_val;
}
