#include "csvwriter.h"
#include <fstream>
using namespace std;



CsvWriter::CsvWriter(const string &path)
{
    filePath = path;  //output file path, buffer start empty
}


void CsvWriter::header(const vector<string> &columns) // CSV header to buffer
{
    buffer += join(columns);
} // header fields into one CSV line and append


void CsvWriter::row(const vector<string> &columns)
{ // add data row to buffer
    buffer += join(columns); // one csv line and append
}


void CsvWriter::close()
{
    ofstream out(filePath); //open file to write
    out << buffer; //write entire buffer to file
}


// vector of different fields into single csv string
string CsvWriter::join(const vector<string> &v)
{
    string s; // to be comma accumulator
    for (size_t i = 0; i < v.size(); i++)
    {
        if (i) //all but first index (skip 0)
        {
            s += ",";
        }

        s += v[i]; //append to fields text
    }
    s += "\n"; // end csv list

    return s;
}
