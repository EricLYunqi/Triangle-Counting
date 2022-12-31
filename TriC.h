#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

struct ReorderVertex
{
    int reorder, origianl;

    ReorderVertex() = default;
    ReorderVertex(int _reorder, int _origianl): reorder(_reorder), origianl(_origianl) {}
};

struct Edge
{
    int from, to;

    Edge() = default;
    Edge(int _from, int _to): from(_from), to(_to) {}
    ~Edge() = default;
    bool operator<(const Edge& e) const
    {
        return this -> from > e.from;
    }
};

struct labelVlist
{
    int label;
    vector<int> vlist;
    bool operator<(const labelVlist& lv) const 
    {
        return this -> label < lv.label;
    }
    bool operator==(const labelVlist& lv) const
    {
        return this -> label == lv.label;
    }
};

struct Tag
{
    int u, v, w;

    Tag() = default;
    Tag(int _u, int _v, int _w): u(_u), v(_v), w(_w) {}
    bool operator==(const Tag& x) const
    {
        if((u == x.u || u == x.v || u == x.w) && (v == x.v || v == x.u || v == x.w) 
        && (w == x.w || w == x.v || w == x.u))
            return true;
        return false;
    }
};

class Graph
{
public:
    int numVertex, numEdge;
    int* vertex;
    vector<labelVlist>* gralist;
    priority_queue<Edge> E_star;

    Graph();
    ~Graph();
    int contain(int l, vector<labelVlist>* temp);
    void add_vertex(int v, int l);
    void add_edge(int from, int to);
    void creatgraph(FILE *fp);
    void creatSupergraph(FILE *fp);
};

class MGT
{
private:
    Graph* G; bool* tag; int* super_id;
    vector<Tag> check;
    vector<int>* Neighbor;
    vector<Edge> E_mem; 
    float c = 1.2; 
    bool flag[30][30][30] = {false};

public:
    long long ans = 0;
    MGT();
    ~MGT();
    bool IfEdgeEmpty();
    bool IfEdgeContained(int from, int to);
    bool BinaryIfEdgeContained(int from, int to);
    bool Check_tri(int u, int v, int w);
    bool Check_matrix(int u, int v, int w);
    void Tag_matrix(int u, int v, int w);
    float GetConstant();
    int GetNumnode();
    void ModifyConstant(float x);
    void OutputEdgesInMemory();
    void _init_(FILE* fp);
    void Super_init_(FILE* fp);
    int LoadIntoMemory(int pos);
    long long ReportTriangles();
    long long ReportSuperTriangles();
};

void ReportOriginalResult();
void ReportSuperResult();