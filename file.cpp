#include <iostream>
#include <fstream>
#include <vector>
// #include <opencv2/opencv.hpp> 
// using namespace cv; 
using namespace std; 
  
// Driver code 

int main()
{
            // string s = "dediejdiej ";
            // int f = 412;
            // s += (string) 412;
            // cout << s << endl;
    ifstream in("./test4/ved.mp4");
    if (in.is_open())
    {
        in.seekg(0, ios::end);
        while (1)
        {
            char buffer[1024];
            in.read(buffer, 1024);
            if (in.eof())
            {
                cout << "Rah Salala\n";
                break;
            }
        }
    }
}