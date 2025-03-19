# Investment Plan Simulation

## Description
Simple C++ program to simulate an investment plan conditioned by mortgage amortization. The simulation is divided into three phases as described below.

## Simulation Phases

### Phase 1: Building a Safety Cushion
**Objective:** Ensure the `safety_cushion` reaches the `MIN_CUSHION` value.
- Each month, deposit into `safety_cushion` the remaining amount from `average_savings` after deducting the `additional_contribution` paid on the mortgage.
- Once the objective is reached, `phase_1_completed` becomes true, and the simulation proceeds to Phase 2.

### Phase 2: Investing in Bonds and Stocks
**Objective:** Start investing in bonds and stocks to reach a defined proportion of `invested_capital`.
- Deposit amount is the difference between `average_savings` and `additional_contribution` paid on the mortgage.
- Phase 2 ends when the mortgage is fully paid off.
- The mortgage will be reduced by 5 years once the payment falls below a specific value.
- `additional_contribution` will be lower and occur yearly instead of monthly.

### Phase 3: Long-Term Investment Strategy
**Objective:** Continue investing in all three portfolio components (`safety_cushion`, bonds, and stocks) according to a new proportion.
- Since there is no more `additional_contribution`, all `average_savings` will be invested entirely.
- Phase 3 ends once the simulation is completed.

---
This program helps simulate a disciplined investment approach while balancing mortgage obligations, ensuring financial security, and optimizing investment growth over time.
