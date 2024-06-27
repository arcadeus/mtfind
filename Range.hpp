#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

//
class Range
{
    const char*  m_filepath;
    const size_t m_startpos;
    const size_t m_range_length;
    const string& m_mask;

    size_t m_lines_count = 0;
    bool   m_processed_next_range_1st_line = false;
    map<pair<size_t,size_t>, string> m_matches;

    string get_match(const string& line, size_t pos) const
    {
        string res;
        for(const char& char_mask : m_mask)
        {
            const char& char_line = line[pos++];

            if (char_mask == '?' || char_mask == char_line)
                res += char_line;
            else
                return "";
        }

        return res;
    }

    void process_line(const string& line)
    {
        if (line.length() < m_mask.length())
            return;

        for(size_t pos = 0; pos <= line.length() - m_mask.length(); )
        {
            const string& match = get_match(line, pos);
            if (match.empty())
                pos++;
            else
            {
                m_matches.try_emplace({m_lines_count, pos}, match);
                pos += m_mask.length();
            }
        }
    }

public:
    void operator()()
    {
        ifstream file(m_filepath);
        file.seekg(m_startpos);

        size_t processed_length = 0;
        string line;
        while (getline(file, line))
        {
            process_line(line);
            m_lines_count++;
            processed_length += line.length() + 1; // count EOL too
            if (processed_length >= m_range_length)
            {
                m_processed_next_range_1st_line = (processed_length > m_range_length);
                break;
            }
        }
    }

    Range(const char* filepath, size_t startpos, const size_t range_length, const string& mask)
        : m_filepath(filepath)
        , m_startpos(startpos)
        , m_range_length(range_length)
        , m_mask(mask)
    {
    }

    void add_match_reports(
        vector<string>& match_reports,
        size_t& line_offset,
        bool& processed_next_range_1st_line
    ) const
    {
        for(auto& it : m_matches)
        {
            const size_t& line  = it.first.first;
            const size_t& pos   = it.first.second;
            const string& match = it.second;

            if (line == 0 && processed_next_range_1st_line)
                continue;

            match_reports.push_back(to_string(line_offset + line + 1) + " " + to_string(pos + 1) + " " + match);
        }

        if (m_lines_count)
            line_offset += m_lines_count;
        if (m_processed_next_range_1st_line)
            line_offset--;
        processed_next_range_1st_line = m_processed_next_range_1st_line;
    }
};
