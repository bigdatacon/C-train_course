#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */


RequestHandler::RequestHandler(const TransportCatalogue& tc, const /*renderer::*/ MapRenderer& renderer) : tc_(tc), renderer_(renderer) {}

// ���������� ���������� � �������� (������ Bus)
//std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

// ���������� ��������, ���������� �����
//const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

// ���� ����� ����� ����� � ��������� ����� ��������� �������
svg::Document RequestHandler::RenderMap() const {
    
    return renderer_.DrawRoute(tc_);
};

