// TbtaFileProcessingTests.cpp : Defines the entry point for the console application.
//

//#include <locale>
#include <fcntl.h>
#include <io.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "stdafx.h"
#include "..\TbtaFileProcessing\TbtaFileProcessor.h"

using namespace TbtaFileProcessing;

std::vector<TbtaString> failures;
int successCount = 0;

#define AssertAreEqual(expected, actual) \
	if (expected == actual) \
	{ \
		++successCount; \
	} else \
	{ \
		failures.push_back(TbtaString(_T("Expected (")) + ToString(expected) + _T(") but was (") + ToString(actual) + _T(") on line: ") + ToString(__LINE__)); \
	}

#define AssertAreEqualM(expected, actual, message) \
	if (expected == actual) \
	{ \
		++successCount; \
	} else \
	{ \
		failures.push_back(ToString(message) + _T(" || Expected (") + ToString(expected) + _T(") but was (") + ToString(actual) + _T(") on line: ") + ToString(__LINE__)); \
	}

#define AssertAreEqualNs(expected, actual, message) \
	if (expected == actual) \
	{ \
		++successCount; \
	} else \
	{ \
		failures.push_back(ToString(message) + _T(" || on line: ") + ToString(__LINE__)); \
	}

#define DetectInfiniteLoop(loopCount) \
	if (loopCount > 10000) \
	{ \
		failures.push_back(_T("Apparent infinite loop detected line: ") + ToString(__LINE__)); \
		break; \
	}

#define AssertFail(message) \
	failures.push_back(_T("FAIL: ") + ToString(message) + _T(" | On line:") + ToString(__LINE__));

TbtaString ToString(int value)
{
	wchar_t buffer[2000];
	swprintf_s(buffer, 2000, _T("%d"), value);

	return TbtaString(buffer);
}

TbtaString ToString(const TbtaString &value)
{
	return value;
}

void PrintTable(const TbtaTable &table, std::wostream& output)
{
	output<<std::endl<<"---------------------------------------"<<std::endl;

	//for (unsigned int i=0; i < table.size(); ++i)
	//{
	//	for (unsigned int j=0; j < table[i].size(); ++j)
	//	{
	//		output << table[i][j];// << _T("\t");
	//	}
	//	//output << std::endl;
	//}
	
	for (TbtaTable::const_iterator i=table.begin(); i != table.end(); ++i)
	{
		for (TbtaTableRow::const_iterator j=i->begin(); j != i->end(); ++j)
		{
			output << *(j->CellText) << _T("\t");

			//if (*j == _T("你"))
			//{
			//	std::wcout<<_T("Ni3") <<std::endl;
			//}
		}
		output << std::endl;
	}
}

void AssertTableContentsEqual(const TbtaTable& expected, const TbtaTable& actual, int callLine)
{
	unsigned int row = 0, col = 0;

	for (TbtaTable::const_iterator expectedItr = expected.begin(); expectedItr != expected.end(); ++expectedItr)
	{
		AssertAreEqualM(expectedItr->size(), actual[row].size(), TbtaString(_T("Wrong number of columns. Called from line ")) + ToString(callLine));

		for(TbtaTableRow::const_iterator expectedColItr = (*expectedItr).begin(); expectedColItr != (*expectedItr).end(); ++ expectedColItr)
		{
			if (col < actual[row].size() && row < actual.size()) //avoid index out of bounds on actual table
			{
				//check cell text
				AssertAreEqualM(expectedColItr->CellText, actual[row][col].CellText, TbtaString(_T("cell[")) + ToString(row) + _T(",") + ToString(col) + _T("] called from line ") + ToString(callLine));
				++col;
			}
		}
		++row;
		col = 0;
	}

	AssertAreEqualM(expected.size(), actual.size(), TbtaString(_T("Wrong number of rows. Called from line ")) + ToString(callLine));
}

void AssertStringVectorsEqual(const std::vector<TbtaString>& expected, const std::vector<TbtaString>& actual, int callLine)
{
	unsigned int index = 0;

	AssertAreEqualM(expected.size(), actual.size(), TbtaString(_T("Wrong number of elements. Called from line ")) + ToString(callLine));

	for (auto& expectedItem : expected)
	{
		if (index < actual.size()) //avoid index out of bounds on actual table
		{
			AssertAreEqualM(expectedItem, actual[index], TbtaString(_T("Element at index ") + ToString(index) + _T(" did not match. Called from line ")) + ToString(callLine));
		}
		++index;
	}
}

//void TestPrefixIncrementLoopsCorrectNumberOfTimesWithoutDeReference()
//{
//	TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/Tables of English Pronouns.docx");
//
//	const int numTablesInFile = 4;
//	int loopCount = 0;
//
//	for (TbtaFileProcessor::iterator itr = p.begin(); itr != p.end(); ++itr)
//	{
//		DetectInfiniteLoop(++loopCount);
//	}
//
//	AssertAreEqual(numTablesInFile, loopCount);
//}
//
//void TestPostfixIncrementLoopsCorrectNumberOfTimesWithoutDeReference()
//{
//	TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/Tables of English Pronouns.docx");
//
//	const int numTablesInFile = 4;
//	int loopCount = 0;
//
//	for (TbtaFileProcessor::iterator itr = p.begin(); itr != p.end(); itr++)
//	{
//		++loopCount;
//
//		if (loopCount > 100)
//		{
//			failures.push_back("Apparent infinite loop detected line: " + __LINE__);
//		}
//	}
//
//	AssertAreEqual(numTablesInFile, loopCount);
//}

