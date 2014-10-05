/*---------------------------------------------------------------------------*\
 *  execute-process.h                                                        *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This file contains the interface for the execute-process module.         *
 *  Specifically, it contains the definition for a ProgramInfo struct and    *
 *  several functions that allow one to run an external program.             *
 *  Each such function returns a ProgramInfo struct on success, containing   *
 *  the amount of time the process took (wall, system, and user, each        *
 *  specifying seconds and microseconds), and the exit code of the process.  *
 *  See below for more specific descriptions of each.                        *
\*---------------------------------------------------------------------------*/
#ifndef EXECUTE_PROCESS_H_INCLUDED
#define EXECUTE_PROCESS_H_INCLUDED

#include <string>

struct ProgramInfo {
    unsigned user_sec;
    unsigned user_usec;
    unsigned sys_sec;
    unsigned sys_usec;
    unsigned wall_sec;
    unsigned wall_usec;
    int exit_code;
};

/*  executes the process whose path is stored in _name_, passing it the array
 *    _argv_ as arguments, and redirecting its input to _input_, its output to
 *    _output_, and its error stream to _errput_.  Limits the process to
 *    _max_cpu_time_, representing the maximum number of seconds to execute on
 *    the CPU, and to _max_real_time_ of wall-clock time.
 *
 *  There are several overloads of this function, which allow some parameters
 *    to be omitted.  In each overload, a string may be passed for
 *    redirection instead of a FILE *, in which case the file will be opened.
 *
 *  _argv_ may be NULL to indicate no arguments.  Otherwise, the array must
 *    contain null-terminated C-strings and be terminated with NULL.
 *  Any of the i/o redirection files may be NULL, in which case
 *    the program will use stdin, stdout, and/or stderr.  If the file cannot
 *    be opened, the corresponding stream will not be redirected.
 *  _max_cpu_time_ and _max_real_time_ may be zero, in which case no time
 *    limit will be placed on the child.
 *  This function may throw an exception as a string describing the error.
 *    This would happen only if the process fails to open.
 */
ProgramInfo execute_process(std::string name, char *argv[], FILE *input,
                            FILE *output, FILE *errput,
                            unsigned max_cpu_time, unsigned max_real_time);


ProgramInfo execute_process(std::string name, char *argv[], std::string input,
                            std::string output, std::string errput,
                            unsigned max_cpu_time, unsigned max_real_time);


static inline ProgramInfo execute_process(std::string name, char *argv[],
                                          FILE *input, FILE *output,
                                          FILE *errput)
{
    return execute_process(name, argv, input, output, errput, 0, 0);
}


static inline ProgramInfo execute_process(std::string name, char *argv[],
                                          std::string input, std::string output,
                                          std::string errput)
{
    return execute_process(name, argv, input, output, errput, 0, 0);
}


static inline ProgramInfo execute_process(std::string name, char *argv[],
                                          unsigned max_cpu_time,
                                          unsigned max_real_time)
{
    return execute_process(name, argv, NULL, NULL, NULL,
                           max_cpu_time, max_real_time);
}


static inline ProgramInfo execute_process(std::string name, char *argv[])
{
    return execute_process(name, argv, NULL, NULL, NULL, 0, 0);
}


static inline ProgramInfo execute_process(std::string name)
{
    return execute_process(name, NULL, NULL, NULL, NULL, 0, 0);
}

#endif
