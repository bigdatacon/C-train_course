#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;


std::string reverseAndAddDot(std::string str) {
    std::string reversedStr = "";

    // переворачиваем строку
    for (int i = str.length() - 1; i >= 0; i--) {
        reversedStr += str[i];
    }

    // добавляем точку в конец строки
    reversedStr += ".";

    return reversedStr;
}




class Domain {
public:
    Domain(string domain) {
        inverse_domain_ = reverseAndAddDot(domain);

    }
    // разработайте operator==
    bool operator==(const Domain& other) const {
        return inverse_domain_ == other.inverse_domain_;
    }

    bool operator<(const Domain& other) const {
        //return inverse_domain_ < other.inverse_domain_;
        return lexicographical_compare(inverse_domain_.begin(), inverse_domain_.end(), other.inverse_domain_.begin(), other.inverse_domain_.end());
    }

    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& another_domain) const {
        if (inverse_domain_.length() < another_domain.inverse_domain_.length()) {
            return false;
        }
        std::string subdomain = inverse_domain_.substr(inverse_domain_.length() - another_domain.inverse_domain_.length());
        return subdomain == another_domain.inverse_domain_;
    }


    string GetDomain() const {
        return inverse_domain_;
    }



private:
    string inverse_domain_;

};

/*
bool operator==(const Domain& this_is, const Domain& other) const {
    return this_is.GetDomain() == other.GetDomain();
}

bool operator<(const Domain& this_is, const Domain& other) const {
    return this_is.GetDomain() < other.GetDomain();
}
*/

int binary_search(const std::vector<Domain>& v, const Domain& target) {
    int left = 0;
    int right = v.size() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        if (v[mid] == target) {
            return mid;
        }
        /*
        else if (lexicographical_compare(v[mid].GetDomain().begin(), v[mid].GetDomain().end(),
            target.GetDomain().begin(), target.GetDomain().end())) {
            left = mid + 1;
        }
        */

        else if (v[mid]< target) {
            left = mid + 1;
        }

        else {
            right = mid - 1;
        }
    }

    return -1; // элемент не найден
}

class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    template <typename InputIt>
    DomainChecker(InputIt begin, InputIt end) {
        forbiddenDomains_ = std::vector<Domain>(begin, end);
        
        /*
        std::sort(forbiddenDomains_.begin(), forbiddenDomains_.end(), [](Domain l, Domain r) {return
            lexicographical_compare(l.GetDomain().begin(), l.GetDomain().end(),
                r.GetDomain().begin(), r.GetDomain().end());
            });
        */

        std::sort(forbiddenDomains_.begin(), forbiddenDomains_.end(), [](Domain l, Domain r) {return l < r;});



        // удаляем избыточные поддомены
        auto it_del_subdomens = std::unique(forbiddenDomains_.begin(), forbiddenDomains_.end(),
            [](const Domain& l, const Domain& r) {
                return l.IsSubdomain(r) || r.IsSubdomain(l);
            });
        forbiddenDomains_.erase(it_del_subdomens, forbiddenDomains_.end());
    }


    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain) const {
        //return std::find(forbiddenDomains_.begin(), forbiddenDomains_.end(), domain) != forbiddenDomains_.end() ;
        return  binary_search(forbiddenDomains_, domain);
    }

private:
    std::vector<Domain> forbiddenDomains_;
};


// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
const std::vector<Domain> ReadDomains(istream& input, size_t count) {
    std::vector<Domain> domains;
    domains.reserve(count);



    for (size_t i = 0; i < count; ++i) {
        string domain;
        string line;
        getline(input, line);
        //input >> domain;
        std::istringstream(line) >> domain;
        domains.push_back(Domain(domain));
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
#ifdef _DEBUG
    if (freopen("input.txt.txt", "r", stdin) == nullptr) {
        puts("can't open input.txt.txt");
        return 1;
    }
#endif

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
