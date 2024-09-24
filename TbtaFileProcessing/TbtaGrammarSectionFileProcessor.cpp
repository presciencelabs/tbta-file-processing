#include "TbtaGrammarSectionFileProcessor.h"

#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "DocxSaxHandler.h"
#include "GZipInputSource.h"
#include "AbstractTableReader.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>

using namespace xercesc;

namespace TbtaFileProcessing
{
	struct TbtaGrammarSectionFileProcessor::TbtaGrammarSectionFileProcessorImpl
	{
		TbtaGrammarSectionFileProcessorImpl(const std::string &filePath)
			: _filePath(new std::string(filePath)),
			_inputSource(new GZipInputSource(*_filePath, "word/document.xml")),
			_saxHandler(new DocxSaxHandler()),
			_parser(XMLReaderFactory::createXMLReader()),
			_token(new xercesc::XMLPScanToken())
		{
		}

		~TbtaGrammarSectionFileProcessorImpl()
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

	TbtaGrammarSectionFileProcessor::TbtaGrammarSectionFileProcessor(const std::string &filePath)
		: _pimpl(new TbtaGrammarSectionFileProcessorImpl(filePath))
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

	TbtaGrammarSectionFileProcessor::~TbtaGrammarSectionFileProcessor()
	{
		delete _pimpl;
	}

	TbtaGrammarSectionFileProcessor::iterator::iterator(const TbtaGrammarSectionFileProcessor *outerRef, int position)
		: _outerRef(outerRef), _position(position)
	{
	}

	TbtaGrammarSectionFileProcessor::iterator TbtaGrammarSectionFileProcessor::begin()
	{
		return ++iterator(this);
	}

	TbtaGrammarSectionFileProcessor::iterator TbtaGrammarSectionFileProcessor::end()
	{
		return iterator();
	}

	TbtaGrammarSectionFileProcessor::iterator& TbtaGrammarSectionFileProcessor::iterator::operator ++()
	{
		_outerRef->_pimpl->_saxHandler->ResetGrammarSectionBuffers();

		//TODO code smell? too many levels of indirection?
		while (!_outerRef->_pimpl->_saxHandler->IsNextGrammarSectionReady() && _outerRef->_pimpl->_parser->parseNext(*_outerRef->_pimpl->_token));

		if (_outerRef->_pimpl->_saxHandler->IsNextGrammarSectionReady())
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

	bool TbtaGrammarSectionFileProcessor::iterator::operator!=(const iterator& rv)
	{
		return !(*this == rv);
	}

	bool TbtaGrammarSectionFileProcessor::iterator::operator == (const iterator& rv)
	{
		return (this->_position == rv._position && this->_outerRef == rv._outerRef);
	}

	TbtaString TbtaGrammarSectionFileProcessor::iterator::operator*() const
	{
		return _outerRef->_pimpl->_saxHandler->GetCurrentGrammarSection();
	}
}
