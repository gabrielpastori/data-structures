// Copyright (c) 2022

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include <boost/python.hpp>
#include "./dataframe/dataframe.h"

BOOST_PYTHON_MODULE(capivara) {
    def("points_from_xy", points_from_xy);
    boost::python::class_<DataFrame>("Dataframe",
                    boost::python::init<boost::python::dict>())
        .def_readwrite("df", &DataFrame::df)
        .def("__getitem__", &DataFrame::operator[])
        .def("__setitem__", &DataFrame::set_column)
        .def("find_by_value", &DataFrame::find_by_value)
        .def("set_index", &DataFrame::set_index)
        .def("between", &DataFrame::between)
        .def("insert_line", &DataFrame::insert_line)
        .def("columns", &DataFrame::columns)
        .def("plot", &DataFrame::plot);
}
