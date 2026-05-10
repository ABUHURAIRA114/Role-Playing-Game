using namespace std;

class out_of_space 
{
    string msg = "";

public:
    out_of_space(string msg) : msg("Out of space: " + msg) {}

    const char* what() const  
    {
        return (msg).c_str();
    }
};

class empty_collection 
{
    string msg = "";

public:
    empty_collection(string msg) : msg("Empty collection: " + msg) {}

    // Override the what() method to return your custom error message
    const char* what() const   
    {
        return (msg).c_str();
    }
};

class failed_execution 
{
    string msg = "";

public:
    failed_execution(string msg) : msg("Execution Failed : " + msg) {}

    // Override the what() method to return your custom error message
    const char* what() const  
    {
        return (msg).c_str();
    }
};