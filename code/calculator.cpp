#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <sstream>
#include <windows.h>
#include <thread>

using namespace std;

void pauseForInput(int key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (!(GetAsyncKeyState(key) & 0x8000)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

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

void clean_up_mess(TokenStream& ts) {
    ts.ignore(print);
}

double expression(TokenStream& ts);

double primary(TokenStream& ts) {
    token t = ts.get();
    double d;
    switch (t.kind()) {
        case '(':
            d = expression(ts);
            t = ts.get();
            if (t.kind() != ')') throw runtime_error("')' expected");
            return d;
        case number:
            return t.value();
        case '-':
            return -primary(ts);
        case '#': // Square root function
        {
            double val = primary(ts);
            if (val < 0) {
                throw runtime_error("Square root of negative number");
            }
            return sqrt(val);
        }
        case '@': // Exponential function
            return exp(primary(ts));
        case '~': // Natural logarithm
        {
            double val = primary(ts);
            if (val <= 0) {
                throw runtime_error("Natural logarithm of non-positive number");
            }
            return log(val);
        }
        case 's': // Sine function
            return sin(primary(ts));
        case 'c': // Cosine function
            return cos(primary(ts));
        case 't': // Tangent function
            return tan(primary(ts));
        default:
            throw runtime_error("primary expected");
    }
}

double term(TokenStream& ts) {
    double left = primary(ts);

    while (true) {
        token t = ts.get();

        switch (t.kind()) {
            case '*':
                left *= primary(ts);
                break;
            case '/': {
                double d = primary(ts);
                if (d == 0) {
                    throw runtime_error("division by zero");
                }
                left /= d;
                break;
            }
            case '%': {
                double d = primary(ts);
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

double expression(TokenStream& ts) {
    double left = term(ts);

    while (true) {
        token t = ts.get();

        switch (t.kind()) {
            case '+':
                left += term(ts);
                break;
            case '-':
                left -= term(ts);
                break;
            default:
                ts.pushback(t);
                return left;
        }
    }
}

void calculate(TokenStream& ts) {
    while (cin) {
        try {
            cout << prompt;
            token t = ts.get();
            if (t.kind() == quit) {
                break;
            }

            ts.pushback(t);
            double result = expression(ts);

            t = ts.get();
            if (t.kind() == print) {
                cout << "= " << result << endl;
            } else {
                ts.pushback(t);
            }

        } catch (std::exception const& e) {
            cerr << e.what() << endl;
            clean_up_mess(ts);
        } catch (...) {
            cerr << "Exception occurred." << endl;
            clean_up_mess(ts);
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

void run_tests() {
    auto check_result = [](const string& input, double expected) {
        istringstream in(input);
        TokenStream ts(in);
        double result = expression(ts);
        if (abs(result - expected) < 1e-9) {
            cout << "Test passed for input: " << input
                 << " Result: " << result << endl;
        } else {
            cout << "Test failed for input: " << input
                 << " Expected: " << expected << " Got: " << result << endl;
        }
    };

    // Run test cases
    check_result("3 + 5;", 8.0);
    check_result("10 - 3;", 7.0);
    check_result("4 * 2;", 8.0);
    check_result("10 / 2;", 5.0);
    check_result("9 % 4;", 1.0);
    check_result("3.5 + 2.5;", 6.0);
    check_result("3.5 - 2.5;", 1.0);
    check_result("sqrt(16);", 4.0);
    check_result("exp(2);", exp(2));
    check_result("ln(2);", log(2));
    check_result("sin(3.14159);", sin(3.14159));
    check_result("cos(0);", cos(0));
    check_result("tan(0);", tan(0));
    check_result("5!;", 120.0);
}

int main() {
    run_tests();
    cout << "PRESS ENTER TO USE CALCULATOR" << endl;
    pauseForInput(VK_RETURN);
    printFunctionality();
    try {
        TokenStream ts{cin};
        calculate(ts);
        return 0;
    } catch (std::exception const& e) {
        cerr << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Exception occurred." << endl;
        return 2;
    }
}


