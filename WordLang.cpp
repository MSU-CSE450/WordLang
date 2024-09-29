#include <assert.h>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map> 
#include <vector>

#include "lexer.hpp"

using words_t = std::set<std::string>;

template <typename... Ts>
void Error(size_t line_num, Ts... message) {
  std::cerr << "ERROR (line " << line_num << "): ";
  (std::cerr << ... << message);
  std::cerr << std::endl;
  exit(1);
}

template <typename... Ts>
void Error(emplex::Token token, Ts... message) {
  Error(token.line_id, message...)
}

struct ASTNode {
  enum Type {
    EMPTY=0,
    STATEMENT_BLOCK,
    ASSIGN,
    VARIABLE
  };
  Type type{EMPTY};
  size_t value{0};
  std::vector<ASTNode> children{};

  void SetValue(size_t in) { value = in; }
};

class SymbolTable {
private:
  struct SymbolInfo {
    words_t words{};
    size_t declare_line;

    SymbolInfo(size_t declare_line) : declare_line(declare_line) { }
  };
  std::vector<SymbolInfo> var_info;
  using scope_t = std::unordered_map<std::string, size_t>;
  std::vector<scope_t> scope_stack{1};

public:
  size_t AddVar(size_t line_num, std::string name) {
    auto & scope = scope_stack.back();
    if (scope.count(name)) {
      Error(line_num, "Redeclaring variable '", name, "'.");
    }
    size_t var_id = var_info.size();
    var_info.emplace_back(line_num);
    scope[name] = var_id;
    return var_id;
  }
};

class WordLang {
private:
  std::vector<emplex::Token> tokens{};
  size_t token_id{0};
  ASTNode root{ASTNode::STATEMENT_BLOCK};

  SymbolTable symbols{};

  // === HELPER FUNCTIONS ===

  ASTNode MakeVarNode(size_t var_id) {
    ASTNode out(ASTNode::VARIABLE);
    out.SetValue(var_id);
    return out;
  }

public:
  WordLang(std::string filename) {
    std::ifstream file(filename);
    emplex::Lexer lexer;
    tokens = lexer.Tokenize(file);

    Parse();
  }

  void Parse() {
    while (token_id < tokens.size()) {
      ASTNode cur_node = ParseStatement();
      if (cur_node.type != ASTNode::EMPTY) root.children.push_back(cur_node);
    }
  }

  ASTNode ParseStatement() {
    switch (tokens[token_id]) {
    using namespace emplex;
    case Lexer::ID_PRINT: return ParsePrint();
    case Lexer::ID_TYPE: return ParseDeclare();
    case Lexer::ID_FOREACH: return ParseForeach();
    default:
      return ParseExpression();
    }
  }

  ASTNode ParsePrint() {
    return ASTNode{};
  }

  ASTNode ParseDeclare() {
    auto type_token = tokens[token_id++];

    std::string var_name{};

    switch (type_token) {
    using namespace emplex;
    case Lexer::ID_TYPE:  // @CAO This should be called LIST.
      if (tokens[token_id] != Lexer::ID_ID) {
        Error(tokens[token_id], "Expected identifier in variable declaration.");
      }
      var_name = tokens[token_id];
      size_t var_id = symbols.AddVar(tokens[token_id].line_id, var_name);
      ++token_id;
      if (tokens[token_id] == ';') return ASTNode{};

      if (tokens[token_id] != '=') {
        Error(tokens[token_id], "Expected ';' or '='.");
      }
      ++token_id;

      auto rhs_node = ParseExpression();

      ASTNode out_node(ASTNode::ASSIGN);
      out_node.children.push_back(MakeVarNode(var_id));
      out_node.children.push_back(rhs_node);

      return out_node;
    }

    return ASTNode{};
  }

  ASTNode ParseForeach() {
    return ASTNode{};
  }

  ASTNode ParseExpression() {
    return ASTNode{};
  }

};


int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "Format: " << argv[0] << " {filename}" << std::endl;
    exit(1);
  }

  WordLang lang(argv[1]);
}