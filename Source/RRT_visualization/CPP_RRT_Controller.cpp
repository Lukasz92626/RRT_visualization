// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_RRT_Controller.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>
#include <deque>

#define MIN_DIST 5
#define MAX_DIST 10
#define MAX_ITER 10000

#define START_X 0
#define START_Y 0
#define START_Z 0

#define FINISH_X 50
#define FINISH_Y 50
#define FINISH_Z 50

#define BOARD_X 50
#define BOARD_Y 50
#define BOARD_Z 50

#define MOVEMENT 100

using namespace std;

class Point {
private:
	double x; //x coordinate
	double y; //y coordinate
	double z; //z coordinate
public:
	Point() : x(0.0), y(0.0), z(0.0) {

	}
	Point(double nx, double ny, double nz) : x(nx), y(ny), z(nz) {
	}
	~Point() {
	}

	double get_x() const { return x; }
	double get_y() const { return y; }
	double get_z() const { return z; }

	void set_x(double nx) {
		x = nx;
	}
	void set_y(double ny) {
		y = ny;
	}
	void set_z(double nz) {
		z = nz;
	}

	double get_index(int i) const {
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return x;
	}

	void set_cordinates(double nx, double ny, double nz) {
		x = nx;
		y = ny;
		z = nz;
	}

	void print_values() {
		UE_LOG(LogTemp, Warning, TEXT("( %f, %f, %f)"), x, y, z);
	}
};

class Node {
private:
	double x; //x coordinate
	double y; //y coordinate
	double z; //z coordinate
	int color; //specify color of node
public:
	Node* parent; //pointer to parent node
	vector<Node*> children; //vector of pointers to children nodes
	Node() : x(0.0), y(0.0), z(0.0), color(0) {
		parent = nullptr;
	}
	Node(double nx, double ny, double nz) : x(nx), y(ny), z(nz), color(0) {
		parent = nullptr;
	}
	Node(double nx, double ny, double nz, Node* nparent) : x(nx), y(ny), z(nz), color(0) {
		parent = nparent;
	}
	~Node() {
		for (int i = 0; i < children.size(); ++i) {
			delete children[i];
		}
	}

	double get_x() const { return x; }
	double get_y() const { return y; }
	double get_z() const { return z; }
	int get_color() const { return color; }

	void set_x(double nx) {
		x = nx;
	}
	void set_y(double ny) {
		y = ny;
	}
	void set_z(double nz) {
		z = nz;
	}
	void set_color(int ncolor) {
		if (ncolor >= 0 and ncolor <= 3)
			color = ncolor;
	}

	void set_cordinates(double nx, double ny, double nz) {
		x = nx;
		y = ny;
		z = nz;
	}

	double get_index(int i) const {
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return x;
	}

	void set_parent(Node* nparent) {
		parent = nparent;
	}

	void add_children(Node* nchild) {
		children.push_back(nchild);
	}

	void print_values() {
		UE_LOG(LogTemp, Warning, TEXT("(%f, %f, %f)"), x, y, z);
	}
};

class Cuboid {
public:
	//First point must have higher or equal cordinate than second point.
	Point first; //closer point to (0,0,0)
	Point second; //further point from (0,0,0)

	Cuboid() {
	}
	Cuboid(double nx_1, double ny_1, double nz_1, double nx_2, double ny_2, double nz_2) {
		first.set_cordinates(nx_1, ny_1, nz_1);
		second.set_cordinates(nx_2, ny_2, nz_2);
	}

	double get_edge_x() {
		return first.get_x() - second.get_x();
	}
	double get_edge_y() {
		return first.get_y() - second.get_y();
	}
	double get_edge_z() {
		return first.get_z() - second.get_z();
	}

	bool node_inside(Node* cur_node) {
		if ((first.get_x() <= cur_node->get_x() && first.get_y() <= cur_node->get_y() && first.get_z() <= cur_node->get_z()) && (second.get_x() > cur_node->get_x() && second.get_y() > cur_node->get_y() && second.get_z() > cur_node->get_z())) {
			return true;
		}
		return false;
	}

