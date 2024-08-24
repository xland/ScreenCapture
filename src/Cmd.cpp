#include "Cmd.h"
#include <sstream>
#include <iomanip>
#include <memory>

namespace {
    std::shared_ptr<Cmd> cmd;
}

Cmd::Cmd()
{
}

Cmd::~Cmd()
{
}

void Cmd::Init(const std::wstring& cmdLine) {
    cmd = std::shared_ptr<Cmd>{new Cmd()};
    std::wistringstream stream(cmdLine);
    std::wstring arg;
    while (stream >> std::quoted(arg)) {
        cmd->args.push_back(arg);
    }
}

Cmd* Cmd::Get()
{
    return cmd.get();
}
