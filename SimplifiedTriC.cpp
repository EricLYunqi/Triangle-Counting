#include "TriC.h"

Graph::Graph(): numVertex(0), numEdge(0), vertex(nullptr), gralist(nullptr)
{
}

Graph::~Graph()
{
    if(gralist != nullptr)
        delete[] gralist;
    if(vertex != nullptr)
        delete[] vertex;
}

int Graph::contain(int l, vector<labelVlist>* temp)
{
    int Size = temp -> size();
    for(int i = 0; i < Size; i++)
    {
        if((*temp)[i].label == l)
            return i;
    }
    return -1;
}

void Graph::add_vertex(int v, int l)
{
    vertex[v] = l;
}

void Graph::add_edge(int from, int to)
{
    int vertex_label = vertex[to];
    int pos = contain(vertex_label, &gralist[from]);
    if(pos != -1)
        gralist[from][pos].vlist.push_back(to);
    else
    {
        labelVlist temp_l;
        temp_l.label = vertex_label;
        temp_l.vlist.push_back(to);
        gralist[from].push_back(temp_l);
    }

    Edge temp(from, to);
    E_star.push(temp);
    
    vertex_label = vertex[from];
    pos = contain(vertex_label, &gralist[to]);
    if(pos != -1)
        gralist[to][pos].vlist.push_back(from);
    else
    {
        labelVlist temp_l;
        temp_l.label = vertex_label;
        temp_l.vlist.push_back(from);
        gralist[to].push_back(temp_l);
    }

    Edge temp2(to, from);
    E_star.push(temp2);
}

void Graph::creatgraph(FILE* fp)
{
    int numnode, numedge, numloop;
    char c_flag;
    bool flag = 0;
    while(true)
    {
        fscanf(fp, "%c\n", &c_flag);
        if(c_flag == 't')
        {
            if(flag)
            {
                char c2; int id;
                fscanf(fp, "%c %d\n", &c2, &id);
                break;
            }
            if(!flag)
            {
                char c2, id;
                fscanf(fp, "%c %d\n", &c2, &id);
                fscanf(fp, "%d %d %d\n", &numnode, &numedge, &numloop);
                numVertex = numnode; numEdge = numedge;

                vertex = new int[numnode+10];
                for(int i = 0; i < numnode+10; i++)
                    vertex[i] = 0;
                gralist = new vector<labelVlist>[numnode+10];
                flag = 1;
            }
        }
        else if(c_flag == 'v')
        {
            int v, l;
            fscanf(fp, "%d %d\n", &v, &l);
            add_vertex(v, l);
        }
        else if(c_flag == 'e')
        {
            int from, to, x;
            fscanf(fp, "%d %d %d\n", &from, &to, &x);

            if(from >= 4036530 || to >= 4036530)
                continue;
            if(from == to)
                continue;

            add_edge(from, to);
        }
        else
            cout << "Error" << endl;
    }
}

void Graph::creatSupergraph(FILE* fp)
{
    int numnode, numedge, numloop; char c_flag; bool flag = 0;
    int rv = 0;
    int* hash_table = nullptr;

    while(true)
    {
        fscanf(fp, "%c\n", &c_flag);
        if(c_flag == 't')
        {
            if(flag)
            {
                char c2; int id;
                fscanf(fp, "%c %d\n", &c2, &id);
                break;
            }
            if(!flag)
            {
                char c2, id;
                fscanf(fp, "%c %d\n", &c2, &id);
                fscanf(fp, "%d %d %d\n", &numnode, &numedge, &numloop);
                numVertex = numnode; numEdge = numedge;

                vertex = new int[numnode+10];
                for(int i = 0; i < numnode+10; i++)
                    vertex[i] = 0;
                gralist = new vector<labelVlist>[numnode+10];
                hash_table = new int[2*numnode];
                for(int i = 0; i < 2*numnode; i++)
                    hash_table[i] = 0;
                flag = 1;
            }
        }
        else if(c_flag == 'v')
        {
            int v, l;
            fscanf(fp, "%d %d\n", &v, &l);
            hash_table[v] = rv;
            add_vertex(rv, l);
            rv++;
        }
        else if(c_flag == 'e')
        {
            int from, to, x;
            fscanf(fp, "%d %d %d\n", &from, &to, &x);

            if(from >= 4036530 || to >= 4036530)
                continue;
            if(from == to)
                continue;

            add_edge(hash_table[from], hash_table[to]);
        }
        else
            cout << "Error" << endl;
    }
    if(hash_table != nullptr)
        delete[] hash_table;

    cout << rv << endl;
}