	bool path_inside(Node* nearest, Node* new_node) {
		double t_min = 0.0;
		double t_max = 1.0;
		for (int i = 0; i < 3; ++i) {
			double box_min = min(first.get_index(i), second.get_index(i));
			double box_max = max(first.get_index(i), second.get_index(i));
			double direction = new_node->get_index(i) - nearest->get_index(i);
			if (abs(direction) < 1e-8) {
				if (!(box_min <= nearest->get_index(i) && nearest->get_index(i) <= box_max))
					return false;
			}
			else {
				double t1 = (box_min - nearest->get_index(i)) / direction;
				double t2 = (box_max - nearest->get_index(i)) / direction;
				double t_enter = min(t1, t2);
				double t_exit = max(t1, t2);
				t_min = max(t_min, t_enter);
				t_max = min(t_max, t_exit);
				if (t_min > t_max)
					return false;
			}
		}
		return true;
	}
};

class RRT {
public:
	Node* start; //start node of tree
	Node* finish; //finish node of tree
	const int min_distance; //minimum distance to exist node
	const int max_distance; //maximum distance to exist node
	const int board_size_x; //size of board along x-axis
	const int board_size_y; //size of board along y-axis
	const int board_size_z; //size of board along z-axis
	const int max_iterations; //maximum number of iterations
	deque<Node*> planned_path; //deque with planned path using RRT algorithm
	vector<Cuboid> cuboids; //vector with cuboids obstacles

	RRT() : min_distance(MIN_DIST), max_distance(MAX_DIST), board_size_x(BOARD_X), board_size_y(BOARD_Y), board_size_z(BOARD_Z), max_iterations(MAX_ITER) {
		start = new Node(START_X, START_Y, START_Z);
		finish = new Node(FINISH_X, FINISH_Y, FINISH_Z);
	}
	virtual ~RRT() {
		delete start;
		planned_path.clear();
		cuboids.clear();
	}

	double distance(Node* node_1, Node* node_2) {
		return sqrt(pow(node_1->get_x() - node_2->get_x(), 2) + pow(node_1->get_y() - node_2->get_y(), 2) + pow(node_1->get_z() - node_2->get_z(), 2));
	}

	Node* random_point() {
		int x = FMath::RandRange(0, board_size_x); //x coordinate
		int y = FMath::RandRange(0, board_size_y); //y coordinate
		int z = FMath::RandRange(0, board_size_z); //z coordinate
		return new Node(x, y, z);
	}

	//Node* find_nearest_node(Node* new_node) {
	//	Node* nearest = start; //nearest node to current node
	//	double dist = distance(start, new_node); //distance from nearest node to new_node
	//	queue<Node*> node_queue; //queue with nodes to verify
	//	node_queue.push(start);
	//	while (!node_queue.empty()) {
	//		Node* current = node_queue.front(); //current node
	//		for (int i = 0; i < current->children.size(); ++i) {
	//			node_queue.push(current->children[i]);
	//		}
	//		double current_dist = distance(current, new_node); //distance from new_node to current node
	//		if (current_dist < dist) {
	//			dist = current_dist;
	//			nearest = current;
	//		}
	//		node_queue.pop();
	//	}
	//	return nearest;
	//}

	Node* find_nearest_node(Node* new_node, Node* start_node) {
		Node* nearest = start_node; //nearest node to current node
		double dist = distance(start_node, new_node); //distance from nearest node to new_node
		queue<Node*> node_queue; //queue with nodes to verify
		node_queue.push(start_node);
		while (!node_queue.empty()) {
			Node* current = node_queue.front(); //current node
			for (int i = 0; i < current->children.size(); ++i) {
				node_queue.push(current->children[i]);
			}
			double current_dist = distance(current, new_node); //distance from new_node to current node
			if (current_dist < dist) {
				dist = current_dist;
				nearest = current;
			}
			node_queue.pop();
		}
		return nearest;
	}

	bool verify_finish(Node* new_node, Node* finish_node) {
		if (distance(finish_node, new_node) <= min_distance + 1)
			return true;
		return false;
	}

	void add_node_to_tree(Node* new_node, Node* parent) {
		parent->add_children(new_node);
		new_node->parent = parent;
	}

	void print_path() {
		for (int i = 0; i < planned_path.size(); ++i) {
			UE_LOG(LogTemp, Warning, TEXT("%d: (%f, %f, %f)"),
				i + 1,
				planned_path[i]->get_x(),
				planned_path[i]->get_y(),
				planned_path[i]->get_z());
		}
	}

