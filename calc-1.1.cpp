#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

// Structure to store calculation details
struct Calculation {
    double num1;
    double num2;
    char operation;
    double result;
};

void displayHistory(const vector<Calculation>& history) {
    if (history.empty()) {
        cout << "\nNo calculations performed yet.\n";
        return;
    }
    
    cout << "\n=== Calculation History ===\n";
    cout << setprecision(2) << fixed;  // Set decimal precision for better formatting
    for (size_t i = 0; i < history.size(); i++) {
        cout << i + 1 << ". " << history[i].num1 << " "
             << history[i].operation << " "
             << history[i].num2 << " = "
             << history[i].result << endl;
    }
    cout << "=======================\n";
}

int main() {
    double num1, num2, result;
    char operation;
    char choice;
    vector<Calculation> history;  // Store calculation history
    
    do {
        cout << "\nCalculator Menu:\n";
        cout << "1. Perform calculation\n";
        cout << "2. View history\n";
        cout << "3. Exit\n";
        cout << "Enter your choice (1-3): ";
        cin >> choice;
        
        switch (choice) {
            case '1': {
                cout << "Enter first number: ";
                cin >> num1;
                cout << "Enter an operation (+, -, *, /): ";
                cin >> operation;
                cout << "Enter second number: ";
                cin >> num2;
                
                bool validCalculation = true;
                Calculation calc;
                calc.num1 = num1;
                calc.num2 = num2;
                calc.operation = operation;
                
                switch (operation) {
                    case '+':
                        result = num1 + num2;
                        break;
                    case '-':
                        result = num1 - num2;
                        break;
                    case '*':
                        result = num1 * num2;
                        break;
                    case '/':
                        if (num2 != 0) {
                            result = num1 / num2;
                        } else {
                            cout << "Error: Cannot divide by zero.\n";
                            validCalculation = false;
                        }
                        break;
                    default:
                        cout << "Invalid operation. Please try again.\n";
                        validCalculation = false;
                }
                
                if (validCalculation) {
                    calc.result = result;
                    history.push_back(calc);
                    cout << "Result: " << fixed << setprecision(2) << result << endl;
                }
                break;
            }
            case '2':
                displayHistory(history);
                break;
            case '3':
                cout << "Thank you for using the calculator!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        
        cout << "\nDo you want to continue? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
    
    return 0;
}