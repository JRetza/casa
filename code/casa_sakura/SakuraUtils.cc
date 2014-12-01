#include <iostream>

#include <libsakura/sakura.h>
//#include <libsakura/config.h>

#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>

#include <casa_sakura/SakuraUtils.h>
#include <casa_sakura/SakuraAlignedArray.h>

namespace casa {
Bool SakuraUtils::is_initialized_ = False;
  
Bool SakuraUtils::InitializeSakura(const std::string &level) {
  // return if already initialized
  if (is_initialized_) {
    return True;
  }
  
  LogIO logger(LogOrigin("SakuraUtils", "InitializeSakura", WHERE));
  logger << LogIO::DEBUGGING << "Initializing Sakura...";
  is_initialized_ = False;
  LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(Initialize)(NULL, NULL);
  if (status == LIBSAKURA_SYMBOL(Status_kOK)) {
    logger << LogIO::DEBUGGING << "SUCCESS!" << LogIO::POST;
    is_initialized_ = True;
  }
  else {
    logger << LogIO::DEBUGGING << "FAILED!" << LogIO::POST;
  }
  return is_initialized_;
}

void SakuraUtils::CleanUpSakura() {
  if (is_initialized_) {
    LogIO logger(LogOrigin("SakuraUtils", "CleanUpSakura", WHERE));
    logger << LogIO::DEBUGGING << "Cleaning up Sakura..." << LogIO::POST;
    LIBSAKURA_SYMBOL(CleanUp)();
    is_initialized_ = False;
  }
}

Bool SakuraUtils::IsSakuraInitialized() {
  LogIO logger(LogOrigin("SakuraUtils", "IsSakuraInitialized", WHERE));
  logger << LogIO::DEBUGGING << "sakura is "
	 << ((is_initialized_) ? "" : "not ") << "initialized." << LogIO::POST;
  return is_initialized_;
}

// SakuraDriver
// initialize instance counter for SakuraDriver
int SakuraDriver::num_instances_ = 0;

// Constructor increments num_instances_
// InitlaizeSakura is called if num_instances_ is 0 (i.e. first instantiation)
// or Sakura is not initialized yet.
SakuraDriver::SakuraDriver() {
  //LogIO os(LogOrigin("SakuraDriver", "SakuraDriver", WHERE));
  if (num_instances_ == 0 || !(SakuraUtils::IsSakuraInitialized())) {
    std::cout << "Initialize Sakura: num_instances_ = " << num_instances_ << endl;
    SakuraUtils::InitializeSakura();
  }
  num_instances_++;
  std::cout << "num_instances_ = " << num_instances_ << std::endl;
}

// Destructor decrements num_instances_
// FinalizeSakura is called when num_instances_ is 0 (i.e. the last
// instance is to be destructed). 
SakuraDriver::~SakuraDriver() {
  //LogIO os(LogOrigin("SakuraDriver", "~SakuraDriver", WHERE));
  std::cout << "num_instances_ = " << num_instances_ << std::endl;
  num_instances_--;
  if (num_instances_ == 0 && SakuraUtils::IsSakuraInitialized()) {
    std::cout << "Finalize Sakura: num_instances_ = " << num_instances_ << std::endl;
    SakuraUtils::CleanUpSakura();
  }
}
  
}  // End of casa namespace.
