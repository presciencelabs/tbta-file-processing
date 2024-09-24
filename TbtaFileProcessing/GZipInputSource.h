#pragma once

#include <string>
#include <xercesc/sax/InputSource.hpp>

namespace TbtaFileProcessing
{
	class GZipInputSource :	public xercesc::InputSource
	{
	public:
		GZipInputSource(const std::string &zipFilePath, const std::string &embeddedXmlFilePath);

		xercesc::BinInputStream* makeStream(void) const;

	private:
		std::string _zipFilePath;
		std::string _embeddedXmlFilePath;

		// prevent copying and assignment; not implemented
		GZipInputSource(const GZipInputSource &);
		GZipInputSource& operator=(const GZipInputSource &);
	};
};