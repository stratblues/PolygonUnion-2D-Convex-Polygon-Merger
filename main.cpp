#include <string.h>
#include <iostream> 
#include <iomanip>
#include <fstream>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <cmath>
#include <math.h>

/*
* @file
* @author Tony Stanell <ts3552@drexel.edu>
* @date May 13th, 2024
* @section CS536
*
* This is a c++ file for solving the problem of computing the union of two 2D convex polygons.
*/

using namespace std;
using namespace Eigen;


class Point {
public:
    double x;
    double y;
    Point* next;

    Point() : x(0), y(0), next(nullptr) {}

    Point(double x, double y) {
        this->x = x;
        this->y = y;
        this->next = nullptr;
    }
};

class Edge {
public:
    Point start;
    Point end;

    Edge(Point start, Point end) {
        this->start = start;
        this->end = end;
    }
};


class LinkedList {
private:
    Point* head;

public:
    LinkedList() {
        this->head = nullptr;
    }

    void insertAtBeggining(double x, double y) {
        Point* newPoint = new Point(x, y);
        newPoint->next = head;
        head = newPoint;
        return;
    }

    void insertAtEnd(double x, double y) {
        Point* newPoint = new Point(x, y);
        if (head == nullptr) {
            head = newPoint;
            return;
        }
        Point* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newPoint;
    }

    void printList() {
        Point* temp = head;
        while (temp != nullptr) {
            cout << temp->x << "," << temp->y << endl;
            temp = temp->next;
        }
        cout << endl;
    }

    Point* getHead() {
        return head;
    }

    vector<Point> toVector() {
        vector<Point> points;
        Point* temp = head;
        while (temp != nullptr) {
            points.push_back(*temp);
            temp = temp->next;
        }
        return points;
    }

    void insertPoint(Point* point) {
        insertAtEnd(point->x, point->y);
    }

    vector<Edge> getEdges() {
        vector<Edge> edges;
        if (!head) return edges;
        Point* current = head;
        Point* start = head;

        while (current->next != nullptr) {
            Edge newEdge(*current, *(current->next));
            edges.push_back(newEdge);
            current = current->next;
        }
        Edge closingEdge(*current, *start);
        edges.push_back(closingEdge);

        return edges;
    }
};

void evaluateCommandLine(int argc, char* argv[], char** inFileName);
void readFile(char** filename, LinkedList& PointListA, LinkedList& PointListB);
void printVertices(LinkedList& pointList, const string& polygonName);
Point getIntersectionPoint(Edge edgeOne, Edge edgeTwo);
int interesctionTestInsideOut(Edge edgeOne, Edge edgeTwo);
bool isPointInside(Point& point, LinkedList& polygon);




int main(int argc, char* argv[])
{

    char* inFileName = "HW3_a_in.ps";
    LinkedList PointListA;
    LinkedList PointListB;
    LinkedList EdgeIntersections;
    LinkedList Output;
  


 


    evaluateCommandLine(argc, argv, &inFileName);
    readFile(&inFileName, PointListA, PointListB);

    printVertices(PointListA, "Polygon A");
    printVertices(PointListB, "Polygon B");


}

void evaluateCommandLine(int argc, char* argv[], char** inFileName) {
    int i = 0;
    while (i < argc) {
        if (string(argv[i]) == "-f" && i + 1 < argc) {
            *inFileName = argv[i + 1];
            i += 2;
        }
        else {
            i++;
        }
    }
}

void readFile(char** filename, LinkedList& PointListA, LinkedList& PointListB) {
    string fileText;
    ifstream MyFile(*filename);
    int count = 0;
    Vector2d point(0.0, 0.0);
    if (!MyFile.is_open()) {
        cout << "Failed to open file" << endl;
        return;
    }
    string command;
    while (getline(MyFile, fileText)) {
        istringstream iss(fileText);
        if (fileText == "stroke") {
            count += 1;
        }
        if (iss >> point[0] >> point[1] >> command) {
            if (command == "moveto") {
                continue;
            }
            if (count < 1) {
                PointListA.insertAtEnd(point[0], point[1]);

            }
            else {
                PointListB.insertAtEnd(point[0], point[1]);

            }
        }
    }

    MyFile.close();
}

bool isPointInside(Point& point, LinkedList& polygon) {
    vector<Edge> polygonEdges = polygon.getEdges();
    int n = polygonEdges.size();
    bool inside = false;
    Point pPrime(point.x * 100, point.y);
    Edge testEdge(point, pPrime);
    int count = 0;
    for (int i = 0; i < n; i++) {
        count += interesctionTestInsideOut(testEdge, polygonEdges[i]);
    }
    if (count % 2 == 0 || count == 0) {
        return false;

    }
    else {
        return true;
    }


}

Point getIntersectionPoint(Edge edgeOne, Edge edgeTwo) {
    double distancex0 = edgeOne.end.x - edgeOne.start.x;
    double distancey0 = edgeOne.end.y - edgeOne.start.y;
    double distancex2 = edgeTwo.end.x - edgeTwo.start.x;
    double distancey2 = edgeTwo.end.y - edgeTwo.start.y;

    if ((distancey0 * distancex2 - distancex0 * distancey2) == 0 || (distancey2 * distancex0 - distancex2 * distancey0) == 0) {
        return Point(0.0, 0.0);
    }
    double t0 = ((edgeOne.start.x - edgeTwo.start.x) * distancey2 + (edgeTwo.start.y - edgeOne.start.y) * distancex2) / (distancey0 * distancex2 - distancex0 * distancey2);

    double t2 = ((edgeTwo.start.x - edgeOne.start.x) * distancey0 + (edgeOne.start.y - edgeTwo.start.y) * distancex0) / (distancey2 * distancex0 - distancex2 * distancey0);
    if (t0 >= 0 && t0 <= 1 && t2 >= 0 && t2 <= 1) {
        double intersectionX = edgeOne.start.x + t0 * distancex0;
        double intersectionY = edgeOne.start.y + t0 * distancey0;
        return Point(intersectionX, intersectionY);
    }
    return Point(0.0, 0.0);
}

int interesctionTestInsideOut(Edge edgeOne, Edge edgeTwo) {
    double distancex0 = edgeOne.end.x - edgeOne.start.x;
    double distancey0 = edgeOne.end.y - edgeOne.start.y;
    double distancex2 = edgeTwo.end.x - edgeTwo.start.x;
    double distancey2 = edgeTwo.end.y - edgeTwo.start.y;

    if ((distancey0 * distancex2 - distancex0 * distancey2) == 0 || (distancey2 * distancex0 - distancex2 * distancey0) == 0) {
        return 0;
    }
    double t0 = ((edgeOne.start.x - edgeTwo.start.x) * distancey2 + (edgeTwo.start.y - edgeOne.start.y) * distancex2) / (distancey0 * distancex2 - distancex0 * distancey2);

    double t2 = ((edgeTwo.start.x - edgeOne.start.x) * distancey0 + (edgeOne.start.y - edgeTwo.start.y) * distancex0) / (distancey2 * distancex0 - distancex2 * distancey0);
    if (t0 >= 0 && t0 <= 1 && t2 >= 0 && t2 <= 1) {
        double intersectionX = edgeOne.start.x + t0 * distancex0;
        double intersectionY = edgeOne.start.y + t0 * distancey0;
        return 1;
    }
    return 0;
}

void unionPolygon()


void printVertices(LinkedList& pointList, const string& polygonName) {
    cout << polygonName << " vertices:" << endl;
    pointList.printList();
    
}