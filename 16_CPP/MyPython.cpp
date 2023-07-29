-----------------------------------------------------------------------
lexer.h
-----------------------------------------------------------------------
#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <map>

namespace parse {
namespace token_type {
    
struct Number {int value;};
struct Id {std::string value;};
struct Char {char value;};
struct String {std::string value;};

struct Class {};
struct Return {};
struct If {};
struct Else {};
struct Def {};
struct Newline {};
struct Print {};
struct Indent {};
struct Dedent {};
struct Eof {};
struct And {};
struct Or {};
struct Not {};
struct Eq {};
struct NotEq {};
struct LessOrEq {};
struct GreaterOrEq {};
struct None {};
struct True {};
struct False {};

}//end namespace token_type

using TokenBase = std::variant<token_type::Number, 
                               token_type::Id,
                               token_type::Char, 
                               token_type::String, 
                               token_type::Class, 
                               token_type::Return,		
                               token_type::If, 
                               token_type::Else, 
                               token_type::Def, 
                               token_type::Newline,
                               token_type::Print, 
                               token_type::Indent, 
                               token_type::Dedent, 
                               token_type::And,
                               token_type::Or, 
                               token_type::Not,
                               token_type::Eq, 
                               token_type::NotEq,		
                               token_type::LessOrEq, 
                               token_type::GreaterOrEq, 
                               token_type::None,	
                               token_type::True, 
                               token_type::False, 
                               token_type::Eof>;

struct Token : TokenBase {
    using TokenBase::TokenBase;

    template <typename T>
    [[nodiscard]] bool Is() const {return std::holds_alternative<T>(*this);}

    template <typename T>
    [[nodiscard]] const T& As() const {return std::get<T>(*this);}

    template <typename T>
    [[nodiscard]] const T* TryAs() const {return std::get_if<T>(this);}
};

bool operator==(const Token& lhs, const Token& rhs);
bool operator!=(const Token& lhs, const Token& rhs);

std::ostream& operator<<(std::ostream& os, const Token& rhs);

class LexerError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
};

class Lexer {
    
    public:

        explicit Lexer(std::istream& input);

        [[nodiscard]] const Token& CurrentToken() const;
        Token NextToken();

        template <typename T>
        const T& Expect() const {
        
            using namespace std::literals;
        
            if (CurrentToken().Is<T>()) {
                return CurrentToken().As<T>();
            
            } else {
                throw LexerError("expectation error");
            }        
        }

        template <typename T, typename U>
        void Expect(const U& value) const {
        
            using namespace std::literals;
        
            try {
            
                if (CurrentToken().As<T>().value != value) {
                    throw LexerError("expectation error");
                } else {
                    return;
                }
            
            } catch (std::bad_variant_access&) {
                throw LexerError("expectation error");
            }
        }

        template <typename T>
        const T& ExpectNext() {
            NextToken();
            return Expect<T>();
        }

        template <typename T, typename U>
        void ExpectNext(const U& value) {
            NextToken();
            Expect<T>(value);
        }

    private:

        std::map<std::string, Token> keywords_ {
            {"class", token_type::Class{}},
            {"return", token_type::Return{}},
            {"if", token_type::If{}},
            {"else", token_type::Else{}},
            {"def", token_type::Def{}},
            {"print", token_type::Print{}},
            {"or", token_type::Or{}},
            {"None", token_type::None{}},
            {"and", token_type::And{}},
            {"not", token_type::Not{}},
            {"True", token_type::True{}},
            {"False", token_type::False{}}
        };
    
        int spaces_ = 0;
    
        std::vector<Token> tokens_;
        std::vector<Token>::const_iterator current_token_;
    
        void parse_number(std::istream& input);
        void parse_char(std::istream& input);
        void parse_string(std::istream& input);
        void parse_operator(std::istream& input);
        void parse_id(std::istream& input);
        void parse_newline(std::istream& input);
        void parse_indent(std::istream& input);

        void pass_spaces(std::istream& input);
        void pass_comment(std::istream& input);
    
        std::vector<Token>::const_iterator parse_tokens(std::istream& input);
};

}//end namespace parse
-----------------------------------------------------------------------
lexer.cpp
-----------------------------------------------------------------------
#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace parse {

bool operator==(const Token& lhs, const Token& rhs) {
    using namespace token_type;

    if (lhs.index() != rhs.index()) {return false;}
    if (lhs.Is<Char>()) {return lhs.As<Char>().value == rhs.As<Char>().value;}
    if (lhs.Is<Number>()) {return lhs.As<Number>().value == rhs.As<Number>().value;}
    if (lhs.Is<String>()) {return lhs.As<String>().value == rhs.As<String>().value;}
    if (lhs.Is<Id>()) {return lhs.As<Id>().value == rhs.As<Id>().value;}
    return true;
}

bool operator!=(const Token& lhs, const Token& rhs) {return !(lhs == rhs);}

std::ostream& operator<<(std::ostream& os, const Token& rhs) {
    using namespace token_type;

#define VALUED_OUTPUT(type) \
    if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

    VALUED_OUTPUT(Number);
    VALUED_OUTPUT(Id);
    VALUED_OUTPUT(String);
    VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
    if (rhs.Is<type>()) return os << #type;

    UNVALUED_OUTPUT(Class);
    UNVALUED_OUTPUT(Return);
    UNVALUED_OUTPUT(If);
    UNVALUED_OUTPUT(Else);
    UNVALUED_OUTPUT(Def);
    UNVALUED_OUTPUT(Newline);
    UNVALUED_OUTPUT(Print);
    UNVALUED_OUTPUT(Indent);
    UNVALUED_OUTPUT(Dedent);
    UNVALUED_OUTPUT(And);
    UNVALUED_OUTPUT(Or);
    UNVALUED_OUTPUT(Not);
    UNVALUED_OUTPUT(Eq);
    UNVALUED_OUTPUT(NotEq);
    UNVALUED_OUTPUT(LessOrEq);
    UNVALUED_OUTPUT(GreaterOrEq);
    UNVALUED_OUTPUT(None);
    UNVALUED_OUTPUT(True);
    UNVALUED_OUTPUT(False);
    UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

    return os << "Unknown token :("sv;
}

Lexer::Lexer(std::istream& input) : current_token_(parse_tokens(input)) {}

const Token& Lexer::CurrentToken() const {return *current_token_;}

Token Lexer::NextToken() {
    
    if ((current_token_ + 1) == tokens_.end()) {
        return token_type::Eof{};
        
    } else {
        return *++current_token_;
    }   
}

void Lexer::parse_number(std::istream& input) {

    char curr;
    curr = input.peek();

    if (std::isdigit(curr)) {

        string parsed;

        while (input.get(curr)) {

            if (std::isdigit(curr)) {
                parsed.push_back(curr);

            } else {
                input.putback(curr);
                break;
            }
        }

        tokens_.emplace_back(token_type::Number{std::stoi(parsed)});
    }
}
    
void Lexer::parse_char(std::istream& input) {
    
    char curr;
    curr = input.peek();
    
    if (std::ispunct(curr)) {
        
        if (curr == '#') {
            pass_comment(input);
            return;
        }
        
        input.get(curr);        
        tokens_.emplace_back(token_type::Char{curr});
    }
}
    
void Lexer::parse_string(std::istream& input) {
     
    char quote;
    input.get(quote);
     
    if (quote == '\'' || quote == '\"') {
        
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        
        std::string str;
        
        while (it != end) {
            
            const char curr = *it;
            if (curr == quote) {
                ++it;
                break;
                
            } else if (curr == '\\') {
                ++it;
                
                if (it == end) {
                    throw std::logic_error("unable to parse. string");
                }
                
                const char escaped = *(it);             
                switch (escaped) {
                        
                    case 'n':
                        str.push_back('\n');
                        break;
                        
                    case 't':
                        str.push_back('\t');
                        break;
                        
                    case 'r':
                        str.push_back('\r');
                        break;
                        
                    case '"':
                        str.push_back('"');
                        break;
                        
                    case '\'':
                        str.push_back('\'');
                        break;
                    
                    case '\\':
                        str.push_back('\\');
                        break;
                    
                    default:
                        throw std::invalid_argument("invalid escape char");
                }
                
            } else if (curr == '\n' || curr == '\r') {
                throw std::logic_error("invalid eof"s);
                
            } else {
                str.push_back(curr);
            }
            
            ++it;
        }
        
        tokens_.emplace_back(token_type::String{str});
        
    } else {
        input.putback(quote);
    }
}
    
void Lexer::parse_operator(std::istream& input) {
    
    char curr;
    input.get(curr);
    
    char next = input.peek();
    
    if (curr == '=' && next == '=') {
        input.get();
        tokens_.emplace_back(token_type::Eq{});
        
    } else if (curr == '!' && next == '=') {
        input.get();
        tokens_.emplace_back(token_type::NotEq{});
        
    } else if (curr == '<' && next == '=') {
        input.get();
        tokens_.emplace_back(token_type::LessOrEq{});  
        
    } else if (curr == '>' && next == '=') {
        input.get();
        tokens_.emplace_back(token_type::GreaterOrEq{});

    } else {
        input.putback(curr);
    }
}
    
void Lexer::parse_id(std::istream& input) {
    
    char curr;
    curr = input.peek();
    
    if (std::isalpha(curr) || curr == '_') {
        
        std::string parsed;
        
        while (input.get(curr)) {
            
            if (std::isalnum(curr) || curr == '_') {
                parsed.push_back(curr);
                
            } else {
                input.putback(curr);
                break;
            }
        }
        
        if (keywords_.find(parsed) != keywords_.end()) {
            tokens_.push_back(keywords_.at(parsed));
            
        } else {
            tokens_.emplace_back(token_type::Id{parsed});
        }
    }
}
    
void Lexer::parse_newline(std::istream& input) {

    char curr;
    input.get(curr);

    if (curr == '\n') {

        if (!tokens_.empty() && tokens_.back() != token_type::Newline{}) {
            tokens_.emplace_back(token_type::Newline{});
        }

        parse_indent(input);

    } else {
        input.putback(curr);
    }
}
    
void Lexer::parse_indent(std::istream& input) {

    int spaces = 0;
    char curr;
    
    while (input.get(curr) && curr == ' ') {
        ++spaces;
    }
    
    if (input.rdstate() != std::ios_base::eofbit) {
        input.putback(curr);
    }
    
    char next = input.peek();
    
    if (next == '\n' || next == '#') {
        return;
    }
    
    int indents = (spaces - spaces_) / 2;
    spaces_ = spaces;
    
    Token indent;
    
    if (indents > 0) {
        indent = token_type::Indent{};
        
    } else if (indents < 0) {
        indent = token_type::Dedent{};
    }
    
    for (int i = 0; i < std::abs(indents); ++i) {
        tokens_.emplace_back(indent);
    }
}
    
void Lexer::pass_spaces(std::istream& input) {
    
    while (input.peek() == ' ') {
        input.get();
    }
}

void Lexer::pass_comment(std::istream& input) {
    
    char curr;
    input.get(curr);
    
    if (curr == '#') {
        
        string comment;
        getline(input, comment, '\n');
        
        if (input.rdstate() != std::ios_base::eofbit) {
            input.putback('\n');
        }
        
    } else {
        input.putback(curr);
    }
}
    
std::vector<Token>::const_iterator Lexer::parse_tokens(std::istream& input) {
    
    char curr;
    
    while (input.get(curr)) {
        
        input.putback(curr);

        parse_newline(input);
        parse_string(input);
        parse_id(input);
        parse_operator(input);
        parse_char(input);
        parse_number(input);

        pass_spaces(input);
    }
    
    if (!tokens_.empty() && tokens_.back() != token_type::Newline{} && tokens_.back() != token_type::Dedent{}) {
        tokens_.emplace_back(token_type::Newline{});
    }
    
    tokens_.emplace_back(token_type::Eof{});
    return tokens_.begin();
}
    
}//end namespace parse
-----------------------------------------------------------------------
lexer_test_open.cpp
-----------------------------------------------------------------------
#include "lexer.h"
#include "test_runner_p.h"

#include <sstream>
#include <string>

using namespace std;

