/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_CSV_HPP__
#define __LC_CSV_HPP__

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace lc {

class CSVRow
{
public:
    std::string const& operator[](std::size_t index) const
    {
        return m_data[index];
    }
    
	std::size_t size() const
    {
        return m_data.size();
    }

	std::vector<std::string> row() const
	{
		return m_data;
	}
 
	void readNextRow(std::istream& str)
    {
        std::string         line;
        std::getline(str, line);

        std::stringstream   lineStream(line);
        std::string         cell;

        m_data.clear();

		/*  Cannot handle bogus escapes in the middle of a string \\
		boost::tokenizer<boost::escaped_list_separator<char>> tk(line, boost::escaped_list_separator<char>('\\', ',', '\"'));
		for (auto& cell : tk) {
			m_data.push_back(cell);
		}
		*/

		/* Cannot handle commas inside quote
        while(std::getline(lineStream, cell, ',')) {
			if (!cell.empty() && cell[0] == '"') {
				cell = cell.substr(1, cell.length() - 2);
			}
            m_data.push_back(cell);
        }
		*/
    }
private:
    std::vector<std::string> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}   

class CSVIterator
{   
public:
    typedef std::input_iterator_tag     iterator_category;
    typedef CSVRow                      value_type;
    typedef std::size_t                 difference_type;
    typedef CSVRow*                     pointer;
    typedef CSVRow&                     reference;

    CSVIterator(std::istream& str)  : m_str(str.good() ? &str : NULL) { ++(*this); }
    CSVIterator()                   : m_str(NULL) {}

    // Pre Increment
    CSVIterator& operator++()               { if (m_str) { (*m_str) >> m_row; m_str = m_str->good() ? m_str : NULL; } return (*this); }
    // Post increment
    CSVIterator operator++(int)             { CSVIterator    tmp(*this); ++(*this); return tmp; }
    CSVRow const& operator*()   const       { return m_row; }
    CSVRow const* operator->()  const       { return &m_row; }

    bool operator==(CSVIterator const& rhs) { return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL))); }
    bool operator!=(CSVIterator const& rhs) { return !((*this) == rhs); }

private:
    std::istream*       m_str;
    CSVRow              m_row;
};

};

#endif // __LC_CSV_HPP__
