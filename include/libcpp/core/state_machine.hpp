#pragma once
// state_machine.hpp — StateMachine<TState> design pattern.
//
// Manages states, transitions between them, and per-state actions.
// Throws on undefined transitions or missing actions.

#include <functional>
#include <map>
#include <stdexcept>
#include <utility>

namespace libcpp {
namespace core {

/// @brief Finite state machine with transition callbacks and per-state actions.
template <typename TState>
class StateMachine {
public:
    StateMachine() = default;
    ~StateMachine() = default;

    void addState(const TState& state)
    {
        _states[state] = true;
    }

    void addTransition(const TState& startState, const TState& finalState,
                       const std::function<void()>& lambda = nullptr)
    {
        _transitions[std::make_pair(startState, finalState)] = lambda;
    }

    void addAction(const TState& state, const std::function<void()>& lambda)
    {
        _actions[state] = lambda;
    }

    void transitionTo(const TState& state)
    {
        if (_initialized) {
            auto key = std::make_pair(_current, state);
            auto it = _transitions.find(key);
            if (it == _transitions.end())
                throw std::runtime_error("StateMachine: undefined transition");
            if (it->second)
                it->second();
        }
        if (_states.find(state) == _states.end())
            throw std::runtime_error("StateMachine: unknown state");
        _current = state;
        _initialized = true;
    }

    void update()
    {
        if (!_initialized)
            throw std::runtime_error("StateMachine: no current state set");
        auto it = _actions.find(_current);
        if (it == _actions.end())
            throw std::runtime_error("StateMachine: no action for current state");
        it->second();
    }

    [[nodiscard]] TState currentState() const
    {
        if (!_initialized)
            throw std::runtime_error("StateMachine: not initialized");
        return _current;
    }

    [[nodiscard]] bool canTransitionTo(const TState& state) const
    {
        if (!_initialized)
            return false;
        return _transitions.find(std::make_pair(_current, state)) != _transitions.end();
    }

private:
    TState _current{};
    bool   _initialized = false;

    std::map<TState, bool>                                    _states;
    std::map<TState, std::function<void()>>                   _actions;
    std::map<std::pair<TState, TState>, std::function<void()>> _transitions;
};

} // namespace core
} // namespace libcpp
