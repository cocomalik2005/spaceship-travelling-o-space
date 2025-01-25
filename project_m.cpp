#include <iostream> //input and output
#include <limits>   //Error handling
#include <fstream>  //files
#include <vector>   //vector arrays
#include <sstream>  //string streams
#include <ctime>    //time functions
#include <conio.h>  //using getch()
#ifdef _WIN32
#include <windows.h> // For Sleep() on Windows
#else
#include <unistd.h> // For usleep() on Unix-based systems
#endif
using namespace std;

void platform_sleep(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds); // Sleep for Windows (milliseconds)
#else
    usleep(milliseconds * 1000); // Sleep for Unix-based systems (microseconds)
#endif
}

// Constant Values
const double Fuel_container_Capacity[2] = {50000, 150000}; // Total capacity of fuel of space craft container (kg)
const float Fuel_consumption_Rate = 15;                    // fuel consum per million kilometer in space
const double distance_to_LEO = 2000;                       // Distance between earth and lower earth orbit
const double Escape_Velocity = 11.2;                       // The velocity require to escape earth gravitational pull

// Global Variables(scope is global)
bool fuel_insufficiency; // will be manipulated when fuel is insufficient

// Function for plannet selection
// Function to let the user select a planet based on available fuel
void PlanetSelection(float fuel_level);
// Function to calculate the fuel needed for a journey to a selected planet
void FuelCalculation(float, vector<string>, vector<double>, int);
// Function to handle the launch to lower Earth orbit
void Launch_To_Lower_Earth_Orbit(float, const vector<string> &PlanetName, const vector<double> &distance, int, int);
// Function to manage the travel towards the selected planet
void Travel_Towards_Destination(float, const vector<string> &PlanetName, const vector<double> &distance, int, int);
// Function to handle the landing process on the selected planet
void Landing(double, const vector<string> &PlanetName, const vector<double> &distance, int, int);
// Print a log message with its level (like INFO or ERROR)
void log(const string &message, const string &level);

int main()
{
    do
    {
        // Clearing the console First
#ifdef _WIN32
        system("cls"); // Clear screen for Windows
#endif

        fuel_insufficiency = false;
        float Starting_fuel_Level;// Declare a variable to store the starting fuel level
        cout << "\nWelcome to Space exploration";// Print a welcome message to the user
        cout << "\nEnter your spacecraft starting fuel level in percentage(80 for 80%) avoide Entering (%) Sign with fuel: ";
        cin >> Starting_fuel_Level;// Read the user's input for starting fuel level
        // Error handling manage invalid input
        while (cin.fail() || (Starting_fuel_Level < 1 || Starting_fuel_Level > 100))
        {
            cout << "\nInvalid input!\nEnter correct fuel level in percentage avoide Entering (%) Sign with fuel : ";
            cin.clear();// Clear the error flag on cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> Starting_fuel_Level;// tell the user to enter the fuel level again
        }

        PlanetSelection(Starting_fuel_Level); // show options to select

    } while (fuel_insufficiency);

    return 0;
}

// Definition of functions
void PlanetSelection(float fuel_level)
{

#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#endif
    // vectors Declaration
    vector<int> id;
    vector<string> PlanetName;
    vector<double> Distance;

    int choice;
    // Extracting sheet data to show menue
    cout << "\nChoose a planet to Explore: ";
    ifstream planetFile("Planets_Record.txt"); // opening file in reading mode

    if (!planetFile.is_open()) // checking is file open
    {
        cerr << "Error: Unable to open file!" << endl;
        return;
    }
    else
    {
        string line;// Declare a string variable to hold each line from the file
        while (getline(planetFile, line)) // Reading data line by line from sheet
        {
            stringstream ss(line);// Create a string stream to parse the line
            string idStr, planetNameStr, distanceStr;// Declare strings to hold parsed data

            // Separat each segment by comma and than store it into a string
            if (getline(ss, idStr, ',') && getline(ss, planetNameStr, ',') && getline(ss, distanceStr, ','))
            {
                // passing data to dynamic array;
                id.push_back(stoi(idStr)); // Convert the string ID to an integer and add it to the id vector
                PlanetName.push_back(planetNameStr); // Add planet name to the vector
                Distance.push_back(stod(distanceStr)); // Convert distance string to double and add to the vector 
                // Print the planet information to the console

                cout << "\n"
                     << idStr << ". " << planetNameStr << " (" << distanceStr << " Million kilometers)\n";
            }
            else
            {  // Print an error message if the line is malformed
                cerr << "Error: Malformed line in file!" << endl;
                return; // Exit the function if there's an error
            }
        }
        cout << "\nEnter:";// tell  the user to enter their choice
        cin >> choice; // Read the user's choice
        while (cin.fail() || (choice < 1 || choice > PlanetName.size()))// Check if input is invalid
        {
            cout << "\nInvalid input!\nchoose a positive number from the given Menue : ";// Prompt for valid input
            cin.clear();// Clear the error flag on cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> choice; // Read input again
        }
    }

    FuelCalculation(fuel_level, PlanetName, Distance, choice);

    return;
}

