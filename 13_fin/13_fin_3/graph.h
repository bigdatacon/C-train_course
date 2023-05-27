#pragma once

#include "ranges.h"
#include "transport_catalogue.h"

#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace graph {

    using VertexId = size_t;
    using EdgeId = size_t;

    template <typename Weight>
    struct Edge {
        VertexId from;
        VertexId to;
        Weight weight;
    };

    template <typename Weight>
    class DirectedWeightedGraph {
    private:
        using IncidenceList = std::vector<EdgeId>;
        using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

    public:
        DirectedWeightedGraph() = default;
        explicit DirectedWeightedGraph(size_t vertex_count);
        EdgeId AddEdge(const Edge<Weight>& edge);

        size_t GetVertexCount() const;
        size_t GetEdgeCount() const;
        const Edge<Weight>& GetEdge(EdgeId edge_id) const;
        IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

    private:
        std::vector<Edge<Weight>> edges_;
        std::vector<IncidenceList> incidence_lists_;
    };

    template <typename Weight>
    DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
        : incidence_lists_(vertex_count) {
    }

    template <typename Weight>
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
        edges_.push_back(edge);
        const EdgeId id = edges_.size() - 1;
        incidence_lists_.at(edge.from).push_back(id);
        return id;
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
        return incidence_lists_.size();
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
        return edges_.size();
    }

    template <typename Weight>
    const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const {
        return edges_.at(edge_id);
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
        DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
        return ranges::AsRange(incidence_lists_.at(vertex));
    }



    template <typename Weight>
    void AddKnots(DirectedWeightedGraph<Weight>& graph, transport_catalogue::TransportCatalogue tc) {
        auto stops_distance_time_ = tc.GetstopsDistanceTime();
        std::set<domain::Stop, StopComparer> stop_set = tc.GetStopSet();
        for (auto it = stops_distance_time_.begin(); it != stops_distance_time_.end(); ++it) {
            //std::cout << "Key: " << it->first.first << " " << it->first.second
              //  << ", Value: " << it->second << std::endl;
            //int num_vertex_1 = std::distance(stops_distance_time_.begin(), it) * 2;
            //int num_vertex_2 = std::distance(stops_distance_time_.begin(), it) * 2 + 1;

            size_t num_vertex_1 = tc.GetVertexIds(stop_set, it->first.first);
            size_t num_vertex_1 = tc.GetVertexIds(stop_set, it->first.second);


            Edge<int> edge1 = { num_vertex_1, num_vertex_1, tc.GetWaitTime() };
            Edge<int> edge2 = { num_vertex_1, num_vertex_2, it->second };  
            graph.AddEdge(edge1);  // Добавляем ребра в граф
            graph.AddEdge(edge2);


        }
    }





}  // namespace graph  