#pragma once

#include <bpf_core/fast_file.h>
#include <bpf_core/iec_parser.h>
#include <bpf_core/net/socket.h>
#include <bpf_core/net/validator.h>
#include <iostream>

namespace net
{

class BPFDriver
{
private:
    util::Buffer buf;
    Socket sock;
    iec::IecParser parser;
    FastFile output;
    Validator validator;

public:
    explicit BPFDriver() = delete;
    explicit BPFDriver(const BPFDriver &rhs) = delete;
    explicit BPFDriver(BPFDriver &&rhs) = delete;
    BPFDriver &operator=(const BPFDriver &rhs) = delete;
    BPFDriver &operator=(BPFDriver &&rhs) = delete;

    explicit BPFDriver(const Socket &socket, const std::string &filename)
        : buf { nullptr, util::BUFFER_SIZE, 0 }, sock(socket), output(filename)
    {
        buf.data = new ui8[buf.allocSize];
    }

    ~BPFDriver()
    {
        delete[] buf.data;
        sock.closeSock();
    }

    void run()
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
};

}
