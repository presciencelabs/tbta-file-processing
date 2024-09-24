#pragma once

#include <string>
#include <iterator>
#include "TbtaFileProcessingTypeDefs.h"
#include "FileProcessingException.h"

namespace TbtaFileProcessing
{
	class GZipInputSource;

	class TbtaGrammarSectionFileProcessor
	{

	public:
		///Constructor
		TbtaGrammarSectionFileProcessor(const std::string &filePath);

		virtual ~TbtaGrammarSectionFileProcessor();

		class iterator;
		friend class iterator;

		iterator begin(void);
		iterator end(void);

		//TODO: verify all requirements for input iterators http://www.scribd.com/doc/89045912/645/Input-iterators, http://ksvanhorn.com/Articles/input_iter.html
		class iterator : public std::iterator<std::input_iterator_tag, TbtaTable>
		{
		public:
			static const int END = -1;
			iterator(const TbtaGrammarSectionFileProcessor *outerRef = NULL, int position = END);
			//virtual ~iterator();

			//postfix increment (TICPP2)
			class CaptureState { //TODO: don't think this is necessary - probably just because example was a wrapper?
				TbtaTable table;
			public:
				CaptureState(const TbtaTable& t);// : table(t) {}
				TbtaString operator*(); //{ return word; }
			};
			CaptureState operator++(int); /*{
										  CaptureState d(word);
										  ++*this;
										  return d;
										  }*/

			//prefix increment
			iterator& operator++();

			// Produce the actual value:
			TbtaString operator*() const; //{ return word; }
			const TbtaString* operator->() const; //{ return &word; }
			// Compare iterators:
			bool operator==(const iterator&); /*{
											  return word.size() == 0 && first == last;
											  }*/
			bool operator!=(const iterator& rv); /*{
												 return !(*this == rv);
												 }*/

		private:
			const TbtaGrammarSectionFileProcessor *_outerRef;
			int _position;
		};

	private:
		struct TbtaGrammarSectionFileProcessorImpl;
		TbtaGrammarSectionFileProcessorImpl *_pimpl;
	};
}