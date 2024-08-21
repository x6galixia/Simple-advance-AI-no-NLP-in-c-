#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>
#include <limits>
#include <random>

std::string toLower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string removePunctuation(const std::string &str)
{
    std::string result;
    std::locale loc;
    for (char c : str)
    {
        if (!std::ispunct(c, loc))
        {
            result += c;
        }
    }
    return result;
}

int levenshteinDistance(const std::string &s1, const std::string &s2)
{
    int m = s1.length();
    int n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i)
    {
        dp[i][0] = i;
    }

    for (int j = 0; j <= n; ++j)
    {
        dp[0][j] = j;
    }

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
        }
    }

    return dp[m][n];
}

std::string findRootWord(const std::string &word)
{
    // Simple stemming: just remove common suffixes
    if (word.length() >= 7 && word.substr(word.length() - 7) == "dreaming")
    {
        return word.substr(0, word.length() - 4); // remove "ing"
    }
    else if (word.length() >= 6 && word.substr(word.length() - 6) == "dreams")
    {
        return word.substr(0, word.length() - 1); // remove "s"
    }
    return word;
}

std::pair<std::string, std::string> findClosestQuestion(const std::string &userInput, const std::map<std::string, std::string> &answers)
{
    std::string closestQuestion;
    int minDistance = std::numeric_limits<int>::max();

    // Exact match for certain keywords
    if (userInput == "creator")
    {
        return {userInput, answers.at(userInput)}; // Return the exact keyword and its answer
    }

    std::string rootUserInput = findRootWord(userInput);

    for (const auto &pair : answers)
    {
        std::string rootQuestion = findRootWord(pair.first);
        int distance = levenshteinDistance(rootUserInput, rootQuestion);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestQuestion = pair.first;
        }
    }

    return {closestQuestion, answers.at(closestQuestion)};
}

int main()
{
    std::ifstream file("/storage/emulated/0/documents/AI.csv");
    std::map<std::string, std::string> answers;

    std::string line;
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string question, answer;
            if (std::getline(ss, question, ',') && std::getline(ss, answer))
            {
                answers[toLower(removePunctuation(question))] = answer;
            }
        }

        file.close();
    }
    else
    {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    std::string userInput;

    std::cout << "Hello! How can I help you today?\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        std::string userInputLower = toLower(removePunctuation(userInput));

        if (userInputLower.empty())
        {
            std::cout << "Please enter a valid input.\n";
            continue;
        }

        if (userInputLower == "hello" || userInputLower == "hi" || userInputLower == "hey" || userInputLower == "sup" || userInputLower == "wassup")
        {
            switch (dis(gen))
            {
            case 0:
                std::cout << "Hello!\n";
                break;
            case 1:
                std::cout << "Hi!\n";
                break;
            case 2:
                std::cout << "Hey there!\n";
                break;
            }
            continue;
        }

        auto [closestQuestion, answer] = findClosestQuestion(userInputLower, answers);
        if (!closestQuestion.empty())
        {
            if (closestQuestion != userInputLower)
            {
                std::cout << "Did you mean \"" << closestQuestion << "\"?\n";
            }
            std::cout << "The answer to your question \"" << closestQuestion << "\" is:\n";
            std::cout << answer << "\n";
        }
        else
        {
            std::cout << "I'm sorry, I didn't understand that.\n";
        }
    }

    return 0;
}
