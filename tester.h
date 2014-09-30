
class Tester
{
public:
    Tester();

    bool run();
    string run_verbosely();

    void set_benchmark_file(std::string filename);
    void set_comparison_file(std::string filename);

    void ignore_whitespace();
    void ignore_char(char c);
    void ignore_extra_chars();
    void ignore_too_few_chars();

private:
};
