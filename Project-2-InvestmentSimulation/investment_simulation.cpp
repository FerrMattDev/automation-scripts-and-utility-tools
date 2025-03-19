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

// -General variables
constexpr int simulation_years = 30;            // years for which the simulation is calculated
constexpr double average_savings = 1300;        // average savings value taken from tracking
constexpr double average_savings_phase_3 = 1650; // savings after mortgage is paid off

// Investment variables
constexpr double cushion_min = 25000; // minimum value to reach for the safety cushion
constexpr double cushion_max = 35000; // max value for the cushion after which we stop contributing to it

double safety_cushion {25000};     // initial safety cushion value
double bonds {0};                  // initial bonds investment value
double stocks {0};                 // initial stocks investment value
bool phase_1_completed = false;
bool phase_2_completed = false;
bool phase_3_completed = false;
double phase_months = 0;
double total_investment_contributions = 0;

// allocation phase 2 with cushion < cushion_max
constexpr double cushion_allocation_phase_2 = 0.25;
constexpr double bonds_allocation_phase_2 = 0.25;
constexpr double stocks_allocation_phase_2 = 0.5;

// without cushion if cushion_max is reached
constexpr double bonds_allocation_phase_2_wc = 0.5;
constexpr double stocks_allocation_phase_2_wc = 0.5;

// allocation phase 3 with cushion < cushion_max
constexpr double cushion_allocation_phase_3 = 0.20;
constexpr double bonds_allocation_phase_3 = 0.20;
constexpr double stocks_allocation_phase_3 = 0.60;

// without cushion if cushion_max is reached
constexpr double bonds_allocation_phase_3_wc = 0.35;
constexpr double stocks_allocation_phase_3_wc = 0.65;

// assumed average interest for each pillar
constexpr double cushion_interest = 0.015;
constexpr double bonds_interest = 0.02;
constexpr double stocks_interest = 0.06;

// interests calculated monthly for monthly simulation
constexpr double cushion_increase = std::pow(1 + cushion_interest, 1.0/12) - 1;
constexpr double bonds_increase = std::pow(1 + bonds_interest, 1.0/12) - 1;
constexpr double stocks_increase = std::pow(1 + stocks_interest, 1.0/12) - 1;


// Mortgage variables
constexpr double initial_capital = 150000;
constexpr double monthly_mortgage_insurance = 37.5;
constexpr double max_monthly_total_payment = 1900;
constexpr double interest = 0.0252 / 12; // Monthly interest

double current_capital = 0;
int mortgage_months = 20 * 12; // 20 years in months
double additional_contribution = 0;
double monthly_interest = 0;
double payment = 0;
int mortgage_month_count = 1;
bool duration_reduced = false;
double total_voluntary_contributions = 0;
double total_mortgage_interest = 0;

std::ofstream investmentsFile("investment_simulation.txt");
std::ofstream mortgageFile("mortgage_simulation.txt");


void printMortgage() {
    mortgageFile << "----------------------------------------------\n";
    mortgageFile << "Month " << mortgage_month_count << " - Year: " <<  mortgage_month_count/12+1 << "\n";
    mortgageFile << "Monthly mortgage payment: " << payment << " euros\n";
    if (additional_contribution > 0) mortgageFile << "Additional monthly payment: " << additional_contribution << "\n";
    mortgageFile << "Monthly interest: " << monthly_interest << "\n";
    mortgageFile << "Current capital: " << current_capital << "\n";
    mortgageFile << "----------------------------------------------\n";
}

void printPhaseEndSummary(std::string phase){
    double total_portfolio = safety_cushion + bonds + stocks;
    investmentsFile << "##############################################\n";
    investmentsFile << phase << " completed in " << phase_months << " months. Years: " << phase_months/12 << std::endl;
    investmentsFile << "Safety cushion: " << safety_cushion << "\n";
    investmentsFile << "Bonds: " << bonds << "\n";
    investmentsFile << "Stocks: " << stocks << "\n";
    if (current_capital > 0) investmentsFile << "Remaining mortgage: " << current_capital << "\n\n";
    investmentsFile << "Total assets: " << safety_cushion + bonds + stocks << "\n";
    if (phase != "Phase 1"){
        investmentsFile << "\% cushion: " << safety_cushion/total_portfolio << "\n";
        investmentsFile << "\% Bonds: " << bonds/total_portfolio << "\n";
        investmentsFile << "\% Stocks: " << stocks/total_portfolio << "\n";
        investmentsFile << "##############################################\n";
    }
}

