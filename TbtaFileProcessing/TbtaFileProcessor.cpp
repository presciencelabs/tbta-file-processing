#include "TbtaFileProcessor.h"

#include <iostream>
#include <tchar.h>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "DocxSaxHandler.h"
#include "GZipInputSource.h"
#include "AbstractTableReader.h"
#include "TbtaGrammarSectionFileProcessor.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>

using namespace xercesc;

namespace TbtaFileProcessing
{
	struct TbtaFileProcessor::TbtaFileProcessorImpl
	{
		TbtaFileProcessorImpl(const std::string &filePath, const std::unordered_map<wchar_t, wchar_t> &characterReplacements)
			: _filePath(new std::string(filePath)),
			_inputSource(new GZipInputSource(*_filePath, "word/document.xml")),
			_saxHandler(new DocxSaxHandler(characterReplacements)),
			_parser(XMLReaderFactory::createXMLReader()),
			_token(new xercesc::XMLPScanToken())
		{
		}

		~TbtaFileProcessorImpl()
		{
			delete _filePath;
			delete _inputSource;
			delete _saxHandler;
			delete _parser;
			delete _token;
		}

		const std::string *_filePath;
		GZipInputSource *_inputSource; //wrap gzip into an input source that xerces understands.
		AbstractTableReader *_saxHandler;
		xercesc::SAX2XMLReader *_parser;
		xercesc::XMLPScanToken *_token;
	};

	TbtaFileProcessor::TbtaFileProcessor(const std::string &filePath)
		: _pimpl(new TbtaFileProcessorImpl(filePath, std::unordered_map<wchar_t, wchar_t>()))
	{
		_pimpl->_parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		_pimpl->_parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

		_pimpl->_parser->setContentHandler(_pimpl->_saxHandler);
		_pimpl->_parser->setErrorHandler(_pimpl->_saxHandler);

		// Scan to root element in xml file.
		if (!_pimpl->_parser->parseFirst(*_pimpl->_inputSource, *_pimpl->_token))
		{
			throw FileProcessingException(FileProcessingException::UnableToFindRootElement);
		}
	}

	TbtaFileProcessor::TbtaFileProcessor(const std::string &filePath, const std::unordered_map<wchar_t, wchar_t>& characterReplacements)
		: _pimpl(new TbtaFileProcessorImpl(filePath, characterReplacements))
	{
		_pimpl->_parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		_pimpl->_parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

		_pimpl->_parser->setContentHandler(_pimpl->_saxHandler);
		_pimpl->_parser->setErrorHandler(_pimpl->_saxHandler);

		// Scan to root element in xml file.
		if (!_pimpl->_parser->parseFirst(*_pimpl->_inputSource, *_pimpl->_token))
		{
			throw FileProcessingException(FileProcessingException::UnableToFindRootElement);
		}
	}

	void TbtaFileProcessor::Initialize()
	{
		XMLPlatformUtils::Initialize();
	}

	void TbtaFileProcessor::Terminate()
	{
		XMLPlatformUtils::Terminate();
	}

	std::vector<TbtaTable> TbtaFileProcessor::GetAllTables(const std::string filePath)
	{
		return GetAllTables(filePath, std::unordered_map<wchar_t, wchar_t>());
	}

	std::vector<TbtaTable> TbtaFileProcessor::GetAllTables(const std::string filePath, const std::unordered_map<wchar_t, wchar_t>& characterReplacements)
	{
		TbtaFileProcessor p(filePath, characterReplacements);

		std::vector<TbtaTable> allTables;

		for (TbtaFileProcessor::iterator fileIterator = p.begin(); fileIterator != p.end(); ++fileIterator)
		{
			allTables.push_back(*fileIterator);
		}

		return allTables;
	}

	TbtaFileProcessor::~TbtaFileProcessor()
	{
		delete _pimpl;
	}

	TbtaFileProcessor::iterator::iterator(const TbtaFileProcessor *outerRef, int position) 
		: _outerRef(outerRef), _position(position)
	{
	}

	TbtaFileProcessor::iterator TbtaFileProcessor::begin()
	{
		return ++iterator(this);
	}

	TbtaFileProcessor::iterator TbtaFileProcessor::end()
	{
		return iterator();
	}

	TbtaFileProcessor::iterator& TbtaFileProcessor::iterator::operator ++()
	{
		_outerRef->_pimpl->_saxHandler->ResetTableBuffers();

		//TODO code smell? too many levels of indirection?
		while (!_outerRef->_pimpl->_saxHandler->IsTableReady() && _outerRef->_pimpl->_parser->parseNext(*_outerRef->_pimpl->_token));

		if (_outerRef->_pimpl->_saxHandler->IsTableReady())
		{
			++_position;
		}
		else
		{
			_outerRef = NULL;
			_position = END;
		}

		return *this;
	}

	bool TbtaFileProcessor::iterator::operator!=(const iterator& rv)
	{
		return !(*this == rv);
	}

	bool TbtaFileProcessor::iterator::operator == (const iterator& rv)
	{
		return (this->_position == rv._position && this->_outerRef == rv._outerRef);
	}

	TbtaTable TbtaFileProcessor::iterator::operator*() const
	{
		return _outerRef->_pimpl->_saxHandler->GetCurrentTable();
	}
}
