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

#include "./dataframe.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <utility>
#include <string>
#include <variant>
#include <boost/python/dict.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/str.hpp>
#include <boost/python.hpp>

//TODO: create a dict for columns, no need to loop over

DataFrame::DataFrame(boost::python::dict all_values) {
    boost::python::list items = boost::python::list(all_values.items());
    int number_of_rows = 0;
    for (int column_idx = 0; column_idx < len(items); column_idx++) {
        boost::python::tuple item = boost::python::extract
            <boost::python::tuple>(items[column_idx]);
        boost::python::str key = boost::python::
                        extract<boost::python::str>(item[0]);
        boost::python::list values = boost::python::list(item[1]);
        if (column_idx == 0) {
           number_of_rows = len(values);
        }
        if (len(values) != number_of_rows) {
            throw std::runtime_error
                    ("All columns must have the same number of rows");
        }
        Column data_column(values, boost::python::extract<std::string>(key));
        df.push_back(data_column);
    }

    std::cout << "Number of columns inserted: " << df.size() << std::endl;
    std::cout << "Number of rows inserted: " <<
                 df[0].values.size() << std::endl;
}

DataFrame::Column::Column(boost::python::list values, std::string name) {
    for (int value_idx = 0; value_idx < len(values); value_idx++) {
        Value value(values[value_idx]);
        this->values.push_back(value);
    }
    this->name = name;
}

DataFrame::Column::Column(boost::python::list valuesx,
                          boost::python::list valuesy, std::string name) {
    if (len(valuesx) != len(valuesy)) {
        throw std::runtime_error
                        ("All columns must have the same number of rows");
    }
    for (int value_idx = 0; value_idx < len(valuesx); value_idx++) {
        Value valuex(valuesx[value_idx]);
        Value valuey(valuesy[value_idx]);
        this->values.push_back(std::make_pair(valuex, valuey));
    }
    this->name = name;
}

DataFrame::Value::Value(boost::python::object value) {
    boost::python::extract<int> int_value(value);
    if (int_value.check()) {
        this->value = int_value;
        this->type = "int";
    } else {
        boost::python::extract<double> double_value(value);
        if (double_value.check()) {
            this->value = double_value;
            this->type = "double";
        } else {
            throw std::runtime_error("Invalid value type");
        }
    }
}

boost::python::list DataFrame::operator[](std::string column_name) {
    if (column_name == "") {
        throw std::runtime_error("Column name cannot be empty");
    }
    if (df.size() == 0) {
        throw std::runtime_error("Dataframe is empty");
    }
    for (int column_idx = 0;
                    column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            boost::python::list values;
            for (int value_idx = 0;
                value_idx <static_cast<int>(df[column_idx].values.size());
                value_idx++) {
                if (df[column_idx].values[value_idx].index() == 0) {
                    Value value =
                        std::get<DataFrame::Value>
                        (df[column_idx].values[value_idx]);
                    if (value.type == "double") {
                        values.append(std::get<double>(value.value));
                        continue;
                    }
                    values.append(std::get<int>(value.value));
                    continue;
                }
                std::pair<DataFrame::Value, DataFrame::Value> value =
                    std::get<std::pair<DataFrame::Value, DataFrame::Value>>
                    (df[column_idx].values[value_idx]);

                if (value.first.type == "double" &&
                                     value.second.type == "double") {
                    values.append(boost::python::make_tuple(
                        std::get<double>(value.first.value),
                        std::get<double>(value.second.value)));
                    continue;
                }
                if (value.first.type == "double" &&
                                     value.second.type == "int") {
                    values.append(boost::python::make_tuple(
                        std::get<double>(value.first.value),
                        std::get<int>(value.second.value)));
                    continue;
                }
                if (value.first.type == "int" &&
                                     value.second.type == "double") {
                    values.append(boost::python::make_tuple(
                        std::get<int>(value.first.value),
                        std::get<double>(value.second.value)));
                    continue;
                }
                if (value.first.type == "int" &&
                                     value.second.type == "int") {
                    values.append(boost::python::make_tuple(
                        std::get<int>(value.first.value),
                        std::get<int>(value.second.value)));
                    continue;
                }
            }
            return values;
        }
    }
    throw std::runtime_error("Column not found");
}

