#include <iostream>
#include "addition.h"
#include "subtraction.h"
#include "multiplication.h"

using namespace std;

void main() {
    int action = 0;
    cout << "================================\n"
            "== Calculator long arithmetic ==\n"
            "================================";
    cout << "\n\n";
    cout << "1 - Addition\n"
            "2 - Subtraction\n"
            "3 - Multiplication\n"
            "Choose operation: ";
    cin >> action;
    string num1, num2, result;
    cout << "Enter the first number: ";
    cin >> num1;
    cout << "Enter the second number: ";
    cin >> num2;
    switch (action) {
    case 1: // Addition
        result = addition(num1, num2);
        cout << "Result: " << result;
        break;

    case 2: // Subtraction
        result = subtraction(num1, num2);
        cout << "Result: " << result;
        break;

    case 3: // Multiplication
        result = multiplication(num1, num2);
        cout << "Result: " << result;
        break;

    default:
        cout << "Incorrect operation.";
    }
}
