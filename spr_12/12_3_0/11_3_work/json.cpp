
#include "json.h"


using namespace std;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadBool(istream& input) {
            std::string line;
            while (std::isalpha(input.peek())) {
                line.push_back(static_cast<char>(input.get()));
            }
            if (line == "true"sv) {
                return Node{ true };
            }
            else if (line == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Bool error");
            }
        }

        Node LoadArray(istream& input) {
            Array result;
            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            if (!input) {
                throw ParsingError("Array parsing error"s);
            }

            return Node(result);
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            if (input.peek() == '0') {
                read_char();
            }
            else {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        int temp = std::stoi(parsed_num);
                        return Node(temp);
                    }
                    catch (...) {

                    }
                }
                auto temp = std::stod(parsed_num);
                return Node(temp);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(s);
        }

        Node LoadNull(istream& input) {
            std::string line;
            while (std::isalpha(input.peek())) {
                line.push_back(static_cast<char>(input.get()));
            }
            if (line != "null") throw ParsingError("null error"s);
            return Node(nullptr);
        }

        Node LoadDict(std::istream& input) {

            Dict dict;
            for (char current_char; input >> current_char && current_char != '}';) {
                if (current_char == '"') {
                    std::string key = LoadString(input).AsString();

                    if (input >> current_char && current_char == ':') {
                        if (dict.find(key) != dict.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found");
                        }

                        dict.emplace(std::move(key), LoadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + current_char + "' has been found"s);
                    }
                }
                else if (current_char != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + current_char + "' has been found"s);
                }
            }

            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }

            return Node(dict);
        }

        Node LoadNode(istream& input) {
            input >> ws;
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '\"') {
                return LoadString(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            } if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace




    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);
    }



}  // namespace json
