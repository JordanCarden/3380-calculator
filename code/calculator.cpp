#include <iostream>
#include <stdexcept>
#include <cmath>
#include <map>
#include <string>

using namespace std;

char const number = '8';
char const quit = 'q';
char const print = ';';
std::string const prompt = ">";
std::string const result = "= ";

class token {
    char kind_;
    double value_;

public:
    token(char ch) : kind_{ch}, value_{0} {}
    token(double val) : kind_{number}, value_{val} {}

    char kind() const { return kind_; }
    double value() const { return value_; }
};

class TokenStream {
    bool full;
    token buffer;
    std::istream& in;

public:
    TokenStream(std::istream& in) : full{false}, buffer{'\0'}, in{in} {}
    token get();
    void pushback(token t);
    void ignore(char c);
};

TokenStream ts(cin);

double factorial(double n) {
    if (n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}

token TokenStream::get() {
    if (full) {
        full = false;
        return buffer;
    }

    char ch;
    in >> ch;

    switch (ch) {
        case '(': case ')': case ';': case '+': case '-': case '*': case '/': case '%': case 'q':
            return token{ch};
        case '.': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
            in.putback(ch);
            double val;
            in >> val;
            if (in.get(ch) && ch == '!') {
                return token{factorial(val)};
            } else {
                in.putback(ch);
                return token{val};
            }
        }
        default: {
            if (isalpha(ch)) {
                string s;
                s += ch;
                while (in.get(ch) && isalnum(ch)) s += ch;
                in.putback(ch);
                if (s == "pi") return token{3.14159265358979323846};
                if (s == "e") return token{2.71828182845904523536};
                if (s == "phi" || s == "Φ") return token{1.61803398874989484820};
                if (s == "sqrt") return token{'#'}; // '#' symbolizes square root function
                if (s == "exp") return token{'@'};  // '@' symbolizes exponential function
                if (s == "ln") return token{'~'};   // '~' symbolizes natural logarithm
                if (s == "sin") return token{'s'}; // 'sin' symbolizes sine function
                if (s == "cos") return token{'c'}; // 'cos' symbolizes cosine function
                if (s == "tan") return token{'t'}; // 'tan' symbolizes tangent function
                throw runtime_error("Unknown symbol: " + s);
            }
            throw runtime_error("Bad token");
        }
    }
}

void TokenStream::pushback(token t) {
    if (full) {
        throw runtime_error("pushback() into a full buffer");
    }

    buffer = t;
    full = true;
}

void TokenStream::ignore(char c) {
    if (full && c == buffer.kind()) {
        full = false;
        return;
    }

    full = false;

    char ch = 0;
    while (in >> ch) {
        if (ch == c) {
            return;
        }
    }
}

void clean_up_mess() {
    ts.ignore(print);
}

double expression();

double primary() {
    token t = ts.get();
    double d;
    switch (t.kind()) {
        case '(':
            d = expression();
            t = ts.get();
            if (t.kind() != ')') throw runtime_error("')' expected");
            return d;
        case number:
            return t.value();
        case '-':
            return -primary();
        case '#': // Square root function
        {
            double val = primary();
            if (val < 0) {
                throw runtime_error("Square root of negative number");
            }
            return sqrt(val);
        }
        case '@': // Exponential function
            return exp(primary());
        case '~': // Natural logarithm
        {
            double val = primary();
            if (val <= 0) {
                throw runtime_error("Natural logarithm of non-positive number");
            }
            return log(val);
        }
        case 's': // Sine function
            return sin(primary());
        case 'c': // Cosine function
            return cos(primary());
        case 't': // Tangent function
            return tan(primary());
        default:
            throw runtime_error("primary expected");
    }
}

double term() {
    double left = primary();

    while (true) {
        token t = ts.get();

        switch (t.kind()) {
            case '*':
                left *= primary();
                break;
            case '/': {
                double d = primary();
                if (d == 0) {
                    throw runtime_error("division by zero");
                }
                left /= d;
                break;
            }
            case '%': {
                double d = primary();
                if (d == 0) {
                    throw runtime_error("modulus by zero");
                }
                left = fmod(left, d);
                break;
            }
            default:
                ts.pushback(t);
                return left;
        }
    }
}

double expression() {
    double left = term();

    while (true) {
        token t = ts.get();

        switch (t.kind()) {
            case '+':
                left += term();
                break;
            case '-':
                left -= term();
                break;
            default:
                ts.pushback(t);
                return left;
        }
    }
}

void calculate() {
    while (cin) {
        try {
            cout << prompt;
            token t = ts.get();

            if (t.kind() == quit) {
                break;
            }

            ts.pushback(t);
            double result = expression();

            t = ts.get();
            if (t.kind() == print) {
                cout << "= " << result << endl;
            } else {
                ts.pushback(t);
            }
        } catch (std::exception const& e) {
            cerr << e.what();
            clean_up_mess();
        } catch (...) {
            cerr << "Exception occurred.";
            clean_up_mess();
        }
    }
}

void printFunctionality() {
    cout << "Calculator Functionality:\n";
    cout << "1. Addition: a + b\n";
    cout << "2. Subtraction: a - b\n";
    cout << "3. Multiplication: a * b\n";
    cout << "4. Division: a / b\n";
    cout << "5. Modulus: a % b\n";
    cout << "6. Factorial: a!\n";
    cout << "7. Square root: sqrt(a)\n";
    cout << "8. Exponential: exp(a)\n";
    cout << "9. Natural logarithm: ln(a)\n";
    cout << "10. Sine: sin(a)\n";
    cout << "11. Cosine: cos(a)\n";
    cout << "12. Tangent: tan(a)\n";
    cout << "Note: a and b are numbers.\n";
    cout << "End an expression with ; to print the result.\n";
    cout << "Enter q to quit the calculator.\n";
}

int main() {
    printFunctionality();
    try {
        calculate();
        return 0;
    }
    catch (std::exception const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "exception \n";
        return 2;
    }
}


