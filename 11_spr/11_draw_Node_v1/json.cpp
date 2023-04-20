#include "json.h"

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

	}  // namespace

	Node::Node(Array array)
		: as_array_(move(array)) {
	}

	Node::Node(Dict map)
		: as_map_(move(map)) {
	}

	Node::Node(int value)
		: as_int_(value) {
	}


	Node::Node(double value)
		: as_double_(value) {
	}

	Node::Node(string value)
		: as_string_(move(value)) {
	}

	const Array& Node::AsArray() const {
		return as_array_;
	}

	const Dict& Node::AsMap() const {
		return as_map_;
	}

	int Node::AsInt() const {
		return as_int_;
	}

	const string& Node::AsString() const {
		return as_string_;
	}

	/////////////////////
	bool Node::IsInt() const { return std::holds_alternative<int>(value_); };
	bool Node::IsDouble() const { return std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_); }; //Возвращает true, если в Node хранится int либо double.
	bool Node::IsPureDouble() const { return std::holds_alternative<double>(value_); }; //Возвращает true, если в Node хранится double.
	bool Node::IsBool() const { return std::holds_alternative<bool>(value_); };
	bool Node::IsString() const { return std::holds_alternative<std::string>(value_); };
	bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); };
	bool Node::IsArray() const { return std::holds_alternative<Array>(value_); };
	bool Node::IsMap() const { return std::holds_alternative<Dict>(value_); };

	bool Node::AsBool() const { return std::get<bool>(value_); };
	double Node::AsDouble() const {
		if (IsDouble()) {
			return std::holds_alternative<double>(value_) ? std::get<double>(value_) : static_cast<double>(std::get<int>(value_));
		}
		throw std::bad_variant_access();
	}; //.Возвращает значение типа double, если внутри хранится double либо int.В последнем случае возвращается приведённое в double значение.

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
