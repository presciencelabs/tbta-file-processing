#include "DocxSaxHandler.h"
#include <iostream>
#include <tchar.h>
#include <unordered_set>
#include <algorithm>
#include <xercesc/Util/XMLUniDefs.hpp>
#include <regex>
#include "FileProcessingException.h"

#undef max //MFC defines a max macro that interferes with std::max, so we undef it here.

using namespace std;
using namespace xercesc;

namespace TbtaFileProcessing
{

	//table indicator
	const XMLCh tbl[] =
	{
		chLatin_t, chLatin_b, chLatin_l, chNull
	};

	//table row indicator
	const XMLCh tr[] =
	{
		chLatin_t, chLatin_r, chNull
	};

	//table cell indicator
	const XMLCh tc[] =
	{
		chLatin_t, chLatin_c, chNull
	};

	//text indicator (we ignore formatting in the word document)
	const XMLCh t[] =
	{
		chLatin_t, chNull
	};

	//text "paragraph" indicator
	const XMLCh p[] =
	{
		chLatin_p, chNull
	};

	//text "run" indicator.  All text level formatting occurs within a "run".
	const XMLCh r[] =
	{
		chLatin_r, chNull
	};

	//text underline indicator
	const XMLCh u[] =
	{
		chLatin_u, chNull
	};

	//Apply single character replacements, and normalize multiple adjacent whitespace characters to a single space.
	static void NormalizeText(TbtaString &cellText, const std::unordered_map<wchar_t, wchar_t> &characterReplacments)
	{
		//static auto whitespace = std::unordered_set<wchar_t>({ ' ', '\t', '\r', '\n' });
		auto whitespace = std::unordered_set<wchar_t>();
		whitespace.insert(' ');
		whitespace.insert('\t');
		whitespace.insert('\r');
		whitespace.insert('\n');
		bool lastCharacterWasWhitespace = false;
		int currentLength = TbtaStringLength(cellText);

		for (int i = 0; i < currentLength; ++i)
		{
			wchar_t currentChar = TbtaStringGetAt(cellText, i);

			/*auto replacement = characterReplacments.find(currentChar);
			if (replacement != characterReplacments.end())
			{
				TbtaStringSetAt(cellText, i, replacement->second);
				lastCharacterWasWhitespace = false;
			}
			else */
			if (whitespace.find(currentChar) != whitespace.end())
			{
				if (lastCharacterWasWhitespace || i == 0)
				{
					TbtaStringRemoveAt(cellText, i);
					--currentLength;
					--i;
				}
				else 
				{
					TbtaStringSetAt(cellText, i, ' '); //normalize all whitespace characters to a single space.
				}

				lastCharacterWasWhitespace = true;
			}
			else
			{
				lastCharacterWasWhitespace = false;
			}
		}

		//celltext should not begin or end with whitespace (begin is already taken care of in previous code block, so just need to handle end
		//for a single space)
		int iLastChar = TbtaStringLength(cellText) - 1;
		if (iLastChar > -1 && whitespace.find(TbtaStringGetAt(cellText, iLastChar)) != whitespace.end())
		{
			TbtaStringRemoveAt(cellText, iLastChar);
		}
	}

	DocxSaxHandler::DocxSaxHandler() :
		_characterReplacements(std::unordered_map<wchar_t, wchar_t>()),
		_inTable(false),
		_inCell(false),
		_inCellText(false),
		_cellHasPreviousParagraph(false),
		_isTableReady(false),
		_cellRunStartIndex(-1),
		_cellRunIsUnderlined(false)
	{
	}

	DocxSaxHandler::DocxSaxHandler(const std::unordered_map<wchar_t, wchar_t> &characterReplacements) :
		_characterReplacements(characterReplacements),
		_inTable(false),
		_inCell(false),
		_inCellText(false),
		_cellHasPreviousParagraph(false),
		_isTableReady(false),
		_cellRunStartIndex(-1),
		_cellRunIsUnderlined(false)
	{
	}

	void DocxSaxHandler::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&     attrs)
	{
		//TODO: xml namespace checks?

		if (xercesc::XMLString::equals(localname, tbl))
		{
			if (_inTable)
				throw FileProcessingException(FileProcessingException::NestedTablesNotSupported);

			_inTable = true;
			_isTableReady = false;
			_tableBuffer.clear();
		}

		if (_inTable)
		{
			if (_inCell && xercesc::XMLString::equals(localname, r))
			{
				_cellRunStartIndex = TbtaStringLength(_currentCellText);
			}
			else if (_cellRunStartIndex > -1 && xercesc::XMLString::equals(localname, u))
			{
				_cellRunIsUnderlined = true;
			}
			else if (_inCell && xercesc::XMLString::equals(localname, t))
			{
				if (_cellHasPreviousParagraph)
					TbtaStringAppend(_currentCellText, L" ");

				_inCellText = true;
			}
			else if (xercesc::XMLString::equals(localname, tc))
			{
				_inCell = true;
			}
			else if (xercesc::XMLString::equals(localname, tr))
			{
				_tableBuffer.push_back(TbtaTableRow());
			}
		}
	}

	void DocxSaxHandler::characters(const XMLCh* const chars, const XMLSize_t length)
	{
		if (_inCellText)
		{
			TbtaStringAppendBufferLength(_currentCellText, chars, length);
		}
	}

	void DocxSaxHandler::endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname)
	{
		if (_inTable)
		{
			if (_inCell && xercesc::XMLString::equals(localname, t))
			{
				_inCellText = false;
			}
			else if (_inCell && xercesc::XMLString::equals(localname, p))
			{
				_cellHasPreviousParagraph = true;
			}
			else if (_inCell && xercesc::XMLString::equals(localname, r))
			{
				_cellRunIsUnderlined = false;
				_cellRunStartIndex = -1;
			}
			else if (xercesc::XMLString::equals(localname, tc))
			{
				_inCell = false;
				_cellHasPreviousParagraph = false;

				NormalizeText(_currentCellText, _characterReplacements);

				TbtaTableCell cell(_currentCellText);
				_tableBuffer.back().push_back(cell);
				TbtaStringClear(_currentCellText);
			}
			/*else if (xercesc::XMLString::equals(localname, tr))
			{

			}*/
			else if (xercesc::XMLString::equals(localname, tbl))
			{
				_inTable = false;
				_isTableReady = true;
			}
		}
	}

	bool DocxSaxHandler::IsTableReady()
	{
		return _isTableReady;
	}

	TbtaTable DocxSaxHandler::GetCurrentTable()
	{
		if (!_isTableReady)
			throw FileProcessingException(FileProcessingException::TableNotReady);

		return _tableBuffer;
	}

	void DocxSaxHandler::ResetTableBuffers()
	{
		_tableBuffer.clear();
		TbtaStringClear(_currentCellText);

		_inTable = false;
		_inCell = false;
		_inCellText = false;
		_cellHasPreviousParagraph = false;
		_isTableReady = false;
	}


	void DocxSaxHandler::fatalError(const SAXParseException& exception)
	{
		char* message = XMLString::transcode(exception.getMessage());
		wcout << "Fatal Error: " << message
			 << " at line: " << exception.getLineNumber()
			 << endl;
		XMLString::release(&message);
	}
}