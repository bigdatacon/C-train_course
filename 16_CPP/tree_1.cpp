-----------------------------------------------------------------------
FindANTLR.cmake
-----------------------------------------------------------------------
find_package(Java QUIET COMPONENTS Runtime)

if(NOT ANTLR_EXECUTABLE)
  find_program(ANTLR_EXECUTABLE
               NAMES antlr.jar antlr4.jar antlr-4.jar antlr-4.12.0-complete.jar)
endif()

if(ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)
  execute_process(
      COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
      OUTPUT_VARIABLE ANTLR_COMMAND_OUTPUT
      ERROR_VARIABLE ANTLR_COMMAND_ERROR
      RESULT_VARIABLE ANTLR_COMMAND_RESULT
      OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(ANTLR_COMMAND_RESULT EQUAL 0)
    string(REGEX MATCH "Version [0-9]+(\\.[0-9]+)*" ANTLR_VERSION ${ANTLR_COMMAND_OUTPUT})
    string(REPLACE "Version " "" ANTLR_VERSION ${ANTLR_VERSION})
  else()
    message(
        SEND_ERROR
        "Command '${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}' "
        "failed with the output '${ANTLR_COMMAND_ERROR}'")
  endif()

  macro(ANTLR_TARGET Name InputFile)
    set(ANTLR_OPTIONS LEXER PARSER LISTENER VISITOR)
    set(ANTLR_ONE_VALUE_ARGS PACKAGE OUTPUT_DIRECTORY DEPENDS_ANTLR)
    set(ANTLR_MULTI_VALUE_ARGS COMPILE_FLAGS DEPENDS)
    cmake_parse_arguments(ANTLR_TARGET
                          "${ANTLR_OPTIONS}"
                          "${ANTLR_ONE_VALUE_ARGS}"
                          "${ANTLR_MULTI_VALUE_ARGS}"
                          ${ARGN})

    set(ANTLR_${Name}_INPUT ${InputFile})

    get_filename_component(ANTLR_INPUT ${InputFile} NAME_WE)

    if(ANTLR_TARGET_OUTPUT_DIRECTORY)
      set(ANTLR_${Name}_OUTPUT_DIR ${ANTLR_TARGET_OUTPUT_DIRECTORY})
    else()
      set(ANTLR_${Name}_OUTPUT_DIR
          ${CMAKE_CURRENT_BINARY_DIR}/antlr4cpp_generated_src/${ANTLR_INPUT})
    endif()

    unset(ANTLR_${Name}_CXX_OUTPUTS)

    if((ANTLR_TARGET_LEXER AND NOT ANTLR_TARGET_PARSER) OR
       (ANTLR_TARGET_PARSER AND NOT ANTLR_TARGET_LEXER))
      list(APPEND ANTLR_${Name}_CXX_OUTPUTS
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}.cpp)
      set(ANTLR_${Name}_OUTPUTS
          ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}.interp
          ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}.tokens)
    else()
      list(APPEND ANTLR_${Name}_CXX_OUTPUTS
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Lexer.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Lexer.cpp
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Parser.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Parser.cpp)
      list(APPEND ANTLR_${Name}_OUTPUTS
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Lexer.interp
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Lexer.tokens)
    endif()

    if(ANTLR_TARGET_LISTENER)
      list(APPEND ANTLR_${Name}_CXX_OUTPUTS
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}BaseListener.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}BaseListener.cpp
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Listener.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Listener.cpp)
      list(APPEND ANTLR_TARGET_COMPILE_FLAGS -listener)
    endif()

    if(ANTLR_TARGET_VISITOR)
      list(APPEND ANTLR_${Name}_CXX_OUTPUTS
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}BaseVisitor.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}BaseVisitor.cpp
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Visitor.h
           ${ANTLR_${Name}_OUTPUT_DIR}/${ANTLR_INPUT}Visitor.cpp)
      list(APPEND ANTLR_TARGET_COMPILE_FLAGS -visitor)
    endif()

    if(ANTLR_TARGET_PACKAGE)
      list(APPEND ANTLR_TARGET_COMPILE_FLAGS -package ${ANTLR_TARGET_PACKAGE})
    endif()

    list(APPEND ANTLR_${Name}_OUTPUTS ${ANTLR_${Name}_CXX_OUTPUTS})

    if(ANTLR_TARGET_DEPENDS_ANTLR)
      if(ANTLR_${ANTLR_TARGET_DEPENDS_ANTLR}_INPUT)
        list(APPEND ANTLR_TARGET_DEPENDS
             ${ANTLR_${ANTLR_TARGET_DEPENDS_ANTLR}_INPUT})
        list(APPEND ANTLR_TARGET_DEPENDS
             ${ANTLR_${ANTLR_TARGET_DEPENDS_ANTLR}_OUTPUTS})
      else()
        message(SEND_ERROR
                "ANTLR target '${ANTLR_TARGET_DEPENDS_ANTLR}' not found")
      endif()
    endif()

    add_custom_command(
        OUTPUT ${ANTLR_${Name}_OUTPUTS}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
                ${InputFile}
                -o ${ANTLR_${Name}_OUTPUT_DIR}
                -no-listener
                -Dlanguage=Cpp
                ${ANTLR_TARGET_COMPILE_FLAGS}
        DEPENDS ${InputFile}
                ${ANTLR_TARGET_DEPENDS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building ${Name} with ANTLR ${ANTLR_VERSION}")
  endmacro(ANTLR_TARGET)

endif(ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ANTLR
    REQUIRED_VARS ANTLR_EXECUTABLE Java_JAVA_EXECUTABLE
    VERSION_VAR ANTLR_VERSION)
-----------------------------------------------------------------------
CMakeLists.txt
-----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(spreadsheet)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(
        CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
    )
else()
    set(
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-implicit-fallthrough"
    )
endif()

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.12.0-complete.jar)
include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

add_definitions(
    -DANTLR4CPP_STATIC
    -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)
add_subdirectory(antlr4_runtime)

antlr_target(FormulaParser Formula.g4 LEXER PARSER LISTENER)

include_directories(
    ${ANTLR4_INCLUDE_DIRS}
    ${ANTLR_FormulaParser_OUTPUT_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src
)

file(GLOB sources
    *.cpp
    *.h
)

add_executable(
    spreadsheet
    ${ANTLR_FormulaParser_CXX_OUTPUTS}
    ${sources}
)

target_link_libraries(spreadsheet antlr4_static)
if(MSVC)
    target_compile_options(antlr4_static PRIVATE /W0)
endif()

install(
    TARGETS spreadsheet
    DESTINATION bin
    EXPORT spreadsheet
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT spreadsheet)
-----------------------------------------------------------------------
common.h
-----------------------------------------------------------------------
#pragma once

#include <stdexcept>

class FormulaError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class FormulaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
-----------------------------------------------------------------------
Formula.g4
-----------------------------------------------------------------------
grammar Formula;

main
    : expr EOF
    ;

expr
    : '(' expr ')'  # Parens
    | (ADD | SUB) expr  # UnaryOp
    | expr (MUL | DIV) expr  # BinaryOp
    | expr (ADD | SUB) expr  # BinaryOp
    | NUMBER  # Literal
    ;

fragment INT: [-+]? UINT ;
fragment UINT: [0-9]+ ;
fragment EXPONENT: [eE] INT;
NUMBER
    : UINT EXPONENT?
    | UINT? '.' UINT EXPONENT?
    ;

ADD: '+' ;
SUB: '-' ;
MUL: '*' ;
DIV: '/' ;
WS: [ \t\n\r]+ -> skip ;
-----------------------------------------------------------------------
FormulaAST.h
-----------------------------------------------------------------------
#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute() const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
-----------------------------------------------------------------------
FormulaAST.cpp
-----------------------------------------------------------------------
#include "FormulaAST.h"

#include "FormulaBaseListener.h"
#include "FormulaLexer.h"
#include "FormulaParser.h"

#include <cassert>
#include <cmath>
#include <memory>
#include <optional>
#include <sstream>

namespace ASTImpl {

enum ExprPrecedence {
    EP_ADD,
    EP_SUB,
    EP_MUL,
    EP_DIV,
    EP_UNARY,
    EP_ATOM,
    EP_END,
};

enum PrecedenceRule {
    PR_NONE = 0b00,               
    PR_LEFT = 0b01,               
    PR_RIGHT = 0b10,               
    PR_BOTH = PR_LEFT | PR_RIGHT,  
};

constexpr PrecedenceRule PRECEDENCE_RULES[EP_END][EP_END] = {
    /* EP_ADD */ {PR_NONE, PR_NONE, PR_NONE, PR_NONE, PR_NONE, PR_NONE},
    /* EP_SUB */ {PR_RIGHT, PR_RIGHT, PR_NONE, PR_NONE, PR_NONE, PR_NONE},
    /* EP_MUL */ {PR_BOTH, PR_BOTH, PR_NONE, PR_NONE, PR_NONE, PR_NONE},
    /* EP_DIV */ {PR_BOTH, PR_BOTH, PR_RIGHT, PR_RIGHT, PR_NONE, PR_NONE},
    /* EP_UNARY */ {PR_BOTH, PR_BOTH, PR_NONE, PR_NONE, PR_NONE, PR_NONE},
    /* EP_ATOM */ {PR_NONE, PR_NONE, PR_NONE, PR_NONE, PR_NONE, PR_NONE},
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void Print(std::ostream& out) const = 0;
    virtual void DoPrintFormula(std::ostream& out, ExprPrecedence precedence) const = 0;
    virtual double Evaluate() const = 0;

    virtual ExprPrecedence GetPrecedence() const = 0;

    void PrintFormula(std::ostream& out, ExprPrecedence parent_precedence,
                      bool right_child = false) const {
        auto precedence = GetPrecedence();
        auto mask = right_child ? PR_RIGHT : PR_LEFT;
        bool parens_needed = PRECEDENCE_RULES[parent_precedence][precedence] & mask;
        if (parens_needed) {
            out << '(';
        }

        DoPrintFormula(out, precedence);

        if (parens_needed) {
            out << ')';
        }
    }
};

namespace {
class BinaryOpExpr final : public Expr {
public:
    enum Type : char {
        Add = '+',
        Subtract = '-',
        Multiply = '*',
        Divide = '/',
    };

public:
    explicit BinaryOpExpr(Type type, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : type_(type)
        , lhs_(std::move(lhs))
        , rhs_(std::move(rhs)) {
    }

    void Print(std::ostream& out) const override {
        out << '(' << static_cast<char>(type_) << ' ';
        lhs_->Print(out);
        out << ' ';
        rhs_->Print(out);
        out << ')';
    }

    void DoPrintFormula(std::ostream& out, ExprPrecedence precedence) const override {
        lhs_->PrintFormula(out, precedence);
        out << static_cast<char>(type_);
        rhs_->PrintFormula(out, precedence, /* right_child = */ true);
    }

    ExprPrecedence GetPrecedence() const override {
        switch (type_) {
            case Add:
                return EP_ADD;
            case Subtract:
                return EP_SUB;
            case Multiply:
                return EP_MUL;
            case Divide:
                return EP_DIV;
            default:
                assert(false);
                return static_cast<ExprPrecedence>(INT_MAX);
        }
    }

    double Evaluate() const override {
        
        switch (type_) {
                
            case Add:
                return lhs_->Evaluate() + rhs_->Evaluate();
                
            case Subtract:
                return lhs_->Evaluate() - rhs_->Evaluate();
                
            case Multiply:
                return lhs_->Evaluate() * rhs_->Evaluate();
            
            case Divide:
                
                if (rhs_->Evaluate() != 0) {
                    return lhs_->Evaluate() / rhs_->Evaluate(); 
                    
                } else {
                    throw FormulaError("DIV/0");
                }   
            
            default:
                throw FormulaError("unidentified operation type");
        }
    }

private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;
};

class UnaryOpExpr final : public Expr {
public:
    enum Type : char {
        UnaryPlus = '+',
        UnaryMinus = '-',
    };

public:
    explicit UnaryOpExpr(Type type, std::unique_ptr<Expr> operand)
        : type_(type)
        , operand_(std::move(operand)) {
    }

    void Print(std::ostream& out) const override {
        out << '(' << static_cast<char>(type_) << ' ';
        operand_->Print(out);
        out << ')';
    }

    void DoPrintFormula(std::ostream& out, ExprPrecedence precedence) const override {
        out << static_cast<char>(type_);
        operand_->PrintFormula(out, precedence);
    }

    ExprPrecedence GetPrecedence() const override {
        return EP_UNARY;
    }

    double Evaluate() const override {
      
        switch (type_) {
                
            case UnaryPlus:
                return operand_->Evaluate();
                
            case UnaryMinus:
                return -operand_->Evaluate(); 
            
            default:
                throw FormulaError("unidentified operation type");
        }
    }

private:
    Type type_;
    std::unique_ptr<Expr> operand_;
};

class NumberExpr final : public Expr {
public:
    explicit NumberExpr(double value)
        : value_(value) {
    }

    void Print(std::ostream& out) const override {
        out << value_;
    }

    void DoPrintFormula(std::ostream& out, ExprPrecedence /* precedence */) const override {
        out << value_;
    }

    ExprPrecedence GetPrecedence() const override {
        return EP_ATOM;
    }

    double Evaluate() const override {
        return value_;
    }

private:
    double value_;
};

class ParseASTListener final : public FormulaBaseListener {
public:
    std::unique_ptr<Expr> MoveRoot() {
        assert(args_.size() == 1);
        auto root = std::move(args_.front());
        args_.clear();

        return root;
    }

public:
    void exitUnaryOp(FormulaParser::UnaryOpContext* ctx) override {
        assert(args_.size() >= 1);

        auto operand = std::move(args_.back());

        UnaryOpExpr::Type type;
        if (ctx->SUB()) {
            type = UnaryOpExpr::UnaryMinus;
        } else {
            assert(ctx->ADD() != nullptr);
            type = UnaryOpExpr::UnaryPlus;
        }

        auto node = std::make_unique<UnaryOpExpr>(type, std::move(operand));
        args_.back() = std::move(node);
    }

    void exitLiteral(FormulaParser::LiteralContext* ctx) override {
        double value = 0;
        auto valueStr = ctx->NUMBER()->getSymbol()->getText();
        std::istringstream in(valueStr);
        in >> value;
        if (!in) {
            throw ParsingError("Invalid number: " + valueStr);
        }

        auto node = std::make_unique<NumberExpr>(value);
        args_.push_back(std::move(node));
    }

    void exitBinaryOp(FormulaParser::BinaryOpContext* ctx) override {
        assert(args_.size() >= 2);

        auto rhs = std::move(args_.back());
        args_.pop_back();

        auto lhs = std::move(args_.back());

        BinaryOpExpr::Type type;
        if (ctx->ADD()) {
            type = BinaryOpExpr::Add;
        } else if (ctx->SUB()) {
            type = BinaryOpExpr::Subtract;
        } else if (ctx->MUL()) {
            type = BinaryOpExpr::Multiply;
        } else {
            assert(ctx->DIV() != nullptr);
            type = BinaryOpExpr::Divide;
        }

        auto node = std::make_unique<BinaryOpExpr>(type, std::move(lhs), std::move(rhs));
        args_.back() = std::move(node);
    }

    void visitErrorNode(antlr4::tree::ErrorNode* node) override {
        throw ParsingError("Error when parsing: " + node->getSymbol()->getText());
    }

private:
    std::vector<std::unique_ptr<Expr>> args_;
};

class BailErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer* /* recognizer */, antlr4::Token* /* offendingSymbol */,
                     size_t /* line */, size_t /* charPositionInLine */, const std::string& msg,
                     std::exception_ptr /* e */
                     ) override {
        throw ParsingError("Error when lexing: " + msg);
    }
};

}//end namespace
}//end namespace ASTImpl

FormulaAST ParseFormulaAST(std::istream& in) {
    using namespace antlr4;

    ANTLRInputStream input(in);

    FormulaLexer lexer(&input);
    ASTImpl::BailErrorListener error_listener;
    lexer.removeErrorListeners();
    lexer.addErrorListener(&error_listener);

    CommonTokenStream tokens(&lexer);

    FormulaParser parser(&tokens);
    auto error_handler = std::make_shared<BailErrorStrategy>();
    parser.setErrorHandler(error_handler);
    parser.removeErrorListeners();

    tree::ParseTree* tree = parser.main();
    ASTImpl::ParseASTListener listener;
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    return FormulaAST(listener.MoveRoot());
}

FormulaAST ParseFormulaAST(const std::string& in_str) {
    std::istringstream in(in_str);
    try {
        return ParseFormulaAST(in);
    } catch (const std::exception& exc) {
        std::throw_with_nested(FormulaException(exc.what()));
    }
}

void FormulaAST::Print(std::ostream& out) const {
    root_expr_->Print(out);
}

void FormulaAST::PrintFormula(std::ostream& out) const {
    root_expr_->PrintFormula(out, ASTImpl::EP_ATOM);
}

double FormulaAST::Execute() const {
    return root_expr_->Evaluate();
}

FormulaAST::FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr)
    : root_expr_(std::move(root_expr)) {
}

