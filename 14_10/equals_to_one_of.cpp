#include <cassert>
#include <string>
#include <string_view>

/* Напишите вашу реализацию EqualsToOneOf здесь*/
template <typename T, typename... Par>
bool EqualsToOneOf(const T& v, const Par&... par) {
  return ((v == par) || ...);
}

int main() {
using namespace std::literals;
    assert(EqualsToOneOf("hello"sv, "hi"s, "hello"s));
    assert(!EqualsToOneOf(1, 10, 2, 3, 6));
    assert(!EqualsToOneOf(8));
}