#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>  // For C-string functions
#include <limits>   // For clearing cin buffer

using namespace std;

const int INITIAL_LETTERS = 5;
const int DISPLAY_DURATION = 3;
const int PASS_THRESHOLD = 100;
const int MAX_ATTEMPTS = 3;

struct ScoreTracker {
    int numChars;
    vector<int> scores;

    int calculateMean() const {
        int sum = 0;
        for (int score : scores) {
            sum += score;
        }
        return scores.empty() ? 0 : sum / scores.size();
    }
};

class MemoryTest {
private:
    int numChars;
    int failedAttempts;
    string medication;
    vector<ScoreTracker> scoreHistory;

public:
    MemoryTest() : numChars(INITIAL_LETTERS), failedAttempts(0) {}

    void start() {
    cout << "Enter medication information or comments: ";
    getline(cin, medication);

    while (true) {
        char sequence[numChars];
        char userInput[30];  // 30 characters to capture any excessive input

        generateRandomSequence(sequence);
        displaySequence(sequence);

        // Get user input, abort if the user enters "abort"
        if (!getUserInput(userInput, numChars)) {
            // Ask to retry after abort
            if (!confirmRetry()) break;
            system("cls");  // Clear the screen before starting the next sequence
            continue;  // Skip to the next iteration (restart the loop)
        }

        // Calculate and display the score
        int correct = calculateScore(sequence, userInput);
        int score = (correct * 100) / numChars;
        cout << "You scored: " << score << "%\n";

        adjustDifficulty(score);

        int meanScore = updateScoreHistory(score);
        logResult(score, meanScore);

        // Prompt to try again
        if (!confirmRetry()) break;

        system("cls");  // Clear the screen before starting the next sequence
    }
}

    void generateRandomSequence(char sequence[]) {
        const char validChars[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
        srand(time(0));
        for (int i = 0; i < numChars; ++i) {
            sequence[i] = validChars[rand() % (sizeof(validChars) - 1)];
        }
    }

    void displaySequence(const char sequence[]) {
        cout << "Memorize this sequence: ";
        for (int i = 0; i < numChars; ++i) {
            cout << sequence[i] << " ";
        }
        cout << endl;
        delay(DISPLAY_DURATION);
        system("cls");
    }

    bool getUserInput(char userInput[], int numChars) {
    cout << "Enter the characters you remember (up to " << numChars << " characters) or type 'abort' to cancel: ";
    cin.getline(userInput, 30);  // Reads up to 30 characters

    // Check for "abort" entry
    if (strcmp(userInput, "abort") == 0) {
        cout << "Test aborted.\n";
        return false;  // Indicate that the test was aborted
    }

    // Display what the user entered
    cout << "You entered:      ";
    for (int i = 0; i < strlen(userInput); ++i) {
        cout << userInput[i] << " ";
    }
    cout << endl;

    return true;  // Test continues
}

    int calculateScore(const char sequence[], const char userInput[]) {
        int correct = 0;
        cout << "\nCorrect sequence: ";
        for (int i = 0; i < numChars; ++i) {
            cout << sequence[i] << " ";
            if (toupper(userInput[i]) == sequence[i]) {
                ++correct;
            }
        }
        cout << endl;
        return correct;
    }

    void adjustDifficulty(int score) {
        if (score >= PASS_THRESHOLD) {
            if (numChars == 5) numChars = 7;
            else if (numChars == 7) numChars = 9;
            else if (numChars == 9) numChars = 10;
            failedAttempts = 0;
            cout << "Great job! Increasing difficulty to " << numChars << " characters.\n";
        } else {
            ++failedAttempts;
            if (failedAttempts >= MAX_ATTEMPTS && numChars > INITIAL_LETTERS) {
                if (numChars == 11) numChars = 9;
                else if (numChars == 9) numChars = 7;
                else if (numChars == 7) numChars = 5;
                failedAttempts = 0;
                cout << "Difficulty reduced to " << numChars << " characters.\n";
            } else {
                cout << "Try again at " << numChars << " characters.\n";
            }
        }
    }

    int updateScoreHistory(int score) {
        bool foundTracker = false;
        int meanScore = -1;

        for (ScoreTracker& tracker : scoreHistory) {
            if (tracker.numChars == numChars) {
                tracker.scores.push_back(score);
                foundTracker = true;
                if (tracker.scores.size() == 5) {
                    meanScore = tracker.calculateMean();
                    tracker.scores.clear();
                }
                break;
            }
        }

        if (!foundTracker) {
            ScoreTracker newTracker = { numChars, { score } };
            scoreHistory.push_back(newTracker);
        }

        return meanScore;
    }

    void logResult(int score, int meanScore) {
        char datetime[20];
        getCurrentTime(datetime, sizeof(datetime));
        
        ofstream file("memory_test_results.txt", ios::app);
        file << datetime << " - " << score << "% score on " << numChars << " characters, " << medication;
        if (meanScore >= 0) {
            file << " | Mean score for last 5 tests at " << numChars << " characters: " << meanScore << "%";
        }
        file << endl;
        file.close();
    }

    bool confirmRetry() {
        char cont;
        while (true) {
            cout << "Do you want to try again? (y/n): ";
            cin >> cont;

            if (cin.fail() || (cont != 'y' && cont != 'Y' && cont != 'n' && cont != 'N')) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter 'y' or 'n'.\n";
            } else {
                break;
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return (cont == 'y' || cont == 'Y');
    }

    void getCurrentTime(char* buffer, int bufferSize) {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        strftime(buffer, bufferSize, "%m/%d/%Y %I:%M%p", ltm);
    }

    void delay(int seconds) {
        clock_t start_time = clock();
        while (clock() < start_time + seconds * CLOCKS_PER_SEC);
    }
};

int main() {
    srand(time(0));  // Seed the random number generator once
    MemoryTest test;
    test.start();
    return 0;
}
