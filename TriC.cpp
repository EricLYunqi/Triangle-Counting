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
                // for(int i = 0; i < numnode+10; i++)
                //     gralist[i].clear();

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
                // for(int i = 0; i < numnode+10; i++)
                //     gralist[i].clear();
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

            //cout << hash_table[from] << " " << hash_table[to] << endl;
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
}

void MGT::Super_init_(FILE* fp)
{
    G -> creatSupergraph(fp);
    cout << "finish build graph" << endl;
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
}

int MGT::LoadIntoMemory(int pos)
{
    int P = pos;
    cout << "start load" << endl;
    E_mem.clear();
    vector<Edge>().swap(E_mem);

    for(int i = pos+1; i < G -> numVertex; i++)
    {
        //cout << i << " " << Neighbor[i].size() << endl;
        if(Neighbor[i].size() + E_mem.size() <= static_cast<int>(c * G -> numEdge))
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
        else
        {
            cout << "overflow" << endl;
            break;
        }
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

long long MGT::ReportTriangles()
{
    long long _ans = 0;
    bool* V_mem = new bool[2*(G -> numVertex+3)];
    //memset(V_mem, false, sizeof(V_mem));
    bool* V_positive_mem = new bool[2*(G -> numVertex+3)];
    //memset(V_positive_mem, false, sizeof(V_positive_mem));
    for(int i = 0; i < 2*(G -> numVertex+3); i++)
    {
        V_mem[i] = false;
        V_positive_mem[i] = false;
    }

    for(int i = 0; i < E_mem.size(); i++)
    {
        if(!V_mem[E_mem[i].from])
        {
            V_mem[E_mem[i].from] = true;
            
        }
        if(!V_mem[E_mem[i].to])
            V_mem[E_mem[i].to] = true;
        if(!V_positive_mem[E_mem[i].from])
            V_positive_mem[E_mem[i].from] = true;
    }
    cout << "star report" << endl;
    vector<int> Neighbor_mem; vector<int> Neighbor_positive_mem;
    for(int i = 0; i < G -> numVertex; i++)
    {
        //cout << i << endl;
        Neighbor_mem.clear(); Neighbor_positive_mem.clear();
        Neighbor_mem.resize(0); Neighbor_positive_mem.resize(0);

        //cout << i << endl;

        for(int j = 0; j < Neighbor[i].size(); j++)
            if(V_mem[Neighbor[i][j]])
                Neighbor_mem.push_back(Neighbor[i][j]);
        for(int j = 0; j < Neighbor_mem.size(); j++)
            if(V_positive_mem[Neighbor_mem[j]])
                Neighbor_positive_mem.push_back(Neighbor_mem[j]);

        /*cout << i << " information" << endl;
        for(int j = 0; j < Neighbor_mem.size(); j++)
            cout << Neighbor_mem[j] << " ";
        cout << endl;
        for(int j = 0; j < Neighbor_positive_mem.size(); j++)
            cout << Neighbor_positive_mem[j] << " ";
        cout << endl;*/

        

        for(int v = 0; v < Neighbor_positive_mem.size(); v++)
        {
            for(int w = 0; w < Neighbor_mem.size(); w++)
            {
                if(Neighbor_positive_mem[v] != Neighbor_mem[w] && IfEdgeContained(Neighbor_positive_mem[v], Neighbor_mem[w]) == true)
                {
                    _ans ++;
                    tag[i] = true;
                    tag[Neighbor_positive_mem[v]] = true;
                    tag[Neighbor_mem[w]] = true;
                }
            }
        }
        //cout << "here" << endl;
        //_ans += (Neighbor_mem.size()-Neighbor_positive_mem.size()) * Neighbor_positive_mem.size();
       //_ans += ((Neighbor_positive_mem.size()) * (Neighbor_positive_mem.size()-1) / 2);
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

    //cout << _ans << endl;
    return _ans;
}

long long MGT::ReportSuperTriangles()
{
    long long _ans = 0;
    bool* V_mem = new bool[2*(G -> numVertex+3)];
    //memset(V_mem, false, sizeof(V_mem));
    bool* V_positive_mem = new bool[2*(G -> numVertex+3)];
    //memset(V_positive_mem, false, sizeof(V_positive_mem));
    for(int i = 0; i < 2*(G -> numVertex+3); i++)
    {
        V_mem[i] = false;
        V_positive_mem[i] = false;
    }

    for(int i = 0; i < E_mem.size(); i++)
    {
        if(!V_mem[E_mem[i].from])
        {
            V_mem[E_mem[i].from] = true;
            
        }
        if(!V_mem[E_mem[i].to])
            V_mem[E_mem[i].to] = true;
        if(!V_positive_mem[E_mem[i].from])
            V_positive_mem[E_mem[i].from] = true;
    }
    cout << "star report" << endl;
    vector<int> Neighbor_mem; vector<int> Neighbor_positive_mem;
    for(int i = 0; i < G -> numVertex; i++)
    {
        //cout << i << endl;
        Neighbor_mem.clear(); Neighbor_positive_mem.clear();
        Neighbor_mem.resize(0); Neighbor_positive_mem.resize(0);

        //cout << i << endl;

        for(int j = 0; j < Neighbor[i].size(); j++)
            if(V_mem[Neighbor[i][j]])
                Neighbor_mem.push_back(Neighbor[i][j]);
        for(int j = 0; j < Neighbor_mem.size(); j++)
            if(V_positive_mem[Neighbor_mem[j]])
                Neighbor_positive_mem.push_back(Neighbor_mem[j]);

        /*for(int j = 0; j < Neighbor_mem.size(); j++)
            cout << Neighbor_mem[j] << " ";
        cout << endl;
        for(int j = 0; j < Neighbor_positive_mem.size(); j++)
            cout << Neighbor_positive_mem[j] << " ";
        cout << endl;*/

        

        for(int v = 0; v < Neighbor_positive_mem.size(); v++)
        {
            for(int w = 0; w < Neighbor_mem.size(); w++)
            {
                if(Neighbor_positive_mem[v] != Neighbor_mem[w] && IfEdgeContained(Neighbor_positive_mem[v], Neighbor_mem[w]) == true
                    && ((super_id[i] != super_id[Neighbor_positive_mem[v]]) && (super_id[i] != super_id[Neighbor_mem[w]]) && 
                    super_id[Neighbor_positive_mem[v]] != super_id[Neighbor_mem[w]]))
                {
                    _ans ++;
                    tag[i] = true;
                    tag[Neighbor_positive_mem[v]] = true;
                    tag[Neighbor_mem[w]] = true;
                }
            }
        }
        //cout << "here" << endl;
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

    //cout << _ans << endl;
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
    gettimeofday(&tv, NULL);
    int cctime = tv.tv_sec;
    MGT* Test = new MGT;
    int pos = -1;
    FILE* fp = fopen("./dataset/netrodame.txt", "r");
    Test -> _init_(fp); 
    long long c = 0;

    cout << "finish init" << endl;
    gettimeofday(&tv, NULL);
    cctime = tv.tv_sec;
    pos = Test -> LoadIntoMemory(pos);
    c = Test -> ReportTriangles();
    Test -> ans += c;
    cout << "finish report" << endl;
    cout << "Total Triangles: " << Test -> ans << "with constant: " << Test->GetConstant() << endl;
    gettimeofday(&tv, NULL);
    cout << "Triangles Counting: " << tv.tv_sec - cctime << endl;

    delete Test;
}

void ReportSuperResult()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    int cctime = tv.tv_sec;
    MGT* Test = new MGT;
    int pos = -1;
    FILE* fp = fopen("./dataset/Snetrodame.txt", "r");
    Test -> Super_init_(fp); 

    cout << "finish init" << endl;

    gettimeofday(&tv, NULL);
    cctime = tv.tv_sec;
    pos = Test -> LoadIntoMemory(pos);
    Test -> ans += Test -> ReportSuperTriangles();
    cout << "finish report" << endl;
    cout << "Total Triangles: " << Test -> ans << "with constant: " << Test->GetConstant() << endl;
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