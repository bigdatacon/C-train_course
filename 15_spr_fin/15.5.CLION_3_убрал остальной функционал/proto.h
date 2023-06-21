#pragma once
syntax = "proto3";

package transport_catalogue_protobuf;

message Stop{
    uint32 id = 1;
    string name = 2;
    double latitude = 3;
    double longitude = 4;
}

message Bus{
    string name = 1;
    repeated uint32 stops = 2;
    string is_roundtrip = 3;
    uint32 route_length = 4;
}

message Distance{
    uint32 start = 1;
    uint32 end = 2;
    uint32 distance = 3;     
}

message TransportCatalogue{
    repeated Stop stops = 1;
    repeated Bus buses = 2;
    repeated Distance distances = 3;
}