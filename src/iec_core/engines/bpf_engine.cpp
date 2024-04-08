#include "iec_core/engines/bpf_engine.h"

#include <stdexcept>

namespace engines
{

BPFEngine::BPFEngine(const std::string &filename) : output(filename)
{
    buf.data = new u8[buf.allocSize];
}

BPFEngine::BPFEngine(const utils::Socket &socket, const std::string &filename) : BPFEngine(filename)
{
    sock = socket;
}

BPFEngine::~BPFEngine()
{
    delete[] buf.data;
    sock.closeSock();
}

void BPFEngine::run()
{
    while (true)
    {
        sock.nonBlockRead(buf);
        if (parser.update(buf.data, buf.readSize))
        {
            auto sequence = parser.parse();
            validator.update(sequence);

            auto curr = sequence.data[0].smpCnt;
            printf("Count: %04X\n", curr);

            if (sequence.data != nullptr)
                delete[] sequence.data;
        }
    }
}

//////////////////////////////////////////////////////////////////

EBPFEngine::EBPFEngine() : BaseEngine(), executor("bpf/ethernet-parse.c"), socket_fd(-1)
{
}

bool EBPFEngine::setup(const EngineSettings &settings)
{
    executor.filterSourceCode(settings.iface.data(), settings.sourceMAC.data(), settings.svID.data());
    auto status = executor.load();
    if (status.ok())
    {
        status = executor.getDeviceSocket(socket_fd, "iec61850_filter", settings.iface.data());
        if (status.ok() && socket_fd >= 0)
            return true;
    }
    std::cout << status.msg() << '\n';
    return false;
}

void EBPFEngine::run()
{
    utils::Socket socket(socket_fd);
    if (!socket.setNonBlockingMode())
        std::cout << "Couldn't set non-blocking read mode for socket!\n";

    while (running)
    {
        socket.readTo(buffer);
        //        if (buffer.getSize() < 800)
        //            ;
    }
}

} // namespace engines