	void print_cuboids() {
		int size = cuboids.size(); //size of cuboids vector
		for (int i = 0; i < size; ++i) {
			UE_LOG(LogTemp, Warning, TEXT("%d"), i + 1);
			cuboids[i].first.print_values();
			cuboids[i].second.print_values();
		}
	}

	void print_begin_informations(string algorithm) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(algorithm.c_str()));
		UE_LOG(LogTemp, Warning, TEXT("Start: "));
		start->print_values();
		UE_LOG(LogTemp, Warning, TEXT("Finish: "));
		finish->print_values();
		UE_LOG(LogTemp, Warning, TEXT("Cuboids: "));
	}

	void print_end_informations(int iterator) {
		if (iterator >= max_iterations) {
			UE_LOG(LogTemp, Warning, TEXT("Max iterations. Path not found."));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Found path. Interations: %d"), iterator);
			UE_LOG(LogTemp, Warning, TEXT("Path:"));
			save_path();
			print_path();
		}
	}

	bool check_collisions(Node* nearest, Node* new_node) {
		for (Cuboid cub : cuboids) {
			if (cub.node_inside(new_node) || cub.path_inside(nearest, new_node))
				return false;
		}
		return true;
	}

	double path_lenght() {
		double lenght = 0; //lenght of path
		for (int i = 0; i < planned_path.size() - 1; ++i) {
			lenght += distance(planned_path[i], planned_path[i + 1]);
		}
		return lenght;
	}

	void move_node_closer_other(Node* node, Node* other_node) {
		double dx = node->get_x() - other_node->get_x(); //x direction vector component
		double dy = node->get_y() - other_node->get_y(); //y direction vector component
		double dz = node->get_z() - other_node->get_z(); //z direction vector component
		double dist_near = sqrt(dx * dx + dy * dy + dz * dz);
		node->set_x(other_node->get_x() + (dx / dist_near) * MAX_DIST);
		node->set_y(other_node->get_y() + (dy / dist_near) * MAX_DIST);
		node->set_z(other_node->get_z() + (dz / dist_near) * MAX_DIST);
	}

	virtual void save_path() {
		if (!finish)
			return;
		Node* current = finish; //current node
		planned_path.clear();
		while (current->parent != nullptr) {
			planned_path.push_front(current);
			current->set_color(1); //color of path
			current = current->parent;
		}
		planned_path.push_front(current);
		start->set_color(2);
		finish->set_color(3);
	}

	virtual void execute_rrt() {
		bool is_finish = false; //contains bool information about distance to finish
		int i = 0;
		print_begin_informations("Execute RRT.");
		print_cuboids();
		do {
			Node* new_node = nullptr; //new node
			Node* nearest = nullptr; //nearest node to new node
			double dist = 0; //distance new node to nearest
			do {
				new_node = random_point();
				nearest = find_nearest_node(new_node, start);
				dist = distance(new_node, nearest);
			} while (dist < min_distance || !check_collisions(nearest, new_node));

			if (dist > max_distance) {
				move_node_closer_other(new_node, nearest);
			}
			is_finish = verify_finish(new_node, finish);
			if (is_finish) {
				add_node_to_tree(finish, nearest);
				finish->set_parent(nearest);
			}
			else
				add_node_to_tree(new_node, nearest);
			++i;
		} while (!is_finish && i <= max_iterations);
		print_end_informations(i);
	}
};

class Bidirectional_RRT : public RRT {
public:
	Node* connect_node; //node which connect trees
	Node* connect_node_1; //first node which connect trees
	Node* connect_node_2; //second node which connect trees

	~Bidirectional_RRT() {
		delete finish;
	}

	Node* find_connect_node(Node* new_node, Node* current_finish) {
		Node* nearest_finish_tree = nullptr; //nearest node to new node from current finish tree
		bool is_connected = false; //contains bool information connection between trees
		nearest_finish_tree = find_nearest_node(new_node, current_finish);
		is_connected = verify_finish(new_node, nearest_finish_tree);
		if (is_connected) {
			connect_node_1 = new_node;
			connect_node_2 = nearest_finish_tree;
			return new_node;
		}
		return nullptr;
	}

