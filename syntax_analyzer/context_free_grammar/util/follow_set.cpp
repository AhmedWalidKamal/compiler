//
// Created by awalid on 4/27/18.
//

#include "follow_set.h"
#include "../cfg.h"

follow_set::follow_set() = default;

void follow_set::add_symbol(std::string key, cfg_symbol symbol) {
    follow_set::my_set[key].insert(symbol);
}

const std::unordered_map<std::string, std::unordered_set<cfg_symbol, cfg_symbol::hasher, cfg_symbol::comparator>> &
follow_set::get_set_map() const {
    return follow_set::my_set;
}

bool follow_set::has_eps(std::string symbol) {
    for(auto sym : follow_set::my_set[symbol]) {
        if (sym.get_name() == EPS) {
            return true;
        }
    }
    return false;
}

bool follow_set::empty(std::string key) {
    return follow_set::my_set[key].empty();
}
