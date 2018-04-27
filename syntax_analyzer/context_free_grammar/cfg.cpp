#include "cfg.h"
#include <fstream>
#include <utility>
#include <iostream>

cfg::cfg ()
    : non_terminals (), terminals (), rules () {

}

cfg::cfg (std::string grammar_file) 
    : non_terminals (), terminals (), rules () {

}

void cfg::parse (std::string grammar_file) {
    std::ifstream grammar_in_file (grammar_file.c_str ());
    // Checks if grammar file exists or not.
    if (!grammar_in_file.good ()) {
        //TODO: - Report "File doesn't exist!" error.
    }
    std::string line;
    while (std::getline (grammar_in_file, line)) {

    }
}
void cfg::add_rule () {

}

std::vector <cfg_symbol> cfg::get_non_terminals () {
    return non_terminals;
}

std::vector <cfg_symbol> cfg::get_terminals () {
    return terminals;
}

std::vector <cfg_rule> cfg::get_rules () {
    return rules;
}

cfg_symbol cfg::get_start_symbol () {
    return start_symbol;
}

bool cfg::is_ll_1 () {
    return true;
}

void
cfg::process_first_set(int prod_symbol_index, std::shared_ptr<first_set> first_set,
                       cfg_production *prod) {
    if (prod_symbol_index == prod->get_symbols().size()) {
        return;
    }
    auto curr_prod_symbol = prod->get_symbols()[prod_symbol_index];

    // Cycle case
    if (curr_prod_symbol.get_name() == prod->get_lhs_symbol().get_name()) {
        return;
    }
    
    if (curr_prod_symbol.get_type() == TERMINAL) {
        first_set->add_symbol(curr_prod_symbol.get_name(), curr_prod_symbol, prod);
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), curr_prod_symbol, prod);
        return;
    }

    // If current prod symbol isn't processed yet, process it.
    if (first_set->empty(curr_prod_symbol.get_name())) {
        // Go to curr_prod_symbol rule and iterate over all its prods and calc its first set.
        for (auto production : cfg::grammar[curr_prod_symbol].get_productions()) {
            process_first_set(0, first_set, &production);
        }
    }

    // Here curr_proc_symbol's first_set should be already processed.
    auto symbols_map = first_set->get_set_map();
    for (auto symbol : symbols_map[curr_prod_symbol.get_name()]) {
        // Add all symbols of curr_prod_symbol to lhs symbol's first set
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), symbol.first, prod);
    }

    // If curr_prod_symbol has eps in its first set, then continue processing this production.
    if (first_set->has_eps(curr_prod_symbol.get_name())) {
        cfg_symbol eps(EPS, TERMINAL); // TODO: Make this a constant.
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), eps, prod);
        process_first_set(prod_symbol_index + 1, first_set, prod);
    }
}

std::shared_ptr<first_set> cfg::get_first_set() {
    std::shared_ptr<first_set> first_set_ptr = std::make_shared<first_set>();

    /// Build first set
    for (auto non_terminal : cfg::non_terminals) {
        //std::cout << "Current non_terminal = " << non_terminal.get_name() << "\n";
        if (!first_set_ptr->empty(non_terminal.get_name())) {
            continue;
        }
        auto rule = cfg::grammar[non_terminal];
        //std::cout << "Current rule has lhs = " << rule.get_lhs_symbol().get_name() << "\n";
        for (auto production : cfg::grammar[non_terminal].get_productions()) { // Iterate over all productions from this non-terminal
            //std::cout << "Current production's lhs = " << production.get_lhs_symbol().get_name() << "\n";
            process_first_set(0, first_set_ptr, &production);
        }
    }
    for (auto terminal : cfg::terminals) {
        auto set_map = first_set_ptr->get_set_map();
        if (set_map[terminal.get_name()].empty()) {
            first_set_ptr->add_symbol(terminal.get_name(), terminal, nullptr);
        }
    }
    return first_set_ptr;
}

const std::unordered_set <cfg_symbol, cfg_symbol::hasher, cfg_symbol::comparator> 
                        & cfg::get_cfg_symbols () const {
    return cfg::cfg_symbols;
}

void cfg::set_cfg_symbols (
        const std::unordered_set <cfg_symbol, cfg_symbol::hasher
            , cfg_symbol::comparator> & cfg_symbols) {
    cfg::cfg_symbols = cfg_symbols;
}

