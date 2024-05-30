#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

string addition(const string& num1, const string& num2) {
    int len1 = num1.size();
    int len2 = num2.size();
    int maxLen = max(len1, len2);

    string n1 = string(maxLen - len1, '0') + num1;
    string n2 = string(maxLen - len2, '0') + num2;

    string result = "";
    int carry = 0;

    for (int i = maxLen - 1; i >= 0; i--) {
        int digit1 = n1[i] - '0';
        int digit2 = n2[i] - '0';

        int sum = digit1 + digit2 + carry;
        carry = sum / 10;
        result += (sum % 10) + '0';
    }

    if (carry) {
        result += carry + '0';
    }

    reverse(result.begin(), result.end());
    return result;
}
