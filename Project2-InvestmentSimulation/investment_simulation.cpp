/*
# Project: Simulator for investments and mortage
# Filename: investment_simulation.cpp
# Copyright (C) 2025 Matteo Ferrea
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

// General simulation parameters
constexpr int sim_years = 30;                    // Total simulation duration in years
constexpr double average_savings = 500;         // Monthly savings capacity

// Investment parameters
constexpr double min_cushion = //PUT something here;           // Target amount for safety cushion

// Initial portfolio values
double safety_cushion {0};                  // Starting safety cushion amount
double bonds {0};                               // Starting bonds amount
double stocks {0};                              // Starting stocks amount

// Phase tracking variables
bool phase_1_completed = false;                 // Tracks completion of safety cushion phase
bool phase_2_completed = false;                 // Tracks completion of mortgage payment phase
bool phase_3_completed = false;                 // Tracks completion of final investment phase
double phase_months = 0;                        // Counts months in current phase
double total_investment_deposits = 0;           // Tracks total amount invested

// Asset allocation targets for Phase 2 (during mortgage)
constexpr double phase_2_cushion_allocation = 0.35;    // 35% safety cushion
constexpr double phase_2_bonds_allocation = 0.35;      // 35% bonds
constexpr double phase_2_stocks_allocation = 0.30;     // 30% stocks

// Asset allocation targets for Phase 3 (after mortgage)
constexpr double phase_3_cushion_allocation = 0.20;    // 20% safety cushion
constexpr double phase_3_bonds_allocation = 0.20;      // 20% bonds
constexpr double phase_3_stocks_allocation = 0.60;     // 60% stocks

// Annual interest rates for each investment type
constexpr double cushion_interest = 0.015;            // 1.5% annual return on safety cushion
constexpr double bonds_interest = 0.02;               // 2% annual return on bonds
constexpr double stocks_interest = 0.06;              // 6% annual return on stocks

// Convert annual interest rates to monthly rates
constexpr double cushion_increase = std::pow(1 + cushion_interest, 1.0/12) - 1;
constexpr double bonds_increase = std::pow(1 + bonds_interest, 1.0/12) - 1;
constexpr double stocks_increase = std::pow(1 + stocks_interest, 1.0/12) - 1;

// Mortgage parameters
constexpr double initial_capital = 200000;            // Initial mortgage amount
constexpr double monthly_mortgage_insurance = 50.5;   // Monthly insurance cost
constexpr double max_monthly_total_payment = 1000;    // Maximum monthly payment including additional contributions
constexpr double interest = 0.0252 / 12;             // Monthly mortgage interest rate (2.52% annual)

// Mortgage tracking variables
double current_capital = 0;                          // Current mortgage balance
int mortgage_months = 20 * 12;                       // Initial mortgage duration (20 years)
double additional_contribution = 0;                   // Extra payment towards mortgage
double monthly_interest = 0;                         // Monthly interest amount
double payment = 0;                                  // Regular monthly payment
int counter = 1;                                     // Month counter
bool duration_reduction = false;                     // Tracks if mortgage term has been reduced
double total_voluntary_contributions = 0;            // Total extra payments made
double total_mortgage_interest = 0;                  // Total interest paid

// Output file streams
std::ofstream investmentFile("investment_simulation.txt");
std::ofstream mortgageFile("mortgage_simulation.txt");

// Prints current mortgage status to file
void printMortgage() {
    mortgageFile << "----------------------------------------------\n";
    mortgageFile << "Month " << counter << " - Year: " <<  counter/12+1 << "\n";
    mortgageFile << "Monthly mortgage payment: " << payment << " euros\n";
    if (additional_contribution > 0) mortgageFile << "Monthly additional payment: " << additional_contribution << "\n";
    mortgageFile << "Monthly interest: " << monthly_interest << "\n";
    mortgageFile << "Current capital: " << current_capital << "\n";
    mortgageFile << "----------------------------------------------\n";
}

// Prints summary at the end of each phase
void printPhaseEndSummary(std::string phase){
    double total_portfolio = safety_cushion + bonds + stocks;
    investmentFile << "##############################################\n";
    investmentFile << phase << " completed in " << phase_months << " months. Years: " << phase_months/12 << std::endl;
    investmentFile << "Safety cushion: " << safety_cushion << "\n";
    investmentFile << "Bonds: " << bonds << "\n";
    investmentFile << "Stocks: " << stocks << "\n";
    if (current_capital > 0) investmentFile << "Remaining mortgage: " << current_capital << "\n\n";
    investmentFile << "Total assets: " << safety_cushion + bonds + stocks << "\n";
    if (phase != "Phase 1"){
        // Print portfolio allocations as percentages
        investmentFile << "% cushion: " << safety_cushion/total_portfolio << "\n";
        investmentFile << "% Bonds: " << bonds/total_portfolio << "\n";
        investmentFile << "% Stocks: " << stocks/total_portfolio << "\n";
        investmentFile << "##############################################\n";
    }
}

// Prints monthly investment deposits
void printDeposits(std::string phase, double cushion_deposit, double bonds_deposit, double stocks_deposit){
    investmentFile << "**********************************************\n";
    investmentFile << phase << " - Month " << phase_months << " deposits:\n";
    if (cushion_deposit > 0) investmentFile << "Deposit to cushion: " << cushion_deposit << std::endl;
    if (bonds_deposit > 0) investmentFile << "Deposit to bonds: " << bonds_deposit << std::endl;
    if (stocks_deposit > 0) investmentFile << "Deposit to stocks: " << stocks_deposit << std::endl;
    investmentFile << "**********************************************\n";
}

// Handles monthly mortgage calculations and payments
void mortgageAmortization() {
    // Calculate monthly interest
    monthly_interest = current_capital * interest;
    total_mortgage_interest += monthly_interest;

    // Calculate regular payment using amortization formula
    payment = (current_capital * interest) / (1 - std::pow(1 + interest, -mortgage_months));

    // Check if payment is low enough to trigger duration reduction
    if (payment < 250) {
        duration_reduction = true;
        mortgage_months -= 60; // Reduce term by 5 years
    }

    if (!duration_reduction) {
        // Regular monthly additional contribution
        additional_contribution = max_monthly_total_payment - payment;
        current_capital -= (payment - monthly_interest) + additional_contribution;
    } else {
        // Switch to yearly additional contributions after reduction
        additional_contribution = 0;
        if (mortgage_months % 12 == 0) additional_contribution = 1000;
        current_capital -= (payment - monthly_interest) + additional_contribution;
    }

    total_voluntary_contributions += additional_contribution;
    printMortgage();
    mortgage_months--;
    counter++;
}

// Handles Phase 1: Building safety cushion
void phaseOne(){
    double deposit = average_savings - additional_contribution - monthly_mortgage_insurance;
    safety_cushion += deposit;
    phase_months++;
    total_investment_deposits += deposit;
    printDeposits("Phase 1", deposit, 0, 0);

    // Check if safety cushion target is reached
    if (safety_cushion >= min_cushion){
        phase_1_completed = true;
        printPhaseEndSummary("Phase 1");
        phase_months = 0;
    }
}

// Handles proportional deposits to maintain target allocations
void proportionalDeposit(std::string phase, double target_cushion, double target_bonds, double target_stocks, double deposit){
    total_investment_deposits += deposit;
    double total = safety_cushion + bonds + stocks;

    // Calculate how much each asset class needs to reach target allocation
    double cushion_diff = std::max(0.0, target_cushion * total - safety_cushion);
    double bonds_diff = std::max(0.0, target_bonds * total - bonds);
    double stocks_diff = std::max(0.0, target_stocks * total - stocks);

    // Calculate total needs and distribute deposit proportionally
    double diff_sum = cushion_diff + bonds_diff + stocks_diff;
    if (diff_sum > 0) {
        double cushion_dep = (cushion_diff / diff_sum) * deposit;
        double bonds_dep = (bonds_diff / diff_sum) * deposit;
        double stocks_dep = (stocks_diff / diff_sum) * deposit;
        safety_cushion += cushion_dep;
        bonds += bonds_dep;
        stocks += stocks_dep;
        printDeposits(phase, cushion_dep, bonds_dep, stocks_dep);
    }
}

// Handles Phase 2: Balanced investing while paying mortgage
void phaseTwo(){
    if (current_capital > 0){
        phase_months++;
        double deposit = average_savings - additional_contribution - monthly_mortgage_insurance;
        proportionalDeposit("Phase 2", phase_2_cushion_allocation, phase_2_bonds_allocation, phase_2_stocks_allocation, deposit);
    }else{
        phase_2_completed = true;
        printPhaseEndSummary("Phase 2");
        phase_months = 0;
    }
}

// Handles Phase 3: Final investment phase after mortgage
void phaseThree() {
    phase_months++;
    double deposit = average_savings;
    proportionalDeposit("Phase 3", phase_3_cushion_allocation, phase_3_bonds_allocation, phase_3_stocks_allocation, deposit);
}

// Calculates monthly returns on investments
void calculatePortfolioInterest(){
    safety_cushion += safety_cushion * cushion_increase;
    bonds += bonds * bonds_increase;
    stocks += stocks * stocks_increase;
}

// Prints final simulation summary
void printFinalSummary(){
    mortgageFile << "Total additional mortgage contributions: " << total_voluntary_contributions << std::endl;
    mortgageFile << "Total interest paid: " << total_mortgage_interest << std::endl;
    mortgageFile << "Total insurance premium: " << monthly_mortgage_insurance * 15 * 12 << std::endl;
    investmentFile << "Total invested capital: " << total_investment_deposits << std::endl;
    investmentFile << "Total mortgage + investment deposits: " << total_voluntary_contributions + total_investment_deposits << std::endl;
    investmentFile << "Total average savings: " << average_savings * sim_years * 12 << std::endl;
    investmentFile.close();
    mortgageFile.close();
}

// Main simulation loop
int main(){
    int simulation_years = sim_years;
    current_capital = initial_capital;

    // Run simulation year by year
    while (simulation_years > 0){
        investmentFile << "** Simulation year: " << sim_years - simulation_years + 1 << " **\n";

        // Process each month
        for (int i = 0; i < 12; i++){
            // Handle mortgage if still active
            if (current_capital > 0) mortgageAmortization();

            // Execute appropriate phase
            if (!phase_1_completed) phaseOne();
            if (phase_1_completed && !phase_2_completed) phaseTwo();
            if (phase_2_completed && !phase_3_completed) phaseThree();

            // Calculate monthly investment returns
            calculatePortfolioInterest();
        }
        simulation_years--;
    }

    // Print final summaries
    printPhaseEndSummary("Phase 3");
    printFinalSummary();
    return 0;
}
