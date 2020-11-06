/***********************************************/
/**
* @file timeSeriesConditional.h
*
* @brief Create time series based on existence of files/directories
* @see TimeSeries
*
* @author Matthias Ellmer
* @author Torsten Mayer-Guerr
* @date 2018-05-18
*
*/
/***********************************************/

#ifndef __GROOPS_TIMESERIESCONDITIONAL__
#define __GROOPS_TIMESERIESCONDITIONAL__

// Latex documentation
#ifdef DOCSTRING_TimeSeries
static const char *docstringTimeSeriesConditional = R"(
\subsection{Conditional}
Only times for which the \configClass{condition}{conditionType} is met are included in the time series.
The \config{variableLoopTime} is set to every time and the \configClass{condition}{conditionType} is evaluated.
)";
#endif

/***********************************************/

#include "classes/condition/condition.h"
#include "classes/timeSeries/timeSeries.h"

/***** CLASS ***********************************/

/** @brief Only times for which the condition is met are included in the time series.
* @ingroup timeSeriesGroup
* @see TimeSeries */
class TimeSeriesConditional : public TimeSeriesBase
{
  TimeSeriesPtr        timeSeries;
  ConditionPtr         conditionPtr;
  std::string          nameTime;
  mutable VariableList varList;

public:
  TimeSeriesConditional(Config &config);

  std::vector<Time> times() const;
};

/***********************************************/

inline TimeSeriesConditional::TimeSeriesConditional(Config &config)
{
  try
  {
    readConfig(config, "timeSeries",       timeSeries,   Config::MUSTSET,   "",         "only times for which condition is met will be included");
    readConfig(config, "variableLoopTime", nameTime,     Config::OPTIONAL,  "loopTime", "variable with time of each loop");
    readConfig(config, "condition",        conditionPtr, Config::MUSTSET,   "",         "test for each time");
    if(isCreateSchema(config)) return;

    varList = config.getVarList();
    if(!nameTime.empty())
      addVariable(nameTime,  varList);
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

inline std::vector<Time> TimeSeriesConditional::times() const
{
  try
  {
    const std::vector<Time> times = timeSeries->times();
    std::vector<Time> timesToKeep;

    for(const auto &t : times)
    {
      if(!nameTime.empty())
        varList[nameTime]->setValue(t.mjd());
      if(conditionPtr->condition(varList))
        timesToKeep.push_back(t);
    }

    return timesToKeep;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

#endif /* __GROOPS__ */
