#include "TbtaTableCell.h"

namespace TbtaFileProcessing
{
	TbtaTableCell::TbtaTableCell(const TbtaString &cellText)
		:CellText(TbtaString(cellText))
	{
	}


	TbtaTableCell::~TbtaTableCell()
	{
	}

	TbtaTableCell::operator TbtaString(){return CellText;}
}
