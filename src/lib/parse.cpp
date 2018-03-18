#include <sstream>

#include "object.h"

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

static Object error(Token& token, std::string msg)
{
    std::ostringstream payload;
    payload << "At " << token.position() << ": " << msg;
    return Object::Error(Object::Symbol("parse"), Object::String(payload.str()));
}

#define RETURN_IF_ERROR(obj)                                            \
    do { if ((obj).type() == Type::Error) return (obj); } while (0)
#define ERROR_IF(cond, token, msg)                                      \
    do { if (cond) return error((token), (msg)); } while (0)
#define ERROR_IF_UNDEFINED(obj, token, msg)                             \
    ERROR_IF((obj).type() == Type::Error, token, msg)

static Object read_datum(Lexer& source)
{
    if (!source)
        return Object::Undefined();

    Token token;
    source >> token;

    if (token == "#t")
        return Object::True();
    else if (token == "#f")
        return Object::False();
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
                default: return error(token, "unknown escape sequence");
                }
                escaped = false;
            }
        }
        return escaped ? error(token, "should never happen") : Object::String(value);
    }
    else if (token == "(") {
        Object head = Object::EmptyList(), tail;

        while (source && source.peek() != ")" && source.peek() != ".") {
            Object elt = read_datum(source);
            RETURN_IF_ERROR(elt);
            ERROR_IF_UNDEFINED(elt, token, "unmatched paranthesis");
            if (head.type() == Type::EmptyList) {
                head = Object::Pair(elt, Object::EmptyList());
                tail = head;
            }
            else {
                tail.set_cdr(Object::Pair(elt, Object::EmptyList()));
                tail = tail.cdr();
            }
        }

        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;

        if (token == ".") {
            Object elt = read_datum(source);
            RETURN_IF_ERROR(elt);
            ERROR_IF_UNDEFINED(elt, token, "unmatched paranthesis");
            tail.set_cdr(elt);

            ERROR_IF(!source, token, "unmatched paranthesis");
            source >> token;
        }

        ERROR_IF(token != ")", token, "unmatched paranthesis");
        return head;
    }
    else if (token == "#(") {
        std::vector<Object> elements;

        while (source && source.peek() != ")") {
            Object elt = read_datum(source);
            RETURN_IF_ERROR(elt);
            ERROR_IF_UNDEFINED(elt, token, "unmatched paranthesis");
            elements.push_back(elt);
        }

        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;        // Closing parenthesis

        Object ret = Object::Vector(elements.size());
        for (std::size_t i = 0; i < elements.size(); i++)
            ret[i] = elements[i];
        return ret;
    }
    else if (token == "'" || token == "`" || token == "," || token == ",@") {
        Object quoted = read_datum(source);
        RETURN_IF_ERROR(quoted);
        ERROR_IF_UNDEFINED(quoted, token, "quotation must have an argument");
        Object quoter;
        if (token == "'") quoter = Object::Symbol("quote");
        if (token == "`") quoter = Object::Symbol("quasiquote");
        if (token == ",") quoter = Object::Symbol("unquote");
        if (token == ",@") quoter = Object::Symbol("unquote-splicing");
        return Object::Pair(quoter, Object::Pair(quoted, Object::EmptyList()));
    }
    else if (legal_symbol(token))
        return Object::Symbol(token.string());
    else
        return error(token, "unknown token");
}

void Parser::read()
{
    obj = read_datum(source);
}
