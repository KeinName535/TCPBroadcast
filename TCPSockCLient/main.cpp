#include<iostream>
#include<sockpp/version.h>

int main(int argc, char const *argv[])
{
    std::cout<<sockpp::SOCKPP_VERSION;
    std::cout<<"Client starting..."<<std::endl;
    return 0;
}