namespace parse {
namespace {
    
void TestSimpleAssignment() {
    istringstream input("x = 42\n"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{42}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestKeywords() {
    istringstream input("class return if else def print or None and not True False"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Class{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Return{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::If{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Else{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Print{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Or{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::None{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::And{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Not{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::True{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::False{}));
}

void TestNumbers() {
    istringstream input("42 15 -53"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Number{42}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{15}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'-'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{53}));
}

void TestIds() {
    istringstream input("x    _42 big_number   Return Class  dEf"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"_42"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"big_number"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::Id{"Return"s}));  // keywords are case-sensitive
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Class"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"dEf"s}));
}

void TestStrings() {
    istringstream input(
        R"('word' "two words" 'long string with a double quote " inside' "another long string with single quote ' inside")"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::String{"word"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"two words"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::String{"long string with a double quote \" inside"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::String{"another long string with single quote ' inside"s}));
}

void TestOperations() {
    istringstream input("+-*/= > < != == <> <= >="s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'-'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'*'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'/'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'>'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'<'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::NotEq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'<'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'>'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::LessOrEq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::GreaterOrEq{}));
}

void TestIndentsAndNewlines() {
    istringstream input(R"(
no_indent
  indent_one
    indent_two
      indent_three
      indent_three
      indent_three
    indent_two
  indent_one
    indent_two
no_indent
)"s);

    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"no_indent"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_one"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_one"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"no_indent"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestEmptyLinesAreIgnored() {
    istringstream input(R"(
x = 1
  y = 2

  z = 3


)"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{1}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{2}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    // Пустая строка, состоящая только из пробельных символов не меняет текущий отступ,
    // поэтому следующая лексема — это Id, а не Dedent
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"z"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{3}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestMythonProgram() {
    istringstream input(R"(
x = 4
y = "hello"

class Point:
  def __init__(self, x, y):
    self.x = x
    self.y = y

  def __str__(self):
    return str(x) + ' ' + str(y)

p = Point(1, 2)
print str(p)
)"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{4}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"hello"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Class{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Point"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"__init__"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'.'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'.'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"__str__"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Return{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{" "s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"p"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Point"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{1}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{2}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Print{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"p"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestExpect() {
    istringstream is("bugaga"s);
    Lexer lex(is);

    ASSERT_DOESNT_THROW(lex.Expect<token_type::Id>());
    ASSERT_EQUAL(lex.Expect<token_type::Id>().value, "bugaga"s);
    ASSERT_DOESNT_THROW(lex.Expect<token_type::Id>("bugaga"s));
    ASSERT_THROWS(lex.Expect<token_type::Id>("widget"s), LexerError);
    ASSERT_THROWS(lex.Expect<token_type::Return>(), LexerError);
}

void TestExpectNext() {
    istringstream is("+ bugaga + def 52"s);
    Lexer lex(is);

    ASSERT_EQUAL(lex.CurrentToken(), Token(token_type::Char{'+'}));
    ASSERT_DOESNT_THROW(lex.ExpectNext<token_type::Id>());
    ASSERT_DOESNT_THROW(lex.ExpectNext<token_type::Char>('+'));
    ASSERT_THROWS(lex.ExpectNext<token_type::Newline>(), LexerError);
    ASSERT_THROWS(lex.ExpectNext<token_type::Number>(57), LexerError);
}

void TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine() {
    {
        istringstream is("a b"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"a"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"b"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is("+"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Char{'+'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
}
void TestCommentsAreIgnored() {
    {
        istringstream is(R"(# comment
)"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is(R"(# comment

)"s);
        Lexer lexer(is);
        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is(R"(# comment
x #another comment
abc#
'#'
"#123"
#)"s);

        Lexer lexer(is);
        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"abc"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"#"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"#123"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
}
    
}//end namespace

void RunOpenLexerTests(TestRunner& tr) {
    RUN_TEST(tr, parse::TestSimpleAssignment);
    RUN_TEST(tr, parse::TestKeywords);
    RUN_TEST(tr, parse::TestNumbers);
    RUN_TEST(tr, parse::TestIds);
    RUN_TEST(tr, parse::TestStrings);
    RUN_TEST(tr, parse::TestOperations);
    RUN_TEST(tr, parse::TestIndentsAndNewlines);
    RUN_TEST(tr, parse::TestEmptyLinesAreIgnored);
    RUN_TEST(tr, parse::TestExpect);
    RUN_TEST(tr, parse::TestExpectNext);
    RUN_TEST(tr, parse::TestMythonProgram);
    RUN_TEST(tr, parse::TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine);
    RUN_TEST(tr, parse::TestCommentsAreIgnored);
}

}//end namespace parse
-----------------------------------------------------------------------
main.cpp
-----------------------------------------------------------------------
#include "lexer.h"
#include "parse.h"
#include "runtime.h"
#include "statement.h"
#include "test_runner_p.h"

#include <iostream>

using namespace std;

namespace parse {
    
void RunOpenLexerTests(TestRunner& tr);
    
}//end namespace parse

namespace ast {
    
void RunUnitTests(TestRunner& tr);
    
}

namespace runtime {
    
void RunObjectHolderTests(TestRunner& tr);
void RunObjectsTests(TestRunner& tr);
    
}//end namespace runtime

void TestParseProgram(TestRunner& tr);

namespace {

void RunMythonProgram(istream& input, ostream& output) {
    parse::Lexer lexer(input);
    auto program = ParseProgram(lexer);

    runtime::SimpleContext context{output};
    runtime::Closure closure;
    program->Execute(closure, context);
}

void TestSimplePrints() {
    istringstream input(R"(
print 57
print 10, 24, -8
print 'hello'
print "world"
print True, False
print
print None
)");

    ostringstream output;
    RunMythonProgram(input, output);

    ASSERT_EQUAL(output.str(), "57\n10 24 -8\nhello\nworld\nTrue False\n\nNone\n");
}

void TestAssignments() {
    istringstream input(R"(
x = 57
print x
x = 'C++ black belt'
print x
y = False
x = y
print x
x = None
print x, y
)");

    ostringstream output;
    RunMythonProgram(input, output);

    ASSERT_EQUAL(output.str(), "57\nC++ black belt\nFalse\nNone False\n");
}

void TestArithmetics() {
    istringstream input("print 1+2+3+4+5, 1*2*3*4*5, 1-2-3-4-5, 36/4/3, 2*5+10/2");

    ostringstream output;
    RunMythonProgram(input, output);

    ASSERT_EQUAL(output.str(), "15 120 -13 3 15\n");
}

void TestVariablesArePointers() {
    istringstream input(R"(
class Counter:
  def __init__():
    self.value = 0

  def add():
    self.value = self.value + 1

class Dummy:
  def do_add(counter):
    counter.add()

x = Counter()
y = x

x.add()
y.add()

print x.value

d = Dummy()
d.do_add(x)

print y.value
)");

    ostringstream output;
    RunMythonProgram(input, output);

    ASSERT_EQUAL(output.str(), "2\n3\n");
}

void TestAll() {
    TestRunner tr;
    parse::RunOpenLexerTests(tr);
    runtime::RunObjectHolderTests(tr);
    runtime::RunObjectsTests(tr);
    ast::RunUnitTests(tr);
    TestParseProgram(tr);

    RUN_TEST(tr, TestSimplePrints);
    RUN_TEST(tr, TestAssignments);
    RUN_TEST(tr, TestArithmetics);
    RUN_TEST(tr, TestVariablesArePointers);
}

}//end namespace

int main() {
    
    try {
        TestAll();

        RunMythonProgram(cin, cout);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
		return 1;
    }
    return 0;
}
-----------------------------------------------------------------------
parse.cpp
-----------------------------------------------------------------------
#include "parse.h"

#include "lexer.h"
#include "statement.h"

using namespace std;

namespace TokenType = parse::token_type;

namespace {
    
bool operator==(const parse::Token& token, char c) {
    const auto* p = token.TryAs<TokenType::Char>();
    return p != nullptr && p->value == c;
}

bool operator!=(const parse::Token& token, char c) {return !(token == c);}

class Parser {
public:
    
    explicit Parser(parse::Lexer& lexer) : lexer_(lexer) {}

    unique_ptr<ast::Statement> ParseProgram() {
        auto result = make_unique<ast::Compound>();
        
        while (!lexer_.CurrentToken().Is<TokenType::Eof>()) {result->AddStatement(ParseStatement());}
        return result;
    }

private:

    unique_ptr<ast::Statement> ParseSuite() {
        
        lexer_.Expect<TokenType::Newline>();
        lexer_.ExpectNext<TokenType::Indent>();

        lexer_.NextToken();

        auto result = make_unique<ast::Compound>();
        while (!lexer_.CurrentToken().Is<TokenType::Dedent>()) {result->AddStatement(ParseStatement());}

        lexer_.Expect<TokenType::Dedent>();
        lexer_.NextToken();

        return result;
    }

    vector<runtime::Method> ParseMethods() {
        
        vector<runtime::Method> result;

        while (lexer_.CurrentToken().Is<TokenType::Def>()) {
            runtime::Method m;

            m.name = lexer_.ExpectNext<TokenType::Id>().value;
            lexer_.ExpectNext<TokenType::Char>('(');

            if (lexer_.NextToken().Is<TokenType::Id>()) {
                m.formal_params.push_back(lexer_.Expect<TokenType::Id>().value);
                
                while (lexer_.NextToken() == ',') {
                    m.formal_params.push_back(lexer_.ExpectNext<TokenType::Id>().value);
                }
            }

            lexer_.Expect<TokenType::Char>(')');
            lexer_.ExpectNext<TokenType::Char>(':');
            lexer_.NextToken();

            m.body = std::make_unique<ast::MethodBody>(ParseSuite()); 

            result.push_back(std::move(m));
        }
        return result;
    }

    unique_ptr<ast::Statement> ParseClassDefinition() {
        
        string class_name = lexer_.Expect<TokenType::Id>().value;
        lexer_.NextToken();

        const runtime::Class* base_class = nullptr;
        if (lexer_.CurrentToken() == '(') {
            
            auto name = lexer_.ExpectNext<TokenType::Id>().value;
            
            lexer_.ExpectNext<TokenType::Char>(')');
            lexer_.NextToken();

            auto it = declared_classes_.find(name);
            if (it == declared_classes_.end()) {throw ParseError("Base class "s + name + " not found for class "s + class_name);}
            
            base_class = static_cast<const runtime::Class*>(it->second.Get());
        }

        lexer_.Expect<TokenType::Char>(':');
        lexer_.ExpectNext<TokenType::Newline>();
        lexer_.ExpectNext<TokenType::Indent>();
        lexer_.ExpectNext<TokenType::Def>();
        vector<runtime::Method> methods = ParseMethods();

        lexer_.Expect<TokenType::Dedent>();
        lexer_.NextToken();

        auto [it, inserted] = declared_classes_.insert({
            class_name,
            runtime::ObjectHolder::Own(runtime::Class(class_name, std::move(methods), base_class)),
        });

        if (!inserted) {throw ParseError("Class "s + class_name + " already exists"s);}
        return make_unique<ast::ClassDefinition>(it->second);
    }

    vector<string> ParseDottedIds() {
        
        vector<string> result(1, lexer_.Expect<TokenType::Id>().value);

        while (lexer_.NextToken() == '.') {result.push_back(lexer_.ExpectNext<TokenType::Id>().value);}
        
        return result;
    }

    unique_ptr<ast::Statement> ParseAssignmentOrCall() {
        
        lexer_.Expect<TokenType::Id>();

        vector<string> id_list = ParseDottedIds();
        string last_name = id_list.back();
        id_list.pop_back();

        if (lexer_.CurrentToken() == '=') {
            lexer_.NextToken();

            if (id_list.empty()) {
                return make_unique<ast::Assignment>(std::move(last_name), ParseTest());
            }
            
            return make_unique<ast::FieldAssignment>(ast::VariableValue{std::move(id_list)}, std::move(last_name), ParseTest());
        }
        
        lexer_.Expect<TokenType::Char>('(');
        lexer_.NextToken();

        if (id_list.empty()) {throw ParseError("Mython doesn't support functions, only methods: "s + last_name);}

        vector<unique_ptr<ast::Statement>> args;
        if (lexer_.CurrentToken() != ')') {
            args = ParseTestList();
        }
        
        lexer_.Expect<TokenType::Char>(')');
        lexer_.NextToken();

        return make_unique<ast::MethodCall>(make_unique<ast::VariableValue>(std::move(id_list)), std::move(last_name), std::move(args));
    }

    unique_ptr<ast::Statement> ParseExpression() {
        
        unique_ptr<ast::Statement> result = ParseAdder();
        
        while (lexer_.CurrentToken() == '+' || lexer_.CurrentToken() == '-') {
            
            char op = lexer_.CurrentToken().As<TokenType::Char>().value;
            lexer_.NextToken();

            if (op == '+') {
                result = make_unique<ast::Add>(std::move(result), ParseAdder());
            } else {
                result = make_unique<ast::Sub>(std::move(result), ParseAdder());
            }
        }
        return result;
    }

    unique_ptr<ast::Statement> ParseAdder() {
        
        unique_ptr<ast::Statement> result = ParseMult();
        
        while (lexer_.CurrentToken() == '*' || lexer_.CurrentToken() == '/') {
            
            char op = lexer_.CurrentToken().As<TokenType::Char>().value;
            lexer_.NextToken();

            if (op == '*') {
                result = make_unique<ast::Mult>(std::move(result), ParseMult());
            } else {
                result = make_unique<ast::Div>(std::move(result), ParseMult());
            }
        }
        
        return result;
    }

    unique_ptr<ast::Statement> ParseMult() {
        
        if (lexer_.CurrentToken() == '(') {
            
            lexer_.NextToken();
            auto result = ParseTest();
            
            lexer_.Expect<TokenType::Char>(')');
            lexer_.NextToken();
            
            return result;
        }
        
        if (lexer_.CurrentToken() == '-') {
            lexer_.NextToken();
            
            return make_unique<ast::Mult>(ParseMult(), make_unique<ast::NumericConst>(-1));
        }
        
        if (const auto* num = lexer_.CurrentToken().TryAs<TokenType::Number>()) {
            int result = num->value;
            lexer_.NextToken();
            
            return make_unique<ast::NumericConst>(result);
        }
        
        if (const auto* str = lexer_.CurrentToken().TryAs<TokenType::String>()) {
            string result = str->value;
            lexer_.NextToken();
            
            return make_unique<ast::StringConst>(std::move(result));
        }
        
        if (lexer_.CurrentToken().Is<TokenType::True>()) {
            lexer_.NextToken();
            
            return make_unique<ast::BoolConst>(runtime::Bool(true));
        }
        
        if (lexer_.CurrentToken().Is<TokenType::False>()) {
            lexer_.NextToken();
            
            return make_unique<ast::BoolConst>(runtime::Bool(false));
        }
        
        if (lexer_.CurrentToken().Is<TokenType::None>()) {
            lexer_.NextToken();
            
            return make_unique<ast::None>();
        }

        return ParseDottedIdsInMultExpr();
    }

    std::unique_ptr<ast::Statement> ParseDottedIdsInMultExpr() {
        
        vector<string> names = ParseDottedIds();

        if (lexer_.CurrentToken() == '(') {

            vector<unique_ptr<ast::Statement>> args;
            if (lexer_.NextToken() != ')') {
                args = ParseTestList();
            }
            
            lexer_.Expect<TokenType::Char>(')');
            lexer_.NextToken();

            auto method_name = names.back();
            names.pop_back();

            if (!names.empty()) {return make_unique<ast::MethodCall>(make_unique<ast::VariableValue>(std::move(names)), std::move(method_name), std::move(args));}
            
            if (auto it = declared_classes_.find(method_name); it != declared_classes_.end()) {
                return make_unique<ast::NewInstance>(static_cast<const runtime::Class&>(*it->second), std::move(args));
            }
            
            if (method_name == "str"sv) {
                
                if (args.size() != 1) {
                    throw ParseError("Function str takes exactly one argument"s);
                }
                
                return make_unique<ast::Stringify>(std::move(args.front()));
            }
            
            throw ParseError("Unknown call to "s + method_name + "()"s);
        }
        
        return make_unique<ast::VariableValue>(std::move(names));
    }

    vector<unique_ptr<ast::Statement>> ParseTestList() {
        
        vector<unique_ptr<ast::Statement>> result;
        result.push_back(ParseTest());

        while (lexer_.CurrentToken() == ',') {
            lexer_.NextToken();
            result.push_back(ParseTest());
        }
        
        return result;
    }

    unique_ptr<ast::Statement> ParseCondition() {
        
        lexer_.Expect<TokenType::If>();
        lexer_.NextToken();

        auto condition = ParseTest();

        lexer_.Expect<TokenType::Char>(':');
        lexer_.NextToken();

        auto if_body = ParseSuite();

        unique_ptr<ast::Statement> else_body;
        if (lexer_.CurrentToken().Is<TokenType::Else>()) {
            lexer_.ExpectNext<TokenType::Char>(':');
            lexer_.NextToken();
            else_body = ParseSuite();
        }

        return make_unique<ast::IfElse>(std::move(condition), std::move(if_body), std::move(else_body));
    }

    unique_ptr<ast::Statement> ParseTest() {
        
        auto result = ParseAndTest();
        while (lexer_.CurrentToken().Is<TokenType::Or>()) {
            lexer_.NextToken();
            result = make_unique<ast::Or>(std::move(result), ParseAndTest());
        }
        
        return result;
    }

    unique_ptr<ast::Statement> ParseAndTest() {
        
        auto result = ParseNotTest();
        while (lexer_.CurrentToken().Is<TokenType::And>()) {
            lexer_.NextToken();
            result = make_unique<ast::And>(std::move(result), ParseNotTest());
        }
        
        return result;
    }

    unique_ptr<ast::Statement> ParseNotTest() {
        
        if (lexer_.CurrentToken().Is<TokenType::Not>()) {
            lexer_.NextToken();
            return make_unique<ast::Not>(ParseNotTest());
        }
        
        return ParseComparison();
    }

    unique_ptr<ast::Statement> ParseComparison() {
        
        auto result = ParseExpression();
        const auto tok = lexer_.CurrentToken();

        if (tok == '<') {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::Less, std::move(result), ParseExpression());
        }
        
        if (tok == '>') {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::Greater, std::move(result), ParseExpression());
        }
        
        if (tok.Is<TokenType::Eq>()) {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::Equal, std::move(result), ParseExpression());
        }
        
        if (tok.Is<TokenType::NotEq>()) {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::NotEqual, std::move(result), ParseExpression());
        }
        
        if (tok.Is<TokenType::LessOrEq>()) {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::LessOrEqual, std::move(result), ParseExpression());
        }
        
        if (tok.Is<TokenType::GreaterOrEq>()) {
            lexer_.NextToken();
            
            return make_unique<ast::Comparison>(runtime::GreaterOrEqual, std::move(result), ParseExpression());
        }
        
        return result;
    }

    unique_ptr<ast::Statement> ParseStatement() {
        
        const auto& tok = lexer_.CurrentToken();

        if (tok.Is<TokenType::Class>()) {
            lexer_.NextToken();
            
            return ParseClassDefinition();
        }
        
        if (tok.Is<TokenType::If>()) {
            return ParseCondition();
        }
        
        auto result = ParseSimpleStatement();
        
        lexer_.Expect<TokenType::Newline>();
        lexer_.NextToken();
        
        return result;
    }

    unique_ptr<ast::Statement> ParseSimpleStatement() {
        
        const auto& tok = lexer_.CurrentToken();

        if (tok.Is<TokenType::Return>()) {
            lexer_.NextToken();
            
            return make_unique<ast::Return>(ParseTest());
        }
        
        if (tok.Is<TokenType::Print>()) {
            lexer_.NextToken();
            vector<unique_ptr<ast::Statement>> args;
            
            if (!lexer_.CurrentToken().Is<TokenType::Newline>()) {
                args = ParseTestList();
            }
            
            return make_unique<ast::Print>(std::move(args));
        }
        
        return ParseAssignmentOrCall();
    }

    parse::Lexer& lexer_;
    runtime::Closure declared_classes_;
};

}//end namespace

unique_ptr<runtime::Executable> ParseProgram(parse::Lexer& lexer) {return Parser{lexer}.ParseProgram();}
-----------------------------------------------------------------------
parse.h
-----------------------------------------------------------------------
#pragma once

#include <memory>
#include <stdexcept>

namespace parse {
    
class Lexer;
    
}//end namespace parse

namespace runtime {
    
class Executable;
    
}//end namespace runtime

struct ParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

std::unique_ptr<runtime::Executable> ParseProgram(parse::Lexer& lexer);
-----------------------------------------------------------------------
parse_test.cpp
-----------------------------------------------------------------------
#include "lexer.h"
#include "parse.h"
#include "statement.h"
#include "test_runner_p.h"

using namespace std;

namespace parse {

unique_ptr<ast::Statement> ParseProgramFromString(const string& program) {
    istringstream is(program);
    parse::Lexer lexer(is);
    return ParseProgram(lexer);
}

void TestSimpleProgram() {
    const string program = R"(
x = 4
y = 5
z = "hello, "
n = "world"
print x + y, z + n
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "9 hello, world\n"s);
}

void TestProgramWithClasses() {
    const string program = R"(
program_name = "Classes test"

class Empty:
  def __init__():
    x = 0

class Point:
  def __init__(x, y):
    self.x = x
    self.y = y

  def SetX(value):
    self.x = value
  def SetY(value):
    self.y = value

  def __str__():
    return '(' + str(self.x) + '; ' + str(self.y) + ')'

origin = Empty()
origin = Point(0, 0)

far_far_away = Point(10000, 50000)

print program_name, origin, far_far_away, origin.SetX(1)
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "Classes test (0; 0) (10000; 50000) None\n"s);
}

void TestProgramWithIf() {
    const string program = R"(
x = 4
y = 5
if x > y:
  print "x > y"
else:
  print "x <= y"
if x > 0:
  if y < 0:
    print "y < 0"
  else:
    print "y >= 0"
else:
  print 'x <= 0'
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "x <= y\ny >= 0\n"s);
}

void TestReturnFromIf() {
    const string program = R"(
class Abs:
  def calc(n):
    if n > 0:
      return n
    else:
      return -n

x = Abs()
print x.calc(2)
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "2\n"s);
}

void TestRecursion() {
    const string program = R"(
class ArithmeticProgression:
  def calc(n):
    self.result = 0
    self.calc_impl(n)

  def calc_impl(n):
    value = n
    if value > 0:
      self.result = self.result + value
      self.calc_impl(value - 1)

x = ArithmeticProgression()
x.calc(10)
print x.result
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "55\n"s);
}

void TestRecursion2() {
    const string program = R"(
class GCD:
  def __init__():
    self.call_count = 0

  def calc(a, b):
    self.call_count = self.call_count + 1
    if a < b:
      return self.calc(b, a)
    if b == 0:
      return a
    return self.calc(a - b, b)

x = GCD()
print x.calc(510510, 18629977)
print x.calc(22, 17)
print x.call_count
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "17\n1\n115\n"s);
}

void TestComplexLogicalExpression() {
    const string program = R"(
a = 1
b = 2
c = 3
ok = a + b > c and a + c > b and b + c > a
print ok
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "False\n"s);
}

void TestClassicalPolymorphism() {
    const string program = R"(
class Shape:
  def __str__():
    return "Shape"

class Rect(Shape):
  def __init__(w, h):
    self.w = w
    self.h = h

  def __str__():
    return "Rect(" + str(self.w) + 'x' + str(self.h) + ')'

class Circle(Shape):
  def __init__(r):
    self.r = r

  def __str__():
    return 'Circle(' + str(self.r) + ')'

class Triangle(Shape):
  def __init__(a, b, c):
    self.ok = a + b > c and a + c > b and b + c > a
    if (self.ok):
      self.a = a
      self.b = b
      self.c = c

  def __str__():
    if self.ok:
      return 'Triangle(' + str(self.a) + ', ' + str(self.b) + ', ' + str(self.c) + ')'
    else:
      return 'Wrong triangle'

r = Rect(10, 20)
c = Circle(52)
t1 = Triangle(3, 4, 5)
t2 = Triangle(125, 1, 2)

print r, c, t1, t2
)"s;

    runtime::DummyContext context;

    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(),
                 "Rect(10x20) Circle(52) Triangle(3, 4, 5) Wrong triangle\n"s);
}

void TestSelfInConstructor() {
    const string program = (R"--(
class X:
  def __init__(p):
    p.x = self
class XHolder:
  def __init__():
    dummy = 0
xh = XHolder()
x = X(xh)
)--");

    runtime::DummyContext context;
    runtime::Closure closure;
    auto tree = ParseProgramFromString(program);
    tree->Execute(closure, context);

    const auto* xh = closure.at("xh"s).TryAs<runtime::ClassInstance>();
    ASSERT(xh != nullptr);
    ASSERT_EQUAL(xh->Fields().at("x"s).Get(), closure.at("x"s).Get());
}

}//end namespace parse

void TestParseProgram(TestRunner& tr) {
    RUN_TEST(tr, parse::TestSimpleProgram);
    RUN_TEST(tr, parse::TestProgramWithClasses);
    RUN_TEST(tr, parse::TestProgramWithIf);
    RUN_TEST(tr, parse::TestReturnFromIf);
    RUN_TEST(tr, parse::TestRecursion);
    RUN_TEST(tr, parse::TestRecursion2);
    RUN_TEST(tr, parse::TestComplexLogicalExpression);
    RUN_TEST(tr, parse::TestClassicalPolymorphism);
    RUN_TEST(tr, parse::TestSelfInConstructor);
}
-----------------------------------------------------------------------
runtime.h
-----------------------------------------------------------------------
#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

namespace runtime {

class Context {
public:
    virtual std::ostream& GetOutputStream() = 0;

protected:
    ~Context() = default;
};

class Object {
public:
    virtual ~Object() = default;
    virtual void Print(std::ostream& os, Context& context) = 0;
};

class ObjectHolder {
public:
    ObjectHolder() = default;

    template <typename T>
    [[nodiscard]] static ObjectHolder Own(T&& object) {
        return ObjectHolder(std::make_shared<T>(std::forward<T>(object)));
    }

    [[nodiscard]] static ObjectHolder Share(Object& object);
    [[nodiscard]] static ObjectHolder None();

    Object& operator*() const;
    Object* operator->() const;
    [[nodiscard]] Object* Get() const;

    template <typename T>
    [[nodiscard]] T* TryAs() const {return dynamic_cast<T*>(this->Get());}

    explicit operator bool() const;

private:
    
    explicit ObjectHolder(std::shared_ptr<Object> data);   
    void AssertIsValid() const;
    std::shared_ptr<Object> data_;
};

template <typename T>
class ValueObject : public Object {
public:
    
    ValueObject(T v) : value_(v) {}
    void Print(std::ostream& os, [[maybe_unused]] Context& context) override {os << value_;}
    [[nodiscard]] const T& GetValue() const {return value_;}

private:
    T value_;
};

using Closure = std::unordered_map<std::string, ObjectHolder>;
bool IsTrue(const ObjectHolder& object);
    
class Executable {
public:
    virtual ~Executable() = default;
    virtual ObjectHolder Execute(Closure& closure, Context& context) = 0;
};

using String = ValueObject<std::string>;
using Number = ValueObject<int>;

class Bool : public ValueObject<bool> {
public:
    
    using ValueObject<bool>::ValueObject;
    void Print(std::ostream& os, Context& context) override;
};

struct Method {
    std::string name;
    std::vector<std::string> formal_params;
    std::unique_ptr<Executable> body;
};

class Class : public Object {
public:

    explicit Class(std::string name, std::vector<Method> methods, const Class* parent);
    [[nodiscard]] const Method* GetMethod(const std::string& name) const;
    [[nodiscard]] const std::string& GetName() const;
    void Print(std::ostream& os, Context& context) override;
    
private:

    std::unordered_map<std::string_view, const Method*> methods_with_name_;
    std::string name_;
    std::vector<Method> methods_;
    
    const Class* parent_ = nullptr;
};

class ClassInstance : public Object {
public:
    explicit ClassInstance(const Class& cls);

    void Print(std::ostream& os, Context& context) override;

    ObjectHolder Call(const std::string& method, const std::vector<ObjectHolder>& actual_args, Context& context);
    [[nodiscard]] bool HasMethod(const std::string& method, size_t argument_count) const;
    [[nodiscard]] Closure& Fields();
    [[nodiscard]] const Closure& Fields() const;
    
private:
    const Class& class_;
    Closure fields_;
};

bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);
bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);
bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);
bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);
bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);
bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context);

