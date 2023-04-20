#include "json.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>

using namespace std;

namespace json {

	namespace {


		bool operator==(const Node& lhs, const Node& rhs)
		{
			if (lhs.GetValue().index() != rhs.GetValue().index()) {
				return false;
			}

			switch (lhs.GetValue().index()) {
			case 0: // nullptr_t
				return true;
			case 1: // Array
				return std::equal_to<>()(std::get<Array>(lhs.GetValue()), std::get<Array>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			case 2: // Dict
				return std::equal_to<>()(std::get<Dict>(lhs.GetValue()), std::get<Dict>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			case 3: // bool
				return std::equal_to<>()(std::get<bool>(lhs.GetValue()), std::get<bool>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			case 4: // int
				return std::equal_to<>()(std::get<int>(lhs.GetValue()), std::get<int>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			case 5: // double
				return std::equal_to<>()(std::get<double>(lhs.GetValue()), std::get<double>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			case 6: // string
				return std::equal_to<>()(std::get<std::string>(lhs.GetValue()), std::get<std::string>(rhs.GetValue()))
					&& lhs.GetValue().index() == rhs.GetValue().index();
			default:
				throw std::runtime_error("Unknown value type");
			}
		}


		bool operator!=(const Node& lhs, const Node& rhs)
		{
			return !(lhs == rhs);
		}

		Node LoadNode(istream& input);

		Node LoadArray(istream& input) {
			Array result;

			for (char c; input >> c && c != ']';) {
				if (c != ',') {
					input.putback(c);
				}
				result.push_back(LoadNode(input));
			}

			return Node(move(result));
		}

		Node LoadInt(istream& input) {
			int result = 0;
			while (isdigit(input.peek())) {
				result *= 10;
				result += input.get() - '0';
			}
			return Node(result);
		}

		Node LoadString(istream& input) {
			string line;
			getline(input, line, '"');
			return Node(move(line));
		}

		Node LoadDict(istream& input) {
			Dict result;

			for (char c; input >> c && c != '}';) {
				if (c == ',') {
					input >> c;
				}

				string key = LoadString(input).AsString();
				input >> c;
				result.insert({ move(key), LoadNode(input) });
			}

			return Node(move(result));
		}

		Node LoadNode(istream& input) {
			char c;
			input >> c;

			if (c == '[') {
				return LoadArray(input);
			}
			else if (c == '{') {
				return LoadDict(input);
			}
			else if (c == '"') {
				return LoadString(input);
			}
			else {
				input.putback(c);
				return LoadInt(input);
			}
		}



		class ParsingError : public std::runtime_error {
		public:
			using runtime_error::runtime_error;
		};

		using Number = std::variant<int, double>;

		Number LoadNumber(std::istream& input) {
			using namespace std::literals;

			std::string parsed_num;

			// Считывает в parsed_num очередной символ из input
			auto read_char = [&parsed_num, &input] {
				parsed_num += static_cast<char>(input.get());
				if (!input) {
					throw ParsingError("Failed to read number from stream"s);
				}
			};

			// Считывает одну или более цифр в parsed_num из input
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
			// Парсим целую часть числа
			if (input.peek() == '0') {
				read_char();
				// После 0 в JSON не могут идти другие цифры
			}
			else {
				read_digits();
			}

			bool is_int = true;
			// Парсим дробную часть числа
			if (input.peek() == '.') {
				read_char();
				read_digits();
				is_int = false;
			}

			// Парсим экспоненциальную часть числа
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
					// Сначала пробуем преобразовать строку в int
					try {
						return std::stoi(parsed_num);
					}
					catch (...) {
						// В случае неудачи, например, при переполнении,
						// код ниже попробует преобразовать строку в double
					}
				}
				return std::stod(parsed_num);
			}
			catch (...) {
				throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
			}
		}

		// Считывает содержимое строкового литерала JSON-документа
		// Функцию следует использовать после считывания открывающего символа ":
		/*std::string LoadString(std::istream& input) {
			using namespace std::literals;

			auto it = std::istreambuf_iterator<char>(input);
			auto end = std::istreambuf_iterator<char>();
			std::string s;
			while (true) {
				if (it == end) {
					// Поток закончился до того, как встретили закрывающую кавычку?
					throw ParsingError("String parsing error");
				}
				const char ch = *it;
				if (ch == '"') {
					// Встретили закрывающую кавычку
					++it;
					break;
				}
				else if (ch == '\\') {
					// Встретили начало escape-последовательности
					++it;
					if (it == end) {
						// Поток завершился сразу после символа обратной косой черты
						throw ParsingError("String parsing error");
					}
					const char escaped_char = *(it);
					// Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
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
						// Встретили неизвестную escape-последовательность
						throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
					}
				}
				else if (ch == '\n' || ch == '\r') {
					// Строковый литерал внутри- JSON не может прерываться символами \r или \n
					throw ParsingError("Unexpected end of line"s);
				}
				else {
					// Просто считываем очередной символ и помещаем его в результирующую строку
					s.push_back(ch);
				}
				++it;
			}

			return s;
		}*/

	}  // namespace
	Node::Node(std::nullptr_t) : value_(nullptr) {}

	Node::Node(Array array)
		: value_(move(array)) {
	}

	Node::Node(Dict map)
		: value_(move(map)) {
	}

	Node::Node(int value)
		: value_(value) {
	}


	Node::Node(double value)
		: value_(value) {
	}

	Node::Node(string value)
		: value_(move(value)) {
	}

	Node::Node(bool val) : value_(val) {}
	/////////////////////////////////////

	const Array& Node::AsArray() const {
		return std::get<Array>(value_);
	}

	const Dict& Node::AsMap() const {
		return std::get<Dict>(value_);
	}

	int Node::AsInt() const {
		return std::get<int>(value_);
	}

	const string& Node::AsString() const {
		return std::get<std::string>(value_);
	}

	/////////////////////
	bool Node::IsInt() const { return std::holds_alternative<int>(value_); };
	bool Node::IsDouble() const { return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_); }; //Возвращает true, если в Node хранится int либо double.
	bool Node::IsPureDouble() const { return std::holds_alternative<double>(value_); }; //Возвращает true, если в Node хранится double.
	bool Node::IsBool() const { return std::holds_alternative<bool>(value_); };
	bool Node::IsString() const { return std::holds_alternative<std::string>(value_); };
	bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); };
	bool Node::IsArray() const { return std::holds_alternative<Array>(value_); };
	bool Node::IsMap() const { return std::holds_alternative<Dict>(value_); };

