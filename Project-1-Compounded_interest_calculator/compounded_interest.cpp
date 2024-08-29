#include <iostream>
#include <cmath>

void calculateInterest();
void showMenu();

int main() {
    while (true) {
        showMenu();
        int choice;
        std::cin >> choice;
        std::cout << "\n";
        if (choice == 1) {
	    std::cout << "* Interest calculation *\n";
            calculateInterest();
	    std::cout << std::endl;
        } else if (choice == 2) {
            std::cout << "Exiting the program." << std::endl;
            break;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    return 0;
}

void showMenu() {
    std::cout << "Menu:" << std::endl;
    std::cout << "1. Perform a new calculation" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void calculateInterest() {
    double monthly_deposit;
    double annual_interest;
    int years;
    double current_capital = 0;
    double total_deposited = 0;

    std::cout << "Years to calculate interest: ";
    std::cin >> years;
    int months = years * 12;
    std::cout << "Enter the monthly deposit: ";
    std::cin >> monthly_deposit;
    std::cout << "Enter the annual interest rate (in decimal, e.g., 0.03 for 3%): ";
    std::cin >> annual_interest;

    double monthly_interest = std::pow(1 + annual_interest, 1.0 / 12.0) - 1;

    for (int i = 0; i < months; i++) {
        current_capital += monthly_deposit;
        current_capital *= (1 + monthly_interest);
        total_deposited += monthly_deposit;

        if ((i + 1) % 12 == 0) {
            std::cout << "After " << (i + 1) / 12 << " years" << std::endl;
            std::cout << "Total deposited: " << total_deposited << std::endl;
            std::cout << "Total capital: " << current_capital << std::endl;
            std::cout << "----------------------" << std::endl;
        }
    }

    std::cout << "\n######################\n";
    std::cout << "Final result after " << years << " years: " << std::endl;
    std::cout << "Total deposited: " << total_deposited << std::endl;
    std::cout << "Final capital: " << current_capital << std::endl;
    std::cout << "Interest earned: " << current_capital - total_deposited << std::endl;
    std::cout << "Overall gain compared to total deposit: " << (current_capital - total_deposited) / (total_deposited / 100) << "%" << std::endl;
    std::cout << "######################\n";
    std::cout << "Press any key to continue... " << std::endl;
}
