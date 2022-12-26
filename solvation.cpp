// -------- РќР°С‡Р°Р»Рѕ РјРѕРґСѓР»СЊРЅС‹С… С‚РµСЃС‚РѕРІ РїРѕРёСЃРєРѕРІРѕР№ СЃРёСЃС‚РµРјС‹ ----------

// РўРµСЃС‚ РїСЂРѕРІРµСЂСЏРµС‚, С‡С‚Рѕ РїРѕРёСЃРєРѕРІР°СЏ СЃРёСЃС‚РµРјР° РёСЃРєР»СЋС‡Р°РµС‚ СЃС‚РѕРї-СЃР»РѕРІР° РїСЂРё РґРѕР±Р°РІР»РµРЅРёРё РґРѕРєСѓРјРµРЅС‚РѕРІ
/*void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // РЎРЅР°С‡Р°Р»Р° СѓР±РµР¶РґР°РµРјСЃСЏ, С‡С‚Рѕ РїРѕРёСЃРє СЃР»РѕРІР°, РЅРµ РІС…РѕРґСЏС‰РµРіРѕ РІ СЃРїРёСЃРѕРє СЃС‚РѕРї-СЃР»РѕРІ,
    // РЅР°С…РѕРґРёС‚ РЅСѓР¶РЅС‹Р№ РґРѕРєСѓРјРµРЅС‚
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == doc_id);
    }

    // Р—Р°С‚РµРј СѓР±РµР¶РґР°РµРјСЃСЏ, С‡С‚Рѕ РїРѕРёСЃРє СЌС‚РѕРіРѕ Р¶Рµ СЃР»РѕРІР°, РІС…РѕРґСЏС‰РµРіРѕ РІ СЃРїРёСЃРѕРє СЃС‚РѕРї-СЃР»РѕРІ,
    // РІРѕР·РІСЂР°С‰Р°РµС‚ РїСѓСЃС‚РѕР№ СЂРµР·СѓР»СЊС‚Р°С‚
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        assert(server.FindTopDocuments("in"s).empty());
    }
}*/