//void TestBasic4Table()
//{
//	//--------------------- Arrange ------------------------
//	TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/Tables of English Pronouns.docx");
//	
//	TbtaTable table1;
//	TbtaString t1row0[] = {_T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
//	TbtaString t1row1[] = {_T("First Person"), _T("I"), _T("we"), _T("me"), _T("us") };
//	TbtaString t1row2[] = {_T("Second Person"), _T("you"), _T("you"), _T("you"), _T("you") };
//	TbtaString t1row3[] = {_T("Third Person Masculine"), _T("he"), _T("they"), _T("him"), _T("them") };
//	TbtaString t1row4[] = {_T("Third Person Feminine"), _T("she"), _T("they"), _T("her"), _T("them") };
//	TbtaString t1row5[] = {_T("Third Person Neuter"), _T("it"), _T("they"), _T("it"), _T("them") };
//
//	table1.push_back(TbtaTableRow(t1row0, t1row0+5));
//	table1.push_back(TbtaTableRow(t1row1, t1row1+5));
//	table1.push_back(TbtaTableRow(t1row2, t1row2+5));
//	table1.push_back(TbtaTableRow(t1row3, t1row3+5));
//	table1.push_back(TbtaTableRow(t1row4, t1row4+5));
//	table1.push_back(TbtaTableRow(t1row5, t1row5+5));
//	
//	TbtaTable table2;
//	TbtaString t2row0[] = {_T(""), _T("Singular"), _T("Plural") };
//	TbtaString t2row1[] = {_T("First Person"), _T("myself"), _T("ourselves") };
//	TbtaString t2row2[] = {_T("Second Person"), _T("yourself"), _T("yourselves") };
//	TbtaString t2row3[] = {_T("Third Person Masculine"), _T("himself"), _T("themselves") };
//	TbtaString t2row4[] = {_T("Third Person Feminine"), _T("herself"), _T("themselves") };
//	TbtaString t2row5[] = {_T("Third Person Neuter"), _T("itself"), _T("themselves") };
//
//	table2.push_back(TbtaTableRow(t2row0, t2row0+3));
//	table2.push_back(TbtaTableRow(t2row1, t2row1+3));
//	table2.push_back(TbtaTableRow(t2row2, t2row2+3));
//	table2.push_back(TbtaTableRow(t2row3, t2row3+3));
//	table2.push_back(TbtaTableRow(t2row4, t2row4+3));
//	table2.push_back(TbtaTableRow(t2row5, t2row5+3));
//
//	TbtaTable table3;
//	TbtaString t3row0[] = {_T(""), _T("Singular"), _T("Plural") };
//	TbtaString t3row1[] = {_T("First Person"), _T("my"), _T("our") };
//	TbtaString t3row2[] = {_T("Second Person"), _T("your"), _T("your") };
//	TbtaString t3row3[] = {_T("Third Person Masculine"), _T("his"), _T("their") };
//	TbtaString t3row4[] = {_T("Third Person Feminine"), _T("her"), _T("their") };
//	TbtaString t3row5[] = {_T("Third Person Neuter"), _T("its"), _T("their") };
//
//	table3.push_back(TbtaTableRow(t3row0, t3row0+3));
//	table3.push_back(TbtaTableRow(t3row1, t3row1+3));
//	table3.push_back(TbtaTableRow(t3row2, t3row2+3));
//	table3.push_back(TbtaTableRow(t3row3, t3row3+3));
//	table3.push_back(TbtaTableRow(t3row4, t3row4+3));
//	table3.push_back(TbtaTableRow(t3row5, t3row5+3));
//
//	TbtaTable table4;
//	TbtaString t4row0[] = {_T(""), _T("Subject"), _T("Non-Subject") };
//	TbtaString t4row1[] = {_T("Human"), _T("who"), _T("whom") };
//	TbtaString t4row2[] = {_T("Non-Human"), _T("which"), _T("which") };
//
//	table4.push_back(TbtaTableRow(t4row0, t4row0+3));
//	table4.push_back(TbtaTableRow(t4row1, t4row1+3));
//	table4.push_back(TbtaTableRow(t4row2, t4row2+3));
//
//	//--------------------- Act / Assert ------------------------
//	TbtaFileProcessor::iterator fileIterator = p.begin();
//
//	AssertTableContentsEqual(table1, *(fileIterator), __LINE__);
//	AssertTableContentsEqual(table2, *(++fileIterator), __LINE__);
//	AssertTableContentsEqual(table3, *(++fileIterator), __LINE__);
//	AssertTableContentsEqual(table4, *(++fileIterator), __LINE__);
//
//	AssertAreEqualNs(p.end(), ++fileIterator, _T("File had unexpected additional tables."));
//}

