#include "Riddle.h"
#include "Player.h"
#include "Direction.h"
#include "Screen.h"
#include "utils.h"
#include "console.h"
#include "Constants.h"

using namespace GameConstants;

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cctype>

namespace {
    struct Riddle {
        std::string question;
        std::string answer;
    };

    std::vector<Riddle> riddles;
    int nextRiddleIndex = 0;

    std::string trim(const std::string& s) {
    size_t start = 0, end = s.size();
    while (start < end && std::isspace((unsigned char)s[start])) ++start;
    while (end > start && std::isspace((unsigned char)s[end - 1])) --end;
        return s.substr(start, end - start);
    }

    std::string toLowerStr(const std::string& s) {
        std::string res = s;
        for (char& c : res) c = (char)std::tolower((unsigned char)c);
        return res;
    }

    // Helper: Prepare riddle text lines for display
    std::vector<std::string> prepareRiddleLines(const std::string& question) {
        std::vector<std::string> lines;
        std::istringstream iss(question);
        std::string line;
        while (std::getline(iss, line)) lines.push_back(line);
        if (lines.empty()) lines.push_back("No riddle question available");

        int contentWidth = 0;
        for (const auto& s : lines) if ((int)s.size() > contentWidth) contentWidth = (int)s.size();
        if (contentWidth < RIDDLE_MIN_WIDTH) contentWidth = RIDDLE_MIN_WIDTH;
        const int maxContentWidth = Screen::MAX_X - RIDDLE_PADDING;
        if (contentWidth > maxContentWidth) contentWidth = maxContentWidth;
        for (auto& s : lines) if ((int)s.size() > contentWidth) s = s.substr(0, contentWidth);

        return lines;
    }

    // Helper: Draw riddle box and backup screen area
    void drawRiddleBox(int boxX, int boxY, int boxWidth, int boxHeight,
                       const std::vector<std::string>& lines,
                       std::vector<std::string>& backup, Screen* screen) {
        // Backup screen area
        backup.resize(boxHeight, std::string(boxWidth, ' '));
        for (int y = 0; y < boxHeight; ++y) {
            for (int x = 0; x < boxWidth; ++x) {
                backup[y][x] = screen->getCharAt(boxX + x, boxY + y);
            }
        }

        // Draw box
        gotoxy(boxX, boxY);
        std::cout << "+" << std::string(boxWidth - 2, '-') << "+";
        for (int y = 1; y < boxHeight - 1; ++y) {
            gotoxy(boxX, boxY + y);
            std::cout << "|" << std::string(boxWidth - 2, ' ') << "|";
        }
        gotoxy(boxX, boxY + boxHeight - 1);
        std::cout << "+" << std::string(boxWidth - 2, '-') << "+";

        // Draw riddle text
        for (int i = 0; i < (int)lines.size() && (1 + i) < boxHeight - 2; ++i) {
            gotoxy(boxX + 2, boxY + 1 + i);
            std::cout << lines[i];
        }

        int answerY = boxY + 1 + (int)lines.size();
        if (answerY < boxY + boxHeight - 2) {
            gotoxy(boxX + 2, answerY);
            std::cout << "Answer: ";
        }
        std::cout.flush();
    }

    // Helper: Get user's answer to riddle
    char getUserRiddleAnswer() {
        char ans = 0;
        while (true) {
            ans = get_single_char();
            if (ans == '\r' || ans == '\n') continue;

            // Check for ESC before conversion
            if (ans == KEY_ESC) break;

            // Convert to uppercase 
            ans = (char)std::toupper((unsigned char)ans);

            // Only accept valid multiple choice options
            if (ans == 'A' || ans == 'B' || ans == 'C') break;
            
        }
        return ans;
    }

    // Helper: Restore screen from backup
    void restoreScreenArea(int boxX, int boxY, int boxWidth, int boxHeight,
                           const std::vector<std::string>& backup, Screen* screen) {
        for (int y = 0; y < boxHeight; ++y) {
            for (int x = 0; x < boxWidth; ++x) {
                screen->setCharAt(boxX + x, boxY + y, backup[y][x]);
            }
        }
        for (int y = 0; y < boxHeight; ++y) {
            for (int x = 0; x < boxWidth; ++x) {
                screen->drawCharOnly(boxX + x, boxY + y);
            }
        }
    }
} 

