#pragma once

#include <functional>
#include <mutex>
#include <ostream>
#include <streambuf>
#include <string>

// Stream buffer teeing characters to the original stream buffer and dispatching complete lines.
class ConsoleStreamBuffer : public std::streambuf
{
  public:
    ConsoleStreamBuffer(std::ostream& targetStream, std::function<void(const std::string&)> onLine);
    virtual ~ConsoleStreamBuffer();

  protected:
    virtual int_type        overflow(int_type c = traits_type::eof()) override;
    virtual int             sync() override;
    virtual std::streamsize xsputn(const char* s, std::streamsize count) override;

  private:
    void flushCurrentLine();

    std::ostream&                           stream;
    std::streambuf*                         oldBuffer = nullptr;
    std::function<void(const std::string&)> lineCallback;
    std::string                             currentLine;
    mutable std::mutex                      bufferMutex;
};
