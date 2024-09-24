#pragma once

#include <vector>
#include "TbtaString.h"
#include "TbtaTableCell.h"

namespace TbtaFileProcessing
{
	typedef std::vector<TbtaTableCell> TbtaTableRow;
	typedef std::vector<TbtaTableRow> TbtaTable;
}