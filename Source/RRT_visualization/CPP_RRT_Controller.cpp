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
		//cout << "(" << x << ", " << y << ", " << z << ")";
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
		//cout << "(" << x << ", " << y << ", " << z << ")";
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
	~RRT() {
		delete start;
		planned_path.clear();
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

	Node* find_nearest_node(Node* new_node) {
		Node* nearest = start; //nearest node to current node
		double dist = distance(start, new_node); //distance from nearest node to new_node
		queue<Node*> node_queue; //queue with nodes to verify
		node_queue.push(start);
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

	bool verify_finish(Node* new_node) {
		if (distance(finish, new_node) <= min_distance + 1)
			return true;
		return false;
	}

	void add_node_to_tree(Node* new_node, Node* parent) {
		parent->add_children(new_node);
		new_node->parent = parent;
	}

	void save_path() {
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

	void print_path() {
		for (int i = 0; i < planned_path.size(); ++i) {
			UE_LOG(LogTemp, Warning, TEXT("%d: (%f, %f, %f)"),
				i + 1,
				planned_path[i]->get_x(),
				planned_path[i]->get_y(),
				planned_path[i]->get_z());
		}
	}

	bool check_collisions(Node* nearest, Node* new_node) {
		for (Cuboid cub : cuboids) {
			if (cub.node_inside(new_node) || cub.path_inside(nearest, new_node))
				return false;
		}
		return true;
	}

	void print_cuboids() {
		int size = cuboids.size(); //size of cuboids vector
		for (int i = 0; i < size; ++i) {
			//cout << i + 1 << " ";
			UE_LOG(LogTemp, Warning, TEXT("%d"), i + 1);
			cuboids[i].first.print_values();
			//cout << " ";
			cuboids[i].second.print_values();
			//cout << endl;
		}
	}

	double path_lenght() {
		double lenght = 0;
		for (int i = 0; i < planned_path.size() - 1; ++i) {
			lenght += distance(planned_path[i], planned_path[i + 1]);
		}
		return lenght;
	}

	void execute_rrt() {
		bool is_finish = false; //contains bool information about distance to finish
		int i = 0;
		//cout << "Start: ";
		UE_LOG(LogTemp, Warning, TEXT("Start: "));
		start->print_values();
		//cout << endl;
		//cout << "Finish: ";
		UE_LOG(LogTemp, Warning, TEXT("Finish: "));
		finish->print_values();
		//cout << endl;
		//cout << "Cuboids:" << endl;
		UE_LOG(LogTemp, Warning, TEXT("Cuboids: "));
		print_cuboids();
		do {
			Node* new_node = nullptr; //new node
			Node* nearest = nullptr; //nearest node to new node
			double dist = 0;
			do {
				new_node = random_point();
				nearest = find_nearest_node(new_node);
				dist = distance(new_node, nearest);
			} while (dist < min_distance || !check_collisions(nearest, new_node));
			//while (dist < min_distance && check_collisions(nearest, new_node));
			//cout << "New node: ";
			//new_node->print_values();
			if (dist > max_distance) {
				double dx = new_node->get_x() - nearest->get_x(); //x direction vector component
				double dy = new_node->get_y() - nearest->get_y(); //y direction vector component
				double dz = new_node->get_z() - nearest->get_z(); //z direction vector component
				double dist_near = sqrt(dx * dx + dy * dy + dz * dz);
				new_node->set_x(nearest->get_x() + (dx / dist_near) * MAX_DIST);
				new_node->set_y(nearest->get_y() + (dy / dist_near) * MAX_DIST);
				new_node->set_z(nearest->get_z() + (dz / dist_near) * MAX_DIST);
				//cout << "Changed new node: ";
				//new_node->print_values();
			}
			is_finish = verify_finish(new_node);
			if (is_finish) {
				add_node_to_tree(finish, nearest);
				finish->set_parent(nearest);
			}
			else
				add_node_to_tree(new_node, nearest);
			++i;
		} while (!is_finish && i <= max_iterations);
		if (i > max_iterations) {
			//cout << "Max iterations" << endl;
			//cout << "Path not found" << endl;
			UE_LOG(LogTemp, Warning, TEXT("Max iterations. Path not found."));
		}
		else {
			//cout << "Found path" << endl;
			//cout << "Iterations: " << i << endl;
			UE_LOG(LogTemp, Warning, TEXT("Found path. Interations: %d"), i);
			//cout << "Path:" << endl;
			UE_LOG(LogTemp, Warning, TEXT("Path:"));
			save_path();
			print_path();
		}
	}
};

void ACPP_RRT_Controller::start_RRT() {
	delete rrt_class;
	if (algorithm == 0) {
		rrt_class = new RRT;
	}
	else if (algorithm == 1) {

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
		int obstacles = 20;
		double max_x = rrt_class->board_size_x / 3.0; //max size cuboid on x coordinate
		double max_y = rrt_class->board_size_y / 3.0; //max size cuboid on y coordinate
		double max_z = rrt_class->board_size_z / 3.0; //max size cuboid on z coordinate
		for (int i = 0; i < obstacles; ++i) {
			double x = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
			double y = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
			double z = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
			double edge_x = FMath::FRandRange(1.0, max_x);
			double edge_y = FMath::FRandRange(1.0, max_y);
			double edge_z = FMath::FRandRange(1.0, max_z);
			rrt_class->cuboids.push_back(Cuboid(x, y, z, x + edge_x, y + edge_y, z + edge_z));
		}
	}

	if (algorithm == 0) {
		rrt_class->execute_rrt();
		draw_path();
	}
	else if (algorithm == 1) {
		rrt_class->execute_rrt();
		draw_path();
	}
}

void ACPP_RRT_Controller::start_random_map_RRT() {
	int obstacles = 20;
	delete rrt_class;
	rrt_class = new RRT;
	double max_x = rrt_class->board_size_x / 3.0; //max size cuboid on x coordinate
	double max_y = rrt_class->board_size_y / 3.0; //max size cuboid on y coordinate
	double max_z = rrt_class->board_size_z / 3.0; //max size cuboid on z coordinate
	for (int i = 0; i < obstacles; ++i) {
		double x = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
		double y = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
		double z = FMath::FRandRange(2.0, rrt_class->board_size_x * 0.9);
		double edge_x = FMath::FRandRange(1.0, max_x);
		double edge_y = FMath::FRandRange(1.0, max_y);
		double edge_z = FMath::FRandRange(1.0, max_z);
		rrt_class->cuboids.push_back(Cuboid(x, y, z, x + edge_x, y + edge_y, z + edge_z));
	}

	rrt_class->execute_rrt();
	draw_path();
}

void ACPP_RRT_Controller::draw_path() {
	//void ACPP_RRT_Controller::draw_path(RRT * rrt_class) {
	//nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorStart, move_cord(FVector(START_X, START_Y, START_Z)), standard_rotation));
	//nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorFinish, move_cord(FVector(FINISH_X, FINISH_Y, FINISH_Z)), standard_rotation));
	//for (Node* path_node : rrt_class->planned_path) {
	//	nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorPath, move_cord(FVector(path_node->get_x(), path_node->get_y(), path_node->get_z())), standard_rotation));
	//}

	queue<Node*> node_queue; //queue with drawing nodes
	node_queue.push(rrt_class->start);
	nodes_path = 0;
	//path_length = 0;
	while (!node_queue.empty()) {
		Node* current = node_queue.front(); //current node
		for (int i = 0; i < current->children.size(); ++i) {
			node_queue.push(current->children[i]);
		}
		FVector current_position = move_cord(FVector(current->get_x(), current->get_y(), current->get_z()));
		FVector parent_posiotion = standard_position;
		if (current->parent != nullptr) {
			parent_posiotion = move_cord(FVector(current->parent->get_x(), current->parent->get_y(), current->parent->get_z()));
			//path_length += classic_rrt->distance(current, current->parent);
		}
		switch (current->get_color()) {
		case 0:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorTree, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_posiotion);
			break;
		case 1:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorPath, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_posiotion);
			++nodes_path;
			break;
		case 2:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorStart, current_position, standard_rotation));
			break;
		case 3:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorFinish, current_position, standard_rotation));
			lines.Add(GetWorld()->SpawnActor<ALineActor>(LineActorPath, standard_position, standard_rotation));
			lines.Last()->SetLine(current_position, parent_posiotion);
			break;
		default:
			nodes.Add(GetWorld()->SpawnActor<ANodeActor>(NodeActorTree, current_position, standard_rotation));
			break;
		}
		node_queue.pop();
	}
	
	for (Cuboid cub : rrt_class->cuboids) {
		cuboids.Add(GetWorld()->SpawnActor<ACuboidActor>(CuboidActorObstacle, move_cord(FVector(cub.first.get_x() - cub.get_edge_x()*0.5, cub.first.get_y() - cub.get_edge_y() * 0.5, cub.first.get_z() - cub.get_edge_z() * 0.5)), standard_rotation));
		cuboids.Last()->SetBoxDimensions(move_cord(FVector(cub.get_edge_x(), cub.get_edge_y(), cub.get_edge_z())));
	}

	VisualizationMenuInstance->set_board_dimensions(BOARD_X, BOARD_Y, BOARD_Z);

	VisualizationMenuInstance->set_start_position(START_X, START_Y, START_Z);
	VisualizationMenuInstance->set_finish_position(FINISH_X, FINISH_Y, FINISH_Z);

	VisualizationMenuInstance->set_total_nodes(nodes.Num());
	VisualizationMenuInstance->set_total_obstacles(cuboids.Num());
	VisualizationMenuInstance->set_node_path(nodes_path);
	VisualizationMenuInstance->set_path_length(rrt_class->path_lenght());
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

//void ACPP_RRT_Controller::reset_all_menus() {
//
//}

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
		/*if (VisualizationMenuInstance) {
			VisualizationMenuInstance->AddToViewport();
			VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}*/
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
		//if (VisualizationMenuInstance) {
		//	VisualizationMenuInstance->AddToViewport();
		//	VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		//}
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
	if (AlgorithmMenuInstance->connect) {
		algorithm = 1;
		start_RRT();
		AlgorithmMenuInstance->reset();
		VisualizationMenuInstance->reset();

		if (VisualizationMenuInstance) {
			VisualizationMenuInstance->AddToViewport();
			VisualizationMenuInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (AlgorithmMenuInstance->other) {
		algorithm = 2;
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
	standard_rotation = FRotator(0.0f, 0.0f, 0.0f);
	standard_position = FVector(0.0f, 0.0f, 0.0f);
}