struct DummyContext : Context {
    
    std::ostream& GetOutputStream() override {return output;}
    std::ostringstream output;
};

class SimpleContext : public runtime::Context {
public:
    
    explicit SimpleContext(std::ostream& output) : output_(output) {}
    std::ostream& GetOutputStream() override {return output_;}

private:
    std::ostream& output_;
};

}//end namespace runtime
-----------------------------------------------------------------------
runtime.cpp
-----------------------------------------------------------------------
#include "runtime.h"

#include <cassert>
#include <optional>
#include <sstream>

using namespace std;

namespace runtime {

ObjectHolder::ObjectHolder(std::shared_ptr<Object> data) : data_(std::move(data)) {}
void ObjectHolder::AssertIsValid() const {assert(data_ != nullptr);}

ObjectHolder ObjectHolder::Share(Object& object) {return ObjectHolder(std::shared_ptr<Object>(&object, [](auto*) {}));}
ObjectHolder ObjectHolder::None() {return ObjectHolder();}

Object& ObjectHolder::operator*() const {
    AssertIsValid();
    return *Get();
}

Object* ObjectHolder::operator->() const {
    AssertIsValid();
    return Get();
}

Object* ObjectHolder::Get() const {return data_.get();}
ObjectHolder::operator bool() const {return Get() != nullptr;}

bool IsTrue(const ObjectHolder& object) {
    
    if (!object) {return false;}
    
    auto object_boolean = object.TryAs<Bool>();
    if (object_boolean) {return object_boolean->GetValue();}
    
    auto object_string = object.TryAs<String>();
    if (object_string && !object_string->GetValue().empty()) {return true;}
    
    auto object_number = object.TryAs<Number>();
    if (object_number && object_number->GetValue() != 0) {return true;}
    
    auto object_val_boolean = object.TryAs<ValueObject<bool>>();
    if (object_val_boolean) {return object_val_boolean->GetValue();}
    
    return false;
}

void ClassInstance::Print(std::ostream& os, Context& context) {
    
    if (this->HasMethod("__str__", 0)) {
        this->Call("__str__", {}, context)->Print(os, context);
        
    } else {
        os << this;
    }
}

bool ClassInstance::HasMethod(const std::string& method, size_t argument_count) const {
    return (class_.GetMethod(method) && class_.GetMethod(method)->formal_params.size() == argument_count);
}

Closure& ClassInstance::Fields() {return fields_;}
const Closure& ClassInstance::Fields() const {return fields_;}
ClassInstance::ClassInstance(const Class& cls) : class_(cls) {}

ObjectHolder ClassInstance::Call(const std::string& method, const std::vector<ObjectHolder>& actual_args, Context& context) {
    
    if (!this->HasMethod(method, actual_args.size())) {
        throw std::runtime_error("not implemented"s);
    }
    
    Closure closure = {{"self", ObjectHolder::Share(*this)}};
    auto method_ptr = class_.GetMethod(method);
    
    for (size_t i = 0; i < method_ptr->formal_params.size(); ++i) {
        auto arg = method_ptr->formal_params[i];
        closure[arg] = actual_args[i];
    }
    
    return method_ptr->body->Execute(closure, context);
}

Class::Class(std::string name, std::vector<Method> methods, const Class* parent) : name_(std::move(name)) 
                                                                                 , methods_(std::move(methods)) 
                                                                                 , parent_(std::move(parent)) {
    if (parent_) {
        
        for (const auto& method : parent_->methods_) {
            methods_with_name_[method.name] = &method;
        }
    }
    
    for (const auto& method : methods_) {
        methods_with_name_[method.name] = &method;
    }
}

const Method* Class::GetMethod(const std::string& name) const {
    
    if (methods_with_name_.find(name) != methods_with_name_.end()) {
        return methods_with_name_.at(name);
        
    } else {
        return nullptr;
    }
}

const std::string& Class::GetName() const {return name_;}
void Class::Print(ostream& os, [[maybe_unused]] Context& context) {os <<"Class "sv << GetName();}
void Bool::Print(std::ostream& os, [[maybe_unused]] Context& context) {os << (GetValue() ? "True"sv : "False"sv);}

bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    
    if (!lhs && !rhs) {
        return true;
        
    } else {
        
        if (!lhs) {
            throw std::runtime_error("invalid args. cannot compare. == or !="s);
            
        } else {
            
            auto lhs_string = lhs.TryAs<String>();
            auto rhs_string = rhs.TryAs<String>();        
            if (lhs_string && rhs_string) {return lhs_string->GetValue() == rhs_string->GetValue();}

            auto lhs_number = lhs.TryAs<Number>();
            auto rhs_number = rhs.TryAs<Number>();
            if (lhs_number && rhs_number) {return lhs_number->GetValue() == rhs_number->GetValue();}

            auto lhs_boolean = lhs.TryAs<Bool>();
            auto rhs_boolean = rhs.TryAs<Bool>();
            if (lhs_boolean && rhs_boolean) {return lhs_boolean->GetValue() == rhs_boolean->GetValue();}

            auto lhs_class = lhs.TryAs<ClassInstance>();
            if (lhs_class && lhs_class->HasMethod("__eq__", 1)) {

                auto result = lhs_class->Call("__eq__", {rhs}, context);
                return result.TryAs<Bool>()->GetValue();
            }

            throw std::runtime_error("invalid args. cannot compare. == or !="s);
        }
    }
}

bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {
    
    if (!lhs) {
        throw std::runtime_error("invalid args. cannot compare. < or >"s);
        
    } else {
        
        auto lhs_string = lhs.TryAs<String>();
        auto rhs_string = rhs.TryAs<String>();
        if (lhs_string && rhs_string) {return lhs_string->GetValue() < rhs_string->GetValue();}
        
        auto lhs_number = lhs.TryAs<Number>();
        auto rhs_number = rhs.TryAs<Number>();
        if (lhs_number && rhs_number) {return lhs_number->GetValue() < rhs_number->GetValue();}
        
        auto lhs_boolean = lhs.TryAs<Bool>();
        auto rhs_boolean = rhs.TryAs<Bool>();
        if (lhs_boolean && rhs_boolean) {return lhs_boolean->GetValue() < rhs_boolean->GetValue();}
        
        auto lhs_class = lhs.TryAs<ClassInstance>();
        if (lhs_class && lhs_class->HasMethod("__lt__", 1)) {
            
            auto result = lhs_class->Call("__lt__", { rhs }, context);
            return result.TryAs<Bool>()->GetValue();
        }
        
        throw std::runtime_error("invalid args. cannot compare. < or >"s);
    } 
}

bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {return !Equal(lhs, rhs, context);}
bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {return !Less(lhs, rhs, context) && !Equal(lhs, rhs, context);}
bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {return Less(lhs, rhs, context) || Equal(lhs, rhs, context);}
bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context) {return !Less(lhs, rhs, context);}

}//end namespace runtime
-----------------------------------------------------------------------
runtime_test.cpp
-----------------------------------------------------------------------
#include "runtime.h"
#include "test_runner_p.h"

#include <functional>

using namespace std;

namespace runtime {

namespace {
class Logger : public Object {
public:
    static int instance_count;

    explicit Logger(int value_ = 0) : id_(value_) {++instance_count;}

    Logger(const Logger& rhs) : id_(rhs.id_) {++instance_count;}
    Logger(Logger&& rhs) noexcept : id_(rhs.id_) {++instance_count;}
    Logger& operator=(const Logger&) = default;
    Logger& operator=(Logger&&) = default;

    [[nodiscard]] int GetId() const {return id_;}
    
    ~Logger() {--instance_count;}
    void Print(ostream& os, [[maybe_unused]] Context& context) override {os << id_;}

private:
    int id_;
};

int Logger::instance_count = 0;

void TestNumber() {
    Number num(127);

    DummyContext context;

    num.Print(context.output, context);
    ASSERT_EQUAL(context.output.str(), "127"s);
    ASSERT_EQUAL(num.GetValue(), 127);
}

void TestString() {
    String word("hello!"s);

    DummyContext context;
    word.Print(context.output, context);
    ASSERT_EQUAL(context.output.str(), "hello!"s);
    ASSERT_EQUAL(word.GetValue(), "hello!"s);
}

void TestBool() {
    Bool t(true);
    ASSERT_EQUAL(t.GetValue(), true);

    DummyContext context;
    ostringstream out;
    t.Print(out, context);
    ASSERT_EQUAL(out.str(), "True"s);
    out.str({});

    Bool f(false);
    ASSERT_EQUAL(f.GetValue(), false);

    f.Print(out, context);
    ASSERT_EQUAL(out.str(), "False"s);

    ASSERT(context.output.str().empty());
}

struct TestMethodBody : Executable {
    using Fn = std::function<ObjectHolder(Closure& closure, Context& context)>;
    Fn body;

    explicit TestMethodBody(Fn body) : body(std::move(body)) {}

