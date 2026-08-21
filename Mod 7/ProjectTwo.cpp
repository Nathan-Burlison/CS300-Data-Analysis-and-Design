/*
Nathan Burlison
CS 300 Project Two
*/


#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <list>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

// Stores one course and the course numbers of its prerequisites.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Simple hash table that uses separate chaining to handle collisions.
class CourseHashTable {
private:
    vector<list<Course>> buckets;

    // Creates a repeatable hash value from a course number.
    size_t hashCourseNumber(const string& courseNumber) const {
        size_t hashValue = 7;
        for (char character : courseNumber) {
            hashValue = (hashValue * 31 + static_cast<unsigned char>(character)) % buckets.size();
        }
        return hashValue;
    }

public:
    explicit CourseHashTable(size_t bucketCount = 101) : buckets(bucketCount) {}

    // Removes all stored courses while keeping the same number of buckets.
    void clear() {
        for (auto& bucket : buckets) {
            bucket.clear();
        }
    }

    // Inserts a course. Returns false when the course number already exists.
    bool insert(const Course& course) {
        size_t bucketIndex = hashCourseNumber(course.courseNumber);

        for (const Course& existingCourse : buckets[bucketIndex]) {
            if (existingCourse.courseNumber == course.courseNumber) {
                return false;
            }
        }

        buckets[bucketIndex].push_back(course);
        return true;
    }

    // Finds a course by course number. Returns nullptr when no match exists.
    const Course* find(const string& courseNumber) const {
        size_t bucketIndex = hashCourseNumber(courseNumber);

        for (const Course& course : buckets[bucketIndex]) {
            if (course.courseNumber == courseNumber) {
                return &course;
            }
        }

        return nullptr;
    }

    // Copies all courses from the hash table into a vector for sorting/printing.
    vector<Course> getAllCourses() const {
        vector<Course> courses;

        for (const auto& bucket : buckets) {
            for (const Course& course : bucket) {
                courses.push_back(course);
            }
        }

        return courses;
    }
};

// Removes whitespace from the beginning and end of a string.
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Converts a course number to uppercase so searches are case-insensitive.
string toUpper(string text) {
    transform(text.begin(), text.end(), text.begin(), [](unsigned char character) {
        return static_cast<char>(toupper(character));
    });
    return text;
}

// Splits one comma-separated line into trimmed fields.
vector<string> splitCourseLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream lineStream(line);

    while (getline(lineStream, field, ',')) {
        fields.push_back(trim(field));
    }

    // Preserve a trailing empty field so an empty prerequisite can be detected.
    if (!line.empty() && line.back() == ',') {
        fields.push_back("");
    }

    return fields;
}

// Reads, validates, and loads the complete course file into the hash table.
bool loadCourses(const string& fileName, CourseHashTable& courseTable) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: The course file could not be opened." << endl;
        return false;
    }

    vector<Course> temporaryCourses;
    unordered_set<string> knownCourseNumbers;
    string line;
    int lineNumber = 0;
    bool fileHasErrors = false;

    while (getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            cout << "Format error on line " << lineNumber << ": The line is empty." << endl;
            fileHasErrors = true;
            continue;
        }

        vector<string> fields = splitCourseLine(line);

        if (fields.size() < 2) {
            cout << "Format error on line " << lineNumber
                 << ": A course number and title are required." << endl;
            fileHasErrors = true;
            continue;
        }

        string courseNumber = toUpper(fields[0]);
        string courseTitle = fields[1];

        if (courseNumber.empty() || courseTitle.empty()) {
            cout << "Format error on line " << lineNumber
                 << ": The course number and title cannot be empty." << endl;
            fileHasErrors = true;
            continue;
        }

        if (knownCourseNumbers.find(courseNumber) != knownCourseNumbers.end()) {
            cout << "Format error on line " << lineNumber
                 << ": Duplicate course number " << courseNumber << "." << endl;
            fileHasErrors = true;
            continue;
        }

        Course course;
        course.courseNumber = courseNumber;
        course.courseTitle = courseTitle;

        bool courseLineValid = true;
        for (size_t index = 2; index < fields.size(); ++index) {
            string prerequisiteNumber = toUpper(fields[index]);

            if (prerequisiteNumber.empty()) {
                cout << "Format error on line " << lineNumber
                     << ": A prerequisite value is empty." << endl;
                fileHasErrors = true;
                courseLineValid = false;
                continue;
            }

            course.prerequisites.push_back(prerequisiteNumber);
        }

        if (courseLineValid) {
            temporaryCourses.push_back(course);
            knownCourseNumbers.insert(courseNumber);
        }
    }

    inputFile.close();

    if (temporaryCourses.empty()) {
        cout << "Error: No valid course information was found in the file." << endl;
        return false;
    }

    // Validate prerequisites only after all course numbers are known.
    for (const Course& course : temporaryCourses) {
        for (const string& prerequisiteNumber : course.prerequisites) {
            if (knownCourseNumbers.find(prerequisiteNumber) == knownCourseNumbers.end()) {
                cout << "Format error: " << prerequisiteNumber
                     << " is listed as a prerequisite for " << course.courseNumber
                     << " but does not exist in the file." << endl;
                fileHasErrors = true;
            }
        }
    }

    if (fileHasErrors) {
        cout << "The file was not loaded because it contains errors." << endl;
        return false;
    }

    courseTable.clear();
    for (const Course& course : temporaryCourses) {
        if (!courseTable.insert(course)) {
            cout << "Error: Duplicate course number " << course.courseNumber
                 << " could not be inserted." << endl;
            courseTable.clear();
            return false;
        }
    }

    cout << temporaryCourses.size() << " courses loaded successfully." << endl;
    return true;
}

