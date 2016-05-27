
#include <orcus/stream.hpp>
#include <orcus/json_parser.hpp>
#include <orcus/json_parser_thread.hpp>
#include <orcus/string_pool.hpp>

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/time.h>

#define SIMULATE_PROCESSING_OVERHEAD 0

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
    std::vector<double> m_results;

    void do_work()
    {
#if SIMULATE_PROCESSING_OVERHEAD
        double f = m_results.empty() ? 0.0 : m_results.back();

        for (size_t i = 0; i < 1000; ++i)
            f += 0.1;

        m_results.push_back(f);
#endif
    }

public:
    void begin_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_parse);
        do_work();
    }

    void end_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::end_parse);
        do_work();
    }

    void begin_array()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_array);
        do_work();
    }

    void end_array()
    {
        m_tokens.emplace_back(json::parse_token_t::end_array);
        do_work();
    }

    void begin_object()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_object);
        do_work();
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
        do_work();
    }

    void end_object()
    {
        m_tokens.emplace_back(json::parse_token_t::end_object);
        do_work();
    }

    void boolean_true()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_true);
        do_work();
    }

    void boolean_false()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_false);
        do_work();
    }

    void null()
    {
        m_tokens.emplace_back(json::parse_token_t::null);
        do_work();
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
        do_work();
    }

    void number(double val)
    {
        m_tokens.emplace_back(val);
        do_work();
    }

    size_t token_size() const
    {
        return m_tokens.size();
    }

    double work_value() const
    {
        return m_results.back();
    }
};

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    const char* filepath = argv[1];
    string content = orcus::load_file_content(filepath);

    cout << "file: " << filepath << endl;

    handler hdl;

    {
        stack_printer __stack_printer__("parsing");
        orcus::json_parser<handler> parser(content.data(), content.size(), hdl);
        parser.parse();
    }

    cout << "parsed token count: " << hdl.token_size() << endl;
#if SIMULATE_PROCESSING_OVERHEAD
    cout << "work value: " << hdl.work_value() << endl;
#endif

    return EXIT_SUCCESS;
}
