#ifndef SIMULATION_CLOCK_H
#define SIMULATION_CLOCK_H

#include <unordered_map>
#include <vector>
#include <functional>

enum class SimState {
    Playing,
    Paused,
    // StepOnce,
};

struct TimedState {
    float time_ms;
    // std::vector<float> neuron_state;
    size_t hash;

    TimedState(float time, size_t hash) : time_ms(time), hash(hash)
    {}
};

class SimulationClock {
public:
    SimulationClock() : 
        sim_state(SimState::Paused),
        sim_time_ms_(0), 
        dt_ms_(1),
        next_id_calculate(0)
    {}

    void update(float dt) {
        if (sim_state == SimState::Paused) {
            return;
        }

        const float speed_ratio = 50.0;
        sim_time_ms_ += dt * speed_ratio;

        uint64_t t = last_time_ms();
        while (sim_time_ms_ > last_time_ms()) {
            t += dt_ms_;
            timeline_.emplace_back(t, 0);
        }
    }

    void pause() {
        sim_state = SimState::Paused;
    }

    void play() {
        sim_state = SimState::Playing;
    }

    void step() {
        sim_state = SimState::Paused;

        sim_time_ms_ += dt_ms_;
        uint64_t t = last_time_ms() + dt_ms_;
        timeline_.emplace_back(t, 0);
    }

    void reset() {
        sim_time_ms_ = 0;
        timeline_.clear();
        state_hash_index_.clear();
    }

    bool advance() {
        if (timeline_.empty()) {
            // std::cerr << "Bad order before `advance`" << std::endl;
            return false;
        }
        if (next_id_calculate >= timeline_.size()) {
            return false;
        }

        return true;
    }

    float store(const std::vector<float>& neuron_state) {
        size_t hash = hash_state(neuron_state);
        timeline_[next_id_calculate].hash = hash;
        float t = timeline_[next_id_calculate].time_ms;
        auto it = state_hash_index_.find(hash);
        if (it != state_hash_index_.end()) {
            std::cout << "repeated state\n";
        }
        state_hash_index_[hash] = t;
        ++next_id_calculate;
        std::cout << "stored <<" << hash << ">> at " << t << "ms\n";
        return t;
    }

    float current_time_ms() const {
        return sim_time_ms_;
    }

    void set_dt(int dt) {
        dt_ms_ = dt;
    }

    bool has_seen_state(const std::vector<float>& neuron_state, float& matched_time) const {
        size_t hash = hash_state(neuron_state);
        auto it = state_hash_index_.find(hash);
        if (it != state_hash_index_.end()) {
            matched_time = it->second;
            return true;
        }
        return false;
    }

    const std::vector<float>& get_state_at(uint64_t time_ms) const {
        if (time_ms <= last_time_ms()) {

        }
    }

    uint64_t last_time_ms() const {
        if (timeline_.empty()) {
            return 0;
        } else {
            return timeline_.back().time_ms;
        }
    }

private:
    SimState sim_state;
    float sim_time_ms_;
    int dt_ms_;
    size_t next_id_calculate;

    std::vector<TimedState> timeline_;
    std::unordered_map<size_t, float> state_hash_index_;

    size_t hash_state(const std::vector<float>& state) const {
        std::hash<float> hasher;
        size_t hash = 0;
        for (float v : state) {
            hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

#endif