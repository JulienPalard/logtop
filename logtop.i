%module logtop

%{
#include "src/logtop.h"
%}

%typemap(out) log_line_t **
{
    int i;
    PyObject *log_line;

    $result = PyList_New(0);
    while (result[i] != NULL)
    {
        log_line = PyTuple_New(2);
        PyTuple_SetItem(log_line, 0, PyString_FromString(result[i]->string));
        PyTuple_SetItem(log_line, 1, PyInt_FromLong(result[i]->count));
        PyList_Append($result, log_line);
        i++;
    }
}

struct logtop
{
    %extend {
        logtop(size_t history_size);
        ~logtop();
        void feed(char *line);
        log_line_t **get(size_t qte);
        double timespan();
        unsigned int qte_of_elements();
    }
};
