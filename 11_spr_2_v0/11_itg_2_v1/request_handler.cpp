#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */


    std::deque<transport_catalogue::Bus>  RequestToTc::GetBuses() {
        return TransportCatalogue::GetBuses();

    };
    std::deque<transport_catalogue::Stop>  RequestToTc::GetStops() {
        return TransportCatalogue::GetStops();
    };
