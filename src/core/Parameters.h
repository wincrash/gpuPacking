#pragma once
#include "utils/cxxopts.hpp"


class Parameters
{
private:
    /* Here will be the instance stored. */
    static Parameters* instance;

    /* Private constructor to prevent instancing. */
    Parameters();


public:
    char **copy_argv(int argc, char *argv[]);
    int argc;
    char** argv;
    /* Static access method. */
    static Parameters* getInstance();
    void InitArgs(int argc, char *argv[]);
    cxxopts::ParseResult getResults();
};
