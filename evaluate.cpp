#include <iostream>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "execute-process.h"
using namespace std;

struct ProgramOptions {
    string program_name;
    vector<string> args;
    string input_dir;
    string output_dir;
    string input_suffix;
    string output_suffix;
    bool just_test;
    bool just_time;
};

void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts);
char **vector_to_argv(string name, vector<string> *vect);
void evaluate(string name, vector<string> args,
              vector<string> inputs, vector<string> outputs,
              unsigned times, bool time, bool test);
void get_io(vector<string> &inputs, vector<string> &outputs, string input_dir,
            string output_dir, string input_suffix, string output_suffix);
bool compare(string file1, string file2);
void print_results(ProgramInfo results);

int main(int argc, char *argv[])
{
    vector<string> inputs, outputs;
    ProgramOptions opts;
    parse_command_line_args(argc, argv, &opts);
    get_io(inputs, outputs, opts.input_dir, opts.output_dir,
           opts.input_suffix, opts.output_suffix);
    evaluate(opts.program_name, opts.args, inputs, outputs, 1,
             opts.just_time, opts.just_test);

    return 0;
}

/*  parse_command_line_args()
 *  Goes through the array of arguments and sets appropriate values in the
 *    structure pointed to by opts.
 */
void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts)
{
    namespace po = boost::program_options;
    po::variables_map args;
    po::options_description generic("Generic options");
    po::positional_options_description p_desc;
    generic.add_options()
        ("program", po::value<string>(&(opts->program_name))->required(),
            "Specify executable to run")
        ("arg", po::value< vector<string> >(&(opts->args)),
            "Specify arguments to pass to the executable")
        ("input-dir,d", po::value<string>(&(opts->input_dir)),
            "Specify the directory containing input files")
        ("output-dir,D", po::value<string>(&(opts->output_dir)),
            "Specify the directory containing input files")
        ("input-ext,e", po::value<string>(&(opts->input_suffix)),
            "Specify the suffix (eg extension) identifying input files")
        ("output-ext,E", po::value<string>(&(opts->output_suffix)),
            "Specify the suffix (eg extension) identifying output files")
        ("test,s", po::bool_switch(&(opts->just_test)),
            "Only run tests, do not time")
        ("time,m", po::bool_switch(&(opts->just_time)),
            "Only time, do not evaluate tests")
    ;
    p_desc.add("program", 1);
    p_desc.add("arg", -1);
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(generic).positional(p_desc).run(), args);
        po::notify(args);
    } catch (exception &err) {
        cerr << "Error in arguments: " << err.what() << endl;
        cerr << "(For help, use the --help or -h options)" << endl;
        exit(1);
    }
}


/*  vector_to_argv()
 *  Takes the name of a program and a vector of its arguments, as strings,
 *    and converts them into standard argv format (that expected by the exec
 *    family of functions).  That is, it is a NULL-terminated array of
 *    null-terminated character arrays, where the first contains the name
 *    of the program, and the rest contain the program's arguments.
 */
char **vector_to_argv(string name, vector<string> *vect)
{
    vector<char *> temp_vect;
    char *copy = new char[name.size()];
    temp_vect.push_back(strcpy(copy, name.c_str()));
    unsigned end = vect->size();
    for (unsigned i = 0; i < end; ++i) {
        copy = new char[vect->at(i).size()];
        temp_vect.push_back(strcpy(copy, vect->at(i).c_str()));
    }
    temp_vect.push_back(NULL);
    char **argv = new char *[temp_vect.size()];
    end = temp_vect.size();
    for (unsigned i = 0; i < end; ++i) {
        argv[i] = temp_vect[i];
    }
    return argv;
}

void print_results(ProgramInfo results)
{
    double wall =  (double)results.wall_sec
                    + ((double)results.wall_usec / 1000000.0);
    double user = (double) results.user_sec
                    + ((double)results.user_usec / 1000000.0);
    double sys  = (double) results.sys_sec
                    + ((double)results.sys_usec  / 1000000.0);
    cout.precision(4);
    cout.setf( std::ios::fixed, std:: ios::floatfield );
    cout << "Wall: " << wall << endl;
    cout << "User: " << user << endl;
    cout << "Sys:  " << sys  << endl;
    cout << "Exit code: " << results.exit_code << endl;
}