MGT::MGT(): G(new Graph), tag(nullptr), super_id(nullptr), Neighbor(nullptr), ans(0)
{
}

MGT::~MGT()
{
    if(G != nullptr)
        delete G;
    if(tag != nullptr)
        delete[] tag;
    if(Neighbor != nullptr)
        delete[] Neighbor;
    if(super_id != nullptr)
        delete[] super_id;
}

void MGT::_init_(FILE* fp)
{
    G -> creatgraph(fp);
    tag = new bool[G -> numVertex+3];
    memset(tag, false, sizeof(tag));
    Neighbor = new vector<int>[G -> numVertex+3];

    for(int i = 0; i < G -> numVertex; i++)
        for(int j = 0; j < G -> gralist[i].size(); j++)
            for(int k = 0; k < G -> gralist[i][j].vlist.size(); k++)
                Neighbor[i].push_back(G -> gralist[i][j].vlist[k]);
    for(int i = 0; i < G -> numVertex; i++)
    {
        sort(Neighbor[i].begin(), Neighbor[i].end());
        /*for(int j = 0; j < Neighbor[i].size(); j++)
            cout << Neighbor[i][j] << " ";
        cout << endl;*/
    }
}

void MGT::Super_init_(FILE* fp)
{
    G -> creatSupergraph(fp);
    tag = new bool[G -> numVertex+3];
    memset(tag, false, sizeof(tag));
    super_id = new int[G -> numVertex+3];
    for(int i = 0; i < G -> numVertex+3; i++)
        super_id[i] = 0;
    Neighbor = new vector<int>[G -> numVertex+3];

    for(int i = 0; i < G -> numVertex; i++)
    {
        super_id[i] = G -> vertex[i];
        for(int j = 0; j < G -> gralist[i].size(); j++)
            for(int k = 0; k < G -> gralist[i][j].vlist.size(); k++)
                Neighbor[i].push_back(G -> gralist[i][j].vlist[k]);
    }
    for(int i = 0; i < G -> numVertex; i++)
    {
        sort(Neighbor[i].begin(), Neighbor[i].end());
        /*for(int j = 0; j < Neighbor[i].size(); j++)
            cout << Neighbor[i][j] << " ";
        cout << endl;*/
    }
}

int MGT::LoadIntoMemory(int pos)
{
    int P = pos;
    cout << "start load" << endl;
    E_mem.clear();
    vector<Edge>().swap(E_mem);

    for(int i = 0; i < G -> numVertex; i++)
    {
        while(true)
        {
            if(G -> E_star.empty())
                break;

            Edge temp = G -> E_star.top();
            if(temp.from != i)
                break;

            E_mem.push_back(temp);
            G -> E_star.pop();
        }
        P = i;
    }
    return P;
}

bool MGT::IfEdgeContained(int from, int to)
{
    for(int i = 0; i < Neighbor[from].size(); i++)
        if(Neighbor[from][i] == to)
            return true;
    for(int i = 0; i < Neighbor[to].size(); i++)
        if(Neighbor[to][i] == from)
            return true;

    return false;
}

bool MGT::BinaryIfEdgeContained(int from, int to)
{
    int res = -1; int mid = 0; int start = 0; int end = Neighbor[from].size()-1;
    while(start <= end)
    {
        mid = start + ((end - start) >> 1);
        if(Neighbor[from][mid] < to)
            start = mid + 1;
        else if(Neighbor[from][mid] > to)
            end = mid - 1;
        else
        {
            res = mid;
            return true;
        }
    }
    res = -1; mid = 0; start = 0; end = Neighbor[to].size()-1;
    while(start <= end)
    {
        mid = start + ((end - start) >> 1);
        if(Neighbor[to][mid] < from)
            start = mid + 1;
        else if(Neighbor[to][mid] > from)
            end = mid - 1;
        else
        {
            res = mid;
            return true;
        }
    }
    return false;
}

bool MGT::Check_tri(int u, int v, int w)
{
    Tag temp(u, v, w);
    for(int i = 0; i < check.size(); i++)
        if(temp == check[i])
            return true;
    return false;
}

bool MGT::Check_matrix(int u, int v, int w)
{
    vector<int> temp;
    temp.push_back(u); temp.push_back(v); temp.push_back(w);
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            for(int k = 0; k < 3; k++)
                if(temp[i] != temp[j] != temp[k] && flag[temp[i]][temp[j]][temp[k]] == true)
                    return true;
    return false;
}

