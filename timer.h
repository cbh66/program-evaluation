#include <string>
#include <vector>
#include "execute-process.h"

class Timer
{
public:
    Timer();

    void report_run(ProgramInfo result);
    void report_time(std::vector<ProgramInfo> results);
    void report_times(std::vector< std::vector<ProgramInfo> > all_results);

    Timer &report_only_avg();
    Timer &dont_report_avg();

private:
    bool report_avg;
    bool report_all_times;
};