	bool Node::AsBool() const { return std::get<bool>(value_); };
	double Node::AsDouble() const {
		if (IsPureDouble()) {
			return std::get<double>(value_);
		}
		else {
			return static_cast<double>(std::get<int>(value_));
		}
	} //.Возвращает значение типа double, если внутри хранится double либо int.В последнем случае возвращается приведённое в double значение.

	Document::Document(Node root)
		: root_(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root_;
	}

	Document Load(istream& input) {
		return Document{ LoadNode(input) };
	}

	// Шаблон, подходящий для вывода double и int
	template <typename Value>
	void PrintValue(const Value& value, std::ostream& out) {
		out << value;
	}



	// Перегрузка функции PrintValue для вывода значений null
	void PrintValue(std::nullptr_t, std::ostream& out) {
		out << "null"sv;
	}
	// Другие перегрузки функции PrintValue пишутся аналогично


	//bool
	//template <typename Value>
	void PrintValue(const bool& value, std::ostream& out) {
		out << (value ? "true" : "false");
	}


	void PrintNode(const Node& node, std::ostream& out) {
		std::visit(
			[&out](const auto& value) { PrintValue(value, out); },
			node.GetValue());
	}

	//Dict
//template <typename Value>
	template<typename K, typename V>
	void PrintValue(/*const Value&*/ const std::map<K, V>& value, std::ostream& out) {
		out << "{";
		for (const auto& [key, val] : value) {
			out << key << ":";
			PrintNode(val, out);
			out << ",";

		}
		out << "}";
	}

	//Array
	void PrintValue(const std::vector<Node>& vec, std::ostream& out) {
		out << "[";
		for (size_t i = 0; i < vec.size(); ++i) {
			PrintNode(vec[i], out);
			if (i != vec.size() - 1) {
				out << ", ";
			}
		}
		out << "]";
	}



	void Print(const Document& doc, std::ostream& output) {
		(void)&doc;
		(void)&output;

		// Реализуйте функцию самостоятельно
		const Node& root = doc.GetRoot();

		// Проверяем, что корневой узел действительно существует
		if (/*!root.GetValue() */ root.GetValue().index() == 0) {
			return;
		}

		// Рекурсивно обходим дерево элементов
		PrintNode(root, output);
	}

}  // namespace json
