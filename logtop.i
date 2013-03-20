%module logtop

%{
#include "src/logtop.h"
%}

%typemap(out) struct logtop_state *
{
    int i;
    PyObject *log_line;
    PyObject *lines;

    $result = PyDict_New();
    lines = PyList_New(0);
    while (result->lines[i] != NULL)
    {
        log_line = PyTuple_New(3);
        PyTuple_SetItem(log_line, 0, PyInt_FromLong(result->lines[i]->count));
        PyTuple_SetItem(log_line, 1, PyFloat_FromDouble(result->lines[i]->frequency));
        PyTuple_SetItem(log_line, 2, PyString_FromString(result->lines[i]->string));
        PyList_Append(lines, log_line);
        i++;
    }
    PyDict_SetItemString($result, "lines", lines);
    PyDict_SetItemString($result, "count", PyInt_FromLong(result->count));
    PyDict_SetItemString($result, "timespan", PyFloat_FromDouble(result->timespan));
    free(result->lines);
    free(result);
}

struct logtop
{
    %extend {
        logtop(size_t history_size);
        ~logtop();
        void feed(char *line);
        struct logtop_state *get(size_t qte);
        double timespan();
        unsigned int qte_of_elements();
    }
};
