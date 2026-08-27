
namespace {

void PrintChannelList(
    const char* label,
    const std::vector<int>& channels,
    std::ostream& out)
{
    out << "    " << label << " : ";
    for (int index = 0; index < int(channels.size()); ++index) {
        if (index > 0)
            out << ", ";

        out << label[0] << index << "=";
        if (channels[index] >= 0)
            out << channels[index];
        else
            out << "missing";
    }
    out << '\n';
}

} // namespace


void print_si_channel_mapping()
{
    STDSiArray siArray;
    if (!siArray.Init()) {
        std::cerr << "ERROR: STDSiArray::Init() failed." << std::endl;
        return;
    }

    const int nDetectors = siArray.GetSiNum();
    const int nOhmic = siArray.GetSiOhmicNum();
    const int nJunction = siArray.GetSiJuncNum();
    const int nAget = siArray.GetAgetNum();
    const int nChannels = siArray.GetChanNum();

    std::cout
        << "============================================================\n"
        << " STDSiArray channel mapping\n"
        << "============================================================\n"
        << "FPN channels : ";

    bool firstFPN = true;
    for (int channel = 0; channel < nChannels; ++channel) {
        if (!siArray.IsFPNChannel(channel))
            continue;

        if (!firstFPN)
            std::cout << ", ";
        std::cout << channel;
        firstFPN = false;
    }
    std::cout << "\n\n";

    for (int detectorID = 0; detectorID < nDetectors; ++detectorID) {
        std::vector<int> ohmicChannels(nOhmic, -1);

        // AGET 0의 실제 map을 역조회하여 O0~O3에 해당하는 channel을 찾는다.
        for (int channel = 0; channel < nChannels; ++channel) {
            if (siArray.IsFPNChannel(channel))
                continue;
            if (siArray.GetSiDetID(0, channel) != detectorID)
                continue;

            const int ohmicID = siArray.GetOhmicID(0, channel);
            if (0 <= ohmicID && ohmicID < nOhmic)
                ohmicChannels[ohmicID] = channel;
        }

        // Junction은 AGET 1, 2, 3을 모두 검사한다.
        std::vector<std::vector<int>> junctionChannels(
            nAget, std::vector<int>(nJunction, -1));

        for (int aget = 1; aget < nAget; ++aget) {
            for (int channel = 0; channel < nChannels; ++channel) {
                if (siArray.IsFPNChannel(channel))
                    continue;
                if (siArray.GetSiDetID(aget, channel) != detectorID)
                    continue;

                const int junctionID = siArray.GetJuncID(aget, channel);
                if (0 <= junctionID && junctionID < nJunction)
                    junctionChannels[aget][junctionID] = channel;
            }
        }

        std::cout
            << "------------------------------------------------------------\n"
            << "Detector ID   : " << detectorID << '\n'
            << "Detector name : " << siArray.GetSiName(detectorID) << '\n'
            << "ASAD          : " << siArray.GetAsAdID() << '\n'
            << "Ohmic AGET    : 0\n";

        PrintChannelList("Ohmic", ohmicChannels, std::cout);

        bool foundJunction = false;
        for (int aget = 1; aget < nAget; ++aget) {
            bool hasThisDetector = false;
            for (int junctionID = 0; junctionID < nJunction; ++junctionID) {
                if (junctionChannels[aget][junctionID] >= 0) {
                    hasThisDetector = true;
                    break;
                }
            }

            if (!hasThisDetector)
                continue;

            foundJunction = true;
            std::cout << "Junction AGET : " << aget << '\n';
            PrintChannelList(
                "Junction", junctionChannels[aget], std::cout);

            // J0/J1, J2/J3, ...은 같은 physical strip의 charge-sharing pair이다.
            std::cout << "    Strip pairs: ";
            for (int stripID = 0;
                 stripID < siArray.GetSiStripNum();
                 ++stripID) {
                if (stripID > 0)
                    std::cout << ", ";

                const int firstJunctionID = 2 * stripID;
                const int secondJunctionID = firstJunctionID + 1;
                std::cout
                    << "S" << stripID << "=(ch"
                    << junctionChannels[aget][firstJunctionID]
                    << ",ch"
                    << junctionChannels[aget][secondJunctionID]
                    << ")";
            }
            std::cout << '\n';
        }

        if (!foundJunction)
            std::cout << "Junction AGET : missing\n";

        std::cout << '\n';
    }

    // AGET별로 어느 Si detector가 배치되었는지도 마지막에 요약한다.
    std::cout
        << "============================================================\n"
        << " Junction detector summary by AGET\n"
        << "============================================================\n";

    for (int aget = 1; aget < nAget; ++aget) {
        std::vector<bool> detectorFound(nDetectors, false);

        for (int channel = 0; channel < nChannels; ++channel) {
            if (siArray.IsFPNChannel(channel))
                continue;

            const int detectorID = siArray.GetSiDetID(aget, channel);
            const int junctionID = siArray.GetJuncID(aget, channel);
            if (0 <= detectorID && detectorID < nDetectors &&
                junctionID >= 0)
                detectorFound[detectorID] = true;
        }

        std::cout << "AGET " << aget << " : ";
        bool foundAny = false;
        for (int detectorID = 0; detectorID < nDetectors; ++detectorID) {
            if (!detectorFound[detectorID])
                continue;

            if (foundAny)
                std::cout << ", ";
            std::cout
                << "ID " << detectorID
                << " (" << siArray.GetSiName(detectorID) << ")";
            foundAny = true;
        }

        if (!foundAny)
            std::cout << "no mapped junction detector";
        std::cout << '\n';
    }

    std::cout
        << "============================================================"
        << std::endl;
}