void TestGetAllTablesBasic4()
{
	//--------------------- Arrange ------------------------
	TbtaTable table1;
	TbtaString t1row0[] = {_T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
	TbtaString t1row1[] = {_T("First Person"), _T("I"), _T("we"), _T("me"), _T("us") };
	TbtaString t1row2[] = {_T("Second Person"), _T("you"), _T("you"), _T("you"), _T("you") };
	TbtaString t1row3[] = {_T("Third Person Masculine"), _T("he"), _T("they"), _T("him"), _T("them") };
	TbtaString t1row4[] = {_T("Third Person Feminine"), _T("she"), _T("they"), _T("her"), _T("them") };
	TbtaString t1row5[] = {_T("Third Person Neuter"), _T("it"), _T("they"), _T("it"), _T("them") };

	table1.push_back(TbtaTableRow(t1row0, t1row0+5));
	table1.push_back(TbtaTableRow(t1row1, t1row1+5));
	table1.push_back(TbtaTableRow(t1row2, t1row2+5));
	table1.push_back(TbtaTableRow(t1row3, t1row3+5));
	table1.push_back(TbtaTableRow(t1row4, t1row4+5));
	table1.push_back(TbtaTableRow(t1row5, t1row5+5));
	
	TbtaTable table2;
	TbtaString t2row0[] = {_T(""), _T("Singular"), _T("Plural") };
	TbtaString t2row1[] = {_T("First Person"), _T("myself"), _T("ourselves") };
	TbtaString t2row2[] = {_T("Second Person"), _T("yourself"), _T("yourselves") };
	TbtaString t2row3[] = {_T("Third Person Masculine"), _T("himself"), _T("themselves") };
	TbtaString t2row4[] = {_T("Third Person Feminine"), _T("herself"), _T("themselves") };
	TbtaString t2row5[] = {_T("Third Person Neuter"), _T("itself"), _T("themselves") };

	table2.push_back(TbtaTableRow(t2row0, t2row0+3));
	table2.push_back(TbtaTableRow(t2row1, t2row1+3));
	table2.push_back(TbtaTableRow(t2row2, t2row2+3));
	table2.push_back(TbtaTableRow(t2row3, t2row3+3));
	table2.push_back(TbtaTableRow(t2row4, t2row4+3));
	table2.push_back(TbtaTableRow(t2row5, t2row5+3));

	TbtaTable table3;
	TbtaString t3row0[] = {_T(""), _T("Singular"), _T("Plural") };
	TbtaString t3row1[] = {_T("First Person"), _T("my"), _T("our") };
	TbtaString t3row2[] = {_T("Second Person"), _T("your"), _T("your") };
	TbtaString t3row3[] = {_T("Third Person Masculine"), _T("his"), _T("their") };
	TbtaString t3row4[] = {_T("Third Person Feminine"), _T("her"), _T("their") };
	TbtaString t3row5[] = {_T("Third Person Neuter"), _T("its"), _T("their") };

	table3.push_back(TbtaTableRow(t3row0, t3row0+3));
	table3.push_back(TbtaTableRow(t3row1, t3row1+3));
	table3.push_back(TbtaTableRow(t3row2, t3row2+3));
	table3.push_back(TbtaTableRow(t3row3, t3row3+3));
	table3.push_back(TbtaTableRow(t3row4, t3row4+3));
	table3.push_back(TbtaTableRow(t3row5, t3row5+3));

	TbtaTable table4;
	TbtaString t4row0[] = {_T(""), _T("Subject"), _T("Non-Subject") };
	TbtaString t4row1[] = {_T("Human"), _T("who"), _T("whom") };
	TbtaString t4row2[] = {_T("Non-Human"), _T("which"), _T("which") };

	table4.push_back(TbtaTableRow(t4row0, t4row0+3));
	table4.push_back(TbtaTableRow(t4row1, t4row1+3));
	table4.push_back(TbtaTableRow(t4row2, t4row2+3));

	//--------------------- Act ------------------------
	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTables("../TbtaFileProcessingTests/TestData/Tables of English Pronouns.docx");

	//--------------------- Assert ------------------------
	int actualSize = results.size();
	AssertAreEqual(4, actualSize);

	if (actualSize > 0)
		AssertTableContentsEqual(table1, results[0], __LINE__);
	if (actualSize > 1)
		AssertTableContentsEqual(table2, results[1], __LINE__);
	if (actualSize > 2)
		AssertTableContentsEqual(table3, results[2], __LINE__);
	if (actualSize > 3)
		AssertTableContentsEqual(table4, results[3], __LINE__);
}

void TestGetAllTablesRuth()
{
	//--------------------- Arrange ------------------------
	

	//--------------------- Act ------------------------
	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTables("../TbtaFileProcessingTests/TestData/2 Ruth Sentences - Gichuka.docx");

	//--------------------- Assert ------------------------
	int actualSize = results.size();
	AssertAreEqual(4, actualSize);

	AssertAreEqual(L"Ruth 4:1:1", results[3][2][0].CellText);
	AssertAreEqual(L"Title: Boaz marries Ruth.", results[3][2][1].CellText);
	AssertAreEqual(L"Title: Mboaci akĩbikia Ruthu.", results[3][2][2].CellText);

	PrintTable(results[3], std::wcout);
}

//void TestGetAllTablesWithSegments()
//{
//	//--------------------- Arrange ------------------------
//	TbtaTable table1;
//	TbtaString t1row0[] = { _T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
//	TbtaString t1row1[] = { _T("First Person"), _T("I"), _T("we"), _T("me"), _T("us") };
//	TbtaTableCell t1row2[] = { _T("Second Person"), _T("you"), _T("y'all"), _T("you"), _T("you") };
//	t1row2[2].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	t1row2[2].SegmentIndices.push_back(std::pair<int, int>(2, 3));
//	t1row2[4].SegmentIndices.push_back(std::pair<int, int>(0, 3));
//	TbtaString t1row3[] = { _T("Third Person Masculine"), _T("he"), _T("they"), _T("him"), _T("them") };
//	TbtaString t1row4[] = { _T("Third Person Feminine"), _T("she"), _T("they"), _T("her"), _T("them") };
//	TbtaString t1row5[] = { _T("Third Person Neuter"), _T("it"), _T("they"), _T("it"), _T("them") };
//
//	table1.push_back(TbtaTableRow(t1row0, t1row0 + 5));
//	table1.push_back(TbtaTableRow(t1row1, t1row1 + 5));
//	table1.push_back(TbtaTableRow(t1row2, t1row2 + 5));
//	table1.push_back(TbtaTableRow(t1row3, t1row3 + 5));
//	table1.push_back(TbtaTableRow(t1row4, t1row4 + 5));
//	table1.push_back(TbtaTableRow(t1row5, t1row5 + 5));
//
//	TbtaTable table2;
//	TbtaString t2row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaTableCell t2row1[] = { _T("First Person"), _T("myself"), _T("ourselves") };
//	t2row1[1].SegmentIndices.push_back(std::pair<int, int>(3, 3));
//	t2row1[2].SegmentIndices.push_back(std::pair<int, int>(4, 5));
//	TbtaString t2row2[] = { _T("Second Person"), _T("yourself"), _T("yourselves") };
//	TbtaTableCell t2row3[] = { _T("Third Person Masculine"), _T("himself"), _T("themselves") };
//	t2row3[2].SegmentIndices.push_back(std::pair<int, int>(0, 4));
//	TbtaTableCell t2row4[] = { _T("Third Person Feminine"), _T("herself"), _T("themselves") };
//	t2row4[1].SegmentIndices.push_back(std::pair<int, int>(3, 4));
//	TbtaTableCell t2row5[] = { _T("Third Person Neuter"), _T("itself"), _T("themselves") };
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(2, 1));
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(5, 1));
//	t2row5[2].SegmentIndices.push_back(std::pair<int, int>(1, 2));
//	t2row5[2].SegmentIndices.push_back(std::pair<int, int>(5, 5));
//
//	table2.push_back(TbtaTableRow(t2row0, t2row0 + 3));
//	table2.push_back(TbtaTableRow(t2row1, t2row1 + 3));
//	table2.push_back(TbtaTableRow(t2row2, t2row2 + 3));
//	table2.push_back(TbtaTableRow(t2row3, t2row3 + 3));
//	table2.push_back(TbtaTableRow(t2row4, t2row4 + 3));
//	table2.push_back(TbtaTableRow(t2row5, t2row5 + 3));
//
//	TbtaTable table3;
//	TbtaString t3row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaString t3row1[] = { _T("First Person"), _T("my"), _T("our") };
//	TbtaString t3row2[] = { _T("Second Person"), _T("your"), _T("your") };
//	TbtaString t3row3[] = { _T("Third Person Masculine"), _T("his"), _T("their") };
//	TbtaString t3row4[] = { _T("Third Person Feminine"), _T("her"), _T("their") };
//	TbtaString t3row5[] = { _T("Third Person Neuter"), _T("its"), _T("their") };
//
//	table3.push_back(TbtaTableRow(t3row0, t3row0 + 3));
//	table3.push_back(TbtaTableRow(t3row1, t3row1 + 3));
//	table3.push_back(TbtaTableRow(t3row2, t3row2 + 3));
//	table3.push_back(TbtaTableRow(t3row3, t3row3 + 3));
//	table3.push_back(TbtaTableRow(t3row4, t3row4 + 3));
//	table3.push_back(TbtaTableRow(t3row5, t3row5 + 3));
//
//	TbtaTable table4;
//	TbtaString t4row0[] = { _T(""), _T("Subject"), _T("Non-Subject") };
//	TbtaTableCell t4row1[] = { _T("Human"), _T("who"), _T("whom") };
//	t4row1[1].SegmentIndices.push_back(std::pair<int, int>(2, 1));
//	t4row1[2].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	TbtaTableCell t4row2[] = { _T("Non-Human"), _T("how now brown cow"), _T("which") };
//	t4row2[1].SegmentIndices.push_back(std::pair<int, int>(0, 6));
//	t4row2[1].SegmentIndices.push_back(std::pair<int, int>(10, 2));
//
//	table4.push_back(TbtaTableRow(t4row0, t4row0 + 3));
//	table4.push_back(TbtaTableRow(t4row1, t4row1 + 3));
//	table4.push_back(TbtaTableRow(t4row2, t4row2 + 3));
//
//	//--------------------- Act ------------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTables("../TbtaFileProcessingTests/TestData/Grammar Sections And Fake Segments.docx");
//
//	//--------------------- Assert ------------------------
//	int actualSize = results.size();
//	AssertAreEqual(4, actualSize);
//
//	if (actualSize > 0)
//		AssertTableContentsEqual(table1, results[0], __LINE__);
//	if (actualSize > 1)
//		AssertTableContentsEqual(table2, results[1], __LINE__);
//	if (actualSize > 2)
//		AssertTableContentsEqual(table3, results[2], __LINE__);
//	if (actualSize > 3)
//		AssertTableContentsEqual(table4, results[3], __LINE__);
//}