void printAnnualSummary(int simulation_year){
    double total_portfolio = safety_cushion + bonds + stocks;
    investmentsFile << "##############################################\n";
    investmentsFile << "Investment summary for simulation year: " << simulation_year << std::endl;
    investmentsFile << "Total assets: " << total_portfolio << "\n";
    investmentsFile << "Safety cushion: " << safety_cushion << "\n";
    investmentsFile << "Bonds: " << bonds << "\n";
    investmentsFile << "Stocks: " << stocks << "\n";
    investmentsFile << "\% cushion: " << safety_cushion/total_portfolio << "\n";
    investmentsFile << "\% Bonds: " << bonds/total_portfolio << "\n";
    investmentsFile << "\% Stocks: " << stocks/total_portfolio << "\n";
    investmentsFile << "##############################################\n";
}

void printContributions(std::string phase, double cushion_contr, double bonds_contr, double stocks_contr){
    investmentsFile << "**********************************************\n";
    investmentsFile << phase << " - Contributions for month " << phase_months << ":\n";
    if (cushion_contr > 0) investmentsFile << "Contribution to cushion: " << cushion_contr << std::endl;
    if (bonds_contr > 0) investmentsFile << "Contribution to bonds: " << bonds_contr << std::endl;
    if (stocks_contr > 0) investmentsFile << "Contribution to stocks: " << stocks_contr << std::endl;
    investmentsFile << "**********************************************\n";
}

void mortgageAmortization() {
    monthly_interest = current_capital * interest;
    total_mortgage_interest += monthly_interest;
    payment = (current_capital * interest) / (1 - std::pow(1 + interest, -mortgage_months));

    if (payment < 250 && !duration_reduced) {
        duration_reduced = true;
        mortgage_months -= 60; // Reduction of 5 years
        mortgageFile << "**** Mortgage reduced to 15 years ****\n";
    }
    if (!duration_reduced) {
        if (payment > 400){
            additional_contribution = std::min((max_monthly_total_payment - payment), average_savings);
        }else{
            additional_contribution = std::min(((max_monthly_total_payment / 3) - payment), average_savings);
        }
        current_capital -= (payment - monthly_interest) + additional_contribution;
    } else {
        additional_contribution = 0;
        if (mortgage_months % 12 == 0 && payment > 250) additional_contribution = 1200; // additional payment once a year
        current_capital -= (payment - monthly_interest) + additional_contribution;
    }
    total_voluntary_contributions += additional_contribution;
    printMortgage();
    mortgage_months--;
    mortgage_month_count++;
}

void phaseOne(){
    double contribution = average_savings - additional_contribution - monthly_mortgage_insurance;
    phase_months++;
    if (contribution > 0){
        safety_cushion += contribution;
        total_investment_contributions += contribution;
        //printContributions("Phase 1", contribution, 0, 0);
    }

    if (safety_cushion >= cushion_min){
        phase_1_completed = true;
        printPhaseEndSummary("Phase 1");
        phase_months = 0;
    }
}

void proportionalContributionWithCushion(std::string phase, double target_cushion, double target_bonds, double target_stocks, double contribution){
        total_investment_contributions += contribution;
        double total = safety_cushion + bonds + stocks;

        // Differences from target (only if positive, meaning the component is underrepresented)
        double diff_cushion = std::max(0.0, target_cushion * total - safety_cushion);
        double diff_bonds = std::max(0.0, target_bonds * total - bonds);
        double diff_stocks = std::max(0.0, target_stocks * total - stocks);

        // Total sum of needs (only for those that need a contribution)
        double sum_diff = diff_cushion + diff_bonds + diff_stocks;
        if (sum_diff > 0) {
            // Allocate the contribution proportionally to needs
            double cushion_contr = (diff_cushion / sum_diff) * contribution;
            double bonds_contr = (diff_bonds / sum_diff) * contribution;
            double stocks_contr = (diff_stocks / sum_diff) * contribution;
            safety_cushion += cushion_contr;
            bonds += bonds_contr;
            stocks += stocks_contr;
            //printContributions(phase, cushion_contr, bonds_contr, stocks_contr);
        }
}