	Node* find_new_node(Node* current_start, Node* current_finish) {
		//bool is_connected = false; //contains bool information connection between trees
		Node* new_node = nullptr; //new node
		Node* nearest = nullptr; //nearest node to new node
		//Node* nearest_finish_tree = nullptr; //nearest node to new node from current finish tree
		double dist = 0; //distance new node to nearest
		do {
			new_node = random_point();
			nearest = find_nearest_node(new_node, current_start);
			dist = distance(new_node, nearest);
		} while (dist < min_distance || !check_collisions(nearest, new_node));

		if (dist > max_distance) {
			double dx = new_node->get_x() - nearest->get_x(); //x direction vector component
			double dy = new_node->get_y() - nearest->get_y(); //y direction vector component
			double dz = new_node->get_z() - nearest->get_z(); //z direction vector component
			double dist_near = sqrt(dx * dx + dy * dy + dz * dz);
			new_node->set_x(nearest->get_x() + (dx / dist_near) * MAX_DIST);
			new_node->set_y(nearest->get_y() + (dy / dist_near) * MAX_DIST);
			new_node->set_z(nearest->get_z() + (dz / dist_near) * MAX_DIST);
			new_node->print_values();
		}
		/*is_connected = verify_finish(new_node, current_finish);
		if (is_connected) {
			add_node_to_tree(current_finish, nearest);
			current_finish->set_parent(nearest);
			return is_connected;
		}
		else
			add_node_to_tree(new_node, nearest);*/

		add_node_to_tree(new_node, nearest);

		//nearest_finish_tree = find_nearest_node(new_node, current_finish);
		////dist = distance(new_node, nearest_finish_tree);
		//is_connected = verify_finish(new_node, nearest_finish_tree);
		//if (is_connected) {
		//	connect_node_1 = new_node;
		//	connect_node_2 = nearest_finish_tree;
		//	//new_node->add_children(nearest_finish_tree);
		//	//nearest_finish_tree->add_children(new_node);

		//	//add_node_to_tree(new_node, nearest_finish_tree);
		//	//add_node_to_tree(new_node, nearest);
		//	//add_node_to_tree(nearest_finish_tree, new_node);
		//	return new_node;
		//}
		//return nullptr;
		return find_connect_node(new_node, current_finish);
	}

	void save_path() {
		if (!connect_node)
			return;
		Node* current = connect_node_1; //current node
		planned_path.clear();
		while (current->parent != nullptr) {
			planned_path.push_front(current);
			current->set_color(1); //color of path
			current = current->parent;
		}
		planned_path.push_front(current);

		current = connect_node_2;
		while (current->parent != nullptr) {
			planned_path.push_back(current);
			current->set_color(1); //color of path
			current = current->parent;
		}
		planned_path.push_back(current);

		start->set_color(2);
		finish->set_color(3);
		connect_node_1->set_color(4);
	}

	void execute_rrt() {
		connect_node = nullptr;
		Node* new_node = nullptr; //new node in tree
		int i = 0;
		print_begin_informations("Execute Bidirectional RRT.");
		do {
			connect_node = find_new_node(start, finish);
			if (connect_node)
				break;
			connect_node = find_new_node(finish, start);
		} while (!connect_node && i <= max_iterations);
		print_end_informations(i);
	}
};

class RRT_Connect : public Bidirectional_RRT {
public:
	Node* extend(Node* current_start, Node* target) {
		Node* nearest = find_nearest_node(target, current_start); //nearest node to finish node
		Node* new_node = new Node(target->get_x(), target->get_y(), target->get_z()); //new node in tree
		move_node_closer_other(new_node, nearest);
		if (check_collisions(nearest, new_node)) {
			add_node_to_tree(new_node, nearest);
			return new_node;
		}
		delete new_node;
		return nullptr;
	}

	void execute_rrt() {
		connect_node = nullptr;
		Node* new_node = nullptr; //new node in tree
		int i = 0;
		print_begin_informations("Execute RRT Connect.");
		do {
			new_node = extend(start, finish);
			if (new_node)
				connect_node = find_connect_node(new_node, finish);
			else
				connect_node = find_new_node(start, finish);

			if (connect_node)
				break;

			new_node = extend(finish, start);
			if (new_node)
				connect_node = find_connect_node(new_node, start);
			else
				connect_node = find_new_node(finish, start);
			++i;
		} while (!connect_node && i <= max_iterations);
		print_end_informations(i);
	}
};