//void TestGetAllTablesWithFilteredText()
//{
//	//filter all text
//
//	//verify segments aligned properly
//
//	//verify segment does not start or end with whitespace
//
//	//convert multiple whitespace within segment and text into single space.
//
//	//verify character for character replacements.
//
//	//--------------------- Arrange ------------------------
//	//key is search character, value is replacement character.
//	std::unordered_map<wchar_t, wchar_t> replacements;
//	replacements.insert(std::make_pair(wchar_t(8220), '"'));
//	replacements.insert(std::make_pair(wchar_t(8221), '"'));
//	replacements.insert(std::make_pair('+', 'l'));
//	//{ 
//	//	{ wchar_t(8220), '"' }, //left "smart quote"
//	//	{ wchar_t(8221), '"' }, //right "smart quote"
//	//	{ '+', 'l' },
//	//};
//
//	TbtaTable table1;
//	TbtaTableCell t1row0[] = { _T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
//	TbtaTableCell t1row1[] = { _T("First Person"), _T("I"), _T("we"), _T("me"), _T("us") };
//	TbtaTableCell t1row2[] = { _T("Second Person"), _T("\"you\""), _T("y'all"), _T("you"), _T("y\"o\"u") };
//	t1row2[2].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	t1row2[2].SegmentIndices.push_back(std::pair<int, int>(2, 3));
//	t1row2[4].SegmentIndices.push_back(std::pair<int, int>(0, 5));
//	TbtaTableCell t1row3[] = { _T("Third Person Masculine"), _T("he"), _T("they"), _T("him"), _T("th\"e\"m") };
//	TbtaTableCell t1row4[] = { _T("Third Person Feminine"), _T("she"), _T("they"), _T("her"), _T("th em") };
//	t1row4[4].SegmentIndices.push_back(std::pair<int, int>(1, 3));
//	TbtaTableCell t1row5[] = { _T("Third Person Neuter"), _T("it"), _T("they"), _T("it"), _T("th em") };
//	t1row5[4].SegmentIndices.push_back(std::pair<int, int>(3, 2));
//
//	table1.push_back(TbtaTableRow(t1row0, t1row0 + 5));
//	table1.push_back(TbtaTableRow(t1row1, t1row1 + 5));
//	table1.push_back(TbtaTableRow(t1row2, t1row2 + 5));
//	table1.push_back(TbtaTableRow(t1row3, t1row3 + 5));
//	table1.push_back(TbtaTableRow(t1row4, t1row4 + 5));
//	table1.push_back(TbtaTableRow(t1row5, t1row5 + 5));
//
//	TbtaTable table2;
//	TbtaString t2row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaTableCell t2row1[] = { _T("First Person"), _T("mys\"elf\""), _T("ourselves") };
//	t2row1[1].SegmentIndices.push_back(std::pair<int, int>(4, 4));
//	t2row1[2].SegmentIndices.push_back(std::pair<int, int>(4, 5));
//	TbtaString t2row2[] = { _T("Second Person"), _T("yourself"), _T("yourselves") };
//	TbtaTableCell t2row3[] = { _T("Third Person Masculine"), _T("him self"), _T("themselves") };
//	t2row3[1].SegmentIndices.push_back(std::pair<int, int>(0, 3));
//	t2row3[2].SegmentIndices.push_back(std::pair<int, int>(0, 4));
//	TbtaTableCell t2row4[] = { _T("Third Person Feminine"), _T("herself"), _T("thems elves") };
//	t2row4[1].SegmentIndices.push_back(std::pair<int, int>(3, 4));
//	t2row4[2].SegmentIndices.push_back(std::pair<int, int>(6, 4));
//	TbtaTableCell t2row5[] = { _T("Third Person Neuter"), _T("itself"), _T("themselves") };
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(2, 1));
//	t2row5[1].SegmentIndices.push_back(std::pair<int, int>(5, 1));
//	t2row5[2].SegmentIndices.push_back(std::pair<int, int>(1, 2));
//	t2row5[2].SegmentIndices.push_back(std::pair<int, int>(5, 5));
//
//	table2.push_back(TbtaTableRow(t2row0, t2row0 + 3));
//	table2.push_back(TbtaTableRow(t2row1, t2row1 + 3));
//	table2.push_back(TbtaTableRow(t2row2, t2row2 + 3));
//	table2.push_back(TbtaTableRow(t2row3, t2row3 + 3));
//	table2.push_back(TbtaTableRow(t2row4, t2row4 + 3));
//	table2.push_back(TbtaTableRow(t2row5, t2row5 + 3));
//
//	TbtaTable table3;
//	TbtaString t3row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaString t3row1[] = { _T("First Person"), _T("my"), _T("our") };
//	TbtaString t3row2[] = { _T("Second Person"), _T("yo ur"), _T("your") };
//	TbtaTableCell t3row3[] = { _T("Third Person Masculine"), _T("his"), _T("their") };
//	t3row3[1].SegmentIndices.push_back(std::pair<int, int>(0, 2));
//	TbtaString t3row4[] = { _T("Third Person Feminine"), _T("h er"), _T("t he ir") };
//	TbtaString t3row5[] = { _T("Third Person Neuter"), _T("its"), _T("their") };
//
//	table3.push_back(TbtaTableRow(t3row0, t3row0 + 3));
//	table3.push_back(TbtaTableRow(t3row1, t3row1 + 3));
//	table3.push_back(TbtaTableRow(t3row2, t3row2 + 3));
//	table3.push_back(TbtaTableRow(t3row3, t3row3 + 3));
//	table3.push_back(TbtaTableRow(t3row4, t3row4 + 3));
//	table3.push_back(TbtaTableRow(t3row5, t3row5 + 3));
//
//	TbtaTable table4;
//	TbtaString t4row0[] = { _T(""), _T("Subject"), _T("Non-Subject") };
//	TbtaTableCell t4row1[] = { _T("Human"), _T("who"), _T("whom") };
//	t4row1[1].SegmentIndices.push_back(std::pair<int, int>(2, 1));
//	t4row1[2].SegmentIndices.push_back(std::pair<int, int>(0, 1));
//	TbtaTableCell t4row2[] = { _T("Non-Human"), _T("how now brown cow"), _T("wh ich") };
//	t4row2[1].SegmentIndices.push_back(std::pair<int, int>(0, 6));
//	t4row2[1].SegmentIndices.push_back(std::pair<int, int>(10, 2));
//
//	table4.push_back(TbtaTableRow(t4row0, t4row0 + 3));
//	table4.push_back(TbtaTableRow(t4row1, t4row1 + 3));
//	table4.push_back(TbtaTableRow(t4row2, t4row2 + 3));
//
//	//--------------------- Act ------------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTables("../TbtaFileProcessingTests/TestData/Grammar Sections And Segments For text Replacement.docx", replacements);
//
//	//--------------------- Assert ------------------------
//	int actualSize = results.size();
//	AssertAreEqual(4, actualSize);
//
//	if (actualSize > 0)
//		AssertTableContentsEqual(table1, results[0], __LINE__);
//	if (actualSize > 1)
//		AssertTableContentsEqual(table2, results[1], __LINE__);
//	if (actualSize > 2)
//		AssertTableContentsEqual(table3, results[2], __LINE__);
//	if (actualSize > 3)
//		AssertTableContentsEqual(table4, results[3], __LINE__);
//}

