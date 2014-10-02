#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <string>
#include <vector>
#include "execute-process.h"

struct TimeSet {
    std::vector<ProgramInfo> runs;
    std::string input_file;
    std::string output_file;
    std::string err_file;
    std::string test_name;
};

class Timer
{
public:
    Timer();

    void report_run(const ProgramInfo result);
    void report_time(const TimeSet results);
    void report_times(const std::vector<TimeSet> all_results);

    Timer &report_only_avg();
    Timer &dont_report_avg();
    Timer &report_all();

private:
    bool report_avg;
    bool report_all_times;
};

#endif
