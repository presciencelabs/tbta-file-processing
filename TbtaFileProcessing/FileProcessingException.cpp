#include "FileProcessingException.h"

namespace TbtaFileProcessing
{
	FileProcessingException::FileProcessingException(FailureType failureType)
	{
		_failureType = failureType;
	}

	FileProcessingException::FailureType FileProcessingException::GetFailureType() const
	{
		return _failureType;
	}
}