void cfg::process_follow_set(cfg_symbol non_terminal, std::shared_ptr<follow_set> follow_set_ptr) {
    if (non_terminal.get_name() == cfg::start_symbol.get_name()) {
        cfg_symbol s_$("$", END_MARKER);
        follow_set_ptr->add_symbol(non_terminal.get_name(), s_$);
    }
    for (auto production : cfg::cfg_symbol_productions[non_terminal]) {
        for (int i = 0; i < production.get_symbols().size(); i++) {
            auto symbol = production.get_symbols()[i];
            if (symbol.get_name() == non_terminal.get_name()) {
                if (i + 1 == production.get_symbols().size()) {
                    // This non_terminal is the last symbol in the production.
                    if (follow_set_ptr->empty(production.get_lhs_symbol().get_name())) {
                        // Calc Follow(lhs) if not calculated
                        process_follow_set(production.get_lhs_symbol(), follow_set_ptr);
                    }
                    // Add everything in Follow(lhs) to Follow(non_terminal)
                    /// TODO: Make this a function in the cfg_set class.    
                    auto follow_set_map = follow_set_ptr->get_set_map();
                    for (auto symbol : follow_set_map[production.get_lhs_symbol().get_name()]) {
                        follow_set_ptr->add_symbol(non_terminal.get_name(), symbol);
                    }
                } else {
                    // There is a symbol after this non_terminal in the production.

                    bool has_eps = true;
                    int curr_pos = i + 1;
                    while (has_eps) {
                        if (curr_pos == production.get_symbols().size()) {
                            break;
                        }
                        has_eps = false;
                        // Put everything in first of the next symbol in the follow of non_terminal
                        auto cfg_first_set_map = cfg::get_first_set()->get_set_map();
//                        for (auto elem : cfg_first_set_map) {
//                            std::cout << elem.first << "\n";
//                        }
                        auto next_first_set = cfg_first_set_map[production.get_symbols()[curr_pos++].get_name()];
                        for (auto symbol : next_first_set) {
                            if (symbol.first.get_name() != EPS) {
                                follow_set_ptr->add_symbol(non_terminal.get_name(), symbol.first);
                            } else {
                                has_eps = true;
                            }
                        }    
                    }
                    if (has_eps) {
                        // If eps is in first of last symbol, add everything in Follow(lhs) to Follow(non_terminal)
                        if (follow_set_ptr->empty(production.get_lhs_symbol().get_name())) {
                            // Calc Follow(lhs) if not calculated
                            process_follow_set(production.get_lhs_symbol(), follow_set_ptr);
                        }
                        // Add everything in Follow(lhs) to Follow(non_terminal)
                        // TODO: Make this a function in first_set
                        auto follow_set_map = follow_set_ptr->get_set_map();
                        for (auto symbol : follow_set_map[production.get_lhs_symbol().get_name()]) {
                            follow_set_ptr->add_symbol(non_terminal.get_name(), symbol);
                        }
                    }
                }
                break;
            }
        }
    }
}

std::shared_ptr<follow_set> cfg::get_follow_set() {
    std::shared_ptr<follow_set> follow_set_ptr = std::make_shared<follow_set>();
    for (auto non_terminal : cfg::non_terminals) {
        if (!follow_set_ptr->empty(non_terminal.get_name())) {
            continue;
        }
//        std::cout << "Calculating follow of non_terminal = " << non_terminal.get_name() << "\n";
        process_follow_set(non_terminal, follow_set_ptr);
    }
    return follow_set_ptr;
}

void cfg::set_non_terminals(const std::vector<cfg_symbol> &non_terminals) {
    cfg::non_terminals = non_terminals;
}

const std::unordered_map<cfg_symbol, cfg_rule, cfg_symbol::hasher, cfg_symbol::comparator> &cfg::get_grammar() const {
    return grammar;
}

void
cfg::set_grammar(const std::unordered_map<cfg_symbol, cfg_rule, cfg_symbol::hasher, cfg_symbol::comparator> &grammar) {
    cfg::grammar = grammar;
}

void cfg::set_start_symbol(const cfg_symbol &start_symbol) {
    cfg::start_symbol = start_symbol;
}

const std::unordered_map<cfg_symbol, std::vector<cfg_production>, cfg_symbol::hasher, cfg_symbol::comparator> &
cfg::get_cfg_symbol_productions() const {
    return cfg_symbol_productions;
}

void cfg::set_cfg_symbol_productions(
        const std::unordered_map<cfg_symbol, std::vector<cfg_production>, cfg_symbol::hasher, cfg_symbol::comparator> &cfg_symbol_productions) {
    cfg::cfg_symbol_productions = cfg_symbol_productions;
}

void cfg::set_terminals(const std::vector<cfg_symbol> &terminals) {
    cfg::terminals = terminals;
}

