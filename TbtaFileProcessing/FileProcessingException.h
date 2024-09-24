#pragma once

#include <exception>

namespace TbtaFileProcessing
{
	class FileProcessingException :	public virtual std::exception
	{
	public:
		enum /*class*/ FailureType
		{
			//Nested tables are not currently supported.  This occurs
			//when the file has a table within a table.
			NestedTablesNotSupported,

			//occurs when xerces scanFirst() call failes.  Likely
			//indicates a malformed xml document.
			UnableToFindRootElement,

			//occurs when an attempt is made to retrieve the current table,
			//but there is currently no table ready to be retrieved.
			TableNotReady,

			//occurs when an attempt is made to retrieve the current grammar section,
			//but there is currently no grammar section ready to be retrieved.
			GrammarSectionNotReady,

			//UNZ_CRCERROR from unzip.h
			//Occurs when closing the undelrying gzip file,
			//and all the file was read but the CRC is not good.
			//This indicates that although the file was read, the
			//data may contain errors, because the file has been corrupted somewhere.
			CyclicRedundancyCheckFailed,

			//http://www.zlib.net/manual.html
			//Z_ERRNO
			//If an error occurred in the file system and not in the compression
			//library, errnum is set to Z_ERRNO and the application may consult 
			//errno to get the exact error code.
			//TBTA does not currently capture the errno in its exceptions.
			ZlibFileSystemError,

			//http://www.zlib.net/manual.html
			//Z_STREAM_ERROR
			//Returned if level is not a valid compression level (unlikely in TBTA)
			//Returned if the stream state was inconsistent
			ZlibStreamError,

			//http://www.zlib.net/manual.html
			//Z_DATA_ERROR
			//Returned if there was a format error in the deflate stream (corrupted file)
			//Returned if the input data was corrupted or incomplete.
			//Returned if the stream was freed prematurely (some input or output was discarded)
			//Returned if no flush point has been found
			ZlibDataError,

			//http://www.zlib.net/manual.html
			//Z_MEM_ERROR
			//Returned if out of memory
			ZlibOutOfMemory,

			//http://www.zlib.net/manual.html
			//Z_BUF_ERROR
			//Returned if the last read ended in the middle of a gzip stream.
			//Returned if no more input was provided
			ZlibBufferError,

			//http://www.zlib.net/manual.html
			//Z_VERSION_ERROR
			//Returned if the zlib library version (zlib_version) is incompatible with the version assumed by the caller (unlikely in TBTA)
			ZlibVersionError,

			//Shouldn't ever actually happen, but here as a guard
			//in case ZLib gives a different error than the errors
			//documented in the manual, or if we miss one that is documented.
			ZlibUnknownError
		};

		FileProcessingException(FailureType failureType);

		FailureType GetFailureType() const;

	private:
		FailureType _failureType;
	};
}