void proportionalContributionWithoutCushion(std::string phase, double target_bonds, double target_stocks, double contribution){
        total_investment_contributions += contribution;
        double total = bonds + stocks;

        // Differences from target (only if positive, meaning the component is underrepresented)
        double diff_bonds = std::max(0.0, target_bonds * total - bonds);
        double diff_stocks = std::max(0.0, target_stocks * total - stocks);

        // Total sum of needs (only for those that need a contribution)
        double sum_diff = diff_bonds + diff_stocks;
        if (sum_diff > 0) {
            // Allocate the contribution proportionally to needs
            double bonds_contr = (diff_bonds / sum_diff) * contribution;
            double stocks_contr = (diff_stocks / sum_diff) * contribution;
            bonds += bonds_contr;
            stocks += stocks_contr;
            //printContributions(phase, 0, bonds_contr, stocks_contr);
        }
}

void phaseTwo(){
    if (current_capital > 0){
        phase_months++;
        double contribution = average_savings - additional_contribution - monthly_mortgage_insurance;
        if (safety_cushion < cushion_max)
            proportionalContributionWithCushion("Phase 2", cushion_allocation_phase_2, bonds_allocation_phase_2, stocks_allocation_phase_2, contribution);
        else
            proportionalContributionWithoutCushion("Phase 2", bonds_allocation_phase_2_wc, stocks_allocation_phase_2_wc, contribution);
    }else{
        phase_2_completed = true;
        printPhaseEndSummary("Phase 2");
        phase_months = 0;
    }
}

void phaseThree() {
    phase_months++;
    double contribution = average_savings_phase_3;
    if (safety_cushion < cushion_max)
        proportionalContributionWithCushion("Phase 3", cushion_allocation_phase_3, bonds_allocation_phase_3, stocks_allocation_phase_3, contribution);
    else
        proportionalContributionWithoutCushion("Phase 3", bonds_allocation_phase_3_wc, stocks_allocation_phase_3_wc, contribution);
}

void calculatePortfolioInterest(){
    // Calculate monthly interest based on average annual interest
    safety_cushion += safety_cushion * cushion_increase;
    bonds += bonds * bonds_increase;
    stocks += stocks * stocks_increase;
}

void printFinalSummary(){
    mortgageFile << "Total additional mortgage contributions: " << total_voluntary_contributions << std::endl;
    mortgageFile << "Total interest paid: " << total_mortgage_interest << std::endl;
    mortgageFile << "Total insurance premium: " << monthly_mortgage_insurance * 15 * 12 << std::endl; // multiplied by the mortgage duration in months
    investmentsFile << "Total invested capital: " << total_investment_contributions << std::endl;
    investmentsFile << "Total mortgage + investment contributions: " << total_voluntary_contributions + total_investment_contributions << std::endl;
    investmentsFile << "Total average savings: " << (average_savings * ((simulation_years * 12) - phase_months)) + (average_savings_phase_3 * phase_months) << std::endl;
    investmentsFile.close();
    mortgageFile.close();
}

int main(){
    int simulation_years_remaining = simulation_years;
    current_capital = initial_capital;

    // Simulation
    while (simulation_years_remaining > 0){
        investmentsFile << "** Simulation year: " << simulation_years - simulation_years_remaining + 1 << " **\n";
        for (int i = 0; i < 12; i++){
            if (current_capital > 0) mortgageAmortization();
            if (!phase_1_completed) phaseOne();
            if (phase_1_completed && !phase_2_completed) phaseTwo();
            if (phase_2_completed && !phase_3_completed) phaseThree();

            calculatePortfolioInterest();
        }
        printAnnualSummary(simulation_years - simulation_years_remaining + 1);
        simulation_years_remaining--;
    }
    printPhaseEndSummary("Phase 3");
    printFinalSummary();
    return 0;
}
