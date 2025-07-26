#ifndef SIMULATION_CLOCK_H
#define SIMULATION_CLOCK_H

#include <unordered_map>
#include <vector>
#include <functional>

class SimulationClock {
public:
    SimulationClock()
        : sim_time_ms_(0), dt_ms_(1) {}

    void reset() {
        sim_time_ms_ = 0;
        state_history_.clear();
        state_hash_index_.clear();
    }

    void advance(const std::vector<float>& neuron_state) {
        size_t hash = hash_state(neuron_state);
        state_history_.push_back(neuron_state);
        state_hash_index_[hash] = sim_time_ms_;
        sim_time_ms_ += dt_ms_;
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

    const std::vector<float>& get_state_at(int index) const {
        return state_history_[index];
    }

    size_t state_count() const {
        return state_history_.size();
    }

private:
    float sim_time_ms_;
    int dt_ms_;

    std::vector<std::vector<float>> state_history_;
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