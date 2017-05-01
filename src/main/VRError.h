#ifndef MINVR_VRERROR_H
#define MINVR_VRERROR_H

/// We can get the file, line, and function name from the pre-processor.
#ifdef WIN32
#define VRERROR(what) throw VRError(what, "", __FILE__, __LINE__, "");
#define VRERRORADV(what,advice) throw VRError(what, advice, __FILE__, __LINE__, "");
#define VRWARNING(what) VRError::VRWarning(what, "", __FILE__, __LINE__, "");
#define VRWARNINGADV(what, advice) VRError::VRWarning(what, advice, __FILE__, __LINE__, "");
#else
#define VRERROR(what) throw VRError(what, "", __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define VRERRORADV(what,advice) throw VRError(what, advice, __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define VRWARNING(what) VRError::VRWarning(what, "", __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define VRWARNINGADV(what, advice) VRError::VRWarning(what, advice, __FILE__, __LINE__, __PRETTY_FUNCTION__);
#endif

/// A class to systematize the error handling in MinVR.
class VRError: public std::exception {
public:
  /// Constructor.  Several of the calling values are meant to be picked up
  /// through preprocessor directives, so should be accompanied by a matching
  /// #define to put them in.
  ///  \param whatMsg The error message: what went wrong.  For example,
  ///  "file not found."
  ///  \param adviceMsg Some advice about what the user might do about this.
  ///   For example:  "You need three arguments for this command."
  ///  \param whereFile The source file in which the offending code is located.
  ///  \param whereLine The line in that file on which the error occurred.
  ///  \param whereFunc The function name in which the error occurred.
 VRError(const std::string& whatMsg,
         const std::string& adviceMsg = "",
         const std::string& whereFile = "",
         const int& whereLine = 0,
         const std::string& whereFunc = ""):
  _whatMsg(whatMsg), _adviceMsg(adviceMsg),
    _whereFile(whereFile), _whereFunc(whereFunc) {

    // Convert the line number to a string.
    stringstream ss; ss << whereLine; _whereLine = ss.str();
  };

  std::string _errorMessage() const {

    std::string out;

    out = "Error: " + _whatMsg
#ifdef MinVR_DEBUG
      // There is no need to burden the ultimate user with
      // implementation details, so this is inside a DEBUG condition.
      + "\n(Error reported at " + _whereFile + ":" + _whereLine
      + ", while executing " + _whereFunc + ".)"
#endif
      ;

    if (!_adviceMsg.empty()) {
      out += "\n" + _adviceMsg;
    }

    return out;
  }

  /// Destructor.
  /// Virtual to allow for subclassing.
  ///
  virtual ~VRError() throw (){}

  /// Returns a pointer to the error description.
  /// \return A pointer to a const char*. The underlying memory
  ///         is in posession of the VRError object. Callers must
  ///         not attempt to free the memory.
  virtual const char* what() const throw (){
    return _errorMessage().c_str();
  }

  static void VRWarning(const std::string& whatMsg,
                        const std::string& adviceMsg = "",
                        const std::string& whereFile = "",
                        const int& whereLine = 0,
                        const std::string& whereFunc = "") {

    std::string out;
    std::string wLine;

    // Convert the line number to a string.
    stringstream ss; ss << whereLine; wLine = ss.str();

    out = "Warning: " + whatMsg
#ifdef MinVR_DEBUG
      // There is no need to burden the ultimate user with
      // implementation details, so this is inside a DEBUG condition.
      + "\n(Warning reported at " + whereFile + ":" + wLine
      + ", while executing " + whereFunc + ".)"
#endif
      ;

    if (!adviceMsg.empty()) {
      out += "\n" + adviceMsg;
    }

    std::cerr << out << std::endl;
  }

protected:
  /// The pieces of the error message.
  ///
  std::string _whatMsg;
  std::string _whereFile;
  std::string _whereLine;
  std::string _whereFunc;
  std::string _adviceMsg;

  friend std::ostream & operator<<(std::ostream &os, VRError& e) {
    return os << e._errorMessage();
  }
};
#endif    // MINVR_VRERROR_H