    ObjectHolder Execute(Closure& closure, Context& context) override {
        
        if (body) {
            return body(closure, context);
        } else {
            return {};
        }    
    }
};

void TestMethodInvocation() {
    DummyContext context;
    Closure base_closure;
    auto base_method_1 = [&base_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        base_closure = closure;
        return ObjectHolder::Own(Number{123});
    };
    auto base_method_2 = [&base_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        base_closure = closure;
        return ObjectHolder::Own(Number{456});
    };
    vector<Method> base_methods;
    base_methods.push_back(
        {"test"s, {"arg1"s, "arg2"s}, make_unique<TestMethodBody>(base_method_1)});
    base_methods.push_back({"test_2"s, {"arg1"s}, make_unique<TestMethodBody>(base_method_2)});
    Class base_class{"Base"s, std::move(base_methods), nullptr};
    ClassInstance base_inst{base_class};
    base_inst.Fields()["base_field"s] = ObjectHolder::Own(String{"hello"s});
    ASSERT(base_inst.HasMethod("test"s, 2U));
    auto res = base_inst.Call(
        "test"s, {ObjectHolder::Own(Number{1}), ObjectHolder::Own(String{"abc"s})}, context);
    ASSERT(Equal(res, ObjectHolder::Own(Number{123}), context));
    ASSERT_EQUAL(base_closure.size(), 3U);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.at("self"s).Get(), &base_inst);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.count("arg1"s), 1U);
    ASSERT(Equal(base_closure.at("arg1"s), ObjectHolder::Own(Number{1}), context));
    ASSERT_EQUAL(base_closure.count("arg2"s), 1U);
    ASSERT(Equal(base_closure.at("arg2"s), ObjectHolder::Own(String{"abc"s}), context));
    ASSERT_EQUAL(base_closure.count("base_field"s), 0U);

    Closure child_closure;
    auto child_method_1 = [&child_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        child_closure = closure;
        return ObjectHolder::Own(String("child"s));
    };
    vector<Method> child_methods;
    child_methods.push_back(
        {"test"s, {"arg1_child"s, "arg2_child"s}, make_unique<TestMethodBody>(child_method_1)});
    Class child_class{"Child"s, std::move(child_methods), &base_class};
    ClassInstance child_inst{child_class};
    ASSERT(child_inst.HasMethod("test"s, 2U));
    base_closure.clear();
    res = child_inst.Call(
        "test"s, {ObjectHolder::Own(String{"value1"s}), ObjectHolder::Own(String{"value2"s})},
        context);
    ASSERT(Equal(res, ObjectHolder::Own(String{"child"s}), context));
    ASSERT(base_closure.empty());
    ASSERT_EQUAL(child_closure.size(), 3U);
    ASSERT_EQUAL(child_closure.count("self"s), 1U);
    ASSERT_EQUAL(child_closure.at("self"s).Get(), &child_inst);
    ASSERT_EQUAL(child_closure.count("arg1_child"s), 1U);
    ASSERT(Equal(child_closure.at("arg1_child"s), (ObjectHolder::Own(String{"value1"s})), context));
    ASSERT_EQUAL(child_closure.count("arg2_child"s), 1U);
    ASSERT(Equal(child_closure.at("arg2_child"s), (ObjectHolder::Own(String{"value2"s})), context));

    ASSERT(child_inst.HasMethod("test_2"s, 1U));
    child_closure.clear();
    res = child_inst.Call("test_2"s, {ObjectHolder::Own(String{":)"s})}, context);
    ASSERT(Equal(res, ObjectHolder::Own(Number{456}), context));
    ASSERT_EQUAL(base_closure.size(), 2U);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.at("self"s).Get(), &child_inst);
    ASSERT_EQUAL(base_closure.count("arg1"s), 1U);
    ASSERT(Equal(base_closure.at("arg1"s), (ObjectHolder::Own(String{":)"s})), context));

    ASSERT(!child_inst.HasMethod("test"s, 1U));
    ASSERT_THROWS(child_inst.Call("test"s, {ObjectHolder::None()}, context), runtime_error);
}

void TestNonowning() {
    ASSERT_EQUAL(Logger::instance_count, 0);
    Logger logger(784);
    {
        auto oh = ObjectHolder::Share(logger);
        ASSERT(oh);
    }
    ASSERT_EQUAL(Logger::instance_count, 1);

    auto oh = ObjectHolder::Share(logger);
    ASSERT(oh);
    ASSERT(oh.Get() == &logger);

    DummyContext context;
    oh->Print(context.output, context);

    ASSERT_EQUAL(context.output.str(), "784"sv);
}

void TestOwning() {
    ASSERT_EQUAL(Logger::instance_count, 0);
    {
        auto oh = ObjectHolder::Own(Logger());
        ASSERT(oh);
        ASSERT_EQUAL(Logger::instance_count, 1);
    }
    ASSERT_EQUAL(Logger::instance_count, 0);

    auto oh = ObjectHolder::Own(Logger(312));
    ASSERT(oh);
    ASSERT_EQUAL(Logger::instance_count, 1);

    DummyContext context;
    oh->Print(context.output, context);

    ASSERT_EQUAL(context.output.str(), "312"sv);
}

void TestMove() {
    {
        ASSERT_EQUAL(Logger::instance_count, 0);
        Logger logger;

        auto one = ObjectHolder::Share(logger);
        ObjectHolder two = std::move(one);

        ASSERT_EQUAL(Logger::instance_count, 1);
        ASSERT(two.Get() == &logger);
    }
    {
        ASSERT_EQUAL(Logger::instance_count, 0);
        auto one = ObjectHolder::Own(Logger());
        ASSERT_EQUAL(Logger::instance_count, 1);
        Object* stored = one.Get();
        ObjectHolder two = std::move(one);
        ASSERT_EQUAL(Logger::instance_count, 1);

        ASSERT(two.Get() == stored);
        ASSERT(!one);
    }
}

void TestNullptr() {
    ObjectHolder oh;
    ASSERT(!oh);
    ASSERT(!oh.Get());
}

void TestIsTrue() {
    {
        ASSERT(!IsTrue(ObjectHolder::Own(Bool{false})));
        ASSERT(IsTrue(ObjectHolder::Own(Bool{true})));
    }

    ASSERT(!IsTrue(ObjectHolder::None()));

    {
        ASSERT(!IsTrue(ObjectHolder::Own(Number{0})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{1})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{-1})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{42})));
    }

    {
        ASSERT(!IsTrue(ObjectHolder::Own(String{""s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"0"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"1"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"abc"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"True"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"False"s})));
    }

    {
        Class cls{"Test"s, {}, nullptr};
        ASSERT(!IsTrue(ObjectHolder::Share(cls)));
        ASSERT(!IsTrue(ObjectHolder::Own(ClassInstance{cls})));
    }
}

