#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    ifstream fp1;
    ofstream fp2;
    fp1.open("./dataset/test3.txt");
    fp2.open("./dataset/test2.txt");
    int times = 0;
    while(true)
    {
        char f; 
        fp1 >> f;
        if(f == 't')
        {
            if(times == 0)
            {
                int x,y;
                fp1 >> x >> y;
                fp2 << "t # 0" << endl;
                fp2 << "25 38 30" << endl;
                times ++;
            }
            else
            { 
                fp2 << "t # -1" << endl;
                break;
            }               
        }
        else if(f == 'v')
        {
            int u,l;
            for(int i = 0; i < 25; i++)
            {
                fp1 >> u >> l;
                fp2 << "v " << u-1 << " " << l << endl;
            }
        }
        else if(f == 'e')
        {
            int u, v;
            for(int i = 0; i < 37; i++)
            {
                fp1 >> u >> v;
                fp2 << "e " << u-1 << " " << v-1 << " 1" << endl;
            }
        }
    }

    fp1.close(); fp2.close();
}