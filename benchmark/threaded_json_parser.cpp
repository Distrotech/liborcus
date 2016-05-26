
#include <orcus/stream.hpp>
#include <orcus/threaded_json_parser.hpp>
#include <orcus/string_pool.hpp>

#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/time.h>

using namespace std;
using namespace orcus;

namespace {

class stack_printer
{
public:
    explicit stack_printer(const char* msg) :
        m_msg(msg)
    {
        fprintf(stdout, "%s: --begin\n", m_msg.c_str());
        m_start_time = get_time();
    }

    ~stack_printer()
    {
        double end_time = get_time();
        fprintf(stdout, "%s: --end (duration: %g sec)\n", m_msg.c_str(), (end_time-m_start_time));
    }

private:
    double get_time() const
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }

    ::std::string m_msg;
    double m_start_time;
};

}

class handler
{
    string_pool m_pool;
    json::parse_tokens_t m_tokens;

public:
    void begin_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_parse);
    }

    void end_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::end_parse);
    }

    void begin_array()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_array);
    }

    void end_array()
    {
        m_tokens.emplace_back(json::parse_token_t::end_array);
    }

    void begin_object()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_object);
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        if (transient)
        {
            pstring s = m_pool.intern(p, len).first;
            p = s.get();
            len = s.size();
        }

        m_tokens.emplace_back(json::parse_token_t::object_key, p, len);
    }

    void end_object()
    {
        m_tokens.emplace_back(json::parse_token_t::end_object);
    }

    void boolean_true()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_true);
    }

    void boolean_false()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_false);
    }

    void null()
    {
        m_tokens.emplace_back(json::parse_token_t::null);
    }

    void string(const char* p, size_t len, bool transient)
    {
        if (transient)
        {
            pstring s = m_pool.intern(p, len).first;
            p = s.get();
            len = s.size();
        }

        m_tokens.emplace_back(json::parse_token_t::string, p, len);
    }

    void number(double val)
    {
        m_tokens.emplace_back(val);
    }

    size_t token_size() const
    {
        return m_tokens.size();
    }
};

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    const char* filepath = argv[1];
    string content = orcus::load_file_content(filepath);

    size_t min_token_size = 0;
    size_t max_token_size = 0;

    if (argc >= 3)
    {
        const char* p = argv[2];
        min_token_size = strtol(p, nullptr, 10);
    }

    if (argc >= 4)
    {
        const char* p = argv[3];
        max_token_size = strtol(p, nullptr, 10);
    }

    cout << "file: " << filepath << endl;
    cout << "min token size: " << min_token_size << endl;
    cout << "max token size: " << max_token_size << endl;

    handler hdl;

    {
        stack_printer __stack_printer__("parsing");
        orcus::threaded_json_parser<handler> parser(content.data(), content.size(), hdl, min_token_size, max_token_size);
        parser.parse();
    }

    cout << "parsed token count: " << hdl.token_size() << endl;

    return EXIT_SUCCESS;
}