void TestComparison() {
    auto test_equal = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool equality_result) {
        DummyContext ctx;
        ASSERT(Equal(lhs, rhs, ctx) == equality_result);
        ASSERT(NotEqual(lhs, rhs, ctx) == !equality_result);
    };

    auto test_less = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool less_result) {
        DummyContext ctx;
        ASSERT(Less(lhs, rhs, ctx) == less_result);
        ASSERT(GreaterOrEqual(lhs, rhs, ctx) == !less_result);
    };

    auto test_greater = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool greater_result) {
        DummyContext ctx;
        ASSERT(Greater(lhs, rhs, ctx) == greater_result);
        ASSERT(LessOrEqual(lhs, rhs, ctx) == !greater_result);
    };

    auto test_eq_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Equal(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(NotEqual(lhs, rhs, ctx), runtime_error);
    };

    auto test_lt_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Less(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(GreaterOrEqual(lhs, rhs, ctx), runtime_error);
    };

    auto test_gt_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Greater(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(LessOrEqual(lhs, rhs, ctx), runtime_error);
    };

    {
        test_equal(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), true);
        test_equal(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{4}), false);
        test_equal(ObjectHolder::Own(Number{4}), ObjectHolder::Own(Number{3}), false);

        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{4}), true);
        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), false);
        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{2}), false);

        test_greater(ObjectHolder::Own(Number{4}), ObjectHolder::Own(Number{2}), true);
        test_greater(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), false);
        test_greater(ObjectHolder::Own(Number{2}), ObjectHolder::Own(Number{3}), false);

        test_eq_uncomparable(ObjectHolder::Own(Number{4}), ObjectHolder::Own(String{"4"s}));
        test_eq_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(Bool{false}));

        test_lt_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(Bool{false}));
        test_gt_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(String{"test"s}));
    }

    {
        test_equal(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), true);
        test_equal(ObjectHolder::Own(String{"1"s}), ObjectHolder::Own(String{""s}), false);
        test_equal(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{"1"s}), false);
        test_equal(ObjectHolder::Own(String{"Hello"s}), ObjectHolder::Own(String{"Hello"s}), true);
        test_equal(ObjectHolder::Own(String{"Hello"s}), ObjectHolder::Own(String{"hello"s}), false);

        test_less(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{"1"s}), true);
        test_less(ObjectHolder::Own(String{"abb"s}), ObjectHolder::Own(String{"abc"s}), true);
        test_less(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), false);
        test_less(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abc"s}), false);
        test_less(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abb"s}), false);

        test_greater(ObjectHolder::Own(String{"1"s}), ObjectHolder::Own(String{""s}), true);
        test_greater(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abb"s}), true);
        test_greater(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), false);
        test_greater(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abc"s}), false);
        test_greater(ObjectHolder::Own(String{"abb"s}), ObjectHolder::Own(String{"abc"s}), false);

        test_eq_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_eq_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));

        test_lt_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_lt_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));

        test_gt_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_gt_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));
    }

    {
        test_equal(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), true);
        test_equal(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), true);
        test_equal(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), false);
        test_equal(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), false);

        test_less(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), true);
        test_less(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), false);
        test_less(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), false);
        test_less(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), false);

        test_greater(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), false);
        test_greater(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), false);
        test_greater(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), false);
        test_greater(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), true);

        test_eq_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_eq_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));

        test_lt_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_lt_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));

        test_gt_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_gt_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));
    }

    {
        test_equal(ObjectHolder::None(), ObjectHolder::None(), true);

        test_eq_uncomparable(ObjectHolder::None(), ObjectHolder::Own(Bool{false}));
        test_eq_uncomparable(ObjectHolder::None(), ObjectHolder::Own(String{"None"s}));

        test_lt_uncomparable(ObjectHolder::None(), ObjectHolder::None());
        test_gt_uncomparable(ObjectHolder::None(), ObjectHolder::None());
    }

    {
        Closure eq_closure;
        auto eq_result = ObjectHolder::Own(Bool{true});
        auto eq_body = [&eq_closure, &eq_result](Closure& closure, [[maybe_unused]] Context& ctx) {
            eq_closure = closure;
            return eq_result;
        };

        Closure lt_closure;
        auto lt_result = ObjectHolder::Own(Bool{true});
        auto lt_body = [&lt_closure, &lt_result](Closure& closure, [[maybe_unused]] Context& ctx) {
            lt_closure = closure;
            return lt_result;
        };

        std::vector<Method> cls1_methods;
        cls1_methods.push_back({"__eq__"s, {"rhs"s}, std::make_unique<TestMethodBody>(eq_body)});
        cls1_methods.push_back({"__lt__"s, {"rhs"s}, std::make_unique<TestMethodBody>(lt_body)});
        Class cls1{"Class1"s, std::move(cls1_methods), nullptr};
        ClassInstance lhs{cls1};

        Class cls2{"Class2"s, {}, nullptr};
        ClassInstance rhs{cls2};

        eq_result = ObjectHolder::Own(Bool{true});
        test_equal(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(eq_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(eq_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(lt_closure.empty());
        eq_result = ObjectHolder::Own(Bool{false});
        test_equal(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
        eq_closure.clear();
        lt_closure.clear();

        eq_result = ObjectHolder::Own(Bool{false});
        lt_result = ObjectHolder::Own(Bool{true});
        test_less(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(lt_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(lt_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(eq_closure.empty());
        eq_result = ObjectHolder::Own(Bool{true});
        lt_result = ObjectHolder::Own(Bool{false});
        test_less(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
        eq_closure.clear();
        lt_closure.clear();

        eq_result = ObjectHolder::Own(Bool{false});
        lt_result = ObjectHolder::Own(Bool{false});
        test_greater(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(eq_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(eq_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(lt_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(lt_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        eq_result = ObjectHolder::Own(Bool{true});
        lt_result = ObjectHolder::Own(Bool{true});
        test_greater(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
    }
}

void TestClass() {
    vector<Method> methods;
    Closure* passed_closure = nullptr;
    Context* passed_context = nullptr;
    auto body = [&passed_closure, &passed_context](Closure& closure, Context& ctx) {
        passed_closure = &closure;
        passed_context = &ctx;
        return ObjectHolder::Own(Number{42});
    };
    methods.push_back({"method"s, {"arg1"s, "arg2"s}, make_unique<TestMethodBody>(body)});
    Class cls{"Test"s, move(methods), nullptr};
    ASSERT_EQUAL(cls.GetName(), "Test"s);
    ASSERT_EQUAL(cls.GetMethod("missing_method"s), nullptr);

    const Method* method = cls.GetMethod("method"s);
    ASSERT(method != nullptr);
    DummyContext ctx;
    Closure closure;
    auto result = method->body->Execute(closure, ctx);
    ASSERT_EQUAL(passed_context, &ctx);
    ASSERT_EQUAL(passed_closure, &closure);
    const Number* returned_number = result.TryAs<Number>();
    ASSERT(returned_number != nullptr && returned_number->GetValue() == 42);

    ostringstream out;
    cls.Print(out, ctx);
    ASSERT(ctx.output.str().empty());
    ASSERT_EQUAL(out.str(), "Class Test"s);
}

void TestClassInstance() {
    vector<Method> methods;

    Closure passed_closure;
    auto str_body = [&passed_closure](Closure& closure, [[maybe_unused]] Context& ctx) {
        passed_closure = closure;
        return ObjectHolder::Own(String{"result"s});
    };

    methods.push_back({"__str__", {}, make_unique<TestMethodBody>(str_body)});

    Class cls{"Test"s, move(methods), nullptr};
    ClassInstance instance{cls};

    ASSERT_EQUAL(&instance.Fields(), &const_cast<const ClassInstance&>(instance).Fields());
    ASSERT(instance.HasMethod("__str__"s, 0));

    ostringstream out;
    DummyContext ctx;
    instance.Print(out, ctx);
    ASSERT_EQUAL(out.str(), "result"s);

    ASSERT_THROWS(instance.Call("missing_method"s, {}, ctx), runtime_error);
}

}//end namespace

void RunObjectsTests(TestRunner& tr) {
    RUN_TEST(tr, runtime::TestNumber);
    RUN_TEST(tr, runtime::TestString);
    RUN_TEST(tr, runtime::TestBool);
    RUN_TEST(tr, runtime::TestMethodInvocation);
    RUN_TEST(tr, runtime::TestIsTrue);
    RUN_TEST(tr, runtime::TestComparison);
    RUN_TEST(tr, runtime::TestClass);
    RUN_TEST(tr, runtime::TestClassInstance);
}

void RunObjectHolderTests(TestRunner& tr) {
    RUN_TEST(tr, runtime::TestNonowning);
    RUN_TEST(tr, runtime::TestOwning);
    RUN_TEST(tr, runtime::TestMove);
    RUN_TEST(tr, runtime::TestNullptr);
}
 
}//end namespace runtime
-----------------------------------------------------------------------
statement.cpp
-----------------------------------------------------------------------
#include "statement.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace ast {

using runtime::Closure;
using runtime::Context;
using runtime::ObjectHolder;

namespace {
    
const string ADD_METHOD = "__add__"s;
const string INIT_METHOD = "__init__"s;
    
}//end namespace
    
class ReturnExcept : public std::exception {
public:

    explicit ReturnExcept(const runtime::ObjectHolder& object) : object_(object) {}
    runtime::ObjectHolder GetValue() {return object_;}

private:
    runtime::ObjectHolder object_;
};

ObjectHolder Assignment::Execute(Closure& closure, Context& context) {
    closure[var_] = std::move(rv_->Execute(closure, context));
    return closure.at(var_);
}

Assignment::Assignment(std::string var, std::unique_ptr<Statement> rv) : var_(std::move(var))
                                                                       , rv_(std::move(rv)) {}

VariableValue::VariableValue(const std::string& var_name) {dotted_ids_.push_back(var_name);}
VariableValue::VariableValue(std::vector<std::string> dotted_ids) : dotted_ids_(std::move(dotted_ids)) {}

ObjectHolder VariableValue::Execute(Closure& closure, [[maybe_unused]] Context& context) {
    
    Closure* closure_ptr = &closure;
    ObjectHolder object;
    
    for (const auto& id : dotted_ids_) {
        
        if (closure_ptr->count(id)) {
            
            object = closure_ptr->at(id);
            
            if (object.TryAs<runtime::ClassInstance>()) {
                closure_ptr = &object.TryAs<runtime::ClassInstance>()->Fields();
            }

        } else {
            throw runtime_error("invalid argument. variable value execute");
        }  
    }
    
    return object;
}

unique_ptr<Print> Print::Variable(const std::string& name) {return std::make_unique<Print>(std::make_unique<VariableValue>(name));}

Print::Print(unique_ptr<Statement> argument) {args_.push_back(std::move(argument));}
Print::Print(vector<unique_ptr<Statement>> args) : args_(std::move(args)) {}

ObjectHolder Print::Execute(Closure& closure, Context& context) {
    
    ObjectHolder object;
    
    for (const auto& arg : args_) {
        
        if (arg != args_.front()) {
            context.GetOutputStream() << " "s;
        }
        
        object = arg->Execute(closure, context);
        if (object) {
            object->Print(context.GetOutputStream(), context);
            
        } else {
            context.GetOutputStream() << "None"s;
        }
    }
    
    context.GetOutputStream() << "\n"s;
    return object;
}

MethodCall::MethodCall(std::unique_ptr<Statement> object, std::string method, std::vector<std::unique_ptr<Statement>> args) : object_(std::move(object))
                                                                                                                            , method_(std::move(method))
                                                                                                                            , args_(std::move(args)) {}

ObjectHolder MethodCall::Execute(Closure& closure, Context& context) {
    
    auto object = object_->Execute(closure, context);
    auto class_ptr = object.TryAs<runtime::ClassInstance>();
    std::vector<runtime::ObjectHolder> args_now;

    for (const auto& arg : args_) {
        args_now.push_back(std::move(arg->Execute(closure, context)));
    }

    return class_ptr->Call(method_, args_now, context);
}

ObjectHolder Stringify::Execute(Closure& closure, Context& context) {
    
    auto object = argument_->Execute(closure, context);
    
    if (!object) {
        return ObjectHolder::Own(runtime::String{ "None"s });
        
    } else {
        runtime::DummyContext dummy_context;
        object->Print(dummy_context.GetOutputStream(), dummy_context);

        return ObjectHolder::Own(runtime::String{dummy_context.output.str()});
    }
}

ObjectHolder Add::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. add execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        auto lhs_number_ptr = lhs_object.TryAs<runtime::Number>();
        auto rhs_number_ptr = rhs_object.TryAs<runtime::Number>();

        if (lhs_number_ptr && rhs_number_ptr) {
            
            auto lhs_numuber = lhs_number_ptr->GetValue();
            auto rhs_numuber = rhs_number_ptr->GetValue();

            return ObjectHolder::Own(runtime::Number{lhs_numuber + rhs_numuber});
        }
        
        auto lhs_string_ptr = lhs_object.TryAs<runtime::String>();
        auto rhs_string_ptr = rhs_object.TryAs<runtime::String>();

        if (lhs_string_ptr && rhs_string_ptr) {
            
            auto lhs_string = lhs_string_ptr->GetValue();
            auto rhs_string = rhs_string_ptr->GetValue();

            return ObjectHolder::Own(runtime::String{lhs_string + rhs_string});
        }
        
        auto lhs_class_ptr = lhs_object.TryAs<runtime::ClassInstance>();

        if (lhs_class_ptr != nullptr) {

            if (lhs_class_ptr->HasMethod(ADD_METHOD, 1)) {
                return lhs_class_ptr->Call(ADD_METHOD, {rhs_object}, context);
            }
        }
        
        throw std::runtime_error("invalid arument. add execute"s);
    }
}

ObjectHolder Sub::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. sub execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        auto lhs_number_ptr = lhs_object.TryAs<runtime::Number>();
        auto rhs_number_ptr = rhs_object.TryAs<runtime::Number>();

        if (lhs_number_ptr && rhs_number_ptr) {
            
            auto lhs_numuber = lhs_number_ptr->GetValue();
            auto rhs_numuber = rhs_number_ptr->GetValue();

            return ObjectHolder::Own(runtime::Number{lhs_numuber - rhs_numuber});
            
        } else {
            throw std::runtime_error("invalid argument. sub execute"s);
        }        
    }
}

ObjectHolder Mult::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. mult execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        auto lhs_number_ptr = lhs_object.TryAs<runtime::Number>();
        auto rhs_number_ptr = rhs_object.TryAs<runtime::Number>();

        if (lhs_number_ptr && rhs_number_ptr) {
            
            auto lhs_numuber = lhs_number_ptr->GetValue();
            auto rhs_numuber = rhs_number_ptr->GetValue();

            return ObjectHolder::Own(runtime::Number{lhs_numuber * rhs_numuber});
            
        } else {
            throw std::runtime_error("invalid argument. mult execute"s);  
        }
    }
}

ObjectHolder Div::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. div execute"s);
        
    } else {
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        auto lhs_number_ptr = lhs_object.TryAs<runtime::Number>();
        auto rhs_number_ptr = rhs_object.TryAs<runtime::Number>();

        if (lhs_number_ptr && rhs_number_ptr) {
            
            auto lhs_numuber = lhs_number_ptr->GetValue();
            auto rhs_numuber = rhs_number_ptr->GetValue();

            if (rhs_numuber == 0) {
                throw std::runtime_error("div0"s);
                
            } else {
                return ObjectHolder::Own(runtime::Number{lhs_numuber / rhs_numuber});
            }   
            
        } else {
            throw std::runtime_error("invalid argument. div execute"s);
        }
    }
}

ObjectHolder Compound::Execute(Closure& closure, Context& context) {
    
    for (const auto& statement : statements_) {
        statement->Execute(closure, context);       
    } 
    
    return {};
}

ObjectHolder Return::Execute(Closure& closure, Context& context) {
    
    auto object = statement_->Execute(closure, context);
    throw ReturnExcept(object);
}

ClassDefinition::ClassDefinition(ObjectHolder cls) : class_(cls) {}

ObjectHolder ClassDefinition::Execute(Closure& closure, [[maybe_unused]] Context& context) {
    
    auto object = class_.TryAs<runtime::Class>();
    closure[object->GetName()] = std::move(class_);

    return {};
}

FieldAssignment::FieldAssignment(VariableValue object, std::string field_name, std::unique_ptr<Statement> rv)  : object_(std::move(object))
                                                                                                               , field_name_(std::move(field_name))
                                                                                                               , rv_(std::move(rv)) {}

ObjectHolder FieldAssignment::Execute(Closure& closure, Context& context) {

    Closure *closure_ptr = &closure;
    
    for (const auto& id: object_.GetDottedIds()) {
        
        auto& object = closure_ptr->at(id);
        closure_ptr = &object.TryAs<runtime::ClassInstance>()->Fields();
    }
    
    (*closure_ptr)[field_name_] = std::move(rv_->Execute(closure, context));
    
    return closure_ptr->at(field_name_);
}

IfElse::IfElse(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> if_body, std::unique_ptr<Statement> else_body) : condition_(std::move(condition))
                                                                                                                               , if_body_(std::move(if_body))
                                                                                                                               , else_body_(std::move(else_body)) {}

ObjectHolder IfElse::Execute(Closure& closure, Context& context) {
    
    auto condition = condition_->Execute(closure, context);
    if (runtime::IsTrue(condition)) {
        return if_body_->Execute(closure, context);
        
    } else if (else_body_) {
        return else_body_->Execute(closure, context);
        
    } else {
        return {};
    }
}

ObjectHolder Or::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. or execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        if (runtime::IsTrue(lhs_object)) {
            return ObjectHolder::Own(runtime::Bool{true});
            
        } else if (runtime::IsTrue(rhs_object)) {
            return ObjectHolder::Own(runtime::Bool{true});
            
        } else {
            return ObjectHolder::Own(runtime::Bool{false});
        }
    }
}

ObjectHolder And::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. and execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);

        if (runtime::IsTrue(lhs_object) && runtime::IsTrue(rhs_object)) {
            return ObjectHolder::Own(runtime::Bool{true});
            
        } else {
            return ObjectHolder::Own(runtime::Bool{false});
        }
    }
}