double ACPP_RRT_Controller::reduce_random_edge(double edge, double cordinate, double board_size) {
	if (cordinate + edge >= board_size)
		edge = board_size - cordinate;
	return edge;
}

void ACPP_RRT_Controller::start_RRT() {
	delete rrt_class;
	if (algorithm == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Create RRT class."));
		rrt_class = new RRT;
	}
	else if (algorithm == 1) {
		UE_LOG(LogTemp, Warning, TEXT("Create Bidirectional_RRT class."));
		rrt_class = new Bidirectional_RRT;
	}
	else if (algorithm == 2) {
		UE_LOG(LogTemp, Warning, TEXT("Create RRT_Connect class."));
		rrt_class = new RRT_Connect;
	}

	if (board_type == 0) {
		rrt_class->cuboids.push_back(Cuboid(3, 5, 4, 4, 7, 8));
		rrt_class->cuboids.push_back(Cuboid(1, 40, 1, 3, 45, 3));
		rrt_class->cuboids.push_back(Cuboid(30, 5, 4, 34, 10, 7));
		rrt_class->cuboids.push_back(Cuboid(2, 3, 38, 6, 7, 43));
		rrt_class->cuboids.push_back(Cuboid(15, 10, 12, 19, 15, 17));
		rrt_class->cuboids.push_back(Cuboid(19, 22, 5, 24, 27, 10));
		rrt_class->cuboids.push_back(Cuboid(5, 4, 3, 10, 9, 7));
		rrt_class->cuboids.push_back(Cuboid(35, 32, 40, 38, 34, 42));
		rrt_class->cuboids.push_back(Cuboid(5, 23, 27, 43, 27, 33)); //dlugi poziomy
		rrt_class->cuboids.push_back(Cuboid(40, 37, 3, 45, 33, 36)); //dlugi pionowy
		rrt_class->cuboids.push_back(Cuboid(25, 4, 27, 30, 47, 33)); //dlugi poziomy
		rrt_class->cuboids.push_back(Cuboid(37, 38, 40, 43, 47, 45)); //sredni
		rrt_class->cuboids.push_back(Cuboid(37, 8, 7, 43, 13, 43)); //dlugi pionowy
	}
	else if (board_type == 1) {
		int obstacles = 20; //number of obstacles on board
		double max_x = rrt_class->board_size_x / 3.0; //max size cuboid on x coordinate
		double max_y = rrt_class->board_size_y / 3.0; //max size cuboid on y coordinate
		double max_z = rrt_class->board_size_z / 3.0; //max size cuboid on z coordinate
		for (int i = 0; i < obstacles; ++i) {
			double x = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9); //random x cordinate of cuboid obstacle
			double y = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9); //random y cordinate of cuboid obstacle
			double z = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9); //random z cordinate of cuboid obstacle
			//double edge_x = FMath::FRandRange(1.0, max_x); //random edge parallel to x axis of cuboid obstacle
			//double edge_y = FMath::FRandRange(1.0, max_y); //random edge parallel to y axis of cuboid obstacle
			//double edge_z = FMath::FRandRange(1.0, max_z); //random edge parallel to z axis of cuboid obstacle
			double edge_x = reduce_random_edge(FMath::FRandRange(1.0, max_x), x, rrt_class->board_size_x); //random edge parallel to x axis of cuboid obstacle
			double edge_y = reduce_random_edge(FMath::FRandRange(1.0, max_y), y, rrt_class->board_size_y); //random edge parallel to y axis of cuboid obstacle
			double edge_z = reduce_random_edge(FMath::FRandRange(1.0, max_z), z, rrt_class->board_size_z); //random edge parallel to z axis of cuboid obstacle
			rrt_class->cuboids.push_back(Cuboid(x, y, z, x + edge_x, y + edge_y, z + edge_z));
		}
	}

	if (algorithm == 0) {
		rrt_class->execute_rrt();
		draw_path();
	}
	else if (algorithm == 1 || algorithm == 2) {
		rrt_class->execute_rrt();
		draw_path_connect();
	}
}