//void TestGetSegmentText()
//{
//	//--------------------- Arrange ------------------------
//	//key is search character, value is replacement character.
//	//std::unordered_map<wchar_t, wchar_t> replacements =
//	//{
//	//	{ wchar_t(8220), '"' }, //left "smart quote"
//	//	{ wchar_t(8221), '"' }, //right "smart quote"
//	//	{ '+', 'l' },
//	//};
//	std::unordered_map<wchar_t, wchar_t> replacements;
//	replacements.insert(std::make_pair(wchar_t(8220), '"'));
//	replacements.insert(std::make_pair(wchar_t(8221), '"'));
//	replacements.insert(std::make_pair('+', 'l'));
//
//	//--------------------- Act ------------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTablesInGrammarSection("../TbtaFileProcessingTests/TestData/Grammar Sections And Segments For text Replacement.docx", TbtaString(_T("Grammar Section 2: Test Section Beta")), replacements);
//
//	//--------------------- Assert ------------------------
//	int actualSize = results.size();
//	AssertAreEqual(2, actualSize);
//
//	if (actualSize > 0)
//	{
//		AssertAreEqualM(TbtaString(_T("elf\"")), results[0][1][1].GetSegmentText(results[0][1][1].SegmentIndices[0]), TbtaString(_T("First table, row 1, column 1, segment 0")));
//		AssertAreEqualM(TbtaString(_T("elves")), results[0][1][2].GetSegmentText(results[0][1][2].SegmentIndices[0]), TbtaString(_T("First table, row 1, column 2, segment 0")));
//
//		AssertAreEqualM(TbtaString(_T("him")), results[0][3][1].GetSegmentText(results[0][3][1].SegmentIndices[0]), TbtaString(_T("First table, row 3, column 1, segment 0")));
//		AssertAreEqualM(TbtaString(_T("them")), results[0][3][2].GetSegmentText(results[0][3][2].SegmentIndices[0]), TbtaString(_T("First table, row 3, column 2, segment 0")));
//
//		AssertAreEqualM(TbtaString(_T("self")), results[0][4][1].GetSegmentText(results[0][4][1].SegmentIndices[0]), TbtaString(_T("First table, row 4, column 1, segment 0")));
//		AssertAreEqualM(TbtaString(_T("elve")), results[0][4][2].GetSegmentText(results[0][4][2].SegmentIndices[0]), TbtaString(_T("First table, row 4, column 2, segment 0")));
//
//		AssertAreEqualM(TbtaString(_T("i")), results[0][5][1].GetSegmentText(results[0][5][1].SegmentIndices[0]), TbtaString(_T("First table, row 5, column 1, segment 0")));
//		AssertAreEqualM(TbtaString(_T("s")), results[0][5][1].GetSegmentText(results[0][5][1].SegmentIndices[1]), TbtaString(_T("First table, row 5, column 1, segment 1")));
//		AssertAreEqualM(TbtaString(_T("f")), results[0][5][1].GetSegmentText(results[0][5][1].SegmentIndices[2]), TbtaString(_T("First table, row 5, column 1, segment 2")));
//
//		AssertAreEqualM(TbtaString(_T("he")), results[0][5][2].GetSegmentText(results[0][5][2].SegmentIndices[0]), TbtaString(_T("First table, row 5, column 2, segment 0")));
//		AssertAreEqualM(TbtaString(_T("elves")), results[0][5][2].GetSegmentText(results[0][5][2].SegmentIndices[1]), TbtaString(_T("First table, row 5, column 2, segment 1")));
//	}
//
//	if (actualSize > 1)
//	{
//		AssertAreEqualM(TbtaString(_T("hi")), results[1][3][1].GetSegmentText(results[1][3][1].SegmentIndices[0]), TbtaString(_T("Second table, row 3, column 1, segment 0")));
//	}
//}

