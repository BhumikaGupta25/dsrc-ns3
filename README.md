# DSRC Simulation in ns-3

This project is a **Dedicated Short-Range Communications (DSRC) simulation** implemented using the [ns-3 network simulator](https://www.nsnam.org/).  

The simulation models two vehicles equipped with 802.11p radios communicating Basic Safety Messages (BSMs) at 10Hz while moving towards each other at 72 km/h.  
It demonstrates the use of mobility models, realistic PHY/MAC configuration for 802.11p, and collects performance metrics like packet delivery ratio, delay, and throughput.
---

## Features

Configures two vehicle nodes with:
- IEEE 802.11p (DSRC) PHY/MAC
- Constant velocity mobility model at ±20 m/s (72 km/h)
- Realistic propagation and noise models

Sends periodic UDP packets (BSMs) from client to server.

Collects detailed metrics:
- Packet Delivery Ratio (PDR)
- Average delay
- Throughput
- Lost packets

Produces PCAP and ASCII traces for further analysis.

Logs key simulation events to console.
---

## Technologies Used

- [ns-3](https://www.nsnam.org/) (tested on version 3.xx)
- C++11
---

## Setup

### Install ns-3
Follow the official guide:  
[https://www.nsnam.org/wiki/Installation](https://www.nsnam.org/wiki/Installation)

### Build the simulation

Clone this repository:
```bash
git clone https://github.com/BhumikaGupta25/dsrc-ns3.git
cd dsrc-ns3
```

## Simulation Parameters

| **Parameter**         | **Value**         |
|-------------------------|-------------------|
| Vehicles                | 2                 |
| Initial distance        | 50 meters         |
| Velocity                | ±20 m/s (72 km/h)|
| Antenna height          | 1.5 m             |
| Frequency               | 5.9 GHz           |
| Tx Power                | 23 dBm            |
| Rx Gain                 | 10 dB             |
| Noise Figure            | 2 dB              |
| Packet Size             | 500 bytes         |
| BSM Rate                | 10 Hz             |
| Simulation Time         | 10 seconds        |