void ACPP_RRT_Controller::draw_cuboids() {
	for (Cuboid cub : rrt_class->cuboids) {
		cuboids.Add(GetWorld()->SpawnActor<ACuboidActor>(CuboidActorObstacle, move_cord(FVector(cub.first.get_x() - cub.get_edge_x() * 0.5, cub.first.get_y() - cub.get_edge_y() * 0.5, cub.first.get_z() - cub.get_edge_z() * 0.5)), standard_rotation));
		cuboids.Last()->SetBoxDimensions(move_cord(FVector(cub.get_edge_x(), cub.get_edge_y(), cub.get_edge_z())));
	}
}

void ACPP_RRT_Controller::set_stats_widget(string algorithm_type) {
	VisualizationMenuInstance->set_algorithm_type(algorithm_type);

	VisualizationMenuInstance->set_board_dimensions(BOARD_X, BOARD_Y, BOARD_Z);

	VisualizationMenuInstance->set_start_position(START_X, START_Y, START_Z);
	VisualizationMenuInstance->set_finish_position(FINISH_X, FINISH_Y, FINISH_Z);

	VisualizationMenuInstance->set_total_nodes(nodes.Num());
	VisualizationMenuInstance->set_total_obstacles(cuboids.Num());
	VisualizationMenuInstance->set_node_path(nodes_path);
	VisualizationMenuInstance->set_path_length(rrt_class->path_lenght());
}

void ACPP_RRT_Controller::draw_path() {
	UE_LOG(LogTemp, Warning, TEXT("Draw Path."));
	queue<Node*> node_queue; //queue with drawing nodes
	node_queue.push(rrt_class->start);
	//node_queue.push(start_node);
	nodes_path = 0;
	while (!node_queue.empty()) {
		Node* current = node_queue.front(); //current node
		for (int i = 0; i < current->children.size(); ++i) {
			node_queue.push(current->children[i]);
		}
		FVector current_position = move_cord(FVector(current->get_x(), current->get_y(), current->get_z())); //position of current node scaled to Unreal Engine Map
		FVector parent_position = standard_position; //parent position of current node scaled to Unreal Engine Map
		if (current->parent != nullptr) {
			parent_position = move_cord(FVector(current->parent->get_x(), current->parent->get_y(), current->parent->get_z()));
			//path_length += classic_rrt->distance(current, current->parent);
		}
		switch (current->get_color()) {
		case 0:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorTree, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		case 1:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorPath, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			++nodes_path;
			break;
		case 2:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorStart, current_position, standard_rotation));
			break;
		case 3:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorFinish, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		default:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			break;
		}
		node_queue.pop();
		//if (current->children[0] == current->children[0]->children[0])
		//	break;
	}

	nodes_path += 2;
	
	draw_cuboids();
	set_stats_widget(" RRT");
}

