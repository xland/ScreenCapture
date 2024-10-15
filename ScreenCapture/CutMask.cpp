#include "CutMask.h"

namespace {
	std::unique_ptr<CutMask> cutMask;
}

CutMask::CutMask(QWidget *parent) : QWidget(parent)
{}

CutMask::~CutMask()
{}

void CutMask::Init()
{
	cutMask = std::make_unique<CutMask>(nullptr);
	cutMask->hide();
}

CutMask* CutMask::Get()
{
	return cutMask.get();
}
