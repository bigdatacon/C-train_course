#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */


RequestHandler::RequestHandler( TransportCatalogue& tc,  /*renderer::*/ MapRenderer& renderer) : tc_(tc), renderer_(renderer) {}

// ���������� ���������� � �������� (������ Bus)
//std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

// ���������� ��������, ���������� �����
//const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

// ���� ����� ����� ����� � ��������� ����� ��������� �������
void RequestHandler::RenderMap()  {
    
    renderer_.DrawRoute(tc_);
};


void RequestHandler::RenderMapByString() {
    std::string str = renderer_.DrawRouteGetDoc(tc_);
   
  

    //renderer_.Render(svg::LoadJSON(str), cout )



   
};
