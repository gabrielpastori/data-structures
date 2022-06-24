#ifndef DATAFRAME_H_
#define DATAFRAME_H_

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

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <utility>
#include <variant>
#include "../bplustree/bplustree.h"
#include <boost/python.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/stl_iterator.hpp>

/**
 * @brief class that will be used to store the dataframe
 */
class DataFrame {
 public:
    /**
     * @brief struct that represents a Value variant (float or int)
     */
    struct Value {
        std::variant<double, int> value;
        std::string type;
        explicit Value(boost::python::object);
    };

    /**
     * @brief struct that represents a Column variant (value or a pair of values) 
     */
    struct Column {
        std::vector<std::variant<Value, std::pair<Value, Value> > > values;
        std::string name;
        Column(boost::python::list, std::string);
        Column(boost::python::list, boost::python::list, std::string);
    };

    /**
     * @brief method to allow use of slicing in Python
     * @param column_name name of the column
     * @return boost::python::list 
     */
    boost::python::list operator[](std::string);

    /**
     * @brief Set the column object, given a name and a list of values 
     * (check if it is a 1d or 2d column).
     * @param column_name name of the column
     * @param values list of values
     * @return void
     */
    void set_column(std::string, boost::python::list);

    /**
     * @brief Set the column xy object, this method isn't exposed to Python. 
     * It's called by the set_column method.
     * @param column_name name of the column
     * @param tuple_column list of x values
     * @see set_column
     * @return void
     */
    void set_column_xy(std::string, boost::python::list);


    /**
     * @brief Index a given column adding it's values to the BPlusTree.
     * @param column_name name of the column
     * @return void
     * @see BPlusTree
     * @see BPlusTree::Insert
     */
    void set_index(std::string);


    /**
     * @brief method to return a tuple of a given line position.
     * @param index position of the line
     * @return boost::python::tuple
     */
    boost::python::tuple get_line(int);

    /**
     * @brief method to query a column by a value and return the line(s)
     * @param column_name name of the column
     * @param value value to query
     * @return boost::python::list
     */
    boost::python::list find_by_value(std::string, boost::python::object);


    /**
     * @brief method to query a column by a range of values and return the line(s)
     * @param column_name name of the column
     * @param value_min minimum value to query
     * @param value_max maximum value to query
     * @return boost::python::list 
     */ 
    boost::python::list between(std::string, boost::python::object,
                                 boost::python::object);

    /**
     * @brief method to insert a line in the DataFrame
     * @param line list of values
     * @return void
     */
    void insert_line(boost::python::list);

    /**
     * @brief method to scatterplot given two column names
     * @param x name of the first column
     * @param y name of the second column
     */
    void plot(std::string, std::string);

    /**
     * @brief method to return the list of column names
     * @return boost::python::list 
     */
    boost::python::list columns();

    /**
     * @brief vector that contains the columns
     */
    std::vector<Column> df;

    /**
     * @brief our BPlusTree index
     * 
     */
    std::variant<BPlusTree<double>, BPlusTree<int> > index;

    bool is_indexed = false;
    std::string index_column;

    explicit DataFrame(boost::python::dict);
};

boost::python::list points_from_xy(
    boost::python::list, boost::python::list);


#include "./dataframe.hpp"
#endif  // DATAFRAME_H_
