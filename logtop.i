%module logtop

%{
#include "src/logtop.h"
%}

%typemap(out) struct logtop_state *
{
    int i;
    PyObject *log_line;
    PyObject *lines;
    PyObject *tmp;

    $result = PyDict_New();
    lines = PyList_New(0);
    while (result->lines[i] != NULL)
    {
        log_line = PyTuple_New(3);
        PyTuple_SetItem(log_line, 0, PyInt_FromLong(result->lines[i]->count));
        PyTuple_SetItem(log_line, 1, PyFloat_FromDouble(result->lines[i]->frequency));
        PyTuple_SetItem(log_line, 2, PyString_FromString(result->lines[i]->string));
        PyList_Append(lines, log_line);
        Py_DECREF(log_line);
        i++;
    }
    PyDict_SetItemString($result, "lines", lines);
    Py_DECREF(lines);
    tmp = PyInt_FromLong(result->count);
    PyDict_SetItemString($result, "count", tmp);
    Py_DECREF(tmp);
    tmp = PyFloat_FromDouble(result->timespan);
    PyDict_SetItemString($result, "timespan", tmp);
    Py_DECREF(tmp);
    tmp = PyFloat_FromDouble(result->frequency);
    PyDict_SetItemString($result, "frequency", tmp);
    Py_DECREF(tmp);
    delete_logtop_state(result);
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