void FuelCalculation(float fuel_level, vector<string> PlanetName, vector<double> distance, int choice)
{

    int container;
    if (choice >= 1 && choice <= 5)
    {
        container = 0;
    }
    else
    {
        container = 1;
    }
    double LEO_Fuel = (Fuel_container_Capacity[container] / 100) * 60;

    double current_fuel = (Fuel_container_Capacity[container] / 100) * fuel_level;

    cout << "\nYour Current fuel is : " << current_fuel << "kg";

    cout << "\nThe fuel you are required to reach Lower Earth orbit is : " << LEO_Fuel << "kg";

    if (current_fuel == LEO_Fuel) // 60% fuel is required to reach Lower Earth Orbit just
    {
        cout << "\nYour Spacecraft can only reach Lower Earth Orbit!\nplease increase your fuel level!\nPress any key to restart ";
        fuel_insufficiency = true;
        getch();
        Beep(500, 200);
        return;
    }
    else if (current_fuel > LEO_Fuel) // Calculate if fuel level is greater than fuel required to reach Lower Earth Orbit
    {
        double Remaining_fuel = current_fuel - LEO_Fuel;
        cout << "\nAfter Reaching Lower Earth orbit Remaining fuel would be : " << Remaining_fuel << "kg";

        double Required_fuel = distance[choice - 1] * Fuel_consumption_Rate; // Required fuel for space travel
        cout << "\nThe required fuel to travel toward " << PlanetName[choice - 1] << " is : " << Required_fuel << " kg";

        if (Remaining_fuel >= Required_fuel)
        {
            char Launch;
            cout << "\nDo you want to initiate launch Sequence Press Y \nPress any key to Exist: ";
            cin >> Launch;
            Launch = toupper(Launch);
            if (Launch == 'Y')
            {
                Launch_To_Lower_Earth_Orbit(fuel_level, PlanetName, distance, choice, container);
            }
            else
            {
                cout << "\n";
                Beep(750, 300);
                for (int i = 1; i <= 50; i++) // Line Animation with delay
                {
                    cout << "=" << flush;
                    platform_sleep(20);
                }
                cout<<"\nExist Sucessfully!\npress any key to restart";
                fuel_insufficiency = true;
                getch();
                Beep(500,200);
                return;
            }
        }
        else
        {
            cout << "\nInsuficient fuel to travel " << PlanetName[choice - 1] << "\nIncrease your fuel level!\nPress any key to restart ";
            fuel_insufficiency = true;
            getch();
            Beep(500, 200);

            return;
        }
    }
    else
    {

        cout << "\nInsuficient fuel to reach Lower Earth Orbit\nIncrease your fuel level!\npress any key to Start";
        fuel_insufficiency = true;
        getch();
        Beep(500, 200);
    }

    return;
}

