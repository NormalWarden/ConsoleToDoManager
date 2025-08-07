#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#define FILE_NAME "tasks.txt"

void readAll(std::fstream& file);
void newTask(std::fstream& file, std::string description, int priority);
void changeTask(std::fstream& file, int lineNumber, std::string description, int priority);
void deleteTask(std::fstream& file, int lineNumber);
std::string textCleanCarriageReturn(std::string text);
void fileFill(std::fstream& file, std::string text);

int main()
{
    std::fstream fileMain;
    fileMain.open(FILE_NAME, std::ios::in | std::ios::out);

    if (!fileMain.is_open() or fileMain.fail())
    {
        std::cout << "File can't be open. Creating new...";
        std::ofstream fileCreater(FILE_NAME);
        fileCreater.close();
        fileMain.open(FILE_NAME, std::ios::in | std::ios::out);
    }

    std::string taskDescription;
    int taskPriority{}, selectedLine{}, changeMode{};

    char answer{};
    do
    {
        std::cout << "What do you want:\n"
            << "1. Read ToDo list\n"
            << "2. Write new task\n"
            << "3. Change task\n"
            << "4. Delete task\n"
            << "5. Exit\n";
        std::cin >> answer;

        taskDescription = "0";
        taskPriority = 0;
        selectedLine = 0;
        changeMode = 0;

        switch (answer)
        {
        case '1': // read tasks
            readAll(fileMain);
            break;
        case '2': // write new task
            std::cout << "Enter the task description: ";
            std::getline(std::cin >> std::ws, taskDescription);
            std::cout << "Enter the task priority: ";
            std::cin >> taskPriority;
            newTask(fileMain, taskDescription, taskPriority);
            break;
        case '3': // change task
            std::cout << "Which task do you want change (0 if no one): ";
            std::cin >> selectedLine;
            std::cout << "What do you want to change (task description = 1, task priority = 2, all = 3): ";
            std::cin >> changeMode;

            switch (changeMode)
            {
            case 1: // change only task description
                std::cout << "Enter the task description: ";
                std::getline(std::cin >> std::ws, taskDescription);
                break;
            case 2: // change only task priority
                std::cout << "Enter the task priority: ";
                std::cin >> taskPriority;
                break;
            case 3: // change task description and priority
                std::cout << "Enter the task description: ";
                std::getline(std::cin >> std::ws, taskDescription);
                std::cout << "Enter the task priority: ";
                std::cin >> taskPriority;
                break;
            default:
                std::cout << "Invalid input";
            }

            changeTask(fileMain, selectedLine, taskDescription, taskPriority);
            break;
        case '4': // delete task
            std::cout << "Which task do you want delete (0 if no one): ";
            std::cin >> selectedLine;
            deleteTask(fileMain, selectedLine);
            break;
        case '5': // exit
            break;
        default:
            std::cout << "Enter your answer like number from 1 to 5\n";
        }
    } while (answer != '5');

    fileMain.close();
    return 0;
}

void readAll(std::fstream& file)
{
    file.clear();
    file.seekg(0, std::ios::beg);
    std::string lineIter;
    int i{ 0 };

    std::cout << "------------------------------\n";
    while (std::getline(file, lineIter))
    {
        ++i;
        std::cout << i << ". " << lineIter << "\n";
    }
    std::cout << "------------------------------\n";
}

void newTask(std::fstream& file, std::string description, int priority)
{
    file.clear();
    file.seekp(0, std::ios::end);

    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // get raw time
    file << description << " | " << "priority: " << priority << " | " << std::ctime(&time);
}

void changeTask(std::fstream& file, int lineNumber, std::string description, int priority)
{
    // if no choice
    if (description == "0" and priority == 0)
        return;

    file.clear();
    file.seekg(0, std::ios::beg);

    std::string lineIter, lineRes;
    int i{ 0 };

    while (std::getline(file, lineIter))
    {
        ++i;
        if (lineNumber == i) // correct line for changing
        {
            if (description != "0") // user entered new description
                lineIter.replace(0, lineIter.find(" | "), description);
            if (priority != 0)  // user entered new priority
                lineIter.replace(lineIter.find(" | ") + 13, lineIter.find(" | ", lineIter.find(" | ") + 3) - lineIter.find(" | ") - 13, std::to_string(priority)); // moving from first " | " to 13 positions for changing priority. 13 = 3 (" | ") + 10 ("priority: ")
        }

        lineRes += lineIter;
        lineRes += "\n";
    }

    fileFill(file, textCleanCarriageReturn(lineRes));
}

void deleteTask(std::fstream& file, int lineNumber)
{
    // if no choice
    if (lineNumber == 0)
        return;

    file.clear();
    file.seekg(0, std::ios::beg);

    std::string lineIter, lineRes;
    int i{ 0 };

    while (std::getline(file, lineIter))
    {
        ++i;
        if (lineNumber != i) // selected line don't move to the new file but other do it
        {
            lineRes += lineIter;
            lineRes += "\n";
        }
    }

    fileFill(file, textCleanCarriageReturn(lineRes));
}

// anomaly can appear after work with lines: in the mid of line \r can be created. that function skip it and write correct version in file
std::string textCleanCarriageReturn(std::string text)
{
    for (int j = 1; j < text.size(); j++) // deleting '\r'
    {
        if (text[j - 1] == '\\' and text[j] == 'r')
            text.replace(j - 1, 2, "");
    }

    return text;
}

void fileFill(std::fstream& file, std::string text)
{
    file.close();

    file.open(FILE_NAME, std::ios::out | std::ios::trunc); // clear file from text
    file.close();

    file.open(FILE_NAME, std::ios::in | std::ios::out); // normal mode
    if (!file.is_open() or file.fail())
    {
        std::cout << "File can't be open. Creating new...";
        std::ofstream fileCreater(FILE_NAME);
        fileCreater.close();
        file.open(FILE_NAME, std::ios::in | std::ios::out);
    }

    file.clear();
    file.seekp(0, std::ios_base::beg);

    file << text;
}
