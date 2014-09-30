#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <string>
#include <vector>
#include "execute-process.h"

class Timer
{
public:
    Timer();

    void report_run(const ProgramInfo result);
    void report_time(const std::vector<ProgramInfo> results);
    void report_times(const std::vector< std::vector<ProgramInfo> > all_results);

    Timer &report_only_avg();
    Timer &dont_report_avg();
    Timer &report_all();

private:
    bool report_avg;
    bool report_all_times;
};

#endif
