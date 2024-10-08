#ifndef EMPLEX_LEXER_HPP_INCLUDE_
#define EMPLEX_LEXER_HPP_INCLUDE_

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace emplex {
  // Struct to store information about a found Token
  struct Token {
    int id;                             // Type ID for token
    std::string lexeme;                 // Sequence matched by token
    size_t line_id;                     // Line token started on
    operator int() const { return id; } // Auto-convert tokens to IDs
  };
  
  // Deterministic Finite Automaton (DFA) for token recognition.
  class DFA {
  private:
    static constexpr int NUM_SYMBOLS=128;
    static constexpr int NUM_STATES=50;
    using row_t = std::array<int, NUM_SYMBOLS>;
  
    // DFA transition table
    static constexpr std::array<row_t, NUM_STATES> table = {{
      /* State 0 */ {-1,-1,1,-1,-1,-1,-1,-1,-1,2,2,2,2,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,-1,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,7,5,5,8,5,5,9,5,5,5,10,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 1 */ {-1,-1,1,-1,-1,-1,-1,-1,-1,2,2,2,2,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,-1,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,7,5,5,8,5,5,9,5,5,5,10,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 2 */ {-1,-1,-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 3 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
      /* State 4 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,45,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 5 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 6 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,41,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 7 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,23,5,5,5,5,5,24,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 8 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,21,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 9 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,17,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 10 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,12,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 11 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 12 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,13,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 13 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,14,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 14 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,15,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 15 */ {-1,-1,-1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 16 */ {-1,-1,-1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 17 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,18,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 18 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,19,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 19 */ {-1,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 20 */ {-1,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 21 */ {-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 22 */ {-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 23 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,31,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 24 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,25,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 25 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,26,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 26 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,27,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 27 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,28,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 28 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,29,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 29 */ {-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 30 */ {-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 31 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,32,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 32 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,33,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 33 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,34,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 34 */ {-1,-1,-1,35,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,36,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 35 */ {-1,-1,-1,35,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 36 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,37,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 37 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,38,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 38 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,39,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 39 */ {-1,-1,-1,40,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 40 */ {-1,-1,-1,40,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 41 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,42,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 42 */ {-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,43,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 43 */ {-1,-1,-1,44,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1,-1,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,5,-1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,-1,-1,-1,-1,-1},
      /* State 44 */ {-1,-1,-1,44,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 45 */ {-1,-1,-1,46,-1,-1,-1,-1,-1,45,-1,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45},
      /* State 46 */ {-1,-1,-1,46,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 47 */ {-1,-1,-1,47,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
      /* State 48 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,49,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
      /* State 49 */ {-1,-1,-1,47,-1,-1,-1,-1,-1,3,-1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}
    }};
    // DFA stop states (0 indicates NOT a stop)
    static constexpr std::array<int, NUM_STATES> stop_id = {0,0,246,0,0,248,248,248,248,248,248,248,248,248,248,250,250,248,248,254,254,249,249,248,248,248,248,248,248,251,251,248,248,248,253,253,248,248,248,252,252,248,248,255,255,245,245,247,0,247};
  
  public:
    constexpr static int SYMBOL_START = 2;     ///< Symbol to indicate a start of line.
    constexpr static int SYMBOL_STOP = 3;      ///< Symbol to indicate an end of line.
    constexpr static int SYMBOL_MIN_INPUT = 9; ///< Symbols below this are control symbols.
  
    static constexpr size_t size() { return 50; }
    static constexpr int GetStop(int state) {
      return (state >= 0) ? stop_id[static_cast<size_t>(state)] : 0;
    }
    static constexpr int GetNext(int state, int sym) {
      int next_state = -1;
      if (state >= 0 && sym >= 0) {
        next_state = table[static_cast<size_t>(state)][static_cast<size_t>(sym)];
      }
      // If sym is a control symbol (line begin/end) and not used, keep old state.
      if (sym < SYMBOL_MIN_INPUT && next_state == -1) next_state = state;
      return next_state;
    }
    static int GetNext(int state, const std::string & syms) {
      for (char x : syms) state = GetNext(state, x);
      return state;
    }
    static int Test(const std::string & str) {
      int state = GetNext(0, SYMBOL_START);
      state = GetNext(state, str);
      int eol_state = GetNext(state, SYMBOL_STOP);
      return std::max(GetStop(state), GetStop(eol_state));
    }
  };
  
  class Lexer {
  private:
    static constexpr int NUM_TOKENS=11;
    static constexpr int ERROR_ID = -1;     ///< Code for unknown token ID.
  
    // -- Current State --
    size_t cur_line = 1;   // Track LINE we are reading in the input.
    int start_pos = 0;     // Track INDEX for the start of current lexeme.
    std::string lexeme{};  // Lexeme found for the current token
    std::string errors{};  // Description of any errors encountered
  
  public:
    static constexpr int ID__EOF_ = 0;
    static constexpr int ID_COMMENTS = 245;         // Regex: //.*
    static constexpr int ID_WHITESPACE = 246;       // Regex: \s
    static constexpr int ID_STRING = 247;           // Regex: \"([^"\n]|\\.)*\"
    static constexpr int ID_ID = 248;               // Regex: [a-zA-Z_]\w*
    static constexpr int ID_IN = 249;               // Regex: in
    static constexpr int ID_PRINT = 250;            // Regex: print
    static constexpr int ID_FOREACH = 251;          // Regex: foreach
    static constexpr int ID_FILTER_OUT = 252;       // Regex: filter_out
    static constexpr int ID_FILTER = 253;           // Regex: filter
    static constexpr int ID_LOAD = 254;             // Regex: load
    static constexpr int ID_TYPE = 255;             // Regex: List
  
    // Return the name of a token given its ID.
    static constexpr const char * TokenName(int id) {
      switch (id) {
      case -1: return "_ERROR_";
      case 0: return "_EOF_";
      case 245: return "COMMENTS";
      case 246: return "WHITESPACE";
      case 247: return "STRING";
      case 248: return "ID";
      case 249: return "IN";
      case 250: return "PRINT";
      case 251: return "FOREACH";
      case 252: return "FILTER_OUT";
      case 253: return "FILTER";
      case 254: return "LOAD";
      case 255: return "TYPE";
      default: return "_ASCII_";
      };
    }
  
    // Identify if a token (by ID) should be skipped during tokenizing.
    static constexpr bool IgnoreToken(int id) {
      switch (id) {
      case 0:
      case 245:
      case 246:
        return true;
      default: return false;
      };
    }
  
    // Return the number of token types the lexer recognizes.
    static constexpr int GetNumTokens() { return NUM_TOKENS; }
  
    // Generate and return the next token from the input stream.
    Token NextToken(std::string_view in) {
      // If we cannot read in, return an "EOF" token.
      if (start_pos >= std::ssize(in)) return { 0, "", cur_line };
  
      int cur_pos = start_pos;   // Position in the input that we are actively analyzing
      int best_pos = start_pos;  // Best look-ahead we've found so far
      int cur_state = 0;         // Next state for the DFA analysis
      int cur_stop = 0;          // Current "stop" state (or 0 if we can't stop here)
      int best_stop = -1;        // Best stop state found so far?
  
      // If we are at the START OF A LINE, send a DFA::SYMBOL_START
      if (start_pos == 0 || in[start_pos-1] == '\n') {
        cur_state = DFA::GetNext(0, DFA::SYMBOL_START);
      }
      // Keep looking as long as:
      // 1: We may be able to continue the current lexeme, and
      // 2: We have not entered an invalid state, and
      // 3: Our input string has more symbols to provide
      while (cur_stop >= 0 && cur_state >= 0 && cur_pos < std::ssize(in)) {
        const char next_char = in[cur_pos++];
        if (next_char < 0) break; // Ignore invalid chars.
        cur_state = DFA::GetNext(cur_state, next_char);
        cur_stop = DFA::GetStop(cur_state);
        if (cur_stop > 0) { best_pos = cur_pos; best_stop = cur_stop; }
        // Look ahead to see if we are at the END OF A LINE that can finish a token.
        if (cur_pos == std::ssize(in) || in[cur_pos] == '\n') {
          int eol_state = DFA::GetNext(cur_state, DFA::SYMBOL_STOP);
          int eol_stop = DFA::GetStop(eol_state);
          if (eol_stop > 0) { best_pos = cur_pos; best_stop = eol_stop; }
        }
      }
  
      // If we did not find any options, peel off just one character and use it as id.
      if (best_pos == start_pos) { best_stop=in[start_pos]; best_pos++;}
  
      lexeme = in.substr(start_pos, best_pos-start_pos);
      start_pos += std::ssize(lexeme);
  
      // Update the line number we are on.
      const size_t out_line = cur_line;
      cur_line += static_cast<size_t>(std::count(lexeme.begin(),lexeme.end(),'\n'));
  
      // Return the token we found.
      return { best_stop, lexeme, out_line };
    }
  
    // Convert an input string into a vector of tokens.
    std::vector<Token> Tokenize(std::string_view in) {
      start_pos = 0; // Start processing at beginning of string.
      cur_line = 1;  // Start processing at the first line of the input.
      std::vector<Token> out_tokens;
      while (Token token = NextToken(in)) {
        if (!IgnoreToken(token.id)) out_tokens.push_back(token);
      }
      return out_tokens;
    }
  
    // Convert an input stream to a string, then tokenize.
    std::vector<Token> Tokenize(std::istream & is) {
      return Tokenize(
        std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>())
      );
    }
  };
} // End of namespace emplex
#endif // #ifndef EMPLEX_LEXER_HPP_INCLUDE_
