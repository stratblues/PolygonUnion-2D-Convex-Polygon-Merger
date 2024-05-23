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
    Edge* next;

    Edge(Point start, Point end) {
        this->start = start;
        this->end = end;
        this->next = nullptr;
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

    Point* getTail() {
        if (head == nullptr) {
            return nullptr;
        }
        Point* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        return temp;
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

    int size() {
        int count = 0;
        Point* current = head;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }
};

void evaluateCommandLine(int argc, char* argv[], char** inFileName);
void readFile(char** filename, LinkedList& PointListA, LinkedList& PointListB);
void printVertices(LinkedList& pointList, const string& polygonName);
pair<Point, double> getIntersectionPoint(Edge edgeOne, Edge edgeTwo);
int intersectionTestInsideOut(Edge edgeOne, Edge edgeTwo);
bool isPointInside(const Point& point, LinkedList& polygon);
void unionPolygon(LinkedList& polygonA, LinkedList& polygonB, LinkedList& output);




int main(int argc, char* argv[])
{

    char* inFileName = "HW3_a_in.ps";
    LinkedList PointListA;
    LinkedList PointListB;
    LinkedList EdgeIntersections;
    LinkedList output;
  


 


    evaluateCommandLine(argc, argv, &inFileName);
    readFile(&inFileName, PointListA, PointListB);

    printVertices(PointListA, "Polygon A");
    printVertices(PointListB, "Polygon B");

    unionPolygon(PointListA, PointListB, output);

    cout << "Union of Polygon A and B vertices:" << endl;
    output.printList();

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
   
    Point pPrime(point.x * 1000, point.y);
    Edge testEdge(point, pPrime);
    int count = 0;
    for (int i = 0; i < n; i++) {
        count += intersectionTestInsideOut(testEdge, polygonEdges[i]);
    }
    if (count % 2 == 0 || count == 0) {
        return false;

    }
    else {
        return true;
    }


}

pair<Point, double> getIntersectionPoint(Edge edgeOne, Edge edgeTwo) {
    double distancex0 = edgeOne.end.x - edgeOne.start.x;
    double distancey0 = edgeOne.end.y - edgeOne.start.y;
    double distancex2 = edgeTwo.end.x - edgeTwo.start.x;
    double distancey2 = edgeTwo.end.y - edgeTwo.start.y;

    double denominator = (distancey0 * distancex2 - distancex0 * distancey2);

    cout << "Edge One Start: (" << edgeOne.start.x << ", " << edgeOne.start.y << "), End: (" << edgeOne.end.x << ", " << edgeOne.end.y << ")" << endl;
    cout << "Edge Two Start: (" << edgeTwo.start.x << ", " << edgeTwo.start.y << "), End: (" << edgeTwo.end.x << ", " << edgeTwo.end.y << ")" << endl;
    cout << "Distance x0: " << distancex0 << ", Distance y0: " << distancey0 << endl;
    cout << "Distance x2: " << distancex2 << ", Distance y2: " << distancey2 << endl;
    cout << "Denominator: " << denominator << endl;

    if (denominator == 0) {
        cout << "Edges are parallel no intersection." << endl;
        return { Point(1e+38, 1e+38), -1 };
    }
    double t0 = ((edgeOne.start.x - edgeTwo.start.x) * distancey2 + (edgeTwo.start.y - edgeOne.start.y) * distancex2) / denominator;
    double t2 = ((edgeTwo.start.x - edgeOne.start.x) * distancey0 + (edgeOne.start.y - edgeTwo.start.y) * distancex0) / denominator;
    cout << "t0: " << t0  << endl;
    cout << "t2: " << t2 << endl;
    if (t0 >= 0 && t0 <= 1 && t2 >= 0 && t2 <= 1) {
        double intersectionX = edgeOne.start.x + distancex0 * t0;
        double intersectionY = edgeOne.start.y + distancey0 * t0;
        cout << "Intersection Point: (" << intersectionX << ", " << intersectionY << ")" << endl;
        return { Point(intersectionX, intersectionY), t0 };
    }
    cout << "No valid intersection found within segment bounds." << endl;
    return { Point(1e+38, 1e+38), -1 };
}

int intersectionTestInsideOut(Edge edgeOne, Edge edgeTwo) {
    double distancex0 = edgeOne.end.x - edgeOne.start.x;
    double distancey0 = edgeOne.end.y - edgeOne.start.y;
    double distancex2 = edgeTwo.end.x - edgeTwo.start.x;
    double distancey2 = edgeTwo.end.y - edgeTwo.start.y;

    double denominator = (distancey0 * distancex2 - distancex0 * distancey2);
    if (denominator == 0) {
        return 0;
    }
    double t0 = ((edgeOne.start.x - edgeTwo.start.x) * distancey2 + (edgeTwo.start.y - edgeOne.start.y) * distancex2) / denominator;

    double t2 = ((edgeTwo.start.x - edgeOne.start.x) * distancey0 + (edgeOne.start.y - edgeTwo.start.y) * distancex0) / denominator;
    if (t0 >= 0 && t0 <= 1 && t2 >= 0 && t2 <= 1) {
       
        return 1;
    }
    return 0;
}

void unionPolygon(LinkedList& polygonA, LinkedList& polygonB, LinkedList& output) {
    LinkedList* P0 = &polygonA;
    LinkedList* P1 = &polygonB;
    Point* start = polygonB.getHead();
    Point* vi = nullptr;
    Point* nextVertex = nullptr;


    while (start != nullptr) {
        if (!isPointInside(*start, polygonA)) {
            vi = start;
            nextVertex = start->next;
            break;
        }
        start = start->next;
    }

    output.insertAtEnd(vi->x, vi->y);
    Edge currentEdge(*vi, *nextVertex);
    Point* currentVertex = nextVertex;
    while ((output.size() < 2) || (output.getHead() != output.getTail())) {
        vector<Edge> edgesB = P0->getEdges();
        Point lowestTintersection(0.0, 0.0);
        double lowestT = 1.0;
        bool intersectionFound = false;
        int intersectionCount = 0;

        for (const auto& edgeB : edgesB) {
            auto [intersection, t] = getIntersectionPoint(currentEdge, edgeB);
            if (intersectionCount >= 2) {
                break;
            }
            if (t >= 0) {
                if (t < lowestT) {
                    lowestT = t;
                    lowestTintersection = intersection;
                }
                intersectionFound = true;
                intersectionCount++;
                cout << intersectionCount << endl;
            }

        }
        if (!intersectionFound) {

            cout << "Inserting current vertex: " << currentVertex->x << ", " << currentVertex->y << endl;
            output.insertAtEnd(currentVertex->x, currentVertex->y);
            vi = currentVertex;
            currentVertex = currentVertex->next;
            if (currentVertex == nullptr) {
                cout << "Current vertex is null." << endl;
                break;
            }
            currentEdge = Edge(*vi, *currentVertex);
        }
        else {
            
            cout << "Inserting intersection: " << lowestTintersection.x << ", " << lowestTintersection.y << endl;
            output.insertAtEnd(lowestTintersection.x, lowestTintersection.y);

            cout << "Inserting edge end: " << currentEdge.end.x << ", " << currentEdge.end.y << endl;
            output.insertAtEnd(currentEdge.end.x, currentEdge.end.y);

            vi = &lowestTintersection;
            currentVertex = currentVertex->next;
            if (currentVertex == nullptr) {
                cout << "Current vertex is null." << endl;
                break;
            }
            currentEdge = Edge(*vi, *currentVertex);

            LinkedList* temp = P0;
            P0 = P1;
            P1 = temp;
           




        }
    }
}





    



void printVertices(LinkedList& pointList, const string& polygonName) {
    cout << polygonName << " vertices:" << endl;
    pointList.printList();
    
}