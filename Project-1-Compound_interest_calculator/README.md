# Compound Interest Calculator

This is a simple command-line tool written in C++ that calculates compound interest over a specified period. The program allows you to input the number of years, monthly deposit, and annual interest rate, then provides a detailed breakdown of your total capital, total deposited amount, and the interest earned over time.

## Features

- Calculate compound interest based on user inputs.
- Provide yearly breakdowns of total deposited and total capital.
- Display final results including the total interest earned and overall gain percentage.

## Usage

1. **Compile the program**: Ensure you have a C++ compiler installed. You can compile the program using a command like:
    ```bash
    g++ -o interest_calculator interest_calculator.cpp
    ```

2. **Run the program**:
    ```bash
    ./interest_calculator
    ```

3. **Menu Options**:
    - **Perform a new calculation**: Follow the prompts to input the number of years, monthly deposit, and annual interest rate. The program will then calculate and display the results.
    - **Exit**: Exit the program.

## Example

Here's an example of how the program works:

```bash
Menu:
1. Perform a new calculation
2. Exit
Enter your choice: 1

* Interest calculation *
Years to calculate interest: 5
Enter the monthly deposit: 100
Enter the annual interest rate (in decimal, e.g., 0.03 for 3%): 0.05

After 1 years
Total deposited: 1200
Total capital: 1231.78
----------------------
After 2 years
Total deposited: 2400
Total capital: 2522.68
----------------------
After 3 years
Total deposited: 3600
Total capital: 3907.25
----------------------
After 4 years
Total deposited: 4800
Total capital: 5398.44
----------------------
After 5 years
Total deposited: 6000
Total capital: 7009.65
----------------------

######################
Final result after 5 years:
Total deposited: 6000
Final capital: 7009.65
Interest earned: 1009.65
Overall gain compared to total deposit: 16.83%
######################
```

## License

This project is licensed under the GNU General Public License v3.0.
