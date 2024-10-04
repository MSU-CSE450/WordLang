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
  std::vector<ASTNode> & GetChildren() { return children; }
  const std::vector<ASTNode> & GetChildren() const { return children; }
  ASTNode & GetChild(size_t id) {
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

  size_t GetNumVars() const { return var_info.size(); }

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

  ASTNode MakeVarNode(const emplex::Token & token) {
    size_t var_id = symbols.GetVarID(token.lexeme);
    assert(var_id < symbols.GetNumVars());
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
    case ';': return ASTNode{};
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

    return ASTNode{ASTNode::ASSIGN, lhs_node, rhs_node};
  }

  ASTNode ParseForeach() {
    return ASTNode{};
  }

  ASTNode ParseExpression() {
    ASTNode term_node = ParseTerm();

    // @CAO - Need to handle operators.

    return term_node;
  }

  ASTNode ParseTerm() {
    auto token = UseToken();

    switch (token) {
    using namespace emplex;
    case Lexer::ID_ID:       // Variable
      return MakeVarNode(token);
    case Lexer::ID_LOAD: {   // Load Command
      UseToken('(');
      ASTNode arg_node = ParseExpression();
      UseToken(')');
      return ASTNode{ASTNode::LOAD, arg_node};
    }
    case Lexer::ID_STRING: { // String literal
      words_t words;
      words.insert(token.lexeme.substr(1,token.lexeme.size()-2));  // @CAO Deal with escape chars
      return ASTNode{ASTNode::LITERAL, words};
    }
    case '(': {
      ASTNode out_node = ParseExpression();
      UseToken(')');
      return out_node;
    }
    default:
      Error(token, "Expected expression. Found ", TokenName(token), ".");
    }

    return ASTNode{};
  }

  words_t Run(ASTNode & node) {
    switch (node.GetType()) {
    case ASTNode::EMPTY:
      assert(false); // We should not have any empty nodes.
    case ASTNode::STATEMENT_BLOCK:
      for (ASTNode & child : node.GetChildren()) {
        Run(child);
      }
      break;
    case ASTNode::ASSIGN: {
      assert(node.GetChildren().size() == 2);
      assert(node.GetChild(0).GetType() == ASTNode::VARIABLE);
      size_t var_id = node.GetChild(0).GetValue();
      return symbols.VarValue(var_id) = Run(node.GetChild(1));
    }
    case ASTNode::VARIABLE:
      assert(node.GetChildren().size() == 0);
      return symbols.VarValue(node.GetValue());
    case ASTNode::LITERAL:
      assert(node.GetChildren().size() == 0);
      return node.GetWords();
    case ASTNode::LOAD: {
      assert(node.GetChildren().size() == 1);
      auto filenames = Run(node.GetChild(0));
      words_t out_words;
      std::string word;
      for (auto name : filenames) {
        std::ifstream file(name);
        while (file >> word) {
          out_words.insert(word);
        }
      }
      return out_words;
    }
    case ASTNode::PRINT:
      for (ASTNode & child : node.GetChildren()) {
        words_t words = Run(child);
        std::cout << "[";
        for (const std::string & word : words) {
          std::cout << "," << word;
        }
        std::cout << " ]" << std::endl;
      }
      break;
    }

    return words_t{};
  }

  void Run() { Run(root); }

  void PrintDebug(const ASTNode & node, std::string prefix="") const {
    std::cout << prefix;

    switch (node.GetType()) {
    case ASTNode::EMPTY:
      std::cout << "EMPTY" << std::endl;
      break;
    case ASTNode::STATEMENT_BLOCK:
      std::cout << "STATEMENT_BLOCK" << std::endl;
      break;
    case ASTNode::ASSIGN:
      std::cout << "ASSIGN" << std::endl;
      break;
    case ASTNode::VARIABLE:
      std::cout << "VARIABLE" << std::endl;
      break;
    case ASTNode::LITERAL:
      std::cout << "LITERAL" << std::endl;
      break;
    case ASTNode::LOAD:
      std::cout << "LOAD" << std::endl;
      break;
    case ASTNode::PRINT:
      std::cout << "PRINT" << std::endl;
      break;
    }

    for (const auto & child : node.GetChildren()) {
      PrintDebug(child, prefix);
    }
  }

};


int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "Format: " << argv[0] << " {filename}" << std::endl;
    exit(1);
  }

  WordLang lang(argv[1]);
  lang.Run();
}