// Prints all courses in alphanumeric order by course number.
void printCourseList(const CourseHashTable& courseTable) {
    vector<Course> courses = courseTable.getAllCourses();

    sort(courses.begin(), courses.end(), [](const Course& first, const Course& second) {
        return first.courseNumber < second.courseNumber;
    });

    cout << "\nABCU Computer Science Course List" << endl;
    cout << "---------------------------------" << endl;

    for (const Course& course : courses) {
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }
}

// Prints one course and the numbers/titles of all prerequisite courses.
void printCourseInformation(const CourseHashTable& courseTable, string requestedCourseNumber) {
    requestedCourseNumber = toUpper(trim(requestedCourseNumber));
    const Course* selectedCourse = courseTable.find(requestedCourseNumber);

    if (selectedCourse == nullptr) {
        cout << "Course " << requestedCourseNumber << " was not found." << endl;
        return;
    }

    cout << "\n" << selectedCourse->courseNumber << ", "
         << selectedCourse->courseTitle << endl;

    if (selectedCourse->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (const string& prerequisiteNumber : selectedCourse->prerequisites) {
        const Course* prerequisiteCourse = courseTable.find(prerequisiteNumber);

        if (prerequisiteCourse != nullptr) {
            cout << prerequisiteCourse->courseNumber << ", "
                 << prerequisiteCourse->courseTitle << endl;
        } else {
            // This should not occur after successful file validation.
            cout << prerequisiteNumber << " - Course information unavailable" << endl;
        }
    }
}

// Displays the main program menu.
void displayMenu() {
    cout << "\n1. Load Course Data" << endl;
    cout << "2. Print Course List" << endl;
    cout << "3. Print Course Information" << endl;
    cout << "9. Exit" << endl;
    cout << "Enter a menu option: ";
}

int main() {
    CourseHashTable courseTable;
    bool coursesLoaded = false;
    int menuChoice = 0;

    cout << "Welcome to the ABCU Course Planner." << endl;

    while (menuChoice != 9) {
        displayMenu();

        if (!(cin >> menuChoice)) {
            cout << "Invalid input. Please enter 1, 2, 3, or 9." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (menuChoice) {
        case 1: {
            string fileName;
            cout << "Enter the course data file name: ";
            getline(cin, fileName);
            coursesLoaded = loadCourses(trim(fileName), courseTable);
            break;
        }

        case 2:
            if (!coursesLoaded) {
                cout << "Please load the course data first." << endl;
            } else {
                printCourseList(courseTable);
            }
            break;

        case 3:
            if (!coursesLoaded) {
                cout << "Please load the course data first." << endl;
            } else {
                string requestedCourseNumber;
                cout << "Enter a course number: ";
                getline(cin, requestedCourseNumber);
                printCourseInformation(courseTable, requestedCourseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the ABCU Course Planner." << endl;
            break;

        default:
            cout << "Invalid menu option. Please enter 1, 2, 3, or 9." << endl;
            break;
        }
    }

    return 0;
}
