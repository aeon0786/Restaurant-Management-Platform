#pragma once
#include <string>

using namespace std;

inline string clear = "\033[2J\033[1;1H";
enum Status {Disable, Enable};
class Restaurant
{
    private:
        string name,
            address,
            phone_number,
            additional_Details;
        int ID;
        Status status;
        unsigned int time;
    public:
        Restaurant (string = "",
                    string = "",
                    string = "",
                    int = 0,
                    unsigned int = 0,
                    string = "", 
                    Status = Disable
                    );
        void setName (string);
        void setAddress (string);
        void setPhoneNumber (string);
        void setAdditional (string);
        void setID (int);
        void setStatus (string);
        void setTime (unsigned int);

        void getInfo () const;
        string getName () const;
        string getAddress () const;
        string getPhoneNumber () const;
        string getAdditional () const;
        int getID () const;
        bool getStatus () const;
        unsigned int getTime () const;
};