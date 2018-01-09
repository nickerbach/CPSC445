#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sstream>
#include <iterator>
#include <cstring>
#include <algorithm>

using namespace std;

int main()
{
  const string fileName = "/bin/";
  while(true)
  {
    string temp; //user input
    vector<char *> arguments;

    cout<< "Enter the command and its arguments that you would like to run: " << endl;
    getline(cin,temp);

    if (std::all_of(temp.begin(),temp.end(),[](char wh){ return std::isspace(static_cast<unsigned char>(wh)); }))
        {
            cout << "You did not enter a command, try again: " << endl; //Inform the user
            continue; // Go to next iteration
        }
    istringstream inputStream(temp);
    vector<string> items{istream_iterator<string>{inputStream}, istream_iterator<string>{}};

    if(items.front() == "exit" || items.front() == "Exit")
    {
      exit(0);
    }

    char * filePath = const_cast<char *>((temp + items.front()).c_str());
    for (auto const& item: items)
    {
            arguments.push_back(const_cast<char *>(item.c_str())); // Place each argument into your char *arguments vector
    }
    arguments.push_back(nullptr);

    int pid,status;
    pid = fork();
    if(pid == 0)
    {
      execvp(filePath,arguments.data());
      perror(filePath);
      exit(1);
    }
    else
    {
      waitpid(pid,status,0);
    }
    if(pid < 0)
    {
      cout<< "Fork error!!"<< endl;
      exit(1);
    }
  }
}