void TestUnicodeSingleTableChinesePronouns()
{
	//----------------------- Arrange -----------------------
	TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/SingleTableChinesePronouns.docx");
	
	TbtaTable table1;
	TbtaString t1row0[] = {_T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
	TbtaString t1row1[] = {_T("First Person"), _T("我"), _T("我们"), _T("我"), _T("我们") };
	TbtaString t1row2[] = {_T("Second Person"), _T("你"), _T("你们"), _T("你"), _T("你们") };
	TbtaString t1row3[] = {_T("Third Person Masculine"), _T("他"), _T("他们"), _T("他"), _T("他们") };
	TbtaString t1row4[] = {_T("Third Person Feminine"), _T("她"), _T("她们"), _T("她"), _T("她们") };
	TbtaString t1row5[] = {_T("Third Person Neuter"), _T("他"), _T("他们"), _T("他"), _T("他们") };

	table1.push_back(TbtaTableRow(t1row0, t1row0+5));
	table1.push_back(TbtaTableRow(t1row1, t1row1+5));
	table1.push_back(TbtaTableRow(t1row2, t1row2+5));
	table1.push_back(TbtaTableRow(t1row3, t1row3+5));
	table1.push_back(TbtaTableRow(t1row4, t1row4+5));
	table1.push_back(TbtaTableRow(t1row5, t1row5+5));

	//----------------------- Act -----------------------
	TbtaFileProcessor::iterator fileIterator = p.begin();

	//----------------------- Assert -----------------------
	PrintTable(*fileIterator, std::wcout);
	std::wcout<<std::endl<<"---------------------------------------"<<std::endl;

	AssertTableContentsEqual(table1, *(fileIterator), __LINE__);

	AssertAreEqualNs(p.end(), ++fileIterator, _T("File had unexpected additional tables."));
}

void TestCorruptedGzipThrowsZlibDataError()
{
	try
	{
		TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/Corrupted_ZlibDataError.docx");
		AssertFail(_T("A FileProcessingException was expected, but no exception was thrown"));
	}
	catch (const FileProcessingException& e)
	{
		AssertAreEqual(FileProcessingException::ZlibDataError, e.GetFailureType());
	}
	catch (...)
	{
		AssertFail(_T("A FileProcessingException was expected, but a different type of exception was thrown (TODO RTTI for this assert message?)"));
	}
}

void TestCorruptedGzipThrowsCyclicRedundancyCheckFailed()
{
	try
	{
		{ //this block is not superflous.  It causes the destructor to get called, which calls close on the underlying zlib stream.
			TbtaFileProcessor p("../TbtaFileProcessingTests/TestData/Corrupted_CyclicRedundancyCheckFailed.docx");
		}
		AssertFail(_T("A FileProcessingException was expected, but no exception was thrown"));
	}
	catch (const FileProcessingException& e)
	{
		AssertAreEqual(FileProcessingException::CyclicRedundancyCheckFailed, e.GetFailureType());
	}
	catch (...)
	{
		AssertFail(_T("A FileProcessingException was expected, but a different type of exception was thrown (TODO RTTI for this assert message?)"));
	}
}

//void TestGetAllGrammarSectionsShouldReturnAllSectionsWhenFileHasSections()
//{
//	//----------------------- Arrange -----------------------
//	std::vector<TbtaString> expected;
//	
//	expected.push_back(TbtaString(_T("Grammar Section 1: Test Section Alpha")));
//	expected.push_back(TbtaString(_T("Grammar Section 2: Test Section Beta")));
//	expected.push_back(TbtaString(_T("Grammar Section 3: Test Section 你好Gamma")));
//
//	//----------------------- Act -----------------------
//	std::vector<TbtaString> actual = TbtaFileProcessor::GetAllGrammarSections("../TbtaFileProcessingTests/TestData/Tables of English Pronouns With Grammar Sections.docx");
//
//	//----------------------- Assert -----------------------
//	AssertStringVectorsEqual(expected, actual, __LINE__);
//}

//void TestGetAllGrammarSectionsShouldReturnEmptyWhenFileDoesNotHaveSections()
//{
//	//----------------------- Arrange -----------------------
//	std::vector<TbtaString> expected;
//
//	//----------------------- Act -----------------------
//	std::vector<TbtaString> actual = TbtaFileProcessor::GetAllGrammarSections("../TbtaFileProcessingTests/TestData/Tables of English Pronouns.docx");
//
//	//----------------------- Assert -----------------------
//	AssertStringVectorsEqual(expected, actual, __LINE__);
//}

//void TestGetAllTablesInGrammarSection1()
//{
//	//----------------------- Arrange -----------------------
//	TbtaTable table1;
//	TbtaString t1row0[] = { _T(""), _T("Singular Subject"), _T("Plural Subject"), _T("Singular Non-Subject"), _T("Plural Non-Subject") };
//	TbtaString t1row1[] = { _T("First Person"), _T("I"), _T("we"), _T("me"), _T("us") };
//	TbtaString t1row2[] = { _T("Second Person"), _T("you"), _T("you"), _T("you"), _T("you") };
//	TbtaString t1row3[] = { _T("Third Person Masculine"), _T("he"), _T("they"), _T("him"), _T("them") };
//	TbtaString t1row4[] = { _T("Third Person Feminine"), _T("she"), _T("they"), _T("her"), _T("them") };
//	TbtaString t1row5[] = { _T("Third Person Neuter"), _T("it"), _T("they"), _T("it"), _T("them") };
//
//	table1.push_back(TbtaTableRow(t1row0, t1row0 + 5));
//	table1.push_back(TbtaTableRow(t1row1, t1row1 + 5));
//	table1.push_back(TbtaTableRow(t1row2, t1row2 + 5));
//	table1.push_back(TbtaTableRow(t1row3, t1row3 + 5));
//	table1.push_back(TbtaTableRow(t1row4, t1row4 + 5));
//	table1.push_back(TbtaTableRow(t1row5, t1row5 + 5));
//
//	//----------------------- Act -----------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTablesInGrammarSection("../TbtaFileProcessingTests/TestData/Tables of English Pronouns With Grammar Sections.docx", TbtaString(_T("Grammar Section 1: Test Section Alpha")));
//
//	//----------------------- Assert -----------------------
//	int actualSize = results.size();
//	AssertAreEqual(1, actualSize);
//
//	if (actualSize > 0)
//		AssertTableContentsEqual(table1, results[0], __LINE__);
//}

//void TestGetAllTablesInGrammarSection2()
//{
//	//----------------------- Arrange -----------------------
//	TbtaTable table2;
//	TbtaString t2row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaString t2row1[] = { _T("First Person"), _T("myself"), _T("ourselves") };
//	TbtaString t2row2[] = { _T("Second Person"), _T("yourself"), _T("yourselves") };
//	TbtaString t2row3[] = { _T("Third Person Masculine"), _T("himself"), _T("themselves") };
//	TbtaString t2row4[] = { _T("Third Person Feminine"), _T("herself"), _T("themselves") };
//	TbtaString t2row5[] = { _T("Third Person Neuter"), _T("itself"), _T("themselves") };
//
//	table2.push_back(TbtaTableRow(t2row0, t2row0 + 3));
//	table2.push_back(TbtaTableRow(t2row1, t2row1 + 3));
//	table2.push_back(TbtaTableRow(t2row2, t2row2 + 3));
//	table2.push_back(TbtaTableRow(t2row3, t2row3 + 3));
//	table2.push_back(TbtaTableRow(t2row4, t2row4 + 3));
//	table2.push_back(TbtaTableRow(t2row5, t2row5 + 3));
//
//	TbtaTable table3;
//	TbtaString t3row0[] = { _T(""), _T("Singular"), _T("Plural") };
//	TbtaString t3row1[] = { _T("First Person"), _T("my"), _T("our") };
//	TbtaString t3row2[] = { _T("Second Person"), _T("your"), _T("your") };
//	TbtaString t3row3[] = { _T("Third Person Masculine"), _T("his"), _T("their") };
//	TbtaString t3row4[] = { _T("Third Person Feminine"), _T("her"), _T("their") };
//	TbtaString t3row5[] = { _T("Third Person Neuter"), _T("its"), _T("their") };
//
//	table3.push_back(TbtaTableRow(t3row0, t3row0 + 3));
//	table3.push_back(TbtaTableRow(t3row1, t3row1 + 3));
//	table3.push_back(TbtaTableRow(t3row2, t3row2 + 3));
//	table3.push_back(TbtaTableRow(t3row3, t3row3 + 3));
//	table3.push_back(TbtaTableRow(t3row4, t3row4 + 3));
//	table3.push_back(TbtaTableRow(t3row5, t3row5 + 3));
//
//
//	//----------------------- Act -----------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTablesInGrammarSection("../TbtaFileProcessingTests/TestData/Tables of English Pronouns With Grammar Sections.docx", TbtaString(_T("Grammar Section 2: Test Section Beta")));
//
//	//----------------------- Assert -----------------------
//	int actualSize = results.size();
//	AssertAreEqual(2, actualSize);
//
//	if (actualSize > 0)
//		AssertTableContentsEqual(table2, results[0], __LINE__);
//	if (actualSize > 1)
//		AssertTableContentsEqual(table3, results[1], __LINE__);
//}

//void TestGetAllTablesInGrammarSection3()
//{
//	//----------------------- Arrange -----------------------
//	TbtaTable table4;
//	TbtaString t4row0[] = { _T(""), _T("Subject"), _T("Non-Subject") };
//	TbtaString t4row1[] = { _T("Human"), _T("who"), _T("whom") };
//	TbtaString t4row2[] = { _T("Non-Human"), _T("which"), _T("which") };
//
//	table4.push_back(TbtaTableRow(t4row0, t4row0 + 3));
//	table4.push_back(TbtaTableRow(t4row1, t4row1 + 3));
//	table4.push_back(TbtaTableRow(t4row2, t4row2 + 3));
//
//	//----------------------- Act -----------------------
//	std::vector<TbtaTable> results = TbtaFileProcessor::GetAllTablesInGrammarSection("../TbtaFileProcessingTests/TestData/Tables of English Pronouns With Grammar Sections.docx", TbtaString(_T("Grammar Section 3: Test Section 你好Gamma")));
//
//	//----------------------- Assert -----------------------
//	int actualSize = results.size();
//	AssertAreEqual(1, actualSize);
//
//	if (actualSize > 0)
//		AssertTableContentsEqual(table4, results[0], __LINE__);
//}

int _tmain(int argc, _TCHAR* argv[])
{
	//from http://stackoverflow.com/questions/2492077/output-unicode-strings-in-windows-console-app
	_setmode(_fileno(stdout), _O_U16TEXT);

	//std::locale::global(std::locale(""));

	TbtaFileProcessor::Initialize();

	//TestPrefixIncrementLoopsCorrectNumberOfTimesWithoutDeReference();
	//TestPostfixIncrementLoopsCorrectNumberOfTimesWithoutDeReference();

	//TestBasic4Table();

	TestGetAllTablesBasic4();
	TestGetAllTablesRuth();

	//TestGetAllTablesWithSegments();

	//TestGetAllTablesWithFilteredText();

	//TestGetSegmentText();

	TestUnicodeSingleTableChinesePronouns();

	TestCorruptedGzipThrowsZlibDataError();

	TestCorruptedGzipThrowsCyclicRedundancyCheckFailed();

	//TestGetAllGrammarSectionsShouldReturnAllSectionsWhenFileHasSections();

	//TestGetAllGrammarSectionsShouldReturnEmptyWhenFileDoesNotHaveSections();

	//TestGetAllTablesInGrammarSection1();

	//TestGetAllTablesInGrammarSection2();

	//TestGetAllTablesInGrammarSection3();

	TbtaFileProcessor::Terminate();

	//TODO: Test with Unicode code points that require UTF16 surrogate characters -
	//it appears from what I can find in documentation that compiling with
	//the UNICODE project settings will cause CSTring to use wchar_t with a UTF16 encoding.
	//This appears to be what xerces also uses on the windows platform.

	std::wcout << std::endl << "---------------------- Test Results ----------------------" << std::endl;

	std::wcout << successCount << " asserts passed." <<std::endl;
	std::wcout << failures.size() << " asserts failed." <<std::endl <<std::endl;

	for(std::vector<TbtaString>::iterator itr = failures.begin(); itr != failures.end(); ++itr)
	{
		std::wcout << "> " << *itr << std::endl <<std::endl;
	}

	std::wcout << "----------------------------------------------------------" << std::endl;

	return 0;

	//TODO: C++ Lint?
	//TODO: Check for memory leaks (any way to programmatically check memory used? Each iteration should return to baseline memory usage.)
	//TODO: Check malformed docx
	//TODO: Check auto-follow schema links? (security)
	//TODO: Test in release mode
}