ObjectHolder Not::Execute(Closure& closure, Context& context) {
    
    if (!argument_) {
        throw std::runtime_error("invalid argument. not execute"s);
        
    } else {
        
        auto object = argument_->Execute(closure, context);
        auto result = runtime::IsTrue(object);
        
        return ObjectHolder::Own(runtime::Bool{!result});
    }
}

Comparison::Comparison(Comparator cmp, unique_ptr<Statement> lhs, unique_ptr<Statement> rhs) : BinaryOperation(std::move(lhs), std::move(rhs)) 
                                                                                             , comparator_(std::move(cmp)) {}

ObjectHolder Comparison::Execute(Closure& closure, Context& context) {
    
    if (!lhs_ || !rhs_) {
        throw std::runtime_error("invalid argument. comparison execute"s);
        
    } else {
        
        auto lhs_object = lhs_->Execute(closure, context);
        auto rhs_object = rhs_->Execute(closure, context);
        bool result = comparator_(lhs_object, rhs_object, context);

        return ObjectHolder::Own(runtime::Bool{result});
    }
}

NewInstance::NewInstance(const runtime::Class& class_, std::vector<std::unique_ptr<Statement>> args) : class_(class_)
                                                                                                     , args_(std::move(args)) {}
NewInstance::NewInstance(const runtime::Class& class_) : class_(class_) {}

ObjectHolder NewInstance::Execute(Closure& closure, Context& context) {
    
    std::vector<runtime::ObjectHolder> args_now;
    for (const auto& arg : args_) {
        args_now.push_back(std::move(arg->Execute(closure, context)));
    }
    
    if (class_.HasMethod(INIT_METHOD, args_.size())) {
        class_.Call(INIT_METHOD, std::move(args_now), context);
    }
    
    return runtime::ObjectHolder::Share(class_);
}

MethodBody::MethodBody(std::unique_ptr<Statement>&& body) : body_(std::move(body)) {}

ObjectHolder MethodBody::Execute(Closure& closure, Context& context) {
    
    try {
        body_->Execute(closure, context);
        
    } catch (ReturnExcept& object) {
        return object.GetValue();
    }
    
    return {};
}

}//end namespace ast
-----------------------------------------------------------------------
statement.h
-----------------------------------------------------------------------
#pragma once

#include "runtime.h"

#include <functional>

namespace ast {

using Statement = runtime::Executable;

template <typename T>
class ValueStatement : public Statement {
public:
    
    explicit ValueStatement(T v) : value_(std::move(v)) {}

    runtime::ObjectHolder Execute(runtime::Closure&, runtime::Context&) override {
        return runtime::ObjectHolder::Share(value_);
    }

private:
    T value_;
};

using NumericConst = ValueStatement<runtime::Number>;
using StringConst = ValueStatement<runtime::String>;
using BoolConst = ValueStatement<runtime::Bool>;

class VariableValue : public Statement {
public:
    
    explicit VariableValue(const std::string& var_name);
    explicit VariableValue(std::vector<std::string> dotted_ids);

    runtime::ObjectHolder Execute(runtime::Closure& closure, [[maybe_unused]] runtime::Context& context) override;
    
    const std::vector<std::string>& GetDottedIds() const {return dotted_ids_;}
    
private:
    std::vector<std::string> dotted_ids_;
};

class Assignment : public Statement {
public:
    
    Assignment(std::string var, std::unique_ptr<Statement> rv);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    std::string var_;
    std::unique_ptr<Statement> rv_;
};

class FieldAssignment : public Statement {
public:
    
    FieldAssignment(VariableValue object, std::string field_name, std::unique_ptr<Statement> rv);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    VariableValue object_;
    std::string field_name_;
    std::unique_ptr<Statement> rv_;
};

class None : public Statement {
public:
    runtime::ObjectHolder Execute([[maybe_unused]] runtime::Closure& closure, [[maybe_unused]] runtime::Context& context) override {return {};}
};

class Print : public Statement {
public:

    explicit Print(std::unique_ptr<Statement> argument);
    explicit Print(std::vector<std::unique_ptr<Statement>> args);
    static std::unique_ptr<Print> Variable(const std::string& name);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    std::vector<std::unique_ptr<Statement>> args_;
};

class MethodCall : public Statement {
public:
    
    MethodCall(std::unique_ptr<Statement> object, std::string method, std::vector<std::unique_ptr<Statement>> args);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    std::unique_ptr<Statement> object_;
    std::string method_;
    std::vector<std::unique_ptr<Statement>> args_;
};

class NewInstance : public Statement {
public:
    
    explicit NewInstance(const runtime::Class& class_);
    
    NewInstance(const runtime::Class& class_, std::vector<std::unique_ptr<Statement>> args);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    runtime::ClassInstance class_;
    std::vector<std::unique_ptr<Statement>> args_;
};

class UnaryOperation : public Statement {
public:
    
    explicit UnaryOperation(std::unique_ptr<Statement> argument) : argument_(std::move(argument)) {}
    
protected:
    std::unique_ptr<Statement> argument_;
};

class Stringify : public UnaryOperation {
public:
    
    using UnaryOperation::UnaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class BinaryOperation : public Statement {
public:
    
    BinaryOperation(std::unique_ptr<Statement> lhs, std::unique_ptr<Statement> rhs) : lhs_(std::move(lhs))
                                                                                    , rhs_(std::move(rhs)) {}
    
protected:
    std::unique_ptr<Statement> lhs_;
    std::unique_ptr<Statement> rhs_;
};

class Add : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Sub : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Mult : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Div : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Or : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class And : public BinaryOperation {
public:
    
    using BinaryOperation::BinaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Not : public UnaryOperation {
public:
    
    using UnaryOperation::UnaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

class Compound : public Statement {
public:

    template <typename... Args>
    explicit Compound(Args&&... args) {ReqursiveCompound(args...);}

    void AddStatement(std::unique_ptr<Statement> stmt) {statements_.push_back(std::move(stmt));}
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:

    std::vector<std::unique_ptr<Statement>> statements_;
    
    void ReqursiveCompound() {}
    
    template <typename T, typename... Ts>
    void ReqursiveCompound(T& arg, Ts&&... other) {
        AddStatement(std::forward<T>(arg));
        ReqursiveCompound(other...);
    }
};

class MethodBody : public Statement {
public:
    
    explicit MethodBody(std::unique_ptr<Statement>&& body);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    std::unique_ptr<Statement> body_;
};

class Return : public Statement {
public:
    
    explicit Return(std::unique_ptr<Statement> statement) : statement_(std::move(statement)) {}

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
    std::unique_ptr<Statement> statement_;
};

class ClassDefinition : public Statement {
public:

    explicit ClassDefinition(runtime::ObjectHolder cls);
    runtime::ObjectHolder Execute(runtime::Closure& closure, [[maybe_unused]] runtime::Context& context) override;
    
private:
    runtime::ObjectHolder class_;
};

class IfElse : public Statement {
public:

    IfElse(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> if_body, std::unique_ptr<Statement> else_body);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
	std::unique_ptr<Statement> condition_;
	std::unique_ptr<Statement> if_body_;
	std::unique_ptr<Statement> else_body_;
};

class Comparison : public BinaryOperation {
public:

    using Comparator = std::function<bool(const runtime::ObjectHolder&, const runtime::ObjectHolder&, runtime::Context&)>;
    Comparison(Comparator cmp, std::unique_ptr<Statement> lhs, std::unique_ptr<Statement> rhs);
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
    
private:
	Comparator comparator_;
};

}//end namespace ast
-----------------------------------------------------------------------
statement_test.cpp
-----------------------------------------------------------------------
#include "statement.h"
#include "test_runner_p.h"

using namespace std;

namespace ast {

using runtime::Closure;
using runtime::ObjectHolder;

namespace {

template <typename T>
void AssertObjectValueEqual(const ObjectHolder& obj, const T& expected, const string& msg) {
    ostringstream one;
    runtime::DummyContext context;
    obj->Print(one, context);

    ostringstream two;
    two << expected;

    AssertEqual(one.str(), two.str(), msg);
}

#define ASSERT_OBJECT_VALUE_EQUAL(obj, expected)                                          \
    {                                                                                     \
        std::ostringstream __assert_equal_private_os;                                     \
        __assert_equal_private_os << #obj << "'s value_ "sv                               \
                                  << " != "sv << #expected << "s, "sv << FILE_NAME << ':' \
                                  << __LINE__;                                            \
        AssertObjectValueEqual(obj, expected, __assert_equal_private_os.str());           \
    }

void TestNumericConst() {
    runtime::DummyContext context;

    NumericConst num(runtime::Number(57));
    Closure empty;

    ObjectHolder o = num.Execute(empty, context);
    ASSERT(o);
    ASSERT(empty.empty());

    ostringstream os;
    o->Print(os, context);
    ASSERT_EQUAL(os.str(), "57"s);

    ASSERT(context.output.str().empty());
}

void TestStringConst() {
    runtime::DummyContext context;

    StringConst value_(runtime::String("Hello!"s));
    Closure empty;

    ObjectHolder o = value_.Execute(empty, context);
    ASSERT(o);
    ASSERT(empty.empty());

    ostringstream os;
    o->Print(os, context);
    ASSERT_EQUAL(os.str(), "Hello!"s);

    ASSERT(context.output.str().empty());
}

void TestVariable() {
    runtime::DummyContext context;

    runtime::Number num(42);
    runtime::String word("Hello"s);

    Closure closure = {{"x"s, ObjectHolder::Share(num)}, {"w"s, ObjectHolder::Share(word)}};
    ASSERT(VariableValue("x"s).Execute(closure, context).Get() == &num);
    ASSERT(VariableValue("w"s).Execute(closure, context).Get() == &word);
    ASSERT_THROWS(VariableValue("unknown"s).Execute(closure, context), std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestAssignment() {
    runtime::DummyContext context;

    Assignment assign_x("x"s, make_unique<NumericConst>(runtime::Number(57)));
    Assignment assign_y("y"s, make_unique<StringConst>(runtime::String("Hello"s)));

    Closure closure = {{"y"s, ObjectHolder::Own(runtime::Number(42))}};

    {
        ObjectHolder o = assign_x.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, 57);
    }
    ASSERT(closure.find("x"s) != closure.end());
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("x"s), 57);

    {
        ObjectHolder o = assign_y.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, "Hello"s);
    }
    ASSERT(closure.find("y"s) != closure.end());
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("y"s), "Hello"s);

    ASSERT(context.output.str().empty());
}

void TestFieldAssignment() {
    runtime::DummyContext context;

    runtime::Class empty("Empty"s, {}, nullptr);
    runtime::ClassInstance object{empty};

    FieldAssignment assign_x(VariableValue{"self"s}, "x"s,
                             make_unique<NumericConst>(runtime::Number(57)));
    FieldAssignment assign_y(VariableValue{"self"s}, "y"s, make_unique<NewInstance>(empty));

    Closure closure = {{"self"s, ObjectHolder::Share(object)}};

    {
        ObjectHolder o = assign_x.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, 57);
    }
    ASSERT(object.Fields().find("x"s) != object.Fields().end());
    ASSERT_OBJECT_VALUE_EQUAL(object.Fields().at("x"s), 57);

    assign_y.Execute(closure, context);
    FieldAssignment assign_yz(
        VariableValue{vector<string>{"self"s, "y"s}}, "z"s,
        make_unique<StringConst>(runtime::String("Hello, world! Hooray! Yes-yes!!!"s)));
    {
        ObjectHolder o = assign_yz.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, "Hello, world! Hooray! Yes-yes!!!"s);
    }

    ASSERT(object.Fields().find("y"s) != object.Fields().end());
    const auto* subobject = object.Fields().at("y"s).TryAs<runtime::ClassInstance>();
    ASSERT(subobject != nullptr && subobject->Fields().find("z"s) != subobject->Fields().end());
    ASSERT_OBJECT_VALUE_EQUAL(subobject->Fields().at("z"s), "Hello, world! Hooray! Yes-yes!!!"s);

    ASSERT(context.output.str().empty());
}

void TestPrintVariable() {
    runtime::DummyContext context;

    Closure closure = {{"y"s, ObjectHolder::Own(runtime::Number(42))}};

    auto print_statement = Print::Variable("y"s);
    print_statement->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "42\n"s);
}

