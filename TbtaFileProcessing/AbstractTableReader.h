#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include "TbtaFileProcessingTypeDefs.h"

namespace TbtaFileProcessing
{
	class AbstractTableReader : public xercesc::DefaultHandler
	{
	public:
		virtual bool IsTableReady() = 0;

		//throw exception if table not ready
		virtual TbtaTable GetCurrentTable() = 0;

		//reset the buffers so that the next table can be read.
		virtual void ResetTableBuffers() = 0;
	};
}