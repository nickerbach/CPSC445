#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sstream>
#include <iterator>
#include <cstring>
#include <algorithm>

using namespace std;

int main() {
    const string strFilename = "/bin/"; // Prefix to where the Linux commands reside
    while(true) {
        string userInput; // Full initial user input (i.e. ls -l)
        vector<char *> arguments; // Arguments provided by the user, which will be parsed from userInput

        // Begin by prompting the user for input
        cout << "\nLinux Command Shell: Please enter in your command and arguments (if required): " << endl;
        // Get the full user input and store it in a variable
        getline(cin, userInput); // The entire user input (all on one line)

        // For safety, check to make sure the user didn't just enter in pure whitespace
        if (std::all_of(userInput.begin(),userInput.end(),[](char wh){ return std::isspace(static_cast<unsigned char>(wh)); }))
        {
            cout << "You neglected to enter a command, please try again: " << endl; //Inform the user
            continue; // Go to next iteration
        }

        // Otherwise, parse user input (split into words delimited by whitespace)
        istringstream inputStream(userInput); // Create input stream out of the user input
        vector<string> items{istream_iterator<string>{inputStream}, istream_iterator<string>{}}; // Iterate through input stream and create new String vector with each individual word

        // If the user enters in "exit", terminate the shell
        if (items.front() == "exit") // Terminate the program if exit is the input
            exit(0); // Exit with EXIT_SUCCESS returned to the host environment

        // Set up filename and arguments for passing into the execvp method in the child process
        char * filename = const_cast<char *>((strFilename + items.front()).c_str()); // Concatenate the filename (i.e. "/bin/") with what the user's first input was (i.e. "pwd")

        for (auto const& item: items) {
            arguments.push_back(const_cast<char *>(item.c_str())); // Place each argument into your char *arguments vector
        }
        arguments.push_back(nullptr); // Finally, the last portion of the vector will be a null pointer (a NULL terminator to tell it to end)

        // If the user enters in the cd (change directory) command, handle it separately from the forked Shell process
        if (items.front() == "cd") {
            int cdState = chdir(arguments[1]); // Attempt to change the directory to the second element of your arguments (i.e. cd /Documents)
            if(cdState < 0) // If state is less than zero, the directory change was unsuccessful.
                cout << "Directory Change Failed!" << endl;
            else // Else, the directory was changed successfully!
                cout << "Directory Change Successful!" << endl;
            continue; // Go to next iteration and take in user input for the Shell (or another cd command)
        }
        // Prepare to fork() off child process
        int pid, status; // Process ID and status for potential checks

        pid = fork(); // Fork child process

        if (pid < 0) { // Error during creation
            cout << "Error creating child process" << endl;
            exit(1); // exit (unsuccessful code)
        }

        // Child process
        if (pid == 0) {
            execvp(filename, arguments.data()); // We pass file name and arguments (filename will be, for instance, /bin/ls)
            // Below, the perror and exit methods run only if we experience a major error with execvp reading in the filename
            perror(filename); // Handles erroneous file input easily
            exit(1); // Exit this child process with an unsuccessful exit code
        }
        else // Parent process
           waitpid(pid, &status, 0);  // Busy wait until the child process terminates (shell completion). A wait(&status) would accomplish the same thing
    }
}