//1. Р”РѕР±Р°РІР»РµРЅРёРµ РґРѕРєСѓРјРµРЅС‚РѕРІ. Р”РѕР±Р°РІР»РµРЅРЅС‹Р№ РґРѕРєСѓРјРµРЅС‚ РґРѕР»Р¶РµРЅ РЅР°С…РѕРґРёС‚СЊСЃСЏ РїРѕ РїРѕРёСЃРєРѕРІРѕРјСѓ Р·Р°РїСЂРѕСЃСѓ, РєРѕС‚РѕСЂС‹Р№ СЃРѕРґРµСЂР¶РёС‚ СЃР»РѕРІР° РёР· РґРѕРєСѓРјРµРЅС‚Р°.
void TestAddDocument() {
    SearchServer server;
    server.AddDocument(0, "Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s);
    assert(server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[0].id == 0);
    assert(server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[0].relevance == 0);
    assert(server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[0].rating == 2);
}

//2. РџРѕРґРґРµСЂР¶РєР° СЃС‚РѕРї-СЃР»РѕРІ. РЎС‚РѕРї-СЃР»РѕРІР° РёСЃРєР»СЋС‡Р°СЋС‚СЃСЏ РёР· С‚РµРєСЃС‚Р° РґРѕРєСѓРјРµРЅС‚РѕРІ
void TestExcludeStopWord() {
    SearchServer server;
    server.SetStopWords("Рё РІ РЅР°"s);
    server.AddDocument(0, "Р±РµР»С‹Р№ РєРѕС‚ Рё РІ РЅР° РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("Рё РІ РЅР°"s);
    assert(document.size() == 0);
}

//3. РџРѕРґРґРµСЂР¶РєР° РјРёРЅСѓСЃ-СЃР»РѕРІ. Р”РѕРєСѓРјРµРЅС‚С‹, СЃРѕРґРµСЂР¶Р°С‰РёРµ РјРёРЅСѓСЃ-СЃР»РѕРІР° РїРѕРёСЃРєРѕРІРѕРіРѕ Р·Р°РїСЂРѕСЃР°, РЅРµ РґРѕР»Р¶РЅС‹ РІРєР»СЋС‡Р°С‚СЊСЃСЏ РІ СЂРµР·СѓР»СЊС‚Р°С‚С‹ РїРѕРёСЃРєР°.
void TestMinusWord() {
    SearchServer server;
    //server.SetStopWords("Рё РІ РЅР°"s);
    server.AddDocument(0, "Р±РµР»С‹Р№ РєРѕС‚ РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "С‡РµСЂРЅС‹Р№ РїРµСЃ Р±СѓР»СЊРґРѕРі"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("Р±РµР»С‹Р№ РєРѕС‚ РјРѕРґРЅС‹Р№ -РѕС€РµР№РЅРёРє"s, 0);
    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("С‡РµСЂРЅС‹Р№ РїРµСЃ -Р±СѓР»СЊРґРѕРі"s, 1);
    int size_vec_1 = get<0>(match_data).size();
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_1 == 0);  // РїСЂРѕРІРµСЂСЏСЋ С‡С‚Рѕ СЃ РјРёРЅСѓСЃ СЃР»РѕРІРѕРј РЅРёС‡РµРіРѕ РЅРµ РїРѕРїР°Р»Рѕ
    assert(size_vec_2 == 0);  // РїСЂРѕРІРµСЂСЏСЋ С‡С‚Рѕ СЃ РјРёРЅСѓСЃ СЃР»РѕРІРѕРј РЅРёС‡РµРіРѕ РЅРµ РїРѕРїР°Р»Рѕ
}

// 4. РњР°С‚С‡РёРЅРі РґРѕРєСѓРјРµРЅС‚РѕРІ. РџСЂРё РјР°С‚С‡РёРЅРіРµ РґРѕРєСѓРјРµРЅС‚Р° РїРѕ РїРѕРёСЃРєРѕРІРѕРјСѓ Р·Р°РїСЂРѕСЃСѓ РґРѕР»Р¶РЅС‹ Р±С‹С‚СЊ РІРѕР·РІСЂР°С‰РµРЅС‹ РІСЃРµ СЃР»РѕРІР° РёР· РїРѕРёСЃРєРѕРІРѕРіРѕ Р·Р°РїСЂРѕСЃР°,
//РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‰РёРµ РІ РґРѕРєСѓРјРµРЅС‚Рµ. Р•СЃР»Рё РµСЃС‚СЊ СЃРѕРѕС‚РІРµС‚СЃС‚РІРёРµ С…РѕС‚СЏ Р±С‹ РїРѕ РѕРґРЅРѕРјСѓ РјРёРЅСѓСЃ-СЃР»РѕРІСѓ, РґРѕР»Р¶РµРЅ РІРѕР·РІСЂР°С‰Р°С‚СЊСЃСЏ РїСѓСЃС‚РѕР№ СЃРїРёСЃРѕРє СЃР»РѕРІ.
void TestMatchDoc() {
    SearchServer server;
    //server.SetStopWords("Рё РІ РЅР°"s);
    server.AddDocument(0, "Р±РµР»С‹Р№ РєРѕС‚ РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "С‡РµСЂРЅС‹Р№ РїРµСЃ Р±СѓР»СЊРґРѕРі"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("Р±РµР»С‹Р№ РєРѕС‚ РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s, 0);

    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("С‡РµСЂРЅС‹Р№ РїРµСЃ -Р±СѓР»СЊРґРѕРі"s, 1);
    string result_string = ""s;
    //for (auto el : get<0>(match_data)){cout << el<< " "s ;  result_string+=el; result_string+=" "s;}
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_2 == 0);  // РїСЂРѕРІРµСЂСЏСЋ С‡С‚Рѕ СЃ РјРёРЅСѓСЃ СЃР»РѕРІРѕРј РЅРёС‡РµРіРѕ РЅРµ РїРѕРїР°Р»Рѕ


}

// 5. РЎРѕСЂС‚РёСЂРѕРІРєР° РЅР°Р№РґРµРЅРЅС‹С… РґРѕРєСѓРјРµРЅС‚РѕРІ РїРѕ СЂРµР»РµРІР°РЅС‚РЅРѕСЃС‚Рё. Р’РѕР·РІСЂР°С‰Р°РµРјС‹Рµ РїСЂРё РїРѕРёСЃРєРµ РґРѕРєСѓРјРµРЅС‚РѕРІ СЂРµР·СѓР»СЊС‚Р°С‚С‹ РґРѕР»Р¶РЅС‹ Р±С‹С‚СЊ РѕС‚СЃРѕСЂС‚РёСЂРѕРІР°РЅС‹ РІ РїРѕСЂСЏРґРєРµ СѓР±С‹РІР°РЅРёСЏ СЂРµР»РµРІР°РЅС‚РЅРѕСЃС‚Рё.

void TestSortDocument() {
    SearchServer server;
    server.AddDocument(0, "Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, -7});
    vector<Document> document = server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s);
    assert(server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[0].relevance > server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[1].relevance);

}

//6. Р’С‹С‡РёСЃР»РµРЅРёРµ СЂРµР№С‚РёРЅРіР° РґРѕРєСѓРјРµРЅС‚РѕРІ. Р РµР№С‚РёРЅРі РґРѕР±Р°РІР»РµРЅРЅРѕРіРѕ РґРѕРєСѓРјРµРЅС‚Р° СЂР°РІРµРЅ СЃСЂРµРґРЅРµРјСѓ Р°СЂРёС„РјРµС‚РёС‡РµСЃРєРѕРјСѓ РѕС†РµРЅРѕРє РґРѕРєСѓРјРµРЅС‚Р°.
void TestRatingDocument() {
    SearchServer server;
    server.AddDocument(1, "Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 3});
    vector<Document> document = server.FindTopDocuments("Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s);
    assert(server.FindTopDocuments("Р±РµР»С‹Р№ РєРѕС‚ Рё РјРѕРґРЅС‹Р№ РѕС€РµР№РЅРёРє"s)[0].rating == 5);
}

//7. Р¤РёР»СЊС‚СЂР°С†РёСЏ СЂРµР·СѓР»СЊС‚Р°С‚РѕРІ РїРѕРёСЃРєР° СЃ РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµРј РїСЂРµРґРёРєР°С‚Р°, Р·Р°РґР°РІР°РµРјРѕРіРѕ РїРѕР»СЊР·РѕРІР°С‚РµР»РµРј.
void TestFiltrocument() {
    SearchServer server;
    server.AddDocument(0, "Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "Р±РµР»С‹Р№ РїРµСЃ СЃС‚Р°СЂРѕРјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(2, "РїРѕР»РѕСЃР°С‚С‹Р№ РїРµСЃ РІ РєСЂР°РїРёРЅРєСѓ СЃРѕ С€РЅСѓСЂРєР°РјРё РЅР° Р±РѕС‚РёРЅРєР°С…"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(3, "С‡РµСЂРЅС‹Р№ РїРµСЃ СЃ РїСЏС‚РЅРѕРј Р±РµР»С‹Рј"s,        DocumentStatus::IRRELEVANT, {8, 4});
    server.AddDocument(4, "Р¶РµР»С‚С‹Р№ РєРѕС‚ СЃ СЃРµСЂС‹Рј РѕС€РµР№РЅРёРєРѕРј"s,        DocumentStatus::BANNED, {8, 8});
    vector<Document> document = server.FindTopDocuments("РїСѓС€РёСЃС‚С‹Р№ СѓС…РѕР¶РµРЅРЅС‹Р№ РїРµСЃ"s, [](int document_id, DocumentStatus status __attribute__((unused)), int rating __attribute__((unused))) { return document_id % 2 == 0; });

    assert(document.size() == 2);
}

//8 РџРѕРёСЃРє РґРѕРєСѓРјРµРЅС‚РѕРІ, РёРјРµСЋС‰РёС… Р·Р°РґР°РЅРЅС‹Р№ СЃС‚Р°С‚СѓСЃ.
void TestFiltrSTATDocument() {
    SearchServer server;
    server.AddDocument(0, "Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "Р±РµР»С‹Р№ РїРµСЃ СЃС‚Р°СЂРѕРјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(2, "РїРѕР»РѕСЃР°С‚С‹Р№ РїРµСЃ РІ РєСЂР°РїРёРЅРєСѓ СЃРѕ С€РЅСѓСЂРєР°РјРё РЅР° Р±РѕС‚РёРЅРєР°С…"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(3, "С‡РµСЂРЅС‹Р№ РїРµСЃ СЃ РїСЏС‚РЅРѕРј Р±РµР»С‹Рј"s,        DocumentStatus::IRRELEVANT, {8, 4});
    server.AddDocument(4, "Р¶РµР»С‚С‹Р№ РєРѕС‚ СЃ СЃРµСЂС‹Рј РѕС€РµР№РЅРёРєРѕРј"s,        DocumentStatus::BANNED, {8, 8});
    vector<Document> document = server.FindTopDocuments("РїСѓС€РёСЃС‚С‹Р№ СѓС…РѕР¶РµРЅРЅС‹Р№ РїРµСЃ"s, [](int document_id __attribute__((unused)), DocumentStatus status, int rating __attribute__((unused))) { return status == DocumentStatus::IRRELEVANT; });
    vector<Document> document_2 = server.FindTopDocuments("РїСѓС€РёСЃС‚С‹Р№ СѓС…РѕР¶РµРЅРЅС‹Р№ РїРµСЃ"s, [](int document_id __attribute__((unused)), DocumentStatus status, int rating __attribute__((unused))) { return status == DocumentStatus::ACTUAL; });   // РЅРёС‡РµРіРѕ РЅРµ РЅР°С…РѕРґРёС‚
    assert(document.size() == 1 && document_2.size()==3);
}


//9 РљРѕСЂСЂРµРєС‚РЅРѕРµ РІС‹С‡РёСЃР»РµРЅРёРµ СЂРµР»РµРІР°РЅС‚РЅРѕСЃС‚Рё РЅР°Р№РґРµРЅРЅС‹С… РґРѕРєСѓРјРµРЅС‚РѕРІ.
// Р’РЅРёРјР°РЅРёРµ СЂРµР»РµРІР°РЅС‚РЅРѕСЃС‚СЊ РєР°Рє С‚Рѕ РЅРµ С‚Р°Рє СЃС‡РёС‚Р°С‚РµС‚????
void TestRelevanceDocument() {
    SearchServer server;
    server.AddDocument(0, "Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "Р±РµР»С‹Р№ РїРµСЃ СЃС‚Р°СЂРѕРјРѕРґРЅС‹Р№"s,        DocumentStatus::ACTUAL, {8, 2});

    vector<Document> document = server.FindTopDocuments("Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s);
    assert( (server.FindTopDocuments("Р±РµР»С‹Р№ РїРµСЃ РјРѕРґРЅС‹Р№"s)[0].relevance - 0.231049 ) < 0.00001 );
}




/*
Р Р°Р·РјРµСЃС‚РёС‚Рµ РєРѕРґ РѕСЃС‚Р°Р»СЊРЅС‹С… С‚РµСЃС‚РѕРІ Р·РґРµСЃСЊ
*/

// Р¤СѓРЅРєС†РёСЏ TestSearchServer СЏРІР»СЏРµС‚СЃСЏ С‚РѕС‡РєРѕР№ РІС…РѕРґР° РґР»СЏ Р·Р°РїСѓСЃРєР° С‚РµСЃС‚РѕРІ
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestExcludeStopWord();
    TestMinusWord();
    TestMatchDoc();
    TestSortDocument();
    TestRatingDocument();
    TestFiltrSTATDocument();

    TestFiltrocument();
    TestRelevanceDocument();

    // РќРµ Р·Р°Р±СѓРґСЊС‚Рµ РІС‹Р·С‹РІР°С‚СЊ РѕСЃС‚Р°Р»СЊРЅС‹Рµ С‚РµСЃС‚С‹ Р·РґРµСЃСЊ
}

// --------- РћРєРѕРЅС‡Р°РЅРёРµ РјРѕРґСѓР»СЊРЅС‹С… С‚РµСЃС‚РѕРІ РїРѕРёСЃРєРѕРІРѕР№ СЃРёСЃС‚РµРјС‹ -----------
