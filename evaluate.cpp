#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include "execute-process.h"
using namespace std;

struct ProgramOptions {
    string program_name;
    vector<string> args;
};

void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts);
char **vector_to_argv(string name, vector<string> *vect);

int main(int argc, char *argv[])
{
    ProgramOptions opts;
    parse_command_line_args(argc, argv, &opts);
    execute_process(opts.program_name,
                    vector_to_argv(opts.program_name, &(opts.args)));
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
