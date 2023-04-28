#pragma once

#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"
/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * � �������� ��������� ��� ���� ���������� ��������� �� ���� ������ ����������� ��������.
 * �� ������ ����������� ��������� �������� ��������, ������� ������� ���.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

 // ����� RequestHandler ������ ���� ������, ����������� �������������� JSON reader-�
 // � ������� ������������ ����������.
 // ��. ������� �������������� �����: https://ru.wikipedia.org/wiki/�����_(������_��������������)
 /*
 class RequestHandler {
 public:
     // MapRenderer ����������� � ��������� ����� ��������� �������
     RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);
     // ���������� ���������� � �������� (������ Bus)
     std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;
     // ���������� ��������, ���������� �����
     const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
     // ���� ����� ����� ����� � ��������� ����� ��������� �������
     svg::Document RenderMap() const;
 private:
     // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
     const TransportCatalogue& db_;
     const renderer::MapRenderer& renderer_;
 };
 */

class RequestHandler {
public:
    // MapRenderer ����������� � ��������� ����� ��������� �������
    RequestHandler(const transport_catalogue::TransportCatalogue& tc, const /*renderer::*/ MapRenderer& renderer);

    // ���������� ���������� � �������� (������ Bus)
    //std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // ���������� ��������, ���������� �����
    //const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // ���� ����� ����� ����� � ��������� ����� ��������� �������
    svg::Document RenderMap() const ;

private:
    // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
    const transport_catalogue::TransportCatalogue& tc_;
    const MapRenderer& renderer_;
};