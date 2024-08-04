#include <iostream>
#include <map>
#include <utility>
#include <random>
#include <list>
#include "Delaunay_Triangulation.h"


using namespace std;

void selectRandomRGB(){
    static default_random_engine  rng;
    uniform_int_distribution<int> distr(0,255);
}

bool Triangle::checkNeighbors(Triangle newShape) {
    for(int i = 0; i < sizeof(newShape.vertices); i ++){
        for(int j = 0; j < sizeof(newShape.vertices); j++) {
            if (vertices[i].first == newShape.vertices[j].first && vertices[i].second == newShape.vertices[j].second) {
                return true;
            }
        }
    }
    return false;
}

void Triangle::alterRBG(vector<Color> colors){
    //Check for completely random outputs
    srand((unsigned ) time(0));
    int result = 0 + (rand() % colors.size());

    Color new_color = colors[0];
    color[0] = colors[result].RBG[0];
    color[1] = colors[result].RBG[1];
    color[2] = colors[result].RBG[2];
    RGBvalue = color[0] + (color[1] * 2) + (color[2] * 3);
}

class HashTable{
private:
    static const int  hashGroups = 10;
    list<pair<int,vector<Triangle>>> table[hashGroups];
    array<Vertex,DATASIZE> vertex_list;
    vector<Triangle> triangles;
    vector<Color> color_list;
public:
    HashTable(array<Vertex,DATASIZE> new_list,vector<Triangle> new_triangles,vector<Color> colors){
        vertex_list = new_list;
        triangles = new_triangles;
        color_list = colors;


    };
    vector<Triangle> getTriangleList();
    int hashFunction(int key);
    void insertHash(int key, Triangle &shape);
    void removeItem(int key);
    void printTable();

};

int HashTable::hashFunction(int key) {
    return key % 10;
}

//Check if neighbors, and if so change color until colors are alright
void HashTable::insertHash(int key, Triangle &shape) {
    int hashvalue = hashFunction(key);
    list<pair<int, vector<Triangle>>> &index = table[hashvalue];
    auto iter = begin(index);
    bool existing = false;
    bool sameColors = true;
    for (; iter != end(index); iter++) {
        if (iter->first == key) {
            for (int j = 0; j < iter->second.size(); j++) {
                if(iter->second[j].checkNeighbors(shape)){
                    while (sameColors) {
                        shape.alterRBG(color_list);
                        if(shape.color[0] != iter->second[j].color[0] || shape.color[1] != iter->second[j].color[1] || shape.color[2] != iter->second[j].color[2]){
                            sameColors = false;
                        }
                    }
                    insertHash(shape.RGBvalue, shape);
                    break;
                }
            }
        }
        else{
            vector<Triangle> temp = {shape};
            index.emplace_back(key, temp);
            break;
        }
    }
}


void HashTable::removeItem(int key) {
    int hashvalue = hashFunction(key);
    list<pair<int,vector<Triangle>>> &index = table[hashvalue];
    auto iter = begin(index);
    for(; iter != end(index); iter ++){
        if(iter->first == key){
            iter = index.erase(iter);
        }
    }
}

void HashTable::printTable() {
    for(int i = 0; i < hashGroups; i ++){
        if(table[i].size() == 0){
            continue;
        }
        else{
            auto iter = begin(table[i]);
            for(; iter != end(table[i]); iter ++){
                cout << "Bucket: " << i + 1 << endl;
                cout << "Key: " << iter->first << endl;
                for(int i = 0; i < iter->second.size(); i++) {
                    cout << "Value(s): " << iter->second[i].RGBvalue << ' ';
                }
                cout << endl;
            }
        }
    }
}



int main() {
    int option;
    vector<Color>palette;
    bool done = false;
    while(!done){
        cout << "Project 3: Mapping with Colors" << endl;
        cout << "Menu Options:" << endl;
        cout << "1. Default Color Palette" << endl;
        cout << "2. Personal Color Palette" << endl;
        cout << "3. Color Mapping" << endl;
        cout << "Selection: ";
        cin >> option;
        cout << endl;
        if (option == 1) {
            //create default palette
            Color Red(255,0,0);
            palette.push_back(Red);
            Color Orange(255,127,0);
            palette.push_back(Orange);
            Color Yellow(255,255,0);
            palette.push_back(Yellow);
            Color Green(0,255,0);
            palette.push_back(Green);
            Color Blue(0,0,255);
            palette.push_back(Blue);
            Color Indigo(75,0,130);
            palette.push_back(Indigo);
            Color Violet(148,0,211);
            palette.push_back(Violet);


        } else if (option == 2) {
            cout << "Please input color values. (ERROR)" << endl;
            //Maybe have color vectors instead of Color class

            //insert to palette
        } else if (option == 3) {
            if(palette.size() < 4){
                cout << "Please input at least 4 color options." << endl;
                cout << endl;
            }
            else{
                done = true;
            }
        }
    }

    //RUN DATA
    MeshData mesh;
    mesh = GenerateRandomMesh();

    //colorize mesh
    vector<double> cordinates;

    for (int x = 0; x < mesh.vertices.size(); x += 1)
    {
        cordinates.push_back(mesh.vertices[x].cordinates[0]);
        cordinates.push_back(mesh.vertices[x].cordinates[1]);
    }
    /*
    delaunator::Delaunator del(cordinates);
    for (int tri = 0; tri < del.triangles.size(); tri += 3)
    {
        Triangle triangle;
        triangle.vertices[0] = del.triangles[tri];
        triangle.vertices[1] = del.triangles[tri + 1];
        triangle.vertices[2] = (del.triangles[tri + 2]);

        //array<int, 3> randomColor = { ((float)rand() / RAND_MAX) * 255, ((float)rand() / RAND_MAX) * 255, ((float)rand() / RAND_MAX) * 255 };
        triangle.color = randomColor;

        mesh.triangles.push_back(triangle);
    }
    */
    printMesh(mesh);

    HashTable HTable(mesh.vertices,mesh.triangles,palette);
    //Pass as pointer maybe
    for(int i = 0; i < HTable.getTriangleList().size(); i++){
        mesh.triangles[i].alterRBG(palette);
        HTable.insertHash(mesh.triangles[i].RGBvalue,mesh.triangles[i]);

    }
    //All triangles calculated and table is ready.
    cout << "done" << endl;
}

