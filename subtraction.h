#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

string subtraction(const string& num1, const string& num2) {
    string n1 = num1;
    string n2 = num2;
    if (n1.length() < n2.length() || (n1.length() == n2.length() && n1 < n2)) {
        swap(n1, n2);
    }

    int len1 = n1.size();
    int len2 = n2.size();
    string result = "";

    int carry = 0;
    for (int i = 0; i < len1; i++) {
        int digit1 = n1[len1 - 1 - i] - '0';
        int digit2 = (i < len2) ? n2[len2 - 1 - i] - '0' : 0;

        int sub = digit1 - digit2 - carry;
        if (sub < 0) {
            sub += 10;
            carry = 1;
        }
        else {
            carry = 0;
        }

        result += (sub + '0');
    }

    while (result.size() > 1 && result.back() == '0') {
        result.pop_back();
    }

    reverse(result.begin(), result.end());
    return result;
}