void get_files_by_suffix(vector<boost::filesystem::path> &output,
                         string dir, string suffix)
{
    namespace fs = boost::filesystem;
    fs::directory_iterator end;
    fs::directory_iterator i(".");
    if (dir != "") {
        fs::path d(dir);
        if (d.is_absolute()) i = fs::directory_iterator(d);
        else i = fs::directory_iterator(fs::current_path() / d);
    }
    unsigned suffix_len = suffix.length();
    for ( ; i != end; ++i) {
        string path = i->path().generic_string();
        unsigned path_len = path.length();
        if (path_len >= suffix_len) {
            if (path.substr(path_len - suffix_len) == suffix) {
                output.push_back(path.substr(0, path_len - suffix_len));
            }
        }
    }
}


void get_io(vector<string> &inputs, vector<string> &outputs, string input_dir,
            string output_dir, string input_suffix, string output_suffix)
{
    namespace fs = boost::filesystem;
    vector<fs::path> temp_in, temp_out;
    get_files_by_suffix(temp_in, input_dir, input_suffix);
    get_files_by_suffix(temp_out, output_dir, output_suffix);
    unsigned i_len = temp_in.size();
    unsigned o_len = temp_out.size();
    unsigned i = 0, j = 0;
    /*  Look for file names that correspond.  Add names in parallel to the
     *  vectors.  If names are found without a match, push the empty string
     *  to the other vector.
     */
    while (i < i_len || j < o_len) {
        fs::path cur_in = (i < i_len) ? temp_in[i] : "";
        fs::path cur_out = (j < o_len) ? temp_out[j] : "";
        if (cur_in.filename() == cur_out.filename()) {
            inputs.push_back(cur_in.generic_string() + input_suffix);
            outputs.push_back(cur_out.generic_string() + output_suffix);
            ++i, ++j;
        } else if (cur_in.filename() < cur_out.filename() && cur_in.filename() != "") {
            inputs.push_back(cur_in.generic_string() + input_suffix);
            outputs.push_back("");
            ++i;
        } else {
            outputs.push_back(cur_out.generic_string() + output_suffix);
            inputs.push_back("");
            ++j;
        }
    }
}


void evaluate(string name, vector<string> args,
              vector<string> inputs, vector<string> outputs,
              unsigned times, bool time, bool test)
{
    namespace fs = boost::filesystem;
    vector< vector<ProgramInfo> > results;
    unsigned len = inputs.size();
    string temp_file = fs::temp_directory_path().native()
                            + fs::unique_path().native() + ".eval";
    if (len != outputs.size()) {
        throw string("differing amounts of inputs and outputs");
    }
    for (unsigned i = 0; i < len; ++i) {
        vector<ProgramInfo> these_tests;
        for (unsigned j = 0; j < times; ++j) {
            try {
                these_tests.push_back(execute_process(name,
                                            vector_to_argv(name, &args),
                                            inputs[i], temp_file, ""));
                if (test) {
                    if(compare(temp_file, outputs[i])) {
                        cout << "Output from " << inputs[i] << " matches "
                             << outputs[i] << endl;
                    } else {
                        cout << "Output from " << inputs[i] << " failed"
                             << endl;
                    }
                }
                if (time) print_results(these_tests.back());
            } catch (string err) {
                cerr << "Error: " << err << endl;
            }
        }
        results.push_back(these_tests);
    }
}


bool compare(string file1, string file2)
{
    ifstream str_1(file1.c_str());
    ifstream str_2(file2.c_str());
    char c1, c2;
    if (!str_1.is_open() || !str_2.is_open()) {
        if (file1 == "" && str_2.is_open()) {
            c2 = str_2.get();
            if (!str_2.eof()) return false;
            return true;
        }
        if (file2 == "" && str_1.is_open()) {
            c1 = str_1.get();
            if (!str_1.eof()) return false;
            return true;
        }
        cerr << "Cannot open for testing on " << file2 << endl;
        return false;
    }
    c1 = str_1.get();
    c2 = str_2.get();
    while (!str_1.eof() && !str_2.eof()) {
        if (str_1.eof() || str_2.eof() || c1 != c2) {
            return false;
        }
        c1 = str_1.get();
        c2 = str_2.get();
    }
    return true;
}