void DataFrame::set_column(std::string column_name,
                            boost::python::list values) {
    if (len(values) == 0) {
        throw std::runtime_error("Empty column");
    }
    boost::python::extract<boost::python::tuple> tuple_value(values[0]);
    if (tuple_value.check()) {
        DataFrame::set_column_xy(column_name, values);
        return;
    }
    Column data_column(boost::python::list(values), column_name);
    if (df.size() == 0) {
        df.push_back(data_column);
        return;
    }
    if (df[0].values.size() != data_column.values.size()) {
        throw std::runtime_error
                ("All columns must have the same number of rows");
    }
    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            df[column_idx] = data_column;
            return;
        }
    }
    df.push_back(data_column);
}

void DataFrame::set_column_xy(std::string column_name,
                            boost::python::list tuple_column) {
    if (len(tuple_column) == 0) {
        throw std::runtime_error("Empty column");
    }

    boost::python::list x_values;
    boost::python::list y_values;
    for (int value_idx = 0; value_idx < len(tuple_column); value_idx++) {
        boost::python::extract<boost::python::tuple>
                        tuple_value(tuple_column[value_idx]);
        if (tuple_value.check()) {
            boost::python::tuple value = tuple_value;
            x_values.append(value[0]);
            y_values.append(value[1]);
            continue;
        }
        throw std::runtime_error("Invalid value type");
    }
    DataFrame::Column data_column(x_values, y_values, column_name);
    if (df.size() == 0) {
        df.push_back(data_column);
        return;
    }
    if (df[0].values.size() != data_column.values.size()) {
        throw std::runtime_error
                ("All columns must have the same number of rows");
    }
    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            df[column_idx] = data_column;
            return;
        }
    }
    df.push_back(data_column);
}

boost::python::list points_from_xy(boost::python::list valuesx,
                                 boost::python::list valuesy) {
    if (len(valuesx) != len(valuesy)) {
        throw std::runtime_error
                        ("All columns must have the same number of rows");
    }
    boost::python::list points;
    for (int value_idx = 0; value_idx < len(valuesx); value_idx++) {
        points.append(boost::python::make_tuple(valuesx[value_idx],
                                                valuesy[value_idx]));
    }
    return points;
}

boost::python::tuple DataFrame::get_line(int index) {
    if (index < 0 || index >= static_cast<int>(df[0].values.size())) {
        throw std::runtime_error("Index out of range");
    }
    boost::python::list points;
    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].values[index].index() == 0) {
            Value value =
                std::get<DataFrame::Value>
                (df[column_idx].values[index]);
            if (value.type == "double") {
                points.append(std::get<double>(value.value));
                continue;
            }
            points.append(std::get<int>(value.value));
            continue;
        }
    }
    return boost::python::tuple(points);
}

boost::python::list DataFrame::find_by_value(std::string column_name,
                                   boost::python::object value) {
    if (this->df.size() == 0) {
        throw std::runtime_error("Dataframe is empty");
    }

    DataFrame::Value value_to_find = DataFrame::Value(value);
    if (this->is_indexed && this->index_column == column_name) {
        boost::python::list values;
        int index_line = -1;
        if ((this->index).index() == 0) {
            BPlusTree<double> tree = std::get<BPlusTree<double> >(this->index);
            if (value_to_find.type == "double" || value_to_find.type == "int") {
                index_line = tree.Find(std::get<double>(value_to_find.value));
            }
        } else {
            BPlusTree<int> tree = std::get<BPlusTree<int> >(this->index);
            if (value_to_find.type == "int") {
                index_line = tree.Find(std::get<int>(value_to_find.value));
            }
        }
        if (index_line == -1) {
            return values;
        }
        values.append(DataFrame::get_line(index_line));
        return values;
    }

    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            boost::python::list values;
            for (int value_idx = 0;
                        value_idx <
                        static_cast<int>(df[column_idx].values.size());
                        value_idx++) {
                if (df[column_idx].values[value_idx].index() == 0) {
                    Value value =
                        std::get<DataFrame::Value>
                        (df[column_idx].values[value_idx]);
                    if (value.type == "double" &&
                                        value_to_find.type == "double") {
                        if (std::get<double>(value.value) ==
                                        std::get<double>(value_to_find.value)) {
                            values.append(DataFrame::get_line(value_idx));
                        }
                        continue;
                    }
                    if (value.type == "int" &&
                                        value_to_find.type == "int") {
                        if (std::get<int>(value.value) ==
                                        std::get<int>(value_to_find.value)) {
                            values.append(DataFrame::get_line(value_idx));
                        }
                        continue;
                    }
                }
            }
            return values;
        }
    }
    return boost::python::list();
}

