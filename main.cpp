#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "Range.hpp"

using namespace std;

constexpr size_t THREAD_COUNT = 10;
static_assert(THREAD_COUNT > 0, "Set positive THREAD_COUNT");

//
int main(int argc, char* argv[])
{
    //
    // Process input parameters
    //
    size_t thread_count = THREAD_COUNT;
    switch(argc)
    {
    case 3: // Ok,
        break;
    case 4: // 3rd optional parameter for CTest
        {
            const char* szThreadCount = argv[3];
            thread_count = stoi(szThreadCount);
            if (thread_count < 1)
            {
                cerr << "Bad thread count: " << thread_count << endl;
                return 1;
            }
        }
        break;
    default:
        cerr << "Need 2 or 3 parameters, given " << argc - 1 << endl;
        return 2;
    }

    const char* filepath = argv[1];
    const string mask = argv[2];

    // Just in case check mask length
    const size_t mask_length = mask.length();
    if (!mask_length)
    {
        cerr << "Empty mask given" << endl;
        return 3;
    }

    //
    // Get file size
    //
    ifstream file(filepath, ios::binary | ios::ate);
    if (!file.is_open())
    {
        cerr << "Failed to open " << filepath << endl;
        return 4;
    }

    const size_t filesize = file.tellg();

    // Check for 'too many threads' (optional step)
    const size_t max_thread_count = filesize  / (mask_length + 1); // >= (mask length + EOL) per each thread
    if (thread_count > max_thread_count)
    {
        cout << "Decreasing thread count from " << thread_count << " to " << max_thread_count << endl;
        thread_count = max_thread_count;
    }

    // Get range length
    const size_t range_length = filesize / thread_count;
    assert(range_length > 0);

    //
    // Divide the file to ranges
    //
    vector<Range> ranges;
    size_t startpos = 0;
    for(size_t i = 0; i < thread_count; i++, startpos += range_length)
    {
        assert(startpos < filesize);

        const size_t real_range_length = i != thread_count - 1 ? range_length : (filesize - startpos);

        ranges.emplace_back(filepath, startpos, real_range_length, mask);
    }

    //
    // Run work threads
    //
    {
        vector<thread> pool;

        for(Range& range : ranges)
        {
            pool.emplace_back(std::ref(range));
        }

        for(thread& workThread : pool)
        {
            workThread.join();
        }
    }

    //
    // Output results
    //
    vector<string> match_reports;
    size_t line_offset = 0;
    bool processed_next_range_1st_line = false;
    for(const Range& range : ranges)
    {
        range.add_match_reports(match_reports, line_offset, processed_next_range_1st_line);
    }

    cout << match_reports.size() << endl;

    for(const string& match_report : match_reports)
    {
        cout << match_report << endl;
    }

    return 0;
}
