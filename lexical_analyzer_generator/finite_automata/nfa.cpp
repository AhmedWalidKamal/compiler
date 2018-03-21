//
// Created by awalid on 3/21/18.
//

#include <iostream>
#include <utility>
#include "nfa.h"
#include "nfa_state.h"

nfa::nfa(std::shared_ptr<state> start_state, std::vector<std::shared_ptr<state>> acceptance_states, int total_states)
        : fa(start_state, acceptance_states, total_states) {
    nfa::start_state = start_state;
    nfa::acceptance_states = acceptance_states;
    nfa::total_states = total_states;
}

void nfa::dfs (std::shared_ptr<state> curr_state, std::vector<bool> &visited, std::shared_ptr<std::ofstream> vis)
{
    visited[curr_state->get_id()] = true;

    std::map<std::string, std::vector<std::shared_ptr<nfa_state>>> transitions
            = std::static_pointer_cast<nfa_state>(curr_state)->get_transitions();

//    std::cout << "Current state = " << curr_state->get_id() << "\n";
//    for (auto trans : transitions) {
//        std::cout << "Key = " << trans.first << ", Destinations: ";
//        for (auto curr : trans.second)
//        {
//            std::cout << curr->get_id() << " ";
//        }
//        std::cout << "\n";
//    }
//    std::cout << "\n";

    for (auto edge : transitions)
    {
        std::string label = edge.first;
        std::vector<std::shared_ptr<nfa_state>> next_states = edge.second;
        for (auto state : next_states)
        {
            // Visualize
            if (vis != nullptr) {
                if (label.empty()) {
                    label = "ϵ";
                }
                *vis << curr_state->get_id() << " -> " << state->get_id() << " [ label = \"" << label << "\" ];\n";
            }
            if (!visited[state->get_id()]) {
                dfs(state, visited, vis);
            }
        }
    }
}