void MGT::Tag_matrix(int u, int v, int w)
{
    vector<int> temp;
    temp.push_back(u); temp.push_back(v); temp.push_back(w);
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            for(int k = 0; k < 3; k++)
                flag[temp[i]][temp[j]][temp[k]] = true;
}

long long MGT::ReportTriangles()
{
    long long _ans = 0;
    bool* V_mem = new bool[2*(G -> numVertex+3)];
    bool* V_positive_mem = new bool[2*(G -> numVertex+3)];
    for(int i = 0; i < 2*(G -> numVertex+3); i++)
    {
        V_mem[i] = false;
        V_positive_mem[i] = false;
    }
    for(int i = 0; i < E_mem.size(); i++)
    {
        if(!V_mem[E_mem[i].from])
            V_mem[E_mem[i].from] = true;
        if(!V_mem[E_mem[i].to])
            V_mem[E_mem[i].to] = true;
        if(!V_positive_mem[E_mem[i].from])
            V_positive_mem[E_mem[i].from] = true;
    }
    cout << "star report" << endl;
    vector<int> Neighbor_mem; vector<int> Neighbor_positive_mem;
    for(int i = 0; i < G -> numVertex; i++)
    {
        Neighbor_mem.clear(); Neighbor_positive_mem.clear();
        Neighbor_mem.resize(0); Neighbor_positive_mem.resize(0);
        for(int j = 0; j < Neighbor[i].size(); j++)
            if(V_mem[Neighbor[i][j]])
                Neighbor_mem.push_back(Neighbor[i][j]);
        for(int j = 0; j < Neighbor_mem.size(); j++)
            if(V_positive_mem[Neighbor_mem[j]])
                Neighbor_positive_mem.push_back(Neighbor_mem[j]);

        /*cout << "the" << i << endl;
        for(int j = 0; j < Neighbor_mem.size(); j++)
            cout << Neighbor_mem[j] << " ";
        cout << endl;
        for(int j = 0;j < Neighbor_positive_mem.size(); j++)
            cout << Neighbor_positive_mem[j] << " ";
        cout << endl;
        cout << endl;*/

        for(int v = 0; v < Neighbor_positive_mem.size(); v++)
        {
            for(int w = 0; w < Neighbor_mem.size(); w++)
            {
                if(Neighbor_positive_mem[v] != Neighbor_mem[w] && BinaryIfEdgeContained(Neighbor_positive_mem[v], Neighbor_mem[w]) == true
                    && (i < Neighbor_mem[w]) && (Neighbor_mem[w] < Neighbor_positive_mem[v])
                    && (G -> vertex[i] != G -> vertex[Neighbor_mem[w]]) && (G -> vertex[i] != G -> vertex[Neighbor_positive_mem[v]])
                    && (G -> vertex[Neighbor_mem[w]] != G -> vertex[Neighbor_positive_mem[v]])) /*Check_matrix(i, Neighbor_mem[w], Neighbor_positive_mem[v]) == false*/
                {
                    _ans ++;
                    tag[i] = true;
                    tag[Neighbor_positive_mem[v]] = true;
                    tag[Neighbor_mem[w]] = true;
                    
                }
            }
        }
    }   
    cout << "finish report" << endl;

    if(V_mem != nullptr)
        delete[] V_mem;
    if(V_positive_mem != nullptr)
        delete[] V_positive_mem;    
    for(int i = 0; i < Neighbor_mem.size(); i++)
        Neighbor_mem.pop_back();
    for(int i = 0; i < Neighbor_positive_mem.size(); i++)
        Neighbor_positive_mem.pop_back();
    return _ans;
}

