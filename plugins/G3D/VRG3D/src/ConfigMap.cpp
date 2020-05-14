//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.



#include  "../include/ConfigMap.h"

#include  <iostream>

using namespace std;
using namespace G3D;

namespace MinVR {


bool  ConfigMap::readFile(const std::string &filename)
{
  std::string instr;

  if (FileSystem::exists(filename))
  { std::string output = "ConfigMap parsing file \"" + filename + "\".";

    if (_log)
      _log->println(output);
    else
      std::cout << output << std::endl;
    instr = readWholeFile(filename);
  }
  else
  {  alwaysAssertM(false, "Cannot locate config file " + filename);
  }


  // convert all endline characters to \n's
  for (int i=0;i<instr.size();i++)
  { if (instr[i] == '\r')
      instr[i] = '\n';
  }

  // remove any cases of two \n's next to each other
  for (int i=0;i<instr.size()-1;i++)
  {  if ((instr[i] == '\n') && (instr[i+1] == '\n'))
     { instr = instr.substr(0,i) + instr.substr(i+1);
     }
  }

  // add a \n so that every file ends in at least one \n
  instr = instr + std::string("\n");

  // start pulling out name, value pairs
  // the config file must end with a newline
  //
  while (instr.size())
  { int endline = instr.find("\n");
    std::string nameval = instr.substr(0,endline);

    // a backslash not followed by a second backslash means continue on
    // the next line, wipe out everything from the backslash to the newline
    /**  Note, this will miss the following scenario, which will probably
     never occur in practice:
       myname myvalue with a single slash here \\ and \
       continued on the next line like this
     So, this doesn't handle a \\ on the same line as a \
    **/
    int slash = nameval.find("\\");

	bool nextCharIsSlash = false;
	if (slash < nameval.size() - 1) {
		nextCharIsSlash = (nameval[slash+1] == '\\');
	} else {
		nextCharIsSlash = false;
	}

    while ((slash != nameval.npos) && !nextCharIsSlash
                                   && (endline != nameval.npos))
    {
      std::string fromPrevLine = nameval.substr(0,slash);
      instr = instr.substr(endline+1);
      endline = instr.find("\n");
      nameval = fromPrevLine + instr.substr(0,endline);
      slash = nameval.find("\\");
      if (slash < nameval.size() - 1) {
	    nextCharIsSlash = (nameval[slash+1] == '\\');
	  } else {
		nextCharIsSlash = false;
	  }
    }

    // a line starting with # is a comment, ignore it
    //
	if (nameval.size() > 0 && nameval[0] != '#')
    { // if we have two backslashes \\ treat this as an escape sequence for
      // a single backslash, so replace the two with one
      int doubleslash = nameval.find("\\\\");

      if (doubleslash >= 0)
      {  nameval = nameval.substr(0,doubleslash)
                 + nameval.substr(doubleslash + 1);
      }

      // replace all $(NAME) sequences with the value of the environment
      // variable NAME.  under cygwin, cygwin-style paths are replaced with
      // windows style paths.
      nameval = replaceEnvVars(nameval);

      int firstspace = nameval.find(" ");
      int firsttab = nameval.find('\t');
      if (((firsttab >=0) && (firsttab < firstspace)) ||
          ((firsttab >=0) && (firstspace < 0)))
      {
         firstspace = firsttab;
      }

      std::string name = nameval.substr(0,firstspace);
      std::string val;

      if (firstspace >= 0)
      {  val = trimWhitespace(nameval.substr(firstspace + 1));
      }

      if (name != "")
      {  if ((name.size() > 2) && (name[name.size()-2] == '+')
                               && (name[name.size()-1] == '='))
         {
            name = name.substr(0,name.size()-2);

            if (ConfigMap::containsKey(name))
            {  ConfigMap::set(name, ConfigMap::getValue(name) + " " + val);
            }
            else
            {   ConfigMap::set(name, val);
            }
         }
         else
         {  ConfigMap::set(name, val);
         }
      }
    } // end not a comment

    instr = instr.substr(endline+1);
  }

  return true;
}


void  ConfigMap::debugPrint()
{
  Array<std::string> keys = _map.getKeys();

  for (int i=0;i<keys.size();i++)
  { std::cout << "\"" << keys[i] << "\"" << " --> "
          << "\"" << _map[keys[i]] << "\"" << std::endl;
  }
}


bool ConfigMap::containsKey(const std::string &keyString)
{
  return _map.containsKey(keyString);
}

std::string  ConfigMap::getValue(const std::string &keyString)
{
  debugAssert(_map.containsKey(keyString));

  return _map[keyString];
}

void  ConfigMap::set(const std::string &key, const std::string &value)
{
  _map.set(key, value);
}


ConfigMap::ConfigMap( int  argc, char **argv, Log *log,
                      bool exitOnUnrecognizedArgument )
{
  _log = log;

  // put args into std::strings so they are easier to manipulate
  Array<std::string> args;

  for (int i = 0; i < argc; i++)
  {  args.append(std::string(argv[i]));
  }

  // parse arguments
  //
  for (int i=1;i<args.size();i++)
  { if ((args[i] == "-f") || (args[i] == "--configfile"))
    { i++;

      if (i >= args.size())
      {  printArgumentHelpAndExit(args[0]);
      }
      else
      {  readFile(decygifyPath(args[i]));
      }
    }
    else if ((args[i] == "-c") || (args[i] == "--configval"))
    { i++;
 
      if (i >= args.size())
      {  printArgumentHelpAndExit(args[0]);
      }
      else
      { std::string kv(args[i]);
        int e = kv.find("=");
        alwaysAssertM(e>0,"Invalid key=value command line argument: \""+kv+"\"");

        std::string key = kv.substr(0,e);
        std::string val = kv.substr(e+1);
        set(key, val);
      }
    }
    else if ((args[i] == "-h") || (args[i] == "--") ||
         (args[i] == "--help") || (args[i] == "-help"))
    {
      printArgumentHelpAndExit(args[0]);
    }
    else if (exitOnUnrecognizedArgument)
    { printArgumentHelpAndExit(args[0]);
    }
  }
}

void  ConfigMap::printArgumentHelpAndExit(const std::string &programName)
{
  std::string prog = filenameBaseExt(programName);
  cout << "Usage: " << prog
       << " [-f filename [-f filename...]] [-c key=value [-c key=value...]]" << endl;
  cout << endl;
  cout << "options:            (defaults in [...]'s )" << endl;
  cout << "   -f filename      Specifies a key/value config file to process. [null]" << endl;
  cout << "   -c key=value     Specifies a config key=value pair to process. 'value'" << endl;
  cout << "                    overrides any ConfigVal for 'key' specified that was" << endl;
  cout << "                    specified in a config file. [null]" << endl;
  cout << "   -h, --           Show this help message." << endl;

  exit(0);
}


} // end namespace
