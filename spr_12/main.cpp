#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;
using filesystem::path;
namespace fs = std::filesystem;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}


bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories);

string GetFileContents(string file) {
    ifstream stream(file);
    return { (istreambuf_iterator<char>(stream)), istreambuf_iterator<char>() };
}

bool ProcessInclude(const smatch& match, const path& current_file_path, const vector<path>& include_directories, stringstream& result, int line_number) {
    string include_path_str = match[1].str();
    path include_path;

    // Check if it's a relative path include directive
    if (include_path_str[0] == '.') {
        include_path = current_file_path.parent_path() / include_path_str;
        if (!fs::exists(include_path)) {
            for (const auto& dir : include_directories) {
                include_path = dir / include_path_str;
                if (fs::exists(include_path)) {
                    break;
                }
            }
        }
    }
    else { // It's a system path include directive
        for (const auto& dir : include_directories) {
            include_path = dir / include_path_str;
            if (fs::exists(include_path)) {
                break;
            }
        }
    }

    if (!fs::exists(include_path)) {
        cout << "unknown include file " << include_path_str << " at file " << current_file_path.string() << " at line " << line_number << endl;
        return false;
    }

    ifstream include_file(include_path);
    if (!include_file) {
        cout << "unknown include file " << include_path_str << " at file " << current_file_path.string() << " at line " << line_number << endl;
        return false;
    }

    result << GetFileContents(include_path.string()) << endl;

    return true;
}





bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream input(in_file);
    if (!input) {
        return false;
    }

    ofstream output(out_file);
    if (!output) {
        return false;
    }

    stringstream result;
    string line;
    int line_number = 1;
    regex include_regex(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
        smatch include_match;

    while (getline(input, line)) {
        if (regex_search(line, include_match, include_regex)) {
            if (!ProcessInclude(include_match, in_file, include_directories, result, line_number)) {
                return false;
            }
        }
        else {
            result << line << endl;
        }

        line_number++;
    }

    output << result.str();

    return true;
    

}


void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "#include \"dir1/b.h\"\n"
            "// text between b.h and c.h\n"
            "#include \"dir1/d.h\"\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "#   include<dummy.txt>\n"
            "}\n"sv;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
            "#include \"subdir/c.h\"\n"
            "// text from b.h after include"sv;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
            "#include <std1.h>\n"
            "// text from c.h after include\n"sv;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
            "#include \"lib/std2.h\"\n"
            "// text from d.h after include\n"sv;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"sv;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"sv;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// text from b.h before include\n"
        "// text from c.h before include\n"
        "// std1\n"
        "// text from c.h after include\n"
        "// text from b.h after include\n"
        "// text between b.h and c.h\n"
        "// text from d.h before include\n"
        "// std2\n"
        "// text from d.h after include\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"sv;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}
