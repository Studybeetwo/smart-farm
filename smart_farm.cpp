
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <utility>

using namespace std;

// Abstract class for farming advice
class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() = default;
};

// Base class
class Crop {
protected:
    string name;
    double area;
    string season;
    double yieldPerAcre;

    static int cropCount;

public:
    // Default constructor
    Crop()
        : name("Unknown"), area(1), season("Unknown"),
          yieldPerAcre(1) {
        ++cropCount;
    }

    // Parameterized constructor
    Crop(string n, double a, string s, double y)
        : name(move(n)), area(a), season(move(s)),
          yieldPerAcre(y) {
        if (area <= 0 || yieldPerAcre <= 0)
            throw invalid_argument(
                "Area and yield per acre must be positive.");

        ++cropCount;
    }

    // Virtual destructor
    virtual ~Crop() {
        --cropCount;
    }

    // Calculate total yield
    double calculateYield() const {
        return area * yieldPerAcre;
    }

    // Getter functions
    string getName() const {
        return name;
    }

    double getArea() const {
        return area;
    }

    string getSeason() const {
        return season;
    }

    double getYieldPerAcre() const {
        return yieldPerAcre;
    }

    // Static member function
    static int getCropCount() {
        return cropCount;
    }

    // Operator overloading to compare total yield
    bool operator>(const Crop& other) const {
        return calculateYield() > other.calculateYield();
    }

    // Virtual function for displaying crop details
    virtual void display() const {
        cout << fixed << setprecision(2);

        cout << "\nCrop Name       : " << name
             << "\nArea (acres)    : " << area
             << "\nSeason          : " << season
             << "\nYield per Acre  : " << yieldPerAcre
             << "\nTotal Yield     : " << calculateYield()
             << " units\n";
    }

    // Pure virtual functions
    virtual string getType() const = 0;
    virtual string getExtraData() const = 0;
};

// Initialize static member
int Crop::cropCount = 0;


// Derived class: Cereal crops
class CerealCrop : public Crop, public Advisory {
private:
    string fertilizerType;

public:
    // Default constructor
    CerealCrop()
        : Crop(), fertilizerType("Compost") {}

    // Parameterized constructor
    CerealCrop(string n, double a, string s, double y,
               string fertilizer)
        : Crop(move(n), a, move(s), y),
          fertilizerType(move(fertilizer)) {}

    // Display cereal details
    void display() const override {
        cout << "\n--- Cereal Crop Details ---";
        Crop::display();

        cout << "Fertilizer Type  : "
             << fertilizerType << '\n';
    }

    // Implement abstract function
    void giveAdvice() const override {
        cout << "\nFarming Advice for " << name << ":\n"
             << "- Use fertilizers based on soil tests.\n"
             << "- Monitor weeds and crop diseases regularly.\n"
             << "- Harvest when the grains reach maturity.\n";
    }

    string getType() const override {
        return "Cereal";
    }

    string getExtraData() const override {
        return fertilizerType;
    }
};


// Derived class: Vegetable crops
class VegetableCrop : public Crop, public Advisory {
private:
    int irrigationFrequency;

public:
    // Default constructor
    VegetableCrop()
        : Crop(), irrigationFrequency(3) {}

    // Parameterized constructor
    VegetableCrop(string n, double a, string s, double y,
                  int frequency)
        : Crop(move(n), a, move(s), y),
          irrigationFrequency(frequency) {
        if (frequency <= 0)
            throw invalid_argument(
                "Irrigation frequency must be positive.");
    }

    // Display vegetable details
    void display() const override {
        cout << "\n--- Vegetable Crop Details ---";
        Crop::display();

        cout << "Irrigation      : "
             << irrigationFrequency
             << " times per week\n";
    }

    // Implement abstract function
    void giveAdvice() const override {
        cout << "\nFarming Advice for " << name << ":\n"
             << "- Maintain suitable soil moisture.\n"
             << "- Check regularly for pests and diseases.\n"
             << "- Harvest vegetables at the right maturity.\n";
    }

    string getType() const override {
        return "Vegetable";
    }

    string getExtraData() const override {
        return to_string(irrigationFrequency);
    }
};


