/*=========================================================================

  Program:   Array2InitInRowBug

=========================================================================*/

#include "Array2InitInRowBug.h"

//----------------------------------------------------------------------------
Array2InitInRowBug::Array2InitInRowBug(QObject *parent)
{

}

//-----------------------------------------------------------------------------
Array2InitInRowBug::~Array2InitInRowBug()
{

}

//-----------------------------------------------------------------------------
QString Array2InitInRowBug::decodeXml(const QString & xmlEncoded)
{
  QString encoded[8] =
    {
    "&#x26;", // ampersand &
    "&#x27;", // simple quote '
    "&#x3C;", // smaller than <
    "&#x3E;", // greater than >
    "&#x22;", // double quote "
    "&#x0D;", // carriage return \r
    "&#x0A;", // line feed, new line \n
    "&#x09;", // horizontal tab \t
    };
  QString decoded[8] =
    {
    "&",  // ampersand &
    "\'", // simple quote '
    "<",  // smaller than <
    ">",  // greater than >
    "\"", // double quote
    "\r", // carriage return \r
    "\n", // line feed, new line \n
    "\t", // horizontal tab \t
    };

  return QString();
}
