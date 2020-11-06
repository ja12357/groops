/***********************************************/
/**
* @file graceL1b2Mass.cpp
*
* @brief Read GRACE L1B data.
*
* @author Beate Klinger
* @date 2013-11-24
*
*/
/***********************************************/

// Latex documentation
#define DOCSTRING docstring
static const char *docstring = R"(
This program converts mass data from the GRACE SDS format into \file{instrument file (MASS)}{instrument}.
For further information see \program{GraceL1b2Accelerometer}.
)";

/***********************************************/

#include "programs/program.h"
#include "files/fileInstrument.h"
#include "fileGrace.h"

/***** CLASS ***********************************/

/** @brief Read GRACE L1B data.
* @ingroup programsConversionGroup */
class GraceL1b2Mass
{
public:
  void run(Config &config);
};

GROOPS_REGISTER_PROGRAM(GraceL1b2Mass, SINGLEPROCESS, "read GRACE L1B data", Conversion, Grace, Instrument)

/***********************************************/

void GraceL1b2Mass::run(Config &config)
{
  try
  {
    FileName              fileNameOut;
    std::vector<FileName> fileNameIn;

    readConfig(config, "outputfileMass", fileNameOut, Config::MUSTSET,  "", "");
    readConfig(config, "inputfile",      fileNameIn,  Config::MUSTSET,  "", "");
    if(isCreateSchema(config)) return;

    // =============================================

    logStatus<<"read input files"<<Log::endl;
    Arc arc;
    for(UInt idFile=0; idFile<fileNameIn.size(); idFile++)
    {
      logStatus<<"read file <"<<fileNameIn.at(idFile)<<">"<<Log::endl;
      UInt numberOfRecords;
      FileInGrace file(fileNameIn.at(idFile), numberOfRecords);

      for(UInt idEpoch=0; idEpoch<numberOfRecords; idEpoch++)
      {
        Int32    seconds, time_frac;
        Byte     time_ref, GRACE_id;
        Byte     qualflg, prod_flag;
        Double   mass_thr=0, mass_thr_err=0, mass_tnk=0, mass_tnk_err=0;
        Double   gas_mass_thr1=0, gas_mass_thr2=0, gas_mass_tnk1=0, gas_mass_tnk2=0;

        file>>seconds>>time_frac>>time_ref>>GRACE_id>>FileInGrace::flag(qualflg)>>FileInGrace::flag(prod_flag);
        if(prod_flag & (1<<0)) file>>mass_thr;
        if(prod_flag & (1<<1)) file>>mass_thr_err;
        if(prod_flag & (1<<2)) file>>mass_tnk;
        if(prod_flag & (1<<3)) file>>mass_tnk_err;
        if(prod_flag & (1<<4)) file>>gas_mass_thr1;
        if(prod_flag & (1<<5)) file>>gas_mass_thr2;
        if(prod_flag & (1<<6)) file>>gas_mass_tnk1;
        if(prod_flag & (1<<7)) file>>gas_mass_tnk2;

        const Time time = mjd2time(51544.5) + seconds2time(seconds) + seconds2time(1e-6*time_frac);
        if(arc.size() && (time <= arc.at(arc.size()-1).time))
          logWarning<<"epoch("<<time.dateTimeStr()<<") <= last epoch("<<arc.at(arc.size()-1).time.dateTimeStr()<<")"<<Log::endl;

        MassEpoch epoch;
        epoch.time     = time;
        epoch.massThr  = mass_thr;
        epoch.massTank = mass_tnk;
        arc.push_back(epoch);
      } // for(idEpoch)
    } // for(idFile)

    // =============================================

    logStatus<<"sort epochs"<<Log::endl;
    arc.sort();

    logStatus<<"eliminate duplicates"<<Log::endl;
    const UInt oldSize = arc.size();
    arc.removeDuplicateEpochs(TRUE/*keepFirst*/);
    if(arc.size() < oldSize)
      logInfo<<" "<<oldSize-arc.size()<<" duplicates removed!"<<Log::endl;

    Arc::printStatistics(arc);
    if(arc.size() == 0)
      return;

    if(!fileNameOut.empty())
    {
      logInfo<<"write data to <"<<fileNameOut<<">"<<Log::endl;
      InstrumentFile::write(fileNameOut, arc);
    }
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/
