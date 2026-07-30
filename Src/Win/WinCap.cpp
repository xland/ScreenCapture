#include "pch.h"
#include "WinCap.h"

std::unique_ptr<WinCap> winCap;

WinCap::WinCap() :Ling::WinBase()
{
}

WinCap::~WinCap()
{
}

void WinCap::init()
{
	auto ptr = new WinCap();
	winCap.reset(ptr);
}
