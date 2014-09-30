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

    Timer &ignore_sentinel_whitespace();
    Timer &ignore_char_in_sentinel(char c);

    Timer &report_only_avg();
    Timer &dont_report_avg();

private:
    std::string output_file_name;
    std::string prefix_sentinel;
    std::string postfix_sentinel;
    std::string header;
    std::string footer;
    bool ignore_whitespace;
    std::vector<char> ignore_chars;
    bool report_avg;
    bool report_all_times;
};