void DataFrame::set_index(std::string column_name) {
    if (this->df.size() == 0) {
        throw std::runtime_error("Dataframe is empty");
    }
    this->index = std::variant<BPlusTree<double>, BPlusTree<int>>();
    bool found = false;
    bool isInt = false;
    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            if (static_cast<int>(df[0].values.size()) > 0) {
                if (df[column_idx].values[0].index() == 0) {
                    Value value =
                        std::get<DataFrame::Value>
                        (df[column_idx].values[0]);
                    if (value.type == "double") {
                        isInt = false;
                    }
                    if (value.type == "int") {
                        isInt = true;
                    }
                }
            }
            for (int value_idx = 0;
                        value_idx <
                        static_cast<int>(df[column_idx].values.size());
                        value_idx++) {
                if (df[column_idx].values[value_idx].index() == 0) {
                    Value value =
                        std::get<DataFrame::Value>
                        (df[column_idx].values[value_idx]);

                    if (value.type == "xy") {
                        throw std::runtime_error("Cannot index on xy");
                    }

                    if (value.type == "double") {
                        if (isInt) {
                            throw std::runtime_error
                                    ("Column must be homogeneous");
                        }
                        if (!found) {
                            this->index = BPlusTree<double>();
                            found = true;
                        }
                        std::get<BPlusTree<double>>(this->index).
                                    Insert(std::get<double>(value.value),
                                        value_idx);
                        continue;
                    }

                    if (value.type == "int") {
                        if (!isInt) {
                            throw std::runtime_error
                                    ("Column is not homogeneous");
                        }
                        if (!found) {
                            this->index = BPlusTree<int>();
                            found = true;
                        }
                        std::get<BPlusTree<int>>(this->index).
                                    Insert(std::get<int>(value.value),
                                        value_idx);
                        continue;
                    }
                }
            }
        }
    }
    if (!found) throw std::runtime_error("Column not found");
    this->is_indexed = true;
    this->index_column = column_name;
}


boost::python::list DataFrame::between(std::string column_name,
                boost::python::object minimum, boost::python::object maximum) {
    if (this->df.size() == 0) {
        throw std::runtime_error("Dataframe is empty");
    }

    DataFrame::Value minimum_value = DataFrame::Value(minimum);
    DataFrame::Value maximum_value = DataFrame::Value(maximum);
    if (minimum_value.type == "xy" || maximum_value.type == "xy") {
        throw std::runtime_error("Cannot query on xy");
    }
    double minimum_d, maximum_d;
    if (minimum_value.type == "int") {
        minimum_d = 1.0 * std::get<int>(minimum_value.value);
    } else {
        minimum_d = std::get<double>(minimum_value.value);
    }
    if (maximum_value.type == "int") {
        maximum_d = 1.0 * std::get<int>(maximum_value.value);
    } else {
        maximum_d = std::get<double>(maximum_value.value);
    }
    boost::python::list values;
    if (this->is_indexed && this->index_column == column_name) {
        boost::python::list values;
        std::vector<int> index_lines;
        if ((this->index).index() == 0) {
            BPlusTree<double> tree = std::get<BPlusTree<double>>(this->index);
            index_lines = tree.RangeFind(minimum_d, maximum_d);
        } else {
            BPlusTree<int> tree = std::get<BPlusTree<int>>(this->index);
            index_lines = tree.RangeFind(minimum_d, maximum_d);
        }
        for (int i = 0; i < static_cast<int>(index_lines.size()); i++) {
            values.append(DataFrame::get_line(index_lines[i]));
        }
        return values;
    }
    for (int column_idx = 0;
                column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == column_name) {
            for (int value_idx = 0;
                        value_idx <
                        static_cast<int>(df[column_idx].values.size());
                        value_idx++) {
                if (df[column_idx].values[value_idx].index() == 0) {
                    Value value =
                        std::get<DataFrame::Value>
                        (df[column_idx].values[value_idx]);

                    double value_d;
                    if (value.type == "int") {
                        value_d = 1.0 * std::get<int>(value.value);
                    } else {
                        if (value.type == "double") {
                            value_d = std::get<double>(value.value);
                        } else {
                            throw std::runtime_error
                                ("Can't compare xy with numerical type");
                        }
                    }
                    if (minimum_d <= value_d &&
                        maximum_d >= value_d) {
                        values.append(DataFrame::get_line(value_idx));
                    }
                }
            }
            return values;
        }
    }
    return values;
}

