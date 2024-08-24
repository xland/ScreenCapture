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
        auto pos = arg.find(L":");
        if (pos == std::wstring::npos) {
            cmd->args.insert({ arg ,L""});
        }
        else
        {
            cmd->args.insert({ arg.substr(0,pos) ,arg.substr(pos+1) });
        }        
    }
}

std::wstring Cmd::GetVal(const std::wstring& key) {
    return args[key];
}

Cmd* Cmd::Get()
{
    return cmd.get();
}