void Launch_To_Lower_Earth_Orbit(float fuel, const vector<string> &PlanetName, const vector<double> &distance, int choice, int container)
{
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#endif
    log("\n================initiating Launch for " + PlanetName[choice - 1] + "==================\n", "launch");

    int current_fuel_level = (Fuel_container_Capacity[container] / 100) * fuel;
    cout << "\nYour current fuel level is : " << current_fuel_level << "kg " << fuel << "% of Fuel container.";
    cout << "\nFuel require to reach Lower Earth Orbit : " << (Fuel_container_Capacity[container] / 100) * 60 << "kg";

    cout << "\nDistance from Earth surface  to Lower Earth Orbit : " << distance_to_LEO << "km";
    cout << "\nEscape Velocity Require " << Escape_Velocity << "km/s";
    cout << "\nLaunch Sequence initiated for " << PlanetName[choice - 1] << ".\nLoading:";
    log("Launch Sequence initiated", "launch");
    Beep(750, 500);
    for (int i = 0; i <= 100; i++)
    {
        cout << "\rLoading: " << i << "%";
        platform_sleep(30);
    }
    cout << "\nLaunch in : \n";
    for (int i = 1; i <= 3; i++)
    {
        cout << i << "...\n";
        Beep(1000, 500);
        platform_sleep(30);
    }
    // Animations
    double escape_fuel = (Fuel_container_Capacity[container] / 100) * 60;
    float Decreament = escape_fuel / 100;
    double Distance = 0;
    double Velocity = 0;
    char fuel_bar[10] = {'|', '|', '|', '|', '|', '|', '|', '|', '|', '|'};
    char increase_bar[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    int consume = 9;
    cout << endl;
    for (int i = 1; i <= 100; i++)
    {

        Beep(400, 200);
        cout << "\rFuel:[";
        for (int k = 0; k < 10; k++)
        {
            cout << fuel_bar[k];
        }
        cout << "]" << escape_fuel << "kg\t\t";
        cout << "Velocity:[";
        for (int v = 0; v < 10; v++)
        {
            cout << increase_bar[v];
        }
        cout << "]" << Velocity << "km/s";
        cout << "\t\tDistance:[";
        for (int d = 0; d < 10; d++)
        {
            cout << increase_bar[d];
        }
        cout << "]" << Distance << "km";

        if (i > 1 && (i % 10 == 0))
        {
            fuel_bar[consume] = ' ';
            increase_bar[(i / 10) - 1] = '|';
            consume--;
        }
        platform_sleep(30);
        escape_fuel -= Decreament;
        Velocity += 0.112;
        Distance += 20;
    }

    cout << "\r" << string(100, ' ') << "\r";
    cout << "Fuel:[          ] 0kg\t\t";
    cout << "Velocity:[||||||||||] " << Velocity << "km/s\t\t";
    cout << "Distance:[||||||||||] " << Distance << "km" << endl;
    cout << "\nSuccessfully Reached Lower Earth Orbit!";
    Beep(750, 500);
    log("Launch Sequence Sucessfully operated", "launch");
    log("Reached Lower Earth Orbit ", "launch");
    platform_sleep(30);
    Travel_Towards_Destination(fuel, PlanetName, distance, choice, container);

    return;
}

void Travel_Towards_Destination(float fuel, const vector<string> &PlanetName, const vector<double> &distance, int choice, int container)
{
#ifdef _WIN32
    system("cls"); // Clear screen for Windows
#endif
    log("\n================ Travelling to " + PlanetName[choice - 1] + "==================\n", "Travel");
    double current_fuel_level = ((Fuel_container_Capacity[container] / 100) * fuel) - ((Fuel_container_Capacity[container] / 100) * 60);
    cout << "\nYour current fuel level is : " << current_fuel_level << "kg ";
    cout << "\nFuel require to Travel : " << PlanetName[choice - 1] << " " << distance[choice - 1] * Fuel_consumption_Rate << "kg";
    cout << "\nDistance from Lower Earth Orbit to : " << PlanetName[choice - 1] << " is : " << distance[choice - 1] << "km";
    double Space_Travel_Velocity = 4;
    cout << "\nSpace travel velocity " << Space_Travel_Velocity << "km/s";

    cout << "\nDirecting Towards Destination!";
    platform_sleep(5);
    cout << "\nDirecting Towards " << PlanetName[choice - 1] << " .\nDirecting:";
    Beep(750, 500);
    for (int i = 0; i <= 100; i++)
    {
        cout << "\rDirecting: " << i << "%";
        platform_sleep(30);
    }
    cout << "\nSpaceCraft Successfully Directed toward " << PlanetName[choice - 1];
    log("Spacecraft Direction Setting", "Travel");
    platform_sleep(5);
    cout << "\nTravelling Towards " << PlanetName[choice - 1] << "...\n\n";
    platform_sleep(15);
    double Distance = 0;
    char Decrease_bar[10] = {'|', '|', '|', '|', '|', '|', '|', '|', '|', '|'};
    char increase_bar[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    float increament = distance[choice - 1] / 100;
    float Decreament = current_fuel_level / 100;
    int consume = 9;
    log("SpaceCraft journey started toward " + PlanetName[choice - 1] + ".", "Travel");

    for (int i = 1; i <= 100; i++)
    {
        Beep(400, 200);
        cout << "\rFuel:[";
        for (int k = 0; k < 10; k++)
        {
            cout << Decrease_bar[k];
        }
        cout << "]" << current_fuel_level << "kg\t\t";
        cout << "\t\tDistance:[";
        for (int d = 0; d < 10; d++)
        {
            cout << increase_bar[d];
        }
        cout << "]" << Distance << "km";

        if (i > 1 && (i % 10 == 0))
        {
            Decrease_bar[consume] = ' ';
            increase_bar[(i / 10) - 1] = '|';
            consume--;
        }
        platform_sleep(10);
        current_fuel_level -= Decreament;
        Distance += increament;
    }
    cout << "\r" << string(100, ' ') << "\r";
    cout << "Fuel:[          ] 0kg\t\t";
    cout << "Distance:[||||||||||] " << Distance << "km" << endl;
    cout << "\nSuccessfully Reached " << PlanetName[choice - 1] << "!";
    Beep(750, 500);
    log("SpaceCraft Successfully Reach its Destination", "Travel");
    log("Reached " + PlanetName[choice - 1] + ".", "Travel");
    platform_sleep(10);
    Landing(current_fuel_level, PlanetName, distance, choice, container);
    return;
}

void Landing(double current_fuel, const vector<string> &PlanetName, const vector<double> &distance, int choice, int container_num)
{
#ifdef _WIN32
      system("cls"); // Clear screen for Windows
#endif
    log("\n================ Landing on " + PlanetName[choice - 1] + "==================\n", "Landing");
    cout << "\nYou have reached to " << PlanetName[choice - 1] << "\nDistance " << distance[choice - 1] << " km from Earth.";
    platform_sleep(20);
    cout << "\nReadying SpaceCraft for Landing.";
    log("Ready spaceCraft for landing", "Landing");
    cout << "\nLanding process initiating.";
    log("Landing sequence initiated", "Landing");
    cout << "\nLanding Process:";
    Beep(750, 500);
    for (int i = 0; i <= 100; i++)
    {
        cout << "\rLanding Process: " << i << "%";
        platform_sleep(30);
    }
    platform_sleep(15);
    Beep(750, 500);
    platform_sleep(15);
    log("successfully Landing on " + PlanetName[choice - 1] + ".", "Landing");
    cout << "\nEntering " << PlanetName[choice - 1] << " Atmosphere.";
    Beep(750, 500);
    platform_sleep(15);
    cout<<"\nLanded Successfully!";

}

void log(const string &message, const string &level)
{
    ofstream Log_file("log.txt", ios::app);

    if (Log_file.is_open())
    {
        time_t t = time(nullptr);// Get the current time
        tm *tm = localtime(&t); // Convert the time to local time format
        char timeStr[100]; // Declare a character array to hold the formatted time string
        // Format the time as "YYYY-MM-DD HH:MM:SS"
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
        // Write the timestamp, log level, and message to the log file
        Log_file << "[" << timeStr << "] [" << level << "] " << message << endl;
        Log_file.close(); // Close the log file
    }
    else
    {
        // Print an error message to the console if the file cannot be opened
        cerr << "Error: Unable to open file!" << endl;
    }
    return;// Exit the function
}