void ACPP_RRT_Controller::draw_path_connect() {
	queue<Node*> node_queue; //queue with drawing nodes
	node_queue.push(rrt_class->start);
	nodes_path = 0;
	while (!node_queue.empty()) {
		Node* current = node_queue.front(); //current node
		for (int i = 0; i < current->children.size(); ++i) {
			node_queue.push(current->children[i]);
		}
		FVector current_position = move_cord(FVector(current->get_x(), current->get_y(), current->get_z())); //position of current node scaled to Unreal Engine Map
		FVector parent_position = standard_position; //parent position of current node scaled to Unreal Engine Map
		if (current->parent != nullptr) {
			parent_position = move_cord(FVector(current->parent->get_x(), current->parent->get_y(), current->parent->get_z()));
			//path_length += classic_rrt->distance(current, current->parent);
		}
		switch (current->get_color()) {
		case 0:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorTree, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		case 1:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorPath, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			++nodes_path;
			break;
		case 2:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorStart, current_position, standard_rotation));
			break;
		case 3:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorFinish, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		case 4:
			UE_LOG(LogTemp, Warning, TEXT("ORANGE"));
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorConnect, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		default:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			break;
		}
		node_queue.pop();
		//if (current->children[0] == current->children[0]->children[0])
		//	break;
	}

	node_queue.empty();
	node_queue.push(rrt_class->finish);
	while (!node_queue.empty()) {
		Node* current = node_queue.front(); //current node
		for (int i = 0; i < current->children.size(); ++i) {
			node_queue.push(current->children[i]);
		}
		FVector current_position = move_cord(FVector(current->get_x(), current->get_y(), current->get_z())); //position of current node scaled to Unreal Engine Map
		FVector parent_position = standard_position; //parent position of current node scaled to Unreal Engine Map
		if (current->parent != nullptr) {
			parent_position = move_cord(FVector(current->parent->get_x(), current->parent->get_y(), current->parent->get_z()));
			//path_length += classic_rrt->distance(current, current->parent);
		}
		switch (current->get_color()) {
		case 0:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorTree, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		case 1:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorPath, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			++nodes_path;
			break;
		case 2:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorStart, current_position, standard_rotation));
			break;
		case 3:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorFinish, current_position, standard_rotation));
			//lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			//lines.Last()->SetLine(current_position, parent_position);
			break;
		case 4:
			UE_LOG(LogTemp, Warning, TEXT("ORANGE"));
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorConnect, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_position);
			break;
		default:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			break;
		}
		node_queue.pop();
		//if (current->children[0] == current->children[0]->children[0])
		//	break;
	}

	RRT_Connect* rrt_con = static_cast<RRT_Connect*>(rrt_class);
	FVector pos_node_1 = move_cord(FVector(rrt_con->connect_node_1->get_x(), rrt_con->connect_node_1->get_y(), rrt_con->connect_node_1->get_z()));
	FVector pos_node_2 = move_cord(FVector(rrt_con->connect_node_2->get_x(), rrt_con->connect_node_2->get_y(), rrt_con->connect_node_2->get_z()));
	lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
	lines.Last()->SetLine(pos_node_1, pos_node_2);

	nodes_path += 2;

	draw_cuboids();
	set_stats_widget(" RRT Connect");
}

void ACPP_RRT_Controller::clear_map() {
	for (ANodeActor* current : nodes) {
		current->Destroy();
	}
	nodes.Empty();
	for (ACuboidActor* current : cuboids) {
		current->Destroy();
	}
	cuboids.Empty();
	for (ALineActor* current : lines) {
		current->Destroy();
	}
	lines.Empty();
}

FVector ACPP_RRT_Controller::move_cord(FVector moving_vector) {
	moving_vector[0] = moving_vector[0] * MOVEMENT;
	moving_vector[1] = moving_vector[1] * MOVEMENT;
	moving_vector[2] = moving_vector[2] * MOVEMENT;
	return moving_vector;
}

void ACPP_RRT_Controller::reset_all_menus() {
	MainMenuInstance->reset();
	VisualizationMenuInstance->reset();
	BoardTypeInstance->reset();
	AlgorithmMenuInstance->reset();
}

void ACPP_RRT_Controller::BeginPlay() {
	Super::BeginPlay();

	standard_rotation = FRotator(0.0f, 0.0f, 0.0f);
	nodes_path = 0;
	board_type = 0;
	algorithm = 0;

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	if (MainMenuUserWidget_Class) {
		UE_LOG(LogTemp, Warning, TEXT("Create MainMenu."));
		MainMenuInstance = CreateWidget<UMainMenu_UserWidget>(this, MainMenuUserWidget_Class);
		if (MainMenuInstance) {
			MainMenuInstance->AddToViewport();
		}
	}

	if (BoardTypeUserWidget_Class) {
		UE_LOG(LogTemp, Warning, TEXT("Create Board type manu."));
		BoardTypeInstance = CreateWidget<UBoardType_UserWidget>(this, BoardTypeUserWidget_Class);
	}

	if (VisualizationUserWidget_Class) {
		UE_LOG(LogTemp, Warning, TEXT("Create Visualizaton user widget."));
		VisualizationMenuInstance = CreateWidget<UVisualization_UserWidget>(this, VisualizationUserWidget_Class);
	}

	if (AlgorithmUserWidget_Class) {
		UE_LOG(LogTemp, Warning, TEXT("Create Visualizaton user widget."));
		AlgorithmMenuInstance = CreateWidget<UAlgorithm_UserWidget>(this, AlgorithmUserWidget_Class);
	}

	reset_all_menus();

	//FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_RRT, 1.0f, false);
}

