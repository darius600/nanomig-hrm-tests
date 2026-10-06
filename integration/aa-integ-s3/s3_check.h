// s3_check.h -- grading scaffolding of package aa-integ-s3.
//
// Every proposition, precondition and anchor goes through Grader: PASS:/FAIL:
// on the graded build (TG68K, built with ENABLE_AGA), RECORDED: on the other,
// whatever the condition shows. A line whose precondition does not hold is
// NOT GRADED, never PASS or FAIL.

#pragma once

#include <iostream>
#include <map>
#include <string>
#include "s3_std.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace s3 {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

inline Checks checks;
inline bool g_graded = false;
inline const char* g_mutate = nullptr;

// a label printed as RECORDED on the build without ENABLE_AGA carries no claim id
inline std::string without_ids(const std::string& s) {
    std::string out, tok;
    auto flush = [&](bool space) {
        bool id = tok.rfind("opus:", 0) == 0 || tok.rfind("sol:", 0) == 0;
        if (!id) { out += tok; if (space) out += ' '; }
        tok.clear();
    };
    for (char c : s) {
        if (c == ' ') flush(true);
        else tok += c;
    }
    flush(false);
    return out;
}

class Grader {
public:
    void expect(bool cond, const std::string& label) {
        if (g_graded) checks.expect(cond, label);
        else checks.record(without_ids(label) + " [build without ENABLE_AGA: would have " +
                           (cond ? "PASSed" : "FAILed") + "]");
    }
    void expect_if(bool pre, bool cond, const std::string& label) {
        if (pre) expect(cond, label);
        else checks.record("NOT GRADED: " + without_ids(label) + " [a precondition does not hold]");
    }
    // an assertion line: graded when its preconditions hold; the verdict is kept for the joint assertions
    void assert_line(const std::string& id, bool pre, bool cond, const std::string& label);
    void note(const std::string& msg) { checks.note(msg); }
    void record(const std::string& msg) { checks.record(msg); }
};
inline Grader g;

struct Verdict { bool graded = false, passed = false; };
inline std::map<std::string, Verdict> g_verdict;

inline void Grader::assert_line(const std::string& id, bool pre, bool cond, const std::string& label) {
    g_verdict[id] = {pre, pre && cond};
    expect_if(pre, cond, label);
}

inline bool mutating(const char* label) { return g_mutate && std::string(g_mutate) == label; }

// the proposition `prop` runs in a normal run, and in a mutation run when the
// mutation names it or one of its ids
inline bool want(const char* prop) {
    const char* only = std::getenv("NMH_ONLY");
    if (only) {
        std::string o = only, p = prop;
        if (o != p) return false;
    }
    if (!g_mutate) return true;
    std::string m = g_mutate, p = prop;
    return m == p || m.compare(0, p.size() + 1, p + "-") == 0;
}
// a run that only the normal run (or a named mutation) needs
inline bool full(const char* label) { return !g_mutate || mutating(label); }

inline std::string hexs(uint32_t v, int digits = 4) {
    char b[16];
    std::snprintf(b, sizeof b, "%0*X", digits, v);
    return b;
}
inline std::string dec(long long v) { return std::to_string(v); }

}  // namespace s3
