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
  Error(token.line_id, message...);
}

class ASTNode {
public:
  enum Type {
    EMPTY=0,
    STATEMENT_BLOCK,
    ASSIGN,
    VARIABLE,
    LITERAL,
    LOAD,
    PRINT
  };
private:
  Type type{EMPTY};
  size_t value{0};
  words_t words{};
  std::vector<ASTNode> children{};

public:
  ASTNode(Type type=EMPTY) : type(type) { }
  ASTNode(Type type, size_t value) : type(type), value(value) { }
  ASTNode(Type type, words_t words) : type(type), words(words) { }
  ASTNode(Type type, ASTNode child) : type(type) { AddChild(child); }
  ASTNode(Type type, ASTNode child1, ASTNode child2)
    : type(type) { AddChild(child1); AddChild(child2); }
  ASTNode(const ASTNode &) = default;
  ASTNode(ASTNode &&) = default;
  ASTNode & operator=(const ASTNode &) = default;
  ASTNode & operator=(ASTNode &&) = default;
  ~ASTNode() { }

  Type GetType() const { return type; }
  size_t GetValue() const { return value; }
  const words_t & GetWords() const { return words; }
  const std::vector<ASTNode> & GetChildren() const { return children; }
  const ASTNode & GetChild(size_t id) const {
    assert(id < children.size());
    return children[id];
  }

  void SetValue(size_t in) { value = in; }
  void SetWords(words_t in) { words = in; }
  void AddChild(ASTNode child) {
    assert(child.GetType() != EMPTY);
    children.push_back(child);
  }
};

class SymbolTable {
private:
  struct SymbolInfo {
    std::string name;
    words_t words{};
    size_t declare_line;

    SymbolInfo(std::string name, size_t declare_line)
      : name(name), declare_line(declare_line) { }
  };
  std::vector<SymbolInfo> var_info;
  using scope_t = std::unordered_map<std::string, size_t>;
  std::vector<scope_t> scope_stack{1};

public:
  static constexpr size_t NO_ID = static_cast<size_t>(-1);

  size_t GetVarID(std::string name) const {
    for (auto it = scope_stack.rbegin();
         it != scope_stack.rend();
         ++it)
    {
      if (it->count(name)) return it->find(name)->second;
    }

    return NO_ID;
  }

  bool HasVar(std::string name) const {
    return (GetVarID(name) != NO_ID);
  }

  size_t AddVar(size_t line_num, std::string name) {
    auto & scope = scope_stack.back();
    if (scope.count(name)) {
      Error(line_num, "Redeclaring variable '", name, "'.");
    }
    size_t var_id = var_info.size();
    var_info.emplace_back(name, line_num);
    scope[name] = var_id;
    return var_id;
  }

  words_t & VarValue(size_t id) {
    assert(id < var_info.size());
    return var_info[id].words;
  }
};

class WordLang {
private:
  std::vector<emplex::Token> tokens{};
  size_t token_id{0};
  ASTNode root{ASTNode::STATEMENT_BLOCK};

  SymbolTable symbols{};

  // === HELPER FUNCTIONS ===

  std::string TokenName(int id) const {
    if (id > 0 && id < 128) {
      return std::string("'") + static_cast<char>(id) + "'";
    }
    return emplex::Lexer::TokenName(id);
  }

  emplex::Token CurToken() const { return tokens[token_id]; }

  emplex::Token UseToken() { return tokens[token_id++]; }

  emplex::Token UseToken(int required_id, std::string err_message="") {
    if (CurToken() != required_id) {
      if (err_message.size()) Error(CurToken(), err_message);
      else {
        Error(CurToken(),
          "Expected token type ", TokenName(required_id),
          ", but found ", TokenName(CurToken())
        );
      }
    }
    return UseToken();
  }

  bool UseTokenIf(int test_id) {
    if (CurToken() == test_id) {
      token_id++;
      return true;
    }
    return false;
  }

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
      if (cur_node.GetType()) root.AddChild(cur_node);
    }
  }

  ASTNode ParseStatement() {
    switch (CurToken()) {
    using namespace emplex;
    case Lexer::ID_PRINT: return ParsePrint();
    case Lexer::ID_TYPE: return ParseDeclare();
    case Lexer::ID_FOREACH: return ParseForeach();
    default:
      return ParseExpression();
    }
  }

  ASTNode ParsePrint() {
    ASTNode print_node(ASTNode::PRINT);

    UseToken(emplex::Lexer::ID_PRINT);
    UseToken('(');
    do {
      print_node.AddChild( ParseExpression() );
    } while (UseTokenIf(','));
    UseToken(')');
    UseToken(';');

    return print_node;
  }

  ASTNode ParseDeclare() {
    auto type_token = UseToken(emplex::Lexer::ID_TYPE);
    auto var_token = UseToken(emplex::Lexer::ID_ID);
    symbols.AddVar(var_token, var_token.lexeme);

    if (UseTokenIf(';')) return ASTNode{};

    UseToken('=', "Expected ';' or '='.");

    auto lhs_node = MakeVarNode(var_token);
    auto rhs_node = ParseExpression();
    UseToken(';');

    return ASTNode{ASTNode::ASSIGN, lhs_node, rhs_node}
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