#include "Viewer/core/Viewer.h"

#include <termio.h>
//#include <unistd.h>



int main()
{

    Viewer client("127.0.0.1", 8888);

    client.StarReceive();


    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    char cmd;
    while(read(STDIN_FILENO, &cmd, 1))
    {
        if(cmd == 'q')break;
        if(cmd == 'a')client.IndexShift_L();
        if(cmd == 'd')client.IndexShift_R();
        //else client.SetFilter(cmd);
        system("clear");
        client.ShowByCategory();
        
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);


    printf("所有的标签:\n");
    for(auto v:client.getset())
    {
        std::cout<<v<<" , ";
    }

    
    

    
    return 0;
}
