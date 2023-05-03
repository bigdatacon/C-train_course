#include "request_handler.h"
#include <sstream>
#include <iostream>
/*
 * «десь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * ≈сли вы затрудн€етесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
using namespace transport_catalogue;

RequestHandler::RequestHandler( TransportCatalogue& tc,  /*renderer::*/ MapRenderer& renderer) : tc_(tc), renderer_(renderer) {}

// ¬озвращает информацию о маршруте (запрос Bus)
//std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

// ¬озвращает маршруты, проход€щие через
//const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

// Ётот метод будет нужен в следующей части итогового проекта
/*void RequestHandler::RenderMap() {
    
    renderer_.DrawRoute(tc_);
};
*/

void RequestHandler::RenderMapByString() {
    std::string str = renderer_.DrawRouteGetDoc(tc_);
   
    std::stringstream ss;
    ss << str;
    std::string result = ss.str();
    std::cout << result << std::endl; 




   
};