// Input validation for positive decimal numbers
double readPositiveDouble(const string& prompt) {
    double value;

    while (true) {
        cout << prompt;

        if (cin >> value && value > 0)
            return value;

        cout << "Invalid input! Enter a number greater than zero.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


// Input validation for positive integers
int readPositiveInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value && value > 0)
            return value;

        cout << "Invalid input! Enter a positive whole number.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


// Save crop data to a file
void saveToFile(const vector<unique_ptr<Crop>>& crops) {
    ofstream file("farm_data.txt");

    if (!file)
        throw runtime_error("Unable to open file for saving.");

    for (const auto& crop : crops) {
        file << crop->getType() << '|'
             << crop->getName() << '|'
             << crop->getArea() << '|'
             << crop->getSeason() << '|'
             << crop->getYieldPerAcre() << '|'
             << crop->getExtraData() << '\n';
    }

    if (!file)
        throw runtime_error("Error occurred while saving crop data.");

    file.close();

    cout << "\nCrop data saved to farm_data.txt successfully.\n";
}


// Load crop data from a file
void loadFromFile(vector<unique_ptr<Crop>>& crops) {
    ifstream file("farm_data.txt");

    if (!file)
        throw runtime_error(
            "Unable to open farm_data.txt. Save data first.");

    vector<unique_ptr<Crop>> loadedCrops;
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        if (line.empty())
            continue;

        try {
            stringstream ss(line);

            string type, n, areaText, s, yieldText, extra;

            if (!getline(ss, type, '|') ||
                !getline(ss, n, '|') ||
                !getline(ss, areaText, '|') ||
                !getline(ss, s, '|') ||
                !getline(ss, yieldText, '|') ||
                !getline(ss, extra)) {
                throw runtime_error("Incomplete record.");
            }

            if (n.empty() || s.empty() ||
                n.find('|') != string::npos ||
                s.find('|') != string::npos) {
                throw invalid_argument("Invalid crop name or season.");
            }

            size_t areaPos = 0;
            size_t yieldPos = 0;

            double a = stod(areaText, &areaPos);
            double y = stod(yieldText, &yieldPos);

            if (areaPos != areaText.size() ||
                yieldPos != yieldText.size()) {
                throw invalid_argument("Invalid numeric data.");
            }

            if (type == "Cereal") {
                if (extra.empty() ||
                    extra.find('|') != string::npos) {
                    throw invalid_argument("Invalid fertilizer type.");
                }

                loadedCrops.push_back(
                    make_unique<CerealCrop>(
                        n, a, s, y, extra));

            } else if (type == "Vegetable") {
                size_t pos = 0;
                int frequency = stoi(extra, &pos);

                if (pos != extra.size())
                    throw invalid_argument(
                        "Invalid irrigation frequency.");

                loadedCrops.push_back(
                    make_unique<VegetableCrop>(
                        n, a, s, y, frequency));

            } else {
                throw runtime_error("Unknown crop type.");
            }

        } catch (const exception& e) {
            cerr << "Skipping invalid record on line "
                 << lineNumber << ": " << e.what() << '\n';
        }
    }

    if (file.bad())
        throw runtime_error("An error occurred while reading the file.");

    crops = move(loadedCrops);

    cout << "\nCrop data loaded successfully.\n";
    cout << "Number of records loaded: "
         << crops.size() << '\n';
}


