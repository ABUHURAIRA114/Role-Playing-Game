#include <exception>
using namespace std;

class out_of_space : public exception 
{
    string addWhat = "";

public:
    out_of_space(string addWhat) : addWhat(addWhat) {}

    const char* what() const noexcept 
    {
        return ("Out of space: " + addWhat).c_str();
    }
};

class empty_collection : public exception
{
    string addWhat = "";

public:
    empty_collection(string addWhat) : addWhat(addWhat) {}

    // Override the what() method to return your custom error message
    const char* what() const noexcept override 
    {
        return ("Empty collection: " + addWhat).c_str();
    }
};

class failed_execution : public exception
{
    string addWhat = "";

public:
    failed_execution(string addWhat) : addWhat(addWhat) {}

    // Override the what() method to return your custom error message
    const char* what() const noexcept override 
    {
        return ("Execution Failed : " + addWhat).c_str();
    }
};