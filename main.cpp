#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sstream>
#include <iterator>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <cstddef>
#include <numeric>
#include <functional>
using namespace std;

int main()
{
    const string filePath = "/bin/"; // Helps locate where the Linux command will reside
    while(true)
    {
        string userData; // User inputted command
        vector<char *> arguments; // Arguments provided by the user, which will be parsed from userData

        // Prompts user to input commands
        cout << "\nEnter the command and arguments you want to run: " << endl;
        // Get the full user input and store it in a variable
        getline(cin, userData); // The entire user input (all on one line)

        // Check to be sure the user didn't input nothing
        if (std::all_of(userData.begin(),userData.end(),[](char wh){ return std::isspace(static_cast<unsigned char>(wh)); }))
        {
            cout << "You did not input a command please try again: " << endl; //Inform the user
            continue; // Go to next iteration
        }

        // Parse user input
        istringstream inputStream(userData); // Create input stream out of the user input
        vector<string> parts{istream_iterator<string>{inputStream}, istream_iterator<string>{}}; // Iterate through input stream and create new String vector with each individual word

        // If the user enters in "exit", terminate the shell
        if (parts.front() == "exit")
            exit(0); // Exit with EXIT_SUCCESS returned to the host environment

        // Set up fileName and arguments for passing into the execvp method in the child process
        char * fileName = const_cast<char *>((filePath + parts.front()).c_str()); // Concatenate the fileName (i.e. "/bin/") with what the user's first input was (i.e. "pwd")

        for (auto const& item: parts)
        {
            arguments.push_back(const_cast<char *>(item.c_str())); // Place each argument into your char *arguments vector
        }
        arguments.push_back(nullptr); //Put null pointer at the end to signify it is the end of the command

        // Special case for handling cd
        if (parts.front() == "cd")
        {
            int cdState = chdir(arguments[1]); // Attempt to change the directory to the second element of your arguments
            if(cdState < 0) // If state is less than zero, the directory change was unsuccessful.
                cout << "Your directory change failed!" << endl;
            else // Directory change was successful
                cout << "Your directory change was successful!" << endl;
            continue; // Go to next iteration and take in user input for the Shell (or another cd command)
        }
        // Prepare to fork() off child process
        int pid, status; // Process ID and status for potential checks

        pid = fork(); // Fork child process

        if (pid < 0)
        { // Error during creation
            cout << "Error creating child process" << endl;
            exit(1); // exit (unsuccessful code)
        }

        // Child process
        if (pid == 0) {
            execvp(fileName, arguments.data()); // We pass file name and arguments
            //Below code is only ran if there is an error running execvp
            perror(fileName); // Handles erroneous file input easily
            exit(1); // Exit this child process with an unsuccessful exit code
        }
        else // Parent process
           waitpid(pid, &status, 0);  // Busy wait until the child process terminates (shell completion). A wait(&status) would accomplish the same thing
    }
}