void TestPrintMultipleStatements() {
    runtime::DummyContext context;

    runtime::String hello("hello"s);
    Closure closure = {{"word"s, ObjectHolder::Share(hello)}, {"empty"s, ObjectHolder::None()}};

    vector<unique_ptr<Statement>> args;
    args.push_back(make_unique<VariableValue>("word"s));
    args.push_back(make_unique<NumericConst>(57));
    args.push_back(make_unique<StringConst>("Python"s));
    args.push_back(make_unique<VariableValue>("empty"s));

    Print(std::move(args)).Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "hello 57 Python None\n"s);
}

void TestStringify() {
    runtime::DummyContext context;

    Closure empty;

    {
        auto result = Stringify(make_unique<NumericConst>(57)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "57"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        auto result = Stringify(make_unique<StringConst>("Wazzup!"s)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "Wazzup!"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        vector<runtime::Method> methods;
        methods.push_back({"__str__"s, {}, make_unique<NumericConst>(842)});

        runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);

        auto result = Stringify(make_unique<NewInstance>(cls)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "842"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        runtime::Class cls("BoxedValue"s, {}, nullptr);
        runtime::Closure closure{{"x"s, ObjectHolder::Own(runtime::ClassInstance{cls})}};

        std::ostringstream expected_output;
        expected_output << closure.at("x"s).Get();

        Stringify str(make_unique<VariableValue>("x"s));
        ASSERT_OBJECT_VALUE_EQUAL(str.Execute(closure, context), expected_output.str());
    }
    {
        Stringify str(make_unique<None>());
        ASSERT_OBJECT_VALUE_EQUAL(str.Execute(empty, context), "None"s);
    }

    ASSERT(context.output.str().empty());
}

void TestNumbersAddition() {
    runtime::DummyContext context;

    Add sum(make_unique<NumericConst>(23), make_unique<NumericConst>(34));

    Closure empty;
    ASSERT_OBJECT_VALUE_EQUAL(sum.Execute(empty, context), 57);

    ASSERT(context.output.str().empty());
}

void TestStringsAddition() {
    runtime::DummyContext context;

    Add sum(make_unique<StringConst>("23"s), make_unique<StringConst>("34"s));

    Closure empty;
    ASSERT_OBJECT_VALUE_EQUAL(sum.Execute(empty, context), "2334"s);

    ASSERT(context.output.str().empty());
}

void TestBadAddition() {
    runtime::DummyContext context;

    Closure empty;

    ASSERT_THROWS(
        Add(make_unique<NumericConst>(42), make_unique<StringConst>("4"s)).Execute(empty, context),
        std::runtime_error);
    ASSERT_THROWS(
        Add(make_unique<StringConst>("4"s), make_unique<NumericConst>(42)).Execute(empty, context),
        std::runtime_error);
    ASSERT_THROWS(Add(make_unique<None>(), make_unique<StringConst>("4"s)).Execute(empty, context),
                  std::runtime_error);
    ASSERT_THROWS(Add(make_unique<None>(), make_unique<None>()).Execute(empty, context),
                  std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestSuccessfulClassInstanceAdd() {
    runtime::DummyContext context;

    vector<runtime::Method> methods;
    methods.push_back({"__add__"s,
                       {"value_"s},
                       make_unique<Add>(make_unique<StringConst>("hello, "s),
                                        make_unique<VariableValue>("value_"s))});

    runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);

    Closure empty;
    auto result = Add(make_unique<NewInstance>(cls), make_unique<StringConst>("world"s))
                      .Execute(empty, context);
    ASSERT_OBJECT_VALUE_EQUAL(result, "hello, world"s);

    ASSERT(context.output.str().empty());
}

void TestClassInstanceAddWithoutMethod() {
    runtime::DummyContext context;

    runtime::Class cls("BoxedValue"s, {}, nullptr);

    Closure empty;
    Add addition(make_unique<NewInstance>(cls), make_unique<StringConst>("world"s));
    ASSERT_THROWS(addition.Execute(empty, context), std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestCompound() {
    runtime::DummyContext context;

    Compound cpd{
        make_unique<Assignment>("x"s, make_unique<StringConst>("one"s)),
        make_unique<Assignment>("y"s, make_unique<NumericConst>(2)),
        make_unique<Assignment>("z"s, make_unique<VariableValue>("x"s)),
    };

    Closure closure;
    auto result = cpd.Execute(closure, context);

    ASSERT_OBJECT_VALUE_EQUAL(closure.at("x"s), "one"s);
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("y"s), 2);
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("z"s), "one"s);

    ASSERT(!result);

    ASSERT(context.output.str().empty());
}

void TestFields() {
    runtime::DummyContext context;

    vector<runtime::Method> methods;

    methods.push_back({"__init__"s,
                       {},
                       {make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                     make_unique<NumericConst>(0))}});
    methods.push_back(
        {"value"s, {}, {make_unique<VariableValue>(vector<string>{"self"s, "value"s})}});
    methods.push_back(
        {"add"s,
         {"x"s},
         {make_unique<FieldAssignment>(
             VariableValue{"self"s}, "value"s,
             make_unique<Add>(make_unique<VariableValue>(vector<string>{"self"s, "value"s}),
                              make_unique<VariableValue>("x"s)))}});

    runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);
    runtime::ClassInstance inst(cls);

    inst.Call("__init__"s, {}, context);

    for (int i = 1, expected = 0; i < 10; expected += i, ++i) {
        auto fv = inst.Call("value"s, {}, context);
        auto* obj = fv.TryAs<runtime::Number>();
        ASSERT(obj);
        ASSERT_EQUAL(obj->GetValue(), expected);

        inst.Call("add"s, {ObjectHolder::Own(runtime::Number(i))}, context);
    }

    ASSERT(context.output.str().empty());
}

void TestBaseClass() {
    vector<runtime::Method> methods;
    methods.push_back({"GetValue"s, {}, make_unique<VariableValue>(vector{"self"s, "value"s})});
    methods.push_back({"SetValue"s,
                       {"x"s},
                       make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                    make_unique<ast::VariableValue>("x"s))});

    runtime::Class cls("BoxedValue"s, move(methods), nullptr);

    ASSERT_EQUAL(cls.GetName(), "BoxedValue"s);
    {
        const auto* m = cls.GetMethod("GetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "GetValue"s);
        ASSERT(m->formal_params.empty());
    }
    {
        const auto* m = cls.GetMethod("SetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "SetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    ASSERT(!cls.GetMethod("AsString"s));
}

void TestInheritance() {
    vector<runtime::Method> methods;
    methods.push_back({"GetValue"s, {}, make_unique<VariableValue>(vector{"self"s, "value"s})});
    methods.push_back({"SetValue"s,
                       {"x"s},
                       make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                    make_unique<VariableValue>("x"s))});

    runtime::Class base("BoxedValue"s, std::move(methods), nullptr);

    methods.clear();
    methods.push_back({"GetValue"s, {"z"s}, make_unique<VariableValue>("z"s)});
    methods.push_back({"AsString"s, {}, make_unique<StringConst>("value"s)});
    runtime::Class cls("StringableValue"s, std::move(methods), &base);

    ASSERT_EQUAL(cls.GetName(), "StringableValue"s);
    {
        const auto* m = cls.GetMethod("GetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "GetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    {
        const auto* m = cls.GetMethod("SetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "SetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    {
        const auto* m = cls.GetMethod("AsString"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "AsString"s);
        ASSERT(m->formal_params.empty());
    }
    ASSERT(!cls.GetMethod("AsStringValue"s));
}

void TestOr() {
    auto test_or = [](bool lhs, bool rhs) {
        Or or_statement{make_unique<BoolConst>(lhs), make_unique<BoolConst>(rhs)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(or_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     lhs || rhs);
    };

    test_or(true, true);
    test_or(true, false);
    test_or(false, true);
    test_or(false, false);
}

void TestAnd() {
    auto test_and = [](bool lhs, bool rhs) {
        And and_statement{make_unique<BoolConst>(lhs), make_unique<BoolConst>(rhs)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(and_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     lhs && rhs);
    };

    test_and(true, true);
    test_and(true, false);
    test_and(false, true);
    test_and(false, false);
}

void TestNot() {
    auto test_not = [](bool arg) {
        Not not_statement{make_unique<BoolConst>(arg)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(not_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     !arg);
    };

    test_not(true);
    test_not(false);
}

}//end namespace

void RunUnitTests(TestRunner& tr) {
    RUN_TEST(tr, ast::TestNumericConst);
    RUN_TEST(tr, ast::TestStringConst);
    RUN_TEST(tr, ast::TestVariable);
    RUN_TEST(tr, ast::TestAssignment);
    RUN_TEST(tr, ast::TestFieldAssignment);
    RUN_TEST(tr, ast::TestPrintVariable);
    RUN_TEST(tr, ast::TestPrintMultipleStatements);
    RUN_TEST(tr, ast::TestStringify);
    RUN_TEST(tr, ast::TestNumbersAddition);
    RUN_TEST(tr, ast::TestStringsAddition);
    RUN_TEST(tr, ast::TestBadAddition);
    RUN_TEST(tr, ast::TestSuccessfulClassInstanceAdd);
    RUN_TEST(tr, ast::TestClassInstanceAddWithoutMethod);
    RUN_TEST(tr, ast::TestCompound);
    RUN_TEST(tr, ast::TestFields);
    RUN_TEST(tr, ast::TestBaseClass);
    RUN_TEST(tr, ast::TestInheritance);
    RUN_TEST(tr, ast::TestOr);
    RUN_TEST(tr, ast::TestAnd);
    RUN_TEST(tr, ast::TestNot);
}

}//end namespace ast
-----------------------------------------------------------------------
test_runner_p.h
-----------------------------------------------------------------------
#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace TestRunnerPrivate {
    
template <class Map>
std::ostream& PrintMap(std::ostream& os, const Map& m) {
    os << "{";
    bool first = true;
    for (const auto& kv : m) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

}//end namespace TestRunnerPrivate

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {return TestRunnerPrivate::PrintMap(os, m);}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m) {return TestRunnerPrivate::PrintMap(os, m);}

template <class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
    
    if (!(t == u)) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void Assert(bool b, const std::string& hint) {AssertEqual(b, true, hint);}

class TestRunner {
public:
    
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name) {
        try {
            func();
            std::cerr << test_name << " OK" << std::endl;
        } catch (std::exception& e) {
            ++fail_count;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        } catch (...) {
            ++fail_count;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    ~TestRunner() {
        std::cerr.flush();
        if (fail_count > 0) {
            std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

#ifndef FILE_NAME
#define FILE_NAME __FILE__
#endif

#define ASSERT_EQUAL(x, y)                                                                       \
    {                                                                                            \
        std::ostringstream __assert_equal_private_os;                                            \
        __assert_equal_private_os << #x << " != " << #y << ", " << FILE_NAME << ":" << __LINE__; \
        AssertEqual(x, y, __assert_equal_private_os.str());                                      \
    }

#define ASSERT(x)                                                                   \
    {                                                                               \
        std::ostringstream __assert_private_os;                                     \
        __assert_private_os << #x << " is false, " << FILE_NAME << ":" << __LINE__; \
        Assert(static_cast<bool>(x), __assert_private_os.str());                    \
    }

#define RUN_TEST(tr, func) tr.RunTest(func, #func)

#define ASSERT_THROWS(expr, expected_exception)                                                   \
    {                                                                                             \
        bool __assert_private_flag = true;                                                        \
        try {                                                                                     \
            expr;                                                                                 \
            __assert_private_flag = false;                                                        \
        } catch (expected_exception&) {                                                           \
        } catch (...) {                                                                           \
            std::ostringstream __assert_private_os;                                               \
            __assert_private_os << "Expression " #expr                                            \
                                   " threw an unexpected exception"                               \
                                   " " FILE_NAME ":"                                              \
                                << __LINE__;                                                      \
            Assert(false, __assert_private_os.str());                                             \
        }                                                                                         \
        if (!__assert_private_flag) {                                                             \
            std::ostringstream __assert_private_os;                                               \
            __assert_private_os << "Expression " #expr                                            \
                                   " is expected to throw " #expected_exception " " FILE_NAME ":" \
                                << __LINE__;                                                      \
            Assert(false, __assert_private_os.str());                                             \
        }                                                                                         \
    }

#define ASSERT_DOESNT_THROW(expr)                               \
    try {                                                       \
        expr;                                                   \
    } catch (...) {                                             \
        std::ostringstream __assert_private_os;                 \
        __assert_private_os << "Expression " #expr              \
                               " threw an unexpected exception" \
                               " " FILE_NAME ":"                \
                            << __LINE__;                        \
        Assert(false, __assert_private_os.str());               \
    }

#define EOF_GUARD
