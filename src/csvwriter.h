#ifndef CSVWRITER_H
#define CSVWRITER_H
#include <string>
#include <vector>
using namespace std;

class CsvWriter {
public:
    CsvWriter(const string &path);
    void header(const vector<string> &columns); //csv header (index and arrival time)
    void row(const vector<string> &columns); //csv data
    void close();

    string filePath;
    string buffer;
    string join(const vector<string> &v); //commas
};

#endif // CSVWRITER_H
