#include "route_planner.h"

#include <algorithm>

#include <iostream>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // std::cout << "start_x: " << start_x << " start_y: " << start_y << std::endl;
    // std::cout << "end_x: " << end_x << " end_y: " << end_y << std::endl;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    
    // RouteModel::Node some_node = m_Model.FindClosestNode(start_x, start_y);    
    // std::cout << "some_node.x: " << some_node.x << std::endl;
    // RouteModel::Node *some_node_p;
    // some_node_p = &m_Model.FindClosestNode(start_x, start_y);
    // std::cout << "some_node_p.x: " << some_node_p->x << std::endl;

    start_node = &m_Model.FindClosestNode(start_x, start_y);
    std::cout << "assigned start_node (x: " << start_node->x << ", y: " << start_node->y << ")" << std::endl;

    end_node = &m_Model.FindClosestNode(end_x, end_y);
    std::cout << "assigned end_node (x: " << end_node->x << ", y: " << end_node->y << ")" << std::endl;
}

// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    float result = node->distance(*end_node);
    // std::cout << "node.x: " << node->x << std::endl;
    // std::cout << "node.y: " << node->y << std::endl;
    return result;
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

// TODO: determine if visited neighbors are actually being filtered out???

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for(RouteModel::Node *neighbor : current_node->neighbors) {
        if(!neighbor->visited) {
            neighbor->parent = current_node;
            neighbor->h_value = CalculateHValue(neighbor);
            neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
            neighbor->visited = true;
            open_list.push_back(neighbor);
        } else {
            std::cout << "skipping (" << neighbor->x << ", " << neighbor->y << ")" << std::endl;
        }
    }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

// bool compareHandG(RouteModel::Node* n1, RouteModel::Node* n2){
//     return (n1->h_value + n1->g_value < n2->h_value + n2->g_value);
// }

bool compareHandG(RouteModel::Node* n1, RouteModel::Node* n2){
    return (n1->h_value + n1->g_value > n2->h_value + n2->g_value);
}

RouteModel::Node *RoutePlanner::NextNode() {
    if(!open_list.empty()) {
        std::sort(open_list.begin(), open_list.end(), compareHandG);
        RouteModel::Node* nextn = open_list.back();
        open_list.pop_back();
        return nextn;
    }
    return NULL;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    std::cout << "ConstructFinalPath()..." << std::endl;
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    RouteModel::Node *previous_node = nullptr;
    int i = 0;
    while(current_node) {

        // std::cout << "about push node i=" << i << std::endl;

        path_found.push_back(*current_node);
        // std::cout << "about update distance for i=" << i << std::endl;
        if(previous_node) {
            distance += current_node->distance(*previous_node);
        }
        std::cout << "node " << i << " x: " << current_node->x << " y: " << current_node->y << std::endl;
        std::cout << "current_node->parent: " << current_node->parent << std::endl;
        i++;
        // std::cout << "assign current node to it's parent" << std::endl;
        previous_node = current_node;
        current_node = current_node->parent;
    }

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    std::reverse(path_found.begin(), path_found.end());
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    // TODO: Implement your solution here.

    // set start node properties
    start_node->g_value = 0;
    start_node->h_value = CalculateHValue(start_node);
    start_node->parent = nullptr;
    start_node->visited = true;
    open_list.push_back(start_node);

    int i = 0;

    while(!open_list.empty()) {
        i++;
        std::cout << "i: " << i << " open list size: " << open_list.size() << std::endl;
        for (auto n : open_list ) {
            std::cout << "node (" << n->x << ", " << n->y << ")  h: " << n->h_value << "  g: " << n->g_value << "  f: " << n->h_value + n->g_value << std::endl;
        }
        
        current_node = NextNode();
        std::cout << "current_node (x: " << current_node->x << ", y: " << current_node->y << ")" << std::endl; 
        if(current_node->x == end_node->x && current_node->y == end_node->y) {
            break;
        } else {
            AddNeighbors(current_node);
        }
    }
}