void DataFrame::insert_line(boost::python::list line) {
    if (len(line) != static_cast<int>(df.size())) {
        throw std::runtime_error("Line length does not match");
    }
    for (int column_idx = 0; column_idx < static_cast<int>(df.size());
                                                         column_idx++) {
         boost::python::extract<boost::python::tuple>
                                 tuple_value(line[column_idx]);
        if (tuple_value.check()) {
             boost::python::tuple value = tuple_value;
             Value first = Value(value[0]);
            Value second = Value(value[1]);
            df[column_idx].values.push_back(std::make_pair(first, second));
            if (this->df[column_idx].values[0].index() == 0) {
                throw std::runtime_error("Column type does not match");
            }
            continue;
        }
        Value value = DataFrame::Value(line[column_idx]);
        if (this->df[column_idx].values.size() != 0) {
            if (this->df[column_idx].values[0].index() == 1 &&
                                                 value.type != "xy") {
                throw std::runtime_error("Column type does not match");
            }
        }
        df[column_idx].values.push_back(value);
    }
}

boost::python::list DataFrame::columns() {
    boost::python::list columns;
    for (int column_idx = 0;
                    column_idx < static_cast<int>(df.size()); column_idx++) {
        columns.append(df[column_idx].name);
    }
    return columns;
}

void DataFrame::plot(std::string x, std::string y) {
    bool x_found = false;
    bool y_found = false;
    int index_x = -1;
    int index_y = -1;
    for (int column_idx = 0;
                    column_idx < static_cast<int>(df.size()); column_idx++) {
        if (df[column_idx].name == x) {
            if (this->df[column_idx].values[0].index() == 1) {
                throw std::runtime_error("Cannot plot 2d column");
            }
            x_found = true;
            index_x = column_idx;
        }
        if (df[column_idx].name == y) {
            if (this->df[column_idx].values[0].index() == 1) {
                throw std::runtime_error("Cannot plot 2d column");
            }
            y_found = true;
            index_y = column_idx;
        }
     }
    if (!x_found || !y_found) {
        throw std::runtime_error("Column not found");
    }

    boost::python::object plt =
        boost::python::import("matplotlib.pyplot");
    boost::python::list x_values;
    boost::python::list y_values;
    for (int value_idx = 0;
                value_idx < static_cast<int>(df[index_x].values.size());
                value_idx++) {
        if (df[index_x].values[value_idx].index() == 0) {
            Value x_value =
                std::get<DataFrame::Value>
                (df[index_x].values[value_idx]);

            Value y_value =
                std::get<DataFrame::Value>
                (df[index_y].values[value_idx]);

            if (x_value.type == "int") {
                x_values.append(std::get<int>(x_value.value));
            }
            if (x_value.type == "double") {
                x_values.append(std::get<double>(x_value.value));
            }

            if (y_value.type == "int") {
                y_values.append(std::get<int>(y_value.value));
            }

            if (y_value.type == "double") {
                y_values.append(std::get<double>(y_value.value));
            }
        }
    }
    plt.attr("scatter")(x_values, y_values);
    plt.attr("show")();
}
