#include "GZipInputSource.h"

#include <iostream> //TODO: Temp for debugging

#include "GZipBinInputStream.h"

namespace TbtaFileProcessing
{
	const unsigned int READ_BUFFER_SIZE = 16384;

	GZipInputSource::GZipInputSource(const std::string &zipFilePath, const std::string &embeddedXmlFilePath)
	{
		_zipFilePath = zipFilePath;
		_embeddedXmlFilePath = embeddedXmlFilePath;
	}

	xercesc::BinInputStream* GZipInputSource::makeStream(void) const
	{
		//TODO: Will Xerces free this memory, or do I need to?
		return new GZipBinInputStream(_zipFilePath, _embeddedXmlFilePath);
	}
};