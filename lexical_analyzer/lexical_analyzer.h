#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include "../lexical_analyzer_generator/finite_automata/dfa_state.h"
#include "../lexical_analyzer_generator/finite_automata/dfa.h"
#include "token.h"
#include <string>
#include <vector>


struct acceptance_state {
  int state_id;
  std::string token_class;
};

class lexical_analyzer {
private:
  std::string lexical_analyzer_file;
  std::string code_file;
  int start_state_id;
  int total_states;
  std::shared_ptr<dfa> dfa_ptr;
  std::vector<acceptance_state> acceptance_states_info;
  std::vector<std::string> transition_table_inputs;
  std::shared_ptr<dfa> parse_lexical_analyzer_machine ();
public:
  lexical_analyzer (std::string &, std::string &);
  lexical_analyzer (std::shared_ptr<dfa> &, std::string &);
  token get_next_token ();
  const std::shared_ptr<dfa> &get_dfa() const;
};

#endif // LEXICAL_ANALYZER_H
