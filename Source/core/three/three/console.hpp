#ifndef THREE_CONSOLE_HPP
#define THREE_CONSOLE_HPP

#include <three/config.hpp>

#include <memory>
#include <iostream>
#include <sstream>
#include <android/log.h>
#define  LOG_TAG    "Three"
#define  LOG(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__);

namespace three {

class Console {
private:
  class LogProxy;

public:

  typedef void( *LogP )( const char* );

  static Console& instance() {
    static Console sConsole;
    return sConsole;
  }

  void info( const char* msg ) const  { info()  << msg; }
  void log( const char* msg ) const   { log()   << msg; }
  void debug( const char* msg ) const { debug() << msg; }
  void warn( const char* msg ) const  { warn()  << msg; }
  void error( const char* msg ) const { error() << msg; }

  LogProxy info()  const { return LogProxy( output, "INFO:  " ); }
  LogProxy log()   const { return LogProxy( output, "LOG:   " ); }
  LogProxy debug() const { return LogProxy( output, "DEBUG: " ); }
  LogProxy warn()  const { return LogProxy( output, "WARN:  " ); }
  LogProxy error() const { return LogProxy( output, "ERROR: " ); }

  void setLog( LogP log ) { output = log; }

private:

  LogP output;

  class LogProxy {
  public:
    template <class T>
    LogProxy& operator<<( const T& rhs ) {
      *stream << rhs;
      return *this;
    }

    ~LogProxy() {
      if ( stream && log ) {
        log( stream->str().c_str() );
      }
    }
    LogProxy( LogProxy && other )
      : log( nullptr ) {
      std::swap( log, other.log );
      std::swap( stream, other.stream );
    }

  private:

    friend class Console;

    explicit LogProxy( LogP log, const char* msg = nullptr )
      : stream( new std::stringstream() ), log( log ) {
      if ( msg ) {
        ( *this ) << msg;
      }
    }
    LogProxy& operator= ( LogProxy && other )     THREE_DECL_DELETE;
    LogProxy( const LogProxy& other )             THREE_DECL_DELETE;
    LogProxy& operator= ( const LogProxy& other ) THREE_DECL_DELETE;

    std::unique_ptr<std::stringstream> stream;
    LogP log;
  };

private:

  Console() : output( stdcout ) { }

  static void stdcout( const char* msg ) { LOG(msg) }
};

static Console& console() {
  return Console::instance();
}


} // namespace three

#endif // THREE_CONSOLE_HPP
