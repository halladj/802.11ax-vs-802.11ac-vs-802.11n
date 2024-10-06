#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/tcp-westwood-plus.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"




NS_LOG_COMPONENT_DEFINE("wifi-tcp");

using namespace ns3;

Ptr<PacketSink> sink;     //!< Pointer to the packet sink application
uint64_t lastTotalRx = 0; //!< The value of the last total received bytes

/**
 * Calculate the throughput
 */
void
CalculateThroughput()
{
    Time now = Simulator::Now(); /* Return the simulator's virtual time. */
    double cur = (sink->GetTotalRx() - lastTotalRx) * 8.0 /
                 1e5; /* Convert Application RX Packets to MBits. */
    std::cout << now.GetSeconds() << "s: \t" << cur << " Mbit/s" << std::endl;
    lastTotalRx = sink->GetTotalRx();
    Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);
}

int
main(int argc, char* argv[])
{
    uint32_t payloadSize = 1448;           /* Transport layer payload size in bytes. */
    std::string dataRate = "5000Mbps";      /* Application layer datarate. */
    std::string tcpVariant = "TcpNewReno"; /* TCP variant type. */
    std::string phyRate = "VhtMcs9";        /* Physical layer bitrate == QAM-256 5/6*/
    double simulationTime = 2;            /* Simulation time in seconds. */
    bool pcapTracing = false;              /* PCAP Tracing is enabled or not. */

    uint32_t nWifi = 128;


    tcpVariant = std::string("ns3::") + tcpVariant;
    // Select TCP variant
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(tcpVariant, &tcpTid),
                        "TypeId " << tcpVariant << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(tcpVariant)));

    /* Configure TCP Options */
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard(WIFI_STANDARD_80211ac);

    /* Set up Legacy Channel */
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    // 5 Gz
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5e9));

    /* Setup Physical Layer */
    YansWifiPhyHelper wifiPhy;
//    Config::SetDefault("ns3::WifiPhy::ChannelSettings", StringValue("{50, 160, BAND_5GHZ, 0}"));
    Config::SetDefault("ns3::WifiPhy::ChannelSettings", StringValue("{0, 40, BAND_5GHZ, 0}"));

    // guard interval
    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HtConfiguration/LongGuardIntervalSupported", BooleanValue(true));

    // sparail 1
    wifiPhy.Set("Antennas", UintegerValue(1));
    wifiPhy.Set("MaxSupportedRxSpatialStreams", UintegerValue(1));
    wifiPhy.Set("MaxSupportedTxSpatialStreams", UintegerValue(1));

    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");


    wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                       "DataMode", StringValue (phyRate),
                                       "ControlMode", StringValue("HtMcs0"));//HtMcs0
    /*
     *
     *
     *
     * FROM HERE WE START
     *
     *
     *
     */

    NodeContainer networkNodes;
    networkNodes.Create(1);

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);

    Ptr<Node> apWifiNode = networkNodes.Get(0);
    //Ptr<Node> staWifiNode = networkNodes.Get(1);


    //////////////////
    //
    // HERE  MAC layer
    //
    /////////////////

    Ssid ssid = Ssid("network");



    /* Configure AP */

    NetDeviceContainer apDevice;

    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevice = wifiHelper.Install(wifiPhy, wifiMac, apWifiNode);

    /* Configure STA */
    NetDeviceContainer staDevices;

    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    staDevices = wifiHelper.Install(wifiPhy, wifiMac, wifiStaNodes);



    //////////////////
    //
    // END:
    //
    //////////////////


    //////////////////
    //
    // MOBILITY
    //
    //////////////////

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(21.21, 0.0, 0.0));
    positionAlloc->Add(Vector(-21.21, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 21.21, 0.0));
    positionAlloc->Add(Vector(0.0, -21.21, 0.0));
    positionAlloc->Add(Vector(15.0, 15.0, 0.0));

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apWifiNode);
    mobility.Install(wifiStaNodes);

    ///////////////////
    //
    // END OF MOBILITY
    //
    ///////////////////

    /* Internet stack */
    InternetStackHelper stack;
    stack.Install(networkNodes);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.255.0");

    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevice);
    //Ipv4InterfaceContainer staInterface;
    //staInterface = address.Assign(staDevices);
    address.Assign(staDevices);

    /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    /* Install TCP Receiver on the access point */
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
                                InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install(apWifiNode);
    sink = StaticCast<PacketSink>(sinkApp.Get(0));

    /* Install TCP/UDP Transmitter on the station */
    OnOffHelper server("ns3::TcpSocketFactory", (InetSocketAddress(apInterface.GetAddress(0), 9)));
    server.SetAttribute("PacketSize", UintegerValue(payloadSize));
    server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    ApplicationContainer serverApp = server.Install(wifiStaNodes);

    /* Start Applications */
    sinkApp.Start(Seconds(0.0));
    serverApp.Start(Seconds(1.2));
    Simulator::Schedule(Seconds(1.1), &CalculateThroughput);

    /* Enable Traces */
    if (pcapTracing)
    {
        wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        wifiPhy.EnablePcap("AccessPoint", apDevice);
        wifiPhy.EnablePcap("Station", staDevices);
    }

    /* Start Simulation */

    Simulator::Stop(Seconds(3.0));

    AsciiTraceHelper ascii;
    wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("wifi4.tr"));

    Simulator::Run();

    double averageThroughput = ((sink->GetTotalRx() * 8) / (1e6 * simulationTime));

    Simulator::Destroy();

    if (averageThroughput < 50)
    {
        NS_LOG_ERROR("Obtained throughput is not in the expected boundaries!");
        exit(1);
    }
    std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;
    return 0;
}