// Add a new crop
void addCrop(vector<unique_ptr<Crop>>& crops) {
    int type;

    cout << "\n1. Cereal Crop\n";
    cout << "2. Vegetable Crop\n";
    cout << "Choose crop type: ";

    if (!(cin >> type)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid choice.\n";
        return;
    }

    if (type != 1 && type != 2) {
        cout << "Invalid crop type.\n";
        return;
    }

    // Clear the newline before using getline()
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string n, s;

    cout << "Enter crop name: ";
    getline(cin, n);

    if (n.empty() || n.find('|') != string::npos) {
        cout << "Crop name cannot be empty or contain '|'.\n";
        return;
    }

    cout << "Enter cultivation season: ";
    getline(cin, s);

    if (s.empty() || s.find('|') != string::npos) {
        cout << "Season cannot be empty or contain '|'.\n";
        return;
    }

    double a = readPositiveDouble(
        "Enter cultivated area in acres: ");

    double y = readPositiveDouble(
        "Enter yield per acre (units): ");

    try {
        if (type == 1) {
            string fertilizer;

            // FIX: Clear the newline left by numeric input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter fertilizer type: ";
            getline(cin, fertilizer);

            if (fertilizer.empty() ||
                fertilizer.find('|') != string::npos) {
                cout << "Invalid fertilizer type.\n";
                return;
            }

            crops.push_back(
                make_unique<CerealCrop>(
                    n, a, s, y, fertilizer));

        } else {
            int frequency = readPositiveInt(
                "Enter irrigation frequency "
                "(times per week): ");

            crops.push_back(
                make_unique<VegetableCrop>(
                    n, a, s, y, frequency));
        }

        cout << "\nCrop added successfully.\n";

    } catch (const exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
}


// Display all crop records
void displayAll(const vector<unique_ptr<Crop>>& crops) {
    if (crops.empty()) {
        cout << "\nNo crop records available.\n";
        return;
    }

    for (const auto& crop : crops) {
        crop->display();
        cout << "--------------------------------\n";
    }
}


// Analyze total yield for both crop types
void analyzeYield(const vector<unique_ptr<Crop>>& crops) {
    double cerealTotal = 0;
    double vegetableTotal = 0;

    for (const auto& crop : crops) {
        if (crop->getType() == "Cereal")
            cerealTotal += crop->calculateYield();
        else
            vegetableTotal += crop->calculateYield();
    }

    cout << fixed << setprecision(2);

    cout << "\n--- Yield Analysis ---\n";
    cout << "Total cereal yield    : "
         << cerealTotal << " units\n";

    cout << "Total vegetable yield : "
         << vegetableTotal << " units\n";

    cout << "Overall yield         : "
         << cerealTotal + vegetableTotal << " units\n";
}


// Compare two crops using operator overloading
void compareCrops(const vector<unique_ptr<Crop>>& crops) {
    if (crops.size() < 2) {
        cout << "\nAt least two crops are needed for comparison.\n";
        return;
    }

    cout << "\nAvailable crops:\n";

    for (size_t i = 0; i < crops.size(); ++i) {
        cout << i + 1 << ". "
             << crops[i]->getName()
             << " (Total yield: "
             << crops[i]->calculateYield()
             << " units)\n";
    }

    int first, second;

    cout << "Enter the number of the first crop: ";

    if (!(cin >> first)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }

    cout << "Enter the number of the second crop: ";

    if (!(cin >> second)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }

    if (first < 1 || second < 1 ||
        first > static_cast<int>(crops.size()) ||
        second > static_cast<int>(crops.size()) ||
        first == second) {
        cout << "Invalid crop selection.\n";
        return;
    }

    const Crop& c1 = *crops[first - 1];
    const Crop& c2 = *crops[second - 1];

    cout << fixed << setprecision(2);

    if (c1 > c2) {
        cout << c1.getName()
             << " has the higher total yield.\n";

    } else if (c2 > c1) {
        cout << c2.getName()
             << " has the higher total yield.\n";

    } else {
        cout << "Both crops have equal total yields.\n";
    }
}


// Display crop-specific farming advice
void displayAdvice(const vector<unique_ptr<Crop>>& crops) {
    if (crops.empty()) {
        cout << "\nNo crop records available.\n";
        return;
    }

    for (const auto& crop : crops) {
        const Advisory* advisor =
            dynamic_cast<const Advisory*>(crop.get());

        if (advisor != nullptr)
            advisor->giveAdvice();
    }
}


// Main function
int main() {
    vector<unique_ptr<Crop>> crops;
    int choice = 0;

    do {
        cout << "\n========== SMART FARM MANAGEMENT SYSTEM ==========\n";
        cout << "1. Add Crop\n";
        cout << "2. Display All Crops\n";
        cout << "3. Analyze Total Yield\n";
        cout << "4. Compare Two Crops\n";
        cout << "5. Display Farming Advice\n";
        cout << "6. Save Crop Data to File\n";
        cout << "7. Load Crop Data from File\n";
        cout << "8. Display Number of Crop Objects\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Enter a number from 1 to 9.\n";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            continue;
        }

        try {
            switch (choice) {
                case 1:
                    addCrop(crops);
                    break;

                case 2:
                    displayAll(crops);
                    break;

                case 3:
                    analyzeYield(crops);
                    break;

                case 4:
                    compareCrops(crops);
                    break;

                case 5:
                    displayAdvice(crops);
                    break;

                case 6:
                    saveToFile(crops);
                    break;

                case 7:
                    loadFromFile(crops);
                    break;

                case 8:
                    cout << "\nCurrent number of crop objects: "
                         << Crop::getCropCount() << '\n';
                    break;

                case 9:
                    cout << "\nExiting Smart Farm Management System.\n";
                    break;

                default:
                    cout << "Invalid choice. Select 1 to 9.\n";
            }

        } catch (const exception& e) {
            cout << "Operation failed: " << e.what() << '\n';
        }

    } while (choice != 9);

    return 0;
}
