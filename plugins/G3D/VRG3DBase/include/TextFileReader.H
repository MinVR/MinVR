
#ifndef TEXTFILEREADER_H
#define TEXTFILEREADER_H

#include "../../VRG3D/include/CommonInc.H"
#include "StringUtils.H"

namespace MinVR {

#define TFR_DEFAULT_BUFFERSIZE 5120

typedef G3D::ReferenceCountedPointer<class TextFileReader> TextFileReaderRef;
/**
    This class was created to make reading large text files faster.  The issue
    we encountered was that calling find() substr() etc.. on std::strings is really
    really slow if the string is really long - like when you read a whole file in as
    a string.  So in this class we maintain a small buffer that is the next bit of 
    the file we're parsing and only do string manipulations on this small buffer.
    The buffer is automatically grabs more of the file from disk when needed.
*/
class TextFileReader : public G3D::ReferenceCountedObject
{
public:
  PLUGIN_API TextFileReader(const std::string &filename, G3D::int64 maxBufferSize=TFR_DEFAULT_BUFFERSIZE) {
    bufferSize = maxBufferSize;
    filePos = 0;
    fileSize = G3D::FileSystem::size(filename);
    f = fopen(filename.c_str(), "rb");
    debugAssert(f);
  }

  PLUGIN_API virtual ~TextFileReader() {
    fclose(f);
  }

  /// True when every bit of the file has been read into the internal text buffer.  Doesn't
  /// necessarily mean that you have parsed everything that is in the textBuffer variable.
  PLUGIN_API bool eof() { return (filePos == fileSize); }

  /// This is really the percentage of the file that has been either parsed or
  /// read into memory so far.
  PLUGIN_API double getFilePosAsPercentage() {
    return (double)filePos / (double)fileSize * 100.0;
  }


  /**  -----  FOR HELPING TO PARSE REGULAR TEXT FILES ---- **/

  PLUGIN_API bool getNextLine(std::string &line) {
    return getTextUntilToken("\n", line, true);
  }

  PLUGIN_API bool getTextUntilToken(const std::string &stopToken, std::string &readText, bool eatToken=true);

  PLUGIN_API bool peekAtTextUntilToken(const std::string &stopToken, std::string &readText);



  /**  -----  FOR HELPING TO PARSE XML FILES ---- **/

  /** Advances to the next occurance of <fieldName, throwing away anything found before
      it, then reads the xml field header.  When this returns the file position is the
      character after the ">" that closes the field header.*/
  PLUGIN_API bool getXMLHeader(std::string &fieldName,
      G3D::Table<std::string, std::string> &propertiesAndValues);

  /** XMLStartField means the next characters in the file are "<fieldName".
      XMLEndField means the next characters in the file are "</fieldName>".
      XMLFieldData means there is unrecognized text that comes next, but at
      some point it ends with "</fieldName>".
      XMLError means that there are no more "<fieldName" or "</fieldName>"
      strings left in the file. */
  PLUGIN_API enum XMLTokenType { XMLStartField, XMLEndField, XMLFieldData, XMLError, XMLEndFile };

  /** Returns the next token type in the file.  If carriage return characters
      or white space are the next characters in the file, these are ignored 
      when determining the next token type.*/
  PLUGIN_API bool peekAtNextXMLToken(XMLTokenType &type);

  /** The next XML token must be XMLStartField.  This returns that fieldName of
      that field.*/
  PLUGIN_API bool peekAtNextXMLStartFieldName(std::string &fieldName);

  /** Returns the text contained between the start and close of the XML field as in:
      <fieldName>returned text here</fieldName>.  Assumes the file position is already
      at the end of the field header (<fieldName> in this example).*/ 
  PLUGIN_API bool getXMLFieldData(const std::string &fieldName, std::string &fieldData);

  /** Advances the file position to the next character after the next occurance
      of "</fieldName>". */
  PLUGIN_API bool eatXMLFieldClose(const std::string &fieldName);

  /** Advances to the next occurance of <fieldName then reads the xml field header,
      the field data, and the </fieldName> that closes the field.  When this
      returns the filepos is advanced to the next character after the end of the 
      field.  If your XML fields are really long, this will be slower than reading
      each bit of the field in pieces because it will require the whole field to
      be read in and stored in memory at once.  If this is the case, then use the
      other methods in this class to get a header, check the next token type, if it's another
      header, then you have a subfield, so read it in.. keep on reading in and parsing
      the small subfields until you get them all then eat the close of the original big
      XML field.  Note that currently this also doesn't supported fields nested within
      other fields with the same fieldName.*/
  PLUGIN_API bool getXMLField(std::string &fieldName,
	 G3D::Table<std::string, std::string> &propertiesAndValues,
	 std::string &fieldData);

  PLUGIN_API void printCurrentBuffer() { cout << "TextBuffer=\"" << textBuffer << "\"" << endl; }

private:

  PLUGIN_API void getMoreFromFile() {
    char *buffer = (char*)malloc(bufferSize + 1);
    debugAssert(buffer);
    G3D::int64 numToRead = bufferSize;
    if (fileSize-filePos < bufferSize) {
      numToRead = fileSize-filePos;
    }
    G3D::int64 bytesRead = fread(buffer, 1, numToRead, f);
    debugAssert(bytesRead == numToRead);
    filePos += bytesRead;
    buffer[bytesRead] = '\0';
    textBuffer = textBuffer + std::string(buffer);
    free(buffer);
  }

  FILE *f;
  G3D::int64 fileSize;
  G3D::int64 filePos;
  G3D::int64 bufferSize;
  std::string textBuffer;
};

} // end namespace

#endif