bool initRiddles(std::string& errorMessage) {
    riddles.clear();
    nextRiddleIndex = 0;
    errorMessage.clear();

    std::ifstream in("Data/riddles.txt");
    if (!in) {
        errorMessage = "Cannot open riddles.txt - file is missing!";
        return false;
    }

    std::vector<std::string> block;
    std::string line;

    auto flushBlock = [&]() {
        if (block.empty()) return;
        std::string answer = trim(block.back());
        block.pop_back();
        std::string question;
        for (size_t i = 0; i < block.size(); ++i) {
            question += block[i];
            if (i + 1 < block.size()) question += '\n';
        }
        if (!question.empty() && !answer.empty()) {
            Riddle r;
            r.question = question;
            r.answer = answer;
            riddles.push_back(r);
        }
        block.clear();
    };

    while (std::getline(in, line)) {
        if (line.empty()) flushBlock();
        else block.push_back(line);
    }
    flushBlock();

    if (riddles.empty()) {
        errorMessage = "riddles.txt is empty or invalid!";
        return false;
    }

    return true;
}

int getRiddleCount() {
    return (int)riddles.size();
}

bool Player::processRiddle(const Point& next, char nextChar) {
    if (nextChar != RIDDLE) return false;
    handleRiddle(next);
    return true;
}

void Player::handleRiddle(const Point& riddlePos) {
    if (nextRiddleIndex >= (int)riddles.size()) return;

    // Save player direction
    int savedDx = body[0].getDx();
    int savedDy = body[0].getDy();

    const Riddle& r = riddles[nextRiddleIndex];

    // Prepare riddle display
    std::vector<std::string> lines = prepareRiddleLines(r.question);
    int contentWidth = 0;
    for (const auto& s : lines) if ((int)s.size() > contentWidth) contentWidth = (int)s.size();

    int boxWidth = contentWidth + RIDDLE_BOX_WIDTH_EXTRA;
    int boxHeight = (int)lines.size() + RIDDLE_BOX_HEIGHT_EXTRA;
    if (boxWidth > Screen::MAX_X - RIDDLE_BOUNDARY_OFFSET) boxWidth = Screen::MAX_X - RIDDLE_BOUNDARY_OFFSET;
    if (boxHeight > Screen::MAX_Y - RIDDLE_BOUNDARY_OFFSET) boxHeight = Screen::MAX_Y - RIDDLE_BOUNDARY_OFFSET;

    int boxX = (Screen::MAX_X - boxWidth) / 2;
    int boxY = (Screen::MAX_Y - boxHeight) / 2;

    // Draw riddle box
    std::vector<std::string> backup;
    drawRiddleBox(boxX, boxY, boxWidth, boxHeight, lines, backup, screen);

    // Get user answer
    char ans = getUserRiddleAnswer();

    // Process answer
    std::string userAnswer;
    if (ans != KEY_ESC) userAnswer.push_back(ans);

    std::string correct = toLowerStr(trim(r.answer));
    std::string userNorm = toLowerStr(trim(userAnswer));
    bool isCorrect = (!correct.empty() && userNorm == correct);

    // Display result
    gotoxy(boxX + 2, boxY + boxHeight - 2);
    if (ans == KEY_ESC) {
        std::cout << "Canceled.";
    }
    else if (isCorrect) {
        std::cout << "Correct!";
        playSound("correct");
        addScore(SCORE_RIDDLE_CORRECT);
    }
    else {
        std::cout << "Wrong answer. Correct: " << r.answer;
        playSound("wrong");
        loseScore(SCORE_RIDDLE_PENALTY);
    }
    std::cout.flush();
    sleep_ms(RIDDLE_FEEDBACK_DELAY_MS);
    ++nextRiddleIndex;

    // Restore screen
    restoreScreenArea(boxX, boxY, boxWidth, boxHeight, backup, screen);

    // Handle correct answer - move player to riddle position
    if (isCorrect) {
        screen->drawCharOnly(body[0].getX(), body[0].getY());
        screen->setCharAt(riddlePos, EMPTY);
        screen->drawCharOnly(riddlePos.getX(), riddlePos.getY());
        body[0].set(riddlePos.getX(), riddlePos.getY());
        body[0].draw();
    }

    // Restore player direction
    if (savedDx == 0 && savedDy == 0)      body[0].setDirection(Direction::STAY);
    else if (savedDx == 1 && savedDy == 0) body[0].setDirection(Direction::RIGHT);
    else if (savedDx == -1 && savedDy == 0) body[0].setDirection(Direction::LEFT);
    else if (savedDx == 0 && savedDy == 1) body[0].setDirection(Direction::DOWN);
    else if (savedDx == 0 && savedDy == -1) body[0].setDirection(Direction::UP);
}

void resetRiddlesIndex() {
    nextRiddleIndex = 0;
}