#include "GZipBinInputStream.h"

#define ZLIB_WINAPI //TODO: not sure this define is necessary, but has to do with ZLIB integration
#define ZLIB_DLL    //TODO: not sure this define is necessary, but has to do with ZLIB integration
#include <unzip.h>

#include <iostream> //TODO: This is just for temporary tracking of execution

#include "FileProcessingException.h"

//TODO: Document with doxygen conventions
namespace TbtaFileProcessing
{
	//TODO: How to handle error conditions
	GZipBinInputStream::GZipBinInputStream(const std::string &zipFilePath, const std::string &embeddedXmlFilePath)
	{
		//std::wcout << _T("Opening ") << zipFilePath;

		_unzipFileHandle = unzOpen(zipFilePath.c_str());

		if (_unzipFileHandle != NULL)
		{
			//const char* primaryXmlFile = "word/document.xml";

			int locateFileResult = unzLocateFile(_unzipFileHandle, embeddedXmlFilePath.c_str(), 1); //1 is case-sensitive comparison.  See unzip.h

			if (locateFileResult == UNZ_OK)
			{
				std::wcout << "primary xml file located";

				int openFileResult = unzOpenCurrentFile(_unzipFileHandle);

				if (openFileResult == UNZ_OK)
				{
					std::wcout << "primary xml file opened";
				}
				else 
				{
					std::wcout << "error occurred opening " << "embedded xml file"; //<< embeddedXmlFilePath << " from " << zipFilePath <<". Result code " << openFileResult;
				}
			}
			else 
			{
				std::wcout << " Unzip error.  Result Code: " << locateFileResult;
			}
		}
		else
		{
			std::wcout << "File not found or could not be opened";
		}
	}

	GZipBinInputStream::~GZipBinInputStream(void)
	{
		int closeCurrentFileResult = unzCloseCurrentFile(_unzipFileHandle);
		
		if (closeCurrentFileResult == UNZ_CRCERROR)
			throw FileProcessingException(FileProcessingException::CyclicRedundancyCheckFailed); //and write test to demonstratef

		int closeFileResult = unzClose(_unzipFileHandle);

		if (closeFileResult != UNZ_OK)
			throw FileProcessingException(FileProcessingException::ZlibUnknownError);
	}


	XMLFilePos GZipBinInputStream::curPos(void) const
	{
		return NULL; //TODO
	}

	XMLSize_t GZipBinInputStream::readBytes(XMLByte *const readBuffer, const XMLSize_t readSize)
	{
		int readResult;
		
		//readResult will be number of bytes read unless there is an error.
		readResult = unzReadCurrentFile(_unzipFileHandle, readBuffer, readSize);

		//TODO: Does xerces have any expectations on how we deal with exceptional conditions?
		if (readResult < 0)
		{
			FileProcessingException::FailureType failureType;

			switch(readResult)
			{
				case Z_ERRNO:
					failureType = FileProcessingException::ZlibFileSystemError;
					break;
				case Z_STREAM_ERROR:
					failureType = FileProcessingException::ZlibStreamError;
					break;
				case Z_DATA_ERROR:
					failureType = FileProcessingException::ZlibDataError;
					break;
				case Z_MEM_ERROR:
					failureType = FileProcessingException::ZlibOutOfMemory;
					break;
				case Z_BUF_ERROR:
					failureType = FileProcessingException::ZlibBufferError;
					break;
				case Z_VERSION_ERROR:
					failureType = FileProcessingException::ZlibVersionError;
					break;
				default:
					failureType = FileProcessingException::ZlibUnknownError;
			}

			throw FileProcessingException(failureType);
		}

		return readResult;
	}
	
	const XMLCh* GZipBinInputStream::getContentType(void) const
	{
		//From the base class documentation:
		//Return the "out-of-band" content type for the data supplied by this 
		//input stream in the form of the media-type production (mime type with 
		//optional parameters such as encoding) as defined by the HTTP 1.1 
		//specification.
		//
		//If no such content type is provided for the data, 0 is returned. 
		//This function is expected to return the correct value at any time 
		//after the construction of the stream.
		return 0;
	}
}