void ACPP_RRT_Controller::Tick(float DeltaTime) {
	//Main menu
	if (MainMenuInstance->start) {
		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_RRT, 0.1f, false);
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_random_map_RRT, 0.1f, false);
		
		MainMenuInstance->reset();
		BoardTypeInstance->reset();
		//VisualizationMenuInstance->reset();

		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_random_map_RRT, 0.1f, false);
		//if (VisualizationMenuInstance) {
		//	VisualizationMenuInstance->AddToViewport();
		//	VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		//}
		if (BoardTypeInstance) {
			UE_LOG(LogTemp, Warning, TEXT("BoardTypeInstance Visible."));
			BoardTypeInstance->AddToViewport();
			BoardTypeInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//Board type (menu)
	if (BoardTypeInstance->prepared) {
		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_RRT, 0.1f, false);
		board_type = 0;
		BoardTypeInstance->reset();
		//VisualizationMenuInstance->reset();
		AlgorithmMenuInstance->reset();

		if (AlgorithmMenuInstance) {
			UE_LOG(LogTemp, Warning, TEXT("AlgorithmMenuInstance Visible."));
			AlgorithmMenuInstance->AddToViewport();
			AlgorithmMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (BoardTypeInstance->random) {
		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPP_RRT_Controller::start_random_map_RRT, 0.1f, false);
		board_type = 1;
		BoardTypeInstance->reset();
		AlgorithmMenuInstance->reset();

		if (AlgorithmMenuInstance) {
			UE_LOG(LogTemp, Warning, TEXT("AlgorithmMenuInstance Visible."));
			AlgorithmMenuInstance->AddToViewport();
			AlgorithmMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (BoardTypeInstance->back) {
		MainMenuInstance->reset();
		BoardTypeInstance->reset();

		if (MainMenuInstance) {
			MainMenuInstance->AddToViewport();
			MainMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//Algorithm menu
	if (AlgorithmMenuInstance->classic) {
		algorithm = 0;
		start_RRT();
		AlgorithmMenuInstance->reset();
		VisualizationMenuInstance->reset();

		if (VisualizationMenuInstance) {
			VisualizationMenuInstance->AddToViewport();
			VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (AlgorithmMenuInstance->bidirectional) {
		algorithm = 1;
		start_RRT();
		AlgorithmMenuInstance->reset();
		VisualizationMenuInstance->reset();

		if (VisualizationMenuInstance) {
			VisualizationMenuInstance->AddToViewport();
			VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (AlgorithmMenuInstance->connect) {
		algorithm = 2;
		start_RRT();
		AlgorithmMenuInstance->reset();
		VisualizationMenuInstance->reset();

		if (VisualizationMenuInstance) {
			VisualizationMenuInstance->AddToViewport();
			VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (AlgorithmMenuInstance->other) {
		algorithm = 3;
		start_RRT();
		AlgorithmMenuInstance->reset();
		VisualizationMenuInstance->reset();

		//if (VisualizationMenuInstance) {
		//	VisualizationMenuInstance->AddToViewport();
		//	VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		//}
	}
	if (AlgorithmMenuInstance->back) {
		AlgorithmMenuInstance->reset();
		BoardTypeInstance->reset();

		if (BoardTypeInstance) {
			BoardTypeInstance->AddToViewport();
			BoardTypeInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}

	//Visualization menu
	if (VisualizationMenuInstance->exit) {
		MainMenuInstance->reset();
		VisualizationMenuInstance->reset();

		if (MainMenuInstance) {
			MainMenuInstance->AddToViewport();
			MainMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
		clear_map();
	}
}

ACPP_RRT_Controller::ACPP_RRT_Controller() {
	nodes_path = 0;
	rrt_class = nullptr;

	board_type = 0;
	algorithm = 0;

	standard_rotation = FRotator(0.0f, 0.0f, 0.0f);
	standard_position = FVector(0.0f, 0.0f, 0.0f);
}

ACPP_RRT_Controller::~ACPP_RRT_Controller() {
	clear_map();
	delete rrt_class;
}