FormulaAST::~FormulaAST() = default;
-----------------------------------------------------------------------
main.cpp
-----------------------------------------------------------------------
#include "FormulaAST.h"
#include "common.h"
#include "test_runner_p.h"

namespace {
double ExecuteASTFormula(const std::string& expression) {
    return ParseFormulaAST(expression).Execute();
}
    
}//end namespace

int main() {
    
    ASSERT_EQUAL(ExecuteASTFormula("1"), 1.0);
    ASSERT_EQUAL(ExecuteASTFormula("1+2*3-4/5"), 6.2);
    
    try {
        ExecuteASTFormula("1/0");
    } catch (const FormulaError& fe) {
        std::cout << fe.what() << std::endl;
    }

    std::cout << "Tests Passed" << std::endl;
    return 0;
}
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
  template <typename K, typename V, template <typename, typename> class Map>
  std::ostream& PrintMap(std::ostream& os, const Map<K, V>& m) {
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
}

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
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
  return TestRunnerPrivate::PrintMap(os, m);
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m) {
  return TestRunnerPrivate::PrintMap(os, m);
}

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

inline void Assert(bool b, const std::string& hint) {
  AssertEqual(b, true, hint);
}

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

#define ASSERT_EQUAL(x, y)                                               \
  {                                                                      \
    std::ostringstream __assert_equal_private_os;                        \
    __assert_equal_private_os << #x << " != " << #y << ", " << FILE_NAME \
                              << ":" << __LINE__;                        \
    AssertEqual(x, y, __assert_equal_private_os.str());                  \
  }

#define ASSERT(x)                                                  \
  {                                                                \
    std::ostringstream __assert_private_os;                        \
    __assert_private_os << #x << " is false, " << FILE_NAME << ":" \
                        << __LINE__;                               \
    Assert(x, __assert_private_os.str());                          \
  }

#define RUN_TEST(tr, func) tr.RunTest(func, #func)
