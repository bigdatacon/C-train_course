// в качестве заготовки кода используйте последнюю версию своей поисковой системы
//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
#include "document.h"

    Document::Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

std::ostream& operator << (std::ostream& out, const Document search){
    return out << "{ document_id = " << search.id << ", relevance = " << search.relevance << ", rating = " << search.rating << " }";
}