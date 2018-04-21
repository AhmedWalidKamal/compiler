#ifndef COMPILER_PARSING_TABLE_H
#define COMPILER_PARSING_TABLE_H

#include "cfg.h"
#include "first_follow.h"
#include "cfg_symbol.h"

#include <map>

/**
    Parsing table that contains state machine to parse input program.
*/
class parsing_table
{
private:
    /** Grammar that the parsing table represents.*/
    cfg grammar;
    /** First and Follow sets for the grammar above.*/
    first_follow_sets first_follow;
    /** Parsing Table accessed by rule (NON_TERMINAL) and string (TOKEN) to
       get cfg_production used or error.*/
    std::map <std::pair<std::shared_ptr<cfg_rule>, std::string>, cfg_production> table;

    /**
        This function fills the given row (i.e row of the given cfg_rule) table with.
        the given cfg_production under the given tokens.
        @param cfg_production production to fill fill entries
        @param std::shared_ptr<cfg_rule> cfg_rule row
        @param std::vector<std::string> input tokens columns to be filled in the given row
        @return bool if entries filled were already empty or not (denoting ambiguous grammar) 
    */
    bool fill_entries (cfg_production, std::shared_ptr<cfg_rule>, std::vector<std::string>);
    /**
        This function builds the parsing table for the previously defined grammar.
        It fills the table map defined above by productions using the first and.
        follow sets supplied before.
    */
    void build();
public:
    /**
        Contructor for parsing table.
        @param cfg grammar to build parsing table for
        @param first_follow_sets to be used in building parsing table
    */
    parsing_table (cfg, first_follow_sets);
    /**
        Default Contructor for parsing table.
    */
    parsing_table ();
    /**
        This function returns the production corresponding to the given cfg_rule.
        (NON_TERMINAL) and given string (TOKEN) from the parsing table.
        @returns cfg_production or NULL if none found to denote Error
    */
    cfg_production get_production (std::shared_ptr<cfg_rule>, std::string);
};



#endif //COMPILER_PARSING_TABLE_H
