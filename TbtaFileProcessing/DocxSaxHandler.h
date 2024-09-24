#pragma once

#include <list>
#include <unordered_map>
#include "AbstractTableReader.h"
#include "TbtaFileProcessingTypeDefs.h"

namespace TbtaFileProcessing
{
	class DocxSaxHandler : public AbstractTableReader
	{
	public:
		DocxSaxHandler(void);
		DocxSaxHandler(const std::unordered_map<wchar_t, wchar_t> &cellCharacterReplacements);

		virtual void startElement(
			const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   xercesc::Attributes&     attrs
		);

		virtual void characters(
			const   XMLCh* const chars,
			const XMLSize_t length
		);

		virtual void endElement(
			const XMLCh* const uri,
			const XMLCh* const localname,
			const XMLCh* const qname
		);

		virtual bool IsTableReady() override;

		virtual TbtaTable GetCurrentTable() override;

		virtual void ResetTableBuffers() override;

		void fatalError(const xercesc::SAXParseException&) override;

	private:
		TbtaTable _tableBuffer;
		TbtaString _currentCellText;
		std::unordered_map<wchar_t, wchar_t> _characterReplacements;

		bool _inTable;
		bool _inCell;
		bool _inCellText;
		int _cellRunStartIndex; //-1 if not in run
		bool _cellRunIsUnderlined;
		bool _cellHasPreviousParagraph;
		bool _isTableReady;
	};
}
