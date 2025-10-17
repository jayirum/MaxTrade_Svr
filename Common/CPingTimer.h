class CPingTimer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::seconds;

    CPingTimer(int timeout_sec)
        : timeout(Duration(timeout_sec)), start_time(Clock::now()), fired(false) {
    }

    bool tick() {
        auto now = Clock::now();
        if (now - start_time >= timeout) 
        {
            if (fired) return false;

            start_time = Clock::now();
            fired = true;
            return true;
        }
        fired = false;
        return false;
    }

    void reset() {
        start_time = Clock::now();
        fired = false;
    }

private:
    TimePoint start_time;
    Duration timeout;
    bool fired;
};
