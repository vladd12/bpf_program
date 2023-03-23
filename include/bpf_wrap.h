#pragma once

#include <bcc/BPF.h>
#include <code_formater.h>
#include <common.h>

class BpfWrapper
{
private:
    std::unique_ptr<ebpf::BPF> bpfPtr;
    SourceCodeFormater bpfSrc;

    /// \brief Getting a raw socket and binding it to interface, specified by ifaceName.
    int get_raw_socket(const std::string &ifaceName);

public:
    /// \brief Default c-tor.
    explicit BpfWrapper(const std::string &programPath);

    /// \brief Filtering BPF program source code.
    void filterSourceCode(const std::string &iface, const std::string &srcMac, const std::string &svID);

    /// \brief Running BPF program.
    ebpf::StatusTuple run();

    /// \brief Returns eBPF object stored in ebpf::BPF smart pointer.
    ebpf::BPF *getBpfObject();

    /// \brief Attaching raw socket for listening ethernet interface, specified by ifaceName.
    ebpf::StatusTuple attachRawSocket(const std::string &ifaceName, const int function, int &socket);
};