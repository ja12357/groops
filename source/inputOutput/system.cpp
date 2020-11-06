/***********************************************/
/**
* @file system.cpp
*
* @brief Operating system related functions.
*
* @author Andreas Kvas
* @date 2017-03-21
*
*/
/***********************************************/

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

// Please select the appropriate header and namespace
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
// // or change to std::filesystem with c++17
// #include <filesystem>
// namespace fs = std::filesystem;
// // or use boost
// #include <boost/filesystem.hpp>
// namespace fs = boost::filesystem;

#ifdef _WIN32
  #define popen  _popen
  #define pclose _pclose
#endif

#include <ctime>
#include "base/importStd.h"
#include "base/time.h"
#include "system.h"

/***********************************************/

Bool System::exec(const std::string &command, std::vector<std::string> &output)
{
  try
  {
    std::FILE *pipe = popen(command.c_str(), "r");
    if(!pipe)
      throw(Exception("Cannot open pipe"));

    char buffer[1024];
    while(std::fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
      std::string line = buffer;
      const auto pos = line.find_last_of('\n');
      if(pos != std::string::npos)
        line = line.substr(0, pos);
      output.push_back(line);
    }

    return (pclose(pipe) == 0);
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Bool System::exec(const std::string &command)
{
  std::vector<std::string> output;
  return exec(command, output);
}

/***********************************************/
/***********************************************/

Bool System::createDirectories(const FileName &fileName)
{
  if(isDirectory(fileName))
    return TRUE;
  return fs::create_directories(fileName.str());
}

/***********************************************/

Bool System::remove(const FileName &fileName)
{
  return (fs::remove_all(fileName.str()) != 0);
}

/***********************************************/

Bool System::exists(const FileName &fileName)
{
  return fs::exists(fileName.str());
}

/***********************************************/

Bool System::isDirectory(const FileName &fileName)
{
  return fs::is_directory(fileName.str());
}

/***********************************************/

FileName System::currentWorkingDirectory()
{
  return FileName(fs::current_path().string());
}

/***********************************************/
/***********************************************/

Time System::now()
{
  std::time_t tt = std::time(nullptr);
  std::tm     t  = *std::localtime(&tt);
  return date2time(t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

/***********************************************/
