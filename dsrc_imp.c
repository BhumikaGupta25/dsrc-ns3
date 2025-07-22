#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DsrcSimulation");

int main(int argc, char *argv[])
{
    // ========== 1. Enhanced Logging Configuration ==========
    LogComponentEnable("DsrcSimulation", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("WifiPhy", LOG_LEVEL_WARN);
    LogComponentEnable("Ipv4Interface", LOG_LEVEL_INFO);

    // ========== 2. Parameter Configuration ==========
    double distance = 50.0;    // Reduced initial distance (meters)
    double txPower = 23.0;     // Increased Tx power (dBm)
    uint32_t packetSize = 500; // BSM size (bytes)
    double interval = 0.1;     // 10Hz BSM rate

    // ========== 3. Node Creation ==========
    NodeContainer nodes;
    nodes.Create(2);
    NS_LOG_INFO("Created 2 vehicle nodes");

    // ========== 4. Enhanced WiFi Configuration ==========
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211p);

    // Improved channel model
    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::TwoRayGroundPropagationLossModel",
                               "Frequency", DoubleValue(5.9e9),
                               "HeightAboveZ", DoubleValue(1.5));

    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.Set("TxPowerStart", DoubleValue(txPower));
    phy.Set("TxPowerEnd", DoubleValue(txPower));
    phy.Set("RxGain", DoubleValue(10));       // Added receiver gain
    phy.Set("RxNoiseFigure", DoubleValue(2)); // Reduced noise

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac",
                "QosSupported", BooleanValue(true));

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);
    NS_LOG_INFO("Configured 802.11p PHY/MAC with:"
                << "\n  - Frequency: 5.9 GHz"
                << "\n  - TxPower: " << txPower << " dBm"
                << "\n  - DataRate: 6 Mbps");

    // ========== 5. Robust Mobility Setup ==========
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 1.5));      // Vehicle 1
    positionAlloc->Add(Vector(distance, 0.0, 1.5)); // Vehicle 2

    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(nodes);

    // Set velocities (20 m/s = 72 km/h)
    nodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(20, 0, 0));
    nodes.Get(1)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(-20, 0, 0));

    NS_LOG_INFO("Configured mobility:"
                << "\n  - Initial distance: " << distance << "m"
                << "\n  - Speed: ±20 m/s (72 km/h)"
                << "\n  - Antenna height: 1.5m");

    // ========== 6. Reliable Internet Stack ==========
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    // Verify IP assignment
    NS_LOG_INFO("IP Addresses assigned:");
    for (uint32_t i = 0; i < interfaces.GetN(); ++i)
    {
        NS_LOG_INFO("  Node " << i << ": " << interfaces.GetAddress(i));
    }

    // ========== 7. Application Layer with Validation ==========
    // Server (Node 1)
    uint16_t port = 5000; // Changed from port 9 to avoid restrictions
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Client (Node 0)
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(90)); // 10Hz * 9s
    echoClient.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(10.0));

    NS_LOG_INFO("Configured applications:"
                << "\n  - BSM rate: " << 1 / interval << "Hz"
                << "\n  - Packet size: " << packetSize << "B"
                << "\n  - Port: " << port);

    // ========== 8. Comprehensive Tracing ==========
    // PCAP tracing
    phy.EnablePcapAll("dsrc-sim");
    NS_LOG_INFO("Enabled PCAP tracing for all nodes");

    // ASCII tracing
    AsciiTraceHelper ascii;
    phy.EnableAsciiAll(ascii.CreateFileStream("dsrc-trace.tr"));
    NS_LOG_INFO("Enabled ASCII tracing");

    // ========== 9. Advanced Performance Monitoring ==========
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    NS_LOG_INFO("Enabled FlowMonitor");

    // ========== 10. Simulation Execution ==========
    NS_LOG_INFO("Starting simulation for 10 seconds...");
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    // ========== 11. Detailed Results Analysis ==========
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    std::cout << "\n=== Simulation Results ===" << std::endl;
    for (auto const &flow : stats)
    {
        std::cout << "Flow ID: " << flow.first << std::endl;
        std::cout << "  Tx Packets: " << flow.second.txPackets << std::endl;
        std::cout << "  Rx Packets: " << flow.second.rxPackets << std::endl;
        std::cout << "  Packet Delivery Ratio: "
                  << (flow.second.rxPackets * 100.0 / flow.second.txPackets) << "%" << std::endl;
        if (flow.second.rxPackets > 0)
        {
            std::cout << "  Average Delay: "
                      << (flow.second.delaySum.GetSeconds() / flow.second.rxPackets) << "s" << std::endl;
            std::cout << "  Throughput: "
                      << (flow.second.rxBytes * 8.0 / 9.0 / 1000) << " kbps" << std::endl;
        }
        else
        {
            std::cout << "  WARNING: No packets received!" << std::endl;
        }
    }

    Simulator::Destroy();
    NS_LOG_INFO("Simulation completed successfully");
    return 0;
}