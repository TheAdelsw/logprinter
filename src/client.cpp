#include "Viewer/core/Viewer.h"

#include <termios.h>
//#include <unistd.h>



int main()
{

    Viewer client("127.0.0.1", 8888);

    client.StartThread();

    
    // printf("所有的标签:\n");
    // for(auto v:client.getset())
    // {
    //     std::cout<<v<<" , ";
    // }


    
    return 0;
}
