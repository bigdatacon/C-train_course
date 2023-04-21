	#include "json.h"
	#include <cassert>
	#include <iostream>
	#include <sstream>
	#include <string>
	#include <variant>
	#include <cctype>
	#include <regex>
	#include <istream>
	using namespace std;

	namespace json {

		namespace {

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
				//getline(input, line, '"');
				getline(input, line);
				if (line == "null"s || line.find("null"s)!= std::string::npos) {
					return Node(nullptr);
				}
				else {

					std::string s2 = ""s;
					for (auto it = line.begin(); it != line.end()-1; ++it) {
						// ���� ��� �� ������ ������ � ������� ������ ����� �����������
						if (*it == '\\'  && *(it + 1) == '\"') {
							continue;
						}

						else if (*it == '\\' && *(it + 1) == 'n') {
							s2 += '\n';
							break;
						}
						else if (*it == '\\' && *(it + 1) == 'r') {
							s2 += '\r';
							break;
						}
						else if (*it == '\\' && *(it + 1) == 't') {
							s2 += '\r';
							break;
						}
						else {

							s2 += *it;
						}
					}

					return Node(move(s2));

					/*std::string search1 = "\\\\";
					std::string replace1 = "\\";
					std::string search2 = "\"";
					std::string replace2 = "";
					//std::string search2 = "\\\"";
					//std::string replace2 = "\"\"";

					size_t pos = 0;
					while ((pos = line.find(search1, pos)) != std::string::npos) {
						line.replace(pos, search1.length(), replace1);
						pos += replace1.length();
					}

					pos = 0;
					while ((pos = line.find(search2, pos)) != std::string::npos) {
						line.replace(pos, search2.length(), replace2);
						pos += replace2.length();
					}
					
					return Node(move(line));*/
				}
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

			Node LoadBool(std::istream& input) {
				bool value;
				input >> value;
				return Node(value);
			}

			Node LoadNull(std::istream& input) {
				input.ignore(4, 'l'); // ���������� ����� "null"
				return Node(nullptr);
			}



			class ParsingError : public std::runtime_error {
			public:
				using runtime_error::runtime_error;
			};

			using Number = std::variant<int, double>;

			/*Number*/ Node LoadNumber(std::istream& input) {
				using namespace std::literals;

				std::string parsed_num;

				// ��������� � parsed_num ��������� ������ �� input
				auto read_char = [&parsed_num, &input] {
					parsed_num += static_cast<char>(input.get());
					if (!input) {
						throw ParsingError("Failed to read number from stream"s);
					}
				};

				// ��������� ���� ��� ����� ���� � parsed_num �� input
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
				// ������ ����� ����� �����
				if (input.peek() == '0') {
					read_char();
					// ����� 0 � JSON �� ����� ���� ������ �����
				}
				else {
					read_digits();
				}

				bool is_int = true;
				// ������ ������� ����� �����
				if (input.peek() == '.') {
					read_char();
					read_digits();
					is_int = false;
				}

				// ������ ���������������� ����� �����
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
						// ������� ������� ������������� ������ � int
						try {
							return Node(std::stoi(parsed_num));
						}
						catch (...) {
							// � ������ �������, ��������, ��� ������������,
							// ��� ���� ��������� ������������� ������ � double
						}
					}
					return Node(std::stod(parsed_num));
				}
				catch (...) {
					throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
				}
			}

			// ��������� ���������� ���������� �������� JSON-���������
			// ������� ������� ������������ ����� ���������� ������������ ������� ":
			/*std::string*/ Node LoadStringF(std::istream& input) {
				using namespace std::literals;

				auto it = std::istreambuf_iterator<char>(input);
				auto end = std::istreambuf_iterator<char>();
				std::string s;
				while (true) {
					if (it == end) {
						// ����� ���������� �� ����, ��� ��������� ����������� �������?
						throw ParsingError("String parsing error");
					}
					const char ch = *it;
					if (ch == '"') {
						// ��������� ����������� �������
						++it;
						break;
					}
					else if (ch == '\\') {
						// ��������� ������ escape-������������������
						++it;
						if (it == end) {
							// ����� ���������� ����� ����� ������� �������� ����� �����
							throw ParsingError("String parsing error");
						}
						const char escaped_char = *(it);
						// ������������ ���� �� �������������������: \\, \n, \t, \r, \"
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
							// ��������� ����������� escape-������������������
							throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
						}
					}
					else if (ch == '\n' || ch == '\r') {
						// ��������� ������� ������- JSON �� ����� ����������� ��������� \r ��� \n
						throw ParsingError("Unexpected end of line"s);
					}
					else {
						// ������ ��������� ��������� ������ � �������� ��� � �������������� ������
						s.push_back(ch);
					}
					++it;
				}

				return Node(s);
			}

			bool is_integer(const std::string& s) {
				try {
					std::stoi(s);
					return true;
				}
				catch (...) {
					return false;
				}
			}

			bool is_double(const std::string& s) {
				try {
					std::stod(s);
					return true;
				}
				catch (...) {
					return false;
				}
			}


			Node LoadNode(istream& input) {
				input >> ws;
				char c;
				input >> c;
				if (c == 'n') {
					input.putback(c);
					return LoadNull(input);
				}
				else if (c == 't' || c == 'f') {
					input.putback(c);
					return LoadBool(input);
				}
				else if (c == '\"') {
					//input.putback(c);
					return LoadStringF(input);
				}

				if (c == '[') {
					input.putback(c);
					return LoadArray(input);
				}
				else if (c == '{') {
					input.putback(c);
					return LoadDict(input);
				}

				else {
					input.putback(c);
					return LoadNumber(input);

				}
			}


			/*Node LoadNode(istream& input) {
				char c;
				input >> c;
				//cout << "INPUT : " << input << end;

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
					if (!isdigit(c) && c!='-') {
						input.putback(c);
						return LoadString(input);
					}
					else {
						input.putback(c);
						Number num = LoadNumber(input);
						if (std::holds_alternative<int>(num)) {
							int myValue = std::get<int>(num);
							return Node(myValue);
						}
						else if (std::holds_alternative<double>(num)) {
							double myValue = std::get<double>(num);
							return Node(myValue);
						}
					}
	
					
				}
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


		bool Node::operator==(const Node& rhs)  {
			return value_ == rhs.value_;
		}

		bool Node::operator!=(const Node& rhs)  {
			return value_ != rhs.value_;
		}


		bool Node::IsInt() const { return std::holds_alternative<int>(value_); };
		bool Node::IsDouble() const { return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_); }; //���������� true, ���� � Node �������� int ���� double.
		bool Node::IsPureDouble() const { return std::holds_alternative<double>(value_); }; //���������� true, ���� � Node �������� double.
		bool Node::IsBool() const { return std::holds_alternative<bool>(value_); };
		bool Node::IsString() const { return std::holds_alternative<std::string>(value_); };
		bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_) || std::get<int>(value_)==0; };
		//bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); };
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
		} //.���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.

		Document::Document(Node root)
			: root_(move(root)) {
		}

		const Node& Document::GetRoot() const {
			return root_;
		}
		 
		Document Load(istream& input) {
			return Document{ LoadNode(input) };
		}


		bool Document::operator==(const Document& rhs)  {
			return root_ == rhs.GetRoot();
		}

		bool Document::operator!=(const Document& rhs)  {
			return root_ != rhs.GetRoot();
		}


		//�������� PrintNode 
		void PrintNode(const Node& node, std::ostream& out);

		// ������, ���������� ��� ������ double � int
		template <typename Value>
		void PrintValue(const Value& value, std::ostream& out) {
			out << value;
		}

		void PrintValue(const std::string& value, std::ostream& out) {
			out << '"';
			for (const auto c : value) {
				if (c == '\\' || c == '\"') {
					out << '\\';
				}
				out << c;
			}
			out << '"';
		}


		// ���������� ������� PrintValue ��� ������ �������� null
		void PrintValue(std::nullptr_t, std::ostream& out) {
			out << "null"sv;
		}
		// ������ ���������� ������� PrintValue ������� ����������


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

			// ���������� ������� ��������������
			const Node& root = doc.GetRoot();

			// ���������, ��� �������� ���� ������������� ����������
			if (/*!root.GetValue() */ root.GetValue().index() == 0) {
				/*return*/ cout << "null"s;
			}

			// ���������� ������� ������ ���������
			PrintNode(root, output);
		}

	}  // namespace json