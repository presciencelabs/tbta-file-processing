#pragma once

#include <string>
#include <xercesc/util/BinInputStream.hpp>

namespace TbtaFileProcessing
{
	class GZipBinInputStream : public xercesc::BinInputStream
	{
	public:
		GZipBinInputStream(const std::string &zipFilePath, const std::string &embeddedXmlFilePath);
		~GZipBinInputStream(void);

		XMLFilePos curPos(void) const;
		XMLSize_t readBytes(XMLByte *const, const XMLSize_t);
		const XMLCh* getContentType(void) const;

	private:
		void* _unzipFileHandle;
	};
}