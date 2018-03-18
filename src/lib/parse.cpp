#include <sstream>

#include "object.h"
#include "vm.h"

#include "parse.h"


std::ostream& operator<<(std::ostream& out, Token& token)
{
    out << token.string();
    return out;
}


static void read_identifier(std::istream& source, Token& token)
{
    while (true) {
        char e = source.get();
        if (e != '(' && e != ')' && e != '[' && e != ']' && e != '"' &&
            e != ',' && e != '`' && e != '\'' && e != EOF && !std::isspace(e))
            token.push_back(e);
        else {
            source.unget();
            break;
        }
    }
}

void Lexer::read()
{
    while (std::isspace(source.peek()))
        get();

    token = Token(pos);
    int c, d, e;
    bool escaped = false;

    switch ((c = get()))
    {
    case EOF: return;
    case '(': case ')': case '[': case ']': case '`': case '\'':
        token.push_back(c);
        return;
    case ',':
        if ((d = get()) == '@')
            token = ",@";
        else {
            unget();
            token = ",";
        }
        break;
    case '"':
        token = "\"";
        escaped = false;
        while (true) {
            e = get();
            if (escaped) {
                token.push_back(e);
                escaped = false;
            }
            else if (e == '\\') {
                token.push_back(e);
                escaped = true;
            }
            else {
                if (e != EOF)
                    token.push_back(e);
                if (e == '"' || e == EOF)
                    return;
            }
        }
    case '#':
        token.push_back((char)c);
        token.push_back(get());
        if (token == "#(" || token == "#t" || token == "#f")
            return;
        read_identifier(source, token);
        break;
    default:
        token.push_back((char)c);
        read_identifier(source, token);
    }
}

static std::string initials = "!$%&*/:<=>?~_^";
static std::string subsequents = ".+-";

static bool legal_symbol(const Token& token)
{
    if (token == "+" || token == "-" || token == "...")
        return true;

    char init = token[0];
    if ((init < 'a' || init > 'z') && (init < 'A' || init > 'Z')
        && initials.find(init) == std::string::npos)
        return false;

    for (char c : token.substr(1))
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')
            && initials.find(c) == std::string::npos && subsequents.find(c) == std::string::npos)
            return false;

    return true;
}

static void error(Token& token, std::string msg)
{
    std::ostringstream payload;
    payload << "At " << token.position() << ": " << msg;
    VM::Intern("parse");
    VM::String(payload.str());
    Op::error();
}

#define ERROR(token, msg)                                               \
    do { error((token), (msg)); return; } while (0)
#define ERROR_IF(cond, token, msg)                                      \
    do { if (cond) ERROR(token, msg); } while (0)
#define READ_OR_ERROR(token, msg)                                       \
    read_datum(source);                                                 \
    RETURN_IF_ERROR;                                                    \
    ERROR_IF(VM::peek().undefined(), token, msg)

void read_datum(Lexer& source)
{
    // If we return Undefined without setting an error, it signals EOF
    if (!source) {
        VM::push(Object::Undefined);
        return;
    }

    Token token;
    source >> token;

    if (token == "#t")
        VM::push(Object::True);
    else if (token == "#f")
        VM::push(Object::False);

    // String parsing
    else if (token[0] == '"') {
        ERROR_IF(token.size() < 2 || token[token.size()-1] != '"', token, "unmatched quote");
        token = token.substr(1, token.size()-2);
        std::string value;
        bool escaped = false;
        for (char c : token) {
            if (!escaped && c == '\\') {
                escaped = true;
                continue;
            }
            else if (!escaped)
                value.push_back(c);
            else if (escaped) {
                switch (c) {
                case '\\': case '"':
                    value.push_back(c); break;
                case 'n': value.push_back('\n'); break;
                case 't': value.push_back('\t'); break;
                default: ERROR(token, "unknown escape sequence");
                }
                escaped = false;
            }
        }
        if (escaped)
            ERROR(token, "should never happen");
        Op::string(value);
    }

    // List parsing
    else if (token == "(") {
        std::size_t nelems = 0;
        while (source && source.peek() != ")" && source.peek() != ".") {
            READ_OR_ERROR(token, "unmatched paranthesis");
            nelems++;
        }

        // At this point, we have EOF, a closing paren or a period
        // Error if it's EOF
        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;

        // Get the last cdr
        if (token == ".") {
            READ_OR_ERROR(token, "unmatched paranthesis");
            ERROR_IF(!source, token, "unmatched paranthesis");
            source >> token;
        }
        else { VM::push(Object::EmptyList); }

        // Last token must be a closing paren
        ERROR_IF(token != ")", token, "unmatched paranthesis");

        // Form the pairs
        Op::list(nelems, false);
    }

    // Vector parsing
    else if (token == "#(") {
        std::size_t nelems = 0;
        while (source && source.peek() != ")") {
            READ_OR_ERROR(token, "unmatched paranthesis");
            nelems++;
        }

        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;        // Closing parenthesis

        Op::vector(nelems);
    }

    // Quoted structures
    else if (token == "'" || token == "`" || token == "," || token == ",@") {
        if (token == "'") VM::Intern("quote");
        if (token == "`") VM::Intern("quasiquote");
        if (token == ",") VM::Intern("unquote");
        if (token == ",@") VM::Intern("unquote-splicing");
        READ_OR_ERROR(token, "quotation must have an argument");
        Op::list(2);
    }

    else if (legal_symbol(token))
        Op::intern(token.string());

    else
        error(token, "unknown token");
}

void parse_all(std::istream& stream)
{
    VM::push_frame();

    Lexer source(stream);
    std::size_t nelems = 0;
    while (source && !VM::has_error()) {
        read_datum(source);
        nelems++;
    }

    RET_IF_ERROR;

    Op::list(nelems);
    Op::ret();
}

void parse_toplevel(std::istream& stream)
{
    VM::push_frame();

    parse_all(stream);
    RET_IF_ERROR;

    Op::intern("begin");
    VM::swap();
    Op::cons();
    Op::ret();
}