long long MGT::ReportSuperTriangles()
{
    long long _ans = 0;
    bool* V_mem = new bool[2*(G -> numVertex+3)];
    bool* V_positive_mem = new bool[2*(G -> numVertex+3)];
    for(int i = 0; i < 2*(G -> numVertex+3); i++)
    {
        V_mem[i] = false;
        V_positive_mem[i] = false;
    }
    for(int i = 0; i < E_mem.size(); i++)
    {
        if(!V_mem[E_mem[i].from])
            V_mem[E_mem[i].from] = true;
        if(!V_mem[E_mem[i].to])
            V_mem[E_mem[i].to] = true;
        if(!V_positive_mem[E_mem[i].from])
            V_positive_mem[E_mem[i].from] = true;
    }
    cout << "star report" << endl;
    vector<int> Neighbor_mem; vector<int> Neighbor_positive_mem;
    for(int i = 0; i < G -> numVertex; i++)
    {
        Neighbor_mem.clear(); Neighbor_positive_mem.clear();
        Neighbor_mem.resize(0); Neighbor_positive_mem.resize(0);
        for(int j = 0; j < Neighbor[i].size(); j++)
            if(V_mem[Neighbor[i][j]])
                Neighbor_mem.push_back(Neighbor[i][j]);
        for(int j = 0; j < Neighbor_mem.size(); j++)
            if(V_positive_mem[Neighbor_mem[j]])
                Neighbor_positive_mem.push_back(Neighbor_mem[j]);

        /*cout << "the" << i << endl;
        for(int j = 0; j < Neighbor_mem.size(); j++)
            cout << Neighbor_mem[j] << " ";
        cout << endl;
        for(int j = 0;j < Neighbor_positive_mem.size(); j++)
            cout << Neighbor_positive_mem[j] << " ";
        cout << endl;
        cout << endl;*/

        for(int v = 0; v < Neighbor_positive_mem.size(); v++)
        {
            for(int w = 0; w < Neighbor_mem.size(); w++)
            {
                if(Neighbor_positive_mem[v] != Neighbor_mem[w] && BinaryIfEdgeContained(Neighbor_positive_mem[v], Neighbor_mem[w]) == true
                    && (i < Neighbor_mem[w]) && (Neighbor_mem[w] < Neighbor_positive_mem[v])) /*Check_matrix(i, Neighbor_mem[w], Neighbor_positive_mem[v]) == false*/
                {
                    _ans ++;
                    tag[i] = true;
                    tag[Neighbor_positive_mem[v]] = true;
                    tag[Neighbor_mem[w]] = true;
                    
                }
            }
        }
    }   
    cout << "finish report" << endl;

    if(V_mem != nullptr)
        delete[] V_mem;
    if(V_positive_mem != nullptr)
        delete[] V_positive_mem;    
    for(int i = 0; i < Neighbor_mem.size(); i++)
        Neighbor_mem.pop_back();
    for(int i = 0; i < Neighbor_positive_mem.size(); i++)
        Neighbor_positive_mem.pop_back();
    return _ans;
}


void MGT::OutputEdgesInMemory()
{
    for(int i = 0; i < E_mem.size(); i++)
        cout << E_mem[i].from << " " << E_mem[i].to << endl;
}

void MGT::ModifyConstant(float x)
{
    c += x;
}

float MGT::GetConstant()
{
    return c;
}

bool MGT::IfEdgeEmpty()
{
    if(G -> E_star.empty())
        return true;
    return false;
}

int MGT::GetNumnode()
{
    return G -> numVertex;
}

void ReportOriginalResult()
{
    timeval tv;
    MGT* Test = new MGT;
    FILE* fp = fopen("./dataset/Snetrodame.txt", "r");
    Test -> _init_(fp); 
    long long c = 0;
    cout << "finish init" << endl;

    gettimeofday(&tv, NULL);
    int cctime = tv.tv_sec;
    int pos = Test -> LoadIntoMemory(0);
    cout << "Total Triangles: " << Test -> ReportTriangles() << "with constant: " << Test->GetConstant() << endl;
    gettimeofday(&tv, NULL);
    cout << "Triangles Counting: " << tv.tv_sec - cctime << endl;

    delete Test;
}

void ReportSuperResult()
{
    timeval tv;
    MGT* Test = new MGT;
    FILE* fp = fopen("./dataset/Snetrodame2.txt", "r");
    Test -> Super_init_(fp); 
    cout << "finish init" << endl;

    gettimeofday(&tv, NULL);
    int cctime = tv.tv_sec;
    int pos = Test -> LoadIntoMemory(0);
    cout << "finish report" << endl;
    cout << "Total Triangles: " << Test -> ReportSuperTriangles() << "with constant: " << Test->GetConstant() << endl;
    gettimeofday(&tv, NULL);
    cout << "Triangles Counting: " << tv.tv_sec - cctime << endl;

    delete Test;
}

int main()
{
    // origianl graph
    cout << "origianl" << endl;
    ReportOriginalResult();
    cout << "*****************************" << endl;
    // super graph
    cout << "contract" << endl;
    ReportSuperResult();
    return 0;
}