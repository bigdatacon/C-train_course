#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */


    std::deque<transport_catalogue::Bus>  RequestToTc::GetBuses() {
        return TransportCatalogue::GetBuses();

    };
    std::deque<transport_catalogue::Stop>  RequestToTc::GetStops() {
        return TransportCatalogue::GetStops();
    };
