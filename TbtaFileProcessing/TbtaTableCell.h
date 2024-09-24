#pragma once

#include <vector>
#include "TbtaString.h"

namespace TbtaFileProcessing
{
	class TbtaTableCell
	{
	public:

		TbtaTableCell(const TbtaString &cellText);
		~TbtaTableCell();

		//This is the entire text of the cell.
		TbtaString CellText;

		operator TbtaString();
	};
}