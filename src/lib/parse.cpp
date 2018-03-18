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
    VM::error(VM::Intern("parse"), VM::String(payload.str()));
}

#define ERROR(token, msg)                                               \
    do { error((token), (msg)); return Object::Undefined; } while (0)
#define ERROR_IF(cond, token, msg)                                      \
    do { if (cond) ERROR(token, msg); } while (0)
#define READ_OR_ERROR(name, token, msg)                                 \
    Object name = read_datum(source);                                   \
    RETURN_IF_ERROR;                                                    \
    ERROR_IF((name).undefined(), token, msg)

static Object read_datum(Lexer& source)
{
    // If we return Undefined without setting an error, it signals EOF
    if (!source)
        return Object::Undefined;

    Token token;
    source >> token;

    if (token == "#t")
        return Object::True;
    else if (token == "#f")
        return Object::False;

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
        return VM::String(value);
    }

    // List parsing
    else if (token == "(") {
        Object head = Object::EmptyList, tail;

        while (source && source.peek() != ")" && source.peek() != ".") {
            READ_OR_ERROR(elt, token, "unmatched paranthesis");
            if (head.type() == Type::EmptyList) {
                head = VM::Pair(elt, Object::EmptyList);
                tail = head;
            }
            else {
                tail.set_cdr(VM::Pair(elt, Object::EmptyList));
                tail = tail.cdr();
            }
        }

        // At this point, we have EOF, a closing paren or a period
        // Error if it's EOF
        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;

        // If a period, we need one more element, then get the next token
        if (token == ".") {
            READ_OR_ERROR(elt, token, "unmatched paranthesis");
            tail.set_cdr(elt);

            ERROR_IF(!source, token, "unmatched paranthesis");
            source >> token;
        }

        // Last token must be a closing paren
        ERROR_IF(token != ")", token, "unmatched paranthesis");
        return head;
    }

    // Vector parsing
    else if (token == "#(") {
        std::vector<Object> elements;

        while (source && source.peek() != ")") {
            READ_OR_ERROR(elt, token, "unmatched paranthesis");
            elements.push_back(elt);
        }

        ERROR_IF(!source, token, "unmatched paranthesis");
        source >> token;        // Closing parenthesis

        return VM::Vector(elements);
    }

    // Quoted structures
    else if (token == "'" || token == "`" || token == "," || token == ",@") {
        READ_OR_ERROR(quoted, token, "quotation must have an argument");
        Object quoter;
        if (token == "'") quoter = VM::Intern("quote");
        if (token == "`") quoter = VM::Intern("quasiquote");
        if (token == ",") quoter = VM::Intern("unquote");
        if (token == ",@") quoter = VM::Intern("unquote-splicing");
        return VM::List({quoter, quoted});
    }

    else if (legal_symbol(token))
        return VM::Intern(token.string());

    error(token, "unknown token");
    return Object::Undefined;
}

void Parser::read()
{
    obj = read_datum(source);
}

std::vector<Object> parse_all(std::istream& stream)
{
    VM::push_frame();
    Parser parser(stream);

    std::vector<Object> forms;
    Object form;
    while (parser) {
        parser >> form;
        forms.push_back(form);
    }

    VM::pop_frame();
    return forms;
}

Object parse_toplevel(std::istream& stream)
{
    VM::push_frame();
    Parser parser(stream);

    std::vector<Object> forms = { VM::Intern("begin") };
    Object form;
    while (parser) {
        parser >> form;
        forms.push_back(form);
    }

    if (VM::has_error())
        return Object::Undefined;

    form = VM::List(forms);
    VM::pop_frame();
    return form;
}

Object parse_single(std::istream& stream)
{
    VM::push_frame();
    Parser parser(stream);

    if (!parser) {
        if (!VM::has_error())
            VM::error(VM::Intern("parse"), VM::String("No data to parse"));
        return Object::Undefined;
    }

    Object form;
    parser >> form;
    VM::pop_frame();
    return form;
}
