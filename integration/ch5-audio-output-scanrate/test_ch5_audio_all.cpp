// test_ch5_audio_all.cpp -- the one test binary of this package.
//
// It runs test_ch5_audio_output_scanrate.cpp (SPECIFICATION.md) and then
// test_ch5_audio_min_period.cpp (SPECIFICATION-min-period.md) and prints one
// combined summary line last. The earlier test's source is included
// unchanged, with its main() renamed; its output is read back and re-emitted
// without its own summary line, whose counts go into the combined one.
//
// The min-period test grades NTSC and PAL in two processes that run at the
// same time (a forked child grades PAL and hands its output and counts back
// through a temporary file), so a normal run needs about half the time of
// running the two standards one after the other.
//
// NMH_MUTATE: a selector of the earlier test runs only that test (its own
// summary stays); a selector of the min-period test runs only that one.

#include <unistd.h>
#include <sys/wait.h>
#include <functional>

#define main audio_scanrate_main
#include "test_ch5_audio_output_scanrate.cpp"
#undef main
#include "test_ch5_audio_min_period.cpp"

namespace {

std::string read_all(int fd) {
    std::string out;
    char buf[65536];
    ssize_t n;
    while ((n = ::read(fd, buf, sizeof buf)) > 0) out.append(buf, (size_t)n);
    return out;
}

// Runs f with stdout redirected into a temporary file; returns what it printed.
std::string capture_stdout(const std::function<int()>& f, int& rc) {
    std::fflush(stdout); std::cout.flush();
    int saved = ::dup(1);
    FILE* t = std::tmpfile();
    ::dup2(fileno(t), 1);
    rc = f();
    std::fflush(stdout); std::cout.flush();
    ::dup2(saved, 1);
    ::close(saved);
    ::lseek(fileno(t), 0, SEEK_SET);
    std::string text = read_all(fileno(t));
    std::fclose(t);
    return text;
}

// Splits "<n> passed, <m> failed" off the end of text.
bool split_summary(std::string& text, int& p, int& f) {
    while (!text.empty() && text.back() == '\n') text.pop_back();
    size_t nl = text.rfind('\n');
    std::string last = nl == std::string::npos ? text : text.substr(nl + 1);
    if (std::sscanf(last.c_str(), "%d passed, %d failed", &p, &f) != 2) return false;
    text = nl == std::string::npos ? "" : text.substr(0, nl + 1);
    return true;
}

const std::set<std::string> kMine = {"MP1", "MP2", "MP3", "MP4", "MP5CH1", "MC1STD", "MC2DMA", "MC2ADK", "MC3VOL", "MC3TIME",
                                      "MC4LEN", "MC4CPU", "MC5MARK", "MC5STOP", "MC6PER", "MP6LEN", "FIXMP"};

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    const char* env = std::getenv("NMH_MUTATE");
    const std::string mv = env ? env : "";

    if (!mv.empty() && !kMine.count(mv)) return audio_scanrate_main(argc, argv);

    if (!mv.empty()) {
        std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
        minper::Checks ck;
        minper::run_mutation(ck, dir, mv);
        std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
        return ck.failed == 0 ? 0 : 1;
    }

    int rc = 0;
    std::string earlier = capture_stdout([&] { return audio_scanrate_main(argc, argv); }, rc);
    int ep = 0, ef = 0;
    if (!split_summary(earlier, ep, ef)) { ep = 0; ef = 1; earlier += "FAIL: the earlier test printed no summary line\n"; }
    std::fputs(earlier.c_str(), stdout);
    std::fflush(stdout);

    FILE* t = std::tmpfile();
    std::cout.flush(); std::fflush(stdout);
    pid_t pid = ::fork();
    if (pid == 0) {
        ::dup2(fileno(t), 1);
        minper::Checks ck;
        minper::run_standard(ck, dir, false);
        std::cout.flush(); std::fflush(stdout);
        std::printf("@@COUNTS %d %d\n", ck.passed, ck.failed);
        std::fflush(stdout);
        ::_exit(0);
    }
    minper::Checks ck;
    minper::run_standard(ck, dir, true);
    std::cout.flush(); std::fflush(stdout);
    int status = 0;
    ::waitpid(pid, &status, 0);
    ::lseek(fileno(t), 0, SEEK_SET);
    std::string pal = read_all(fileno(t));
    std::fclose(t);
    int pp = 0, pf = 0;
    size_t at = pal.rfind("@@COUNTS ");
    if (at != std::string::npos && std::sscanf(pal.c_str() + at, "@@COUNTS %d %d", &pp, &pf) == 2) {
        pal.erase(at);
    } else {
        pal += "FAIL: the PAL process did not finish\n";
        pf = 1;
    }
    std::fputs(pal.c_str(), stdout);
    int p = ep + ck.passed + pp, f = ef + ck.failed + pf;
    std::printf("%d passed, %d failed\n", p, f);
    return f == 0 ? 0 : 1;
}
