# Wi-Fi Protocol Performance Comparison (WiFi 6 vs WiFi 5 vs WiFi 4)

## Overview

This project compares the performance of three Wi-Fi protocols: **IEEE 802.11ax (WiFi 6)**, **IEEE 802.11ac (WiFi 5)**, and **IEEE 802.11n (WiFi 4)**. The comparison is based on throughput, latency, and client capacity under various scenarios, using a simulation environment to emulate real-world performance.

## Project Structure

- **Abstract**:
  A brief introduction to the evolution of Wi-Fi protocols and the improvements in throughput and latency, especially with WiFi 6 (802.11ax).

- **Parameter Settings**:
  The key simulation parameters used for comparing the protocols:
  - **Modulation and Coding**: QAM-64 (802.11n), QAM-256 (802.11ac), QAM-1024 (802.11ax)
  - **Guard Interval**: 800 ns across all protocols
  - **Channel Width**: 40 MHz
  - **Frequency**: 5 GHz
  - **Spatial Streams**: Single stream using MIMO
  - **Payload Size**: 1448 bytes
  - **Number of Clients**: Ranges from 2 to 128
  - **Simulation Time**: 2 seconds per simulation

- **Scenarios**:
  The performance of each protocol was evaluated in three main scenarios:
  - **Low Client Numbers (2-4 clients)**: Minimal latency, with WiFi 6 showing the highest throughput.
  - **Medium Client Numbers (8-32 clients)**: WiFi 6 continued to outperform WiFi 5 and WiFi 4, especially in terms of bit rate and throughput stability.
  - **High Client Numbers (64-128 clients)**: While WiFi 5 showed faster response times in larger client scenarios, WiFi 6’s throughput remained higher overall.

- **Results and Discussion**:
  - **WiFi 6 (IEEE 802.11ax)** proved to have better performance in most scenarios, particularly with higher client numbers.
  - **WiFi 5 (IEEE 802.11ac)** showed competitive performance but faced throughput dips with increased client numbers.
  - **WiFi 4 (IEEE 802.11n)**, despite being an earlier generation, demonstrated relatively stable performance, though it was outperformed by the more modern protocols.

- **Conclusion**:
  WiFi 6 (802.11ax) emerged as the best performing protocol due to its superior handling of higher client densities and more advanced features like **OFDMA** and **enhanced MIMO**, making it the most reliable choice for modern, dense network environments.

## How to Run

1. **Simulation Environment Setup**:
   - Use the **ns3-40** simulator to replicate the results.
   - Install necessary dependencies and configure the environment as per the given parameter settings.

2. **Running the Simulation**:
   - Run the simulations for each protocol (802.11ax, 802.11ac, 802.11n) under varying client loads (2-128).
   - Observe the throughput and latency for each scenario.

3. **Data Analysis**:
   - Compare the throughput, bit rates, and latency results across all protocols.
   - Utilize the graphs generated from the simulation to visualize performance trends for different client loads.

## Files Included

- **Parameter Settings**: Details of the modulation, coding, and channel configurations used.
- **Simulation Scenarios**: Description of each scenario (low, medium, and high client numbers).
- **Results**: Output graphs showing throughput and latency for each protocol under different client numbers.
- **Conclusion**: Summarized findings highlighting the advantages of IEEE 802.11ax.

## Key Features

- Comparative analysis of Wi-Fi protocols (802.11ax, 802.11ac, 802.11n).
- Simulation of real-world performance scenarios.
- Performance evaluation with varying client numbers (2 to 128).
- Results focused on throughput, latency, and response times.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
