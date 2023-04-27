#pragma once

#include "transport_catalogue.h"
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


class RequestToTc : public transport_catalogue::TransportCatalogue {
public:
    virtual std::deque<transport_catalogue::Bus> GetBuses() ; 
    virtual std::deque<transport_catalogue::Stop> GetStops();
    virtual const transport_catalogue::Stop* FindStop(std::string_view stop) ;
};