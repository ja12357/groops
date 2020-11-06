/***********************************************/
/**
* @file tides.cpp
*
* @brief tidal forces.
* astronomic tides, earth tides, ocean tides, pole tides ...
*
* @author Torsten Mayer-Guerr
* @date 2002-12-13.
*
*/
/***********************************************/

#define DOCSTRING_Tides

#include "base/import.h"
#include "base/sphericalHarmonics.h"
#include "config/configRegister.h"
#include "classes/earthRotation/earthRotation.h"
#include "classes/tides/tidesAstronomical.h"
#include "classes/tides/tidesEarth.h"
#include "classes/tides/tidesDoodsonHarmonic.h"
#include "classes/tides/tidesPole.h"
#include "classes/tides/tidesOceanPole.h"
#include "classes/tides/tidesCentrifugal.h"
#include "classes/tides/tidesSolidMoon.h"
#include "classes/tides/tides.h"

/***********************************************/

GROOPS_REGISTER_CLASS(Tides, "tidesType",
                      TidesAstronomical,
                      TidesEarth,
                      TidesPole,
                      TidesOceanPole,
                      TidesDoodsonHarmonic,
                      TidesCentrifugal,
                      TidesSolidMoon)

GROOPS_READCONFIG_UNBOUNDED_CLASS(Tides, "tidesType")

/***********************************************/

Tides::Tides(Config &config, const std::string &name)
{
  try
  {
    std::string type;
    while(readConfigChoice(config, name, type, Config::OPTIONAL, "", "tidal forces"))
    {
      renameDeprecatedChoice(config, type, "poleTide2010",      "poleTide",      date2time(2020, 8, 24));
      renameDeprecatedChoice(config, type, "poleOceanTide2010", "oceanPoleTide", date2time(2020, 8, 24));
      renameDeprecatedChoice(config, type, "moonTide",          "solidMoonTide", date2time(2020, 8, 24));

      if(readConfigChoiceElement(config, "astronomicalTide",    type, "direct tides from sun, moon and planets"))
        tides.push_back(new TidesAstronomical(config));
      if(readConfigChoiceElement(config, "earthTide",           type, "solid earth tides"))
        tides.push_back(new TidesEarth(config));
      if(readConfigChoiceElement(config, "doodsonHarmonicTide", type, "tides with harmonic representation, e.g. ocean tides"))
        tides.push_back(new TidesDoodsonHarmonic(config));
      if(readConfigChoiceElement(config, "poleTide",            type, "centrifugal effect of polar motion"))
        tides.push_back(new TidesPole(config));
      if(readConfigChoiceElement(config, "oceanPoleTide",       type, "The ocean pole tide is generated by the centrifugal effect of polar motion on the oceans"))
        tides.push_back(new TidesOceanPole(config));
      if(readConfigChoiceElement(config, "centrifugal",         type, "Current centrifugal force from Earth rotation"))
        tides.push_back(new TidesCentrifugal(config));
      if(readConfigChoiceElement(config, "solidMoonTide",       type, "solid moon tides (at moon)"))
        tides.push_back(new TidesSolidMoon(config));
      endChoice(config);
      if(isCreateSchema(config))
        return;
    };
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Tides::~Tides()
{
  for(UInt i=0; i<tides.size(); i++)
    delete tides.at(i);
}

/***********************************************/

Double Tides::potential(const Time &timeGPS, const Vector3d &point,
                        const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  try
  {
    Double V = 0;
    for(UInt i=0; i<tides.size(); i++)
      V += tides.at(i)->potential(timeGPS, point, rotEarth, rotation, ephemerides);
    return V;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Double Tides::radialGradient(const Time &timeGPS, const Vector3d &point,
                             const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  try
  {
    Double dVdr = 0;
    for(UInt i=0; i<tides.size(); i++)
      dVdr += tides.at(i)->radialGradient(timeGPS, point, rotEarth, rotation, ephemerides);
    return dVdr;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Vector3d Tides::acceleration(const Time &timeGPS, const Vector3d &point,
                             const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  try
  {
    Vector3d g;
    for(UInt i=0; i<tides.size(); i++)
      g += tides.at(i)->gravity(timeGPS, point, rotEarth, rotation, ephemerides);
    return g;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Tensor3d Tides::gradient(const Time &timeGPS, const Vector3d &point,
                         const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  try
  {
    Tensor3d T;
    for(UInt i=0; i<tides.size(); i++)
      T += tides.at(i)->gravityGradient(timeGPS, point, rotEarth, rotation, ephemerides);
    return T;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Vector3d Tides::deformation(const Time &timeGPS, const Vector3d &point,
                            const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides,
                            Double gravity, const Vector &hn, const Vector &ln) const
{
  try
  {
    Vector3d pos;
    for(UInt i=0; i<tides.size(); i++)
      pos += tides.at(i)->deformation(timeGPS, point, rotEarth, rotation, ephemerides, gravity, hn, ln);
    return pos;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

void Tides::deformation(const std::vector<Time> &timeGPS, const std::vector<Vector3d> &point,
                        const std::vector<Rotary3d> &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides,
                        const std::vector<Double> &gravity, const Vector &hn, const Vector &ln, std::vector<std::vector<Vector3d>> &disp) const
{
  try
  {
    for(UInt i=0; i<tides.size(); i++)
      tides.at(i)->deformation(timeGPS, point, rotEarth, rotation, ephemerides, gravity, hn, ln, disp);
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

SphericalHarmonics Tides::sphericalHarmonics(const Time &timeGPS, const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides,
                                             UInt maxDegree, UInt minDegree, Double GM, Double R) const
{
  try
  {
    if(tides.empty())
      return SphericalHarmonics();

    SphericalHarmonics harmonics = tides.at(0)->sphericalHarmonics(timeGPS, rotEarth, rotation, ephemerides, maxDegree, minDegree, GM, R);
    for(UInt i=1; i<tides.size(); i++)
      harmonics += tides.at(i)->sphericalHarmonics(timeGPS, rotEarth, rotation, ephemerides, maxDegree, minDegree, GM, R);
    return harmonics;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/
/***********************************************/

Double TidesBase::potential(const Time &time, const Vector3d &point,
                            const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  return sphericalHarmonics(time, rotEarth, rotation, ephemerides).potential(point);
}

/***********************************************/

Double TidesBase::radialGradient(const Time &time, const Vector3d &point,
                                 const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  return sphericalHarmonics(time, rotEarth, rotation, ephemerides).radialGradient(point);
}

/***********************************************/

Vector3d TidesBase::gravity(const Time &time, const Vector3d &point,
                            const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  return sphericalHarmonics(time, rotEarth, rotation, ephemerides).gravity(point);
}

/***********************************************/

Tensor3d TidesBase::gravityGradient(const Time &time, const Vector3d &point,
                                    const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides) const
{
  return sphericalHarmonics(time, rotEarth, rotation, ephemerides).gravityGradient(point);
}

/***********************************************/

Vector3d TidesBase::deformation(const Time &time, const Vector3d &point,
                                const Rotary3d &rotEarth, EarthRotationPtr rotation, EphemeridesPtr ephemerides,
                                Double gravity, const Vector &hn, const Vector &ln) const
{
  return sphericalHarmonics(time, rotEarth, rotation, ephemerides).deformation(point, gravity, hn, ln);
}

/***********************************************/

void TidesBase::deformation (const std::vector<Time> &time, const std::vector<Vector3d> &point, const std::vector<Rotary3d> &rotEarth,
                             EarthRotationPtr rotation, EphemeridesPtr ephemerides, const std::vector<Double> &gravity, const Vector &hn, const Vector &ln,
                             std::vector<std::vector<Vector3d>> &disp) const
{
  try
  {
    if((time.size()==0) || (point.size()==0))
      return;

    SphericalHarmonics harm = sphericalHarmonics(time.at(0), rotEarth.at(0), rotation, ephemerides);
    Matrix A = deformationMatrix(point, gravity, hn, ln, harm.GM(), harm.R(), harm.maxDegree());
    for(UInt i=0; i<time.size(); i++)
    {
      Vector Ax = A * sphericalHarmonics(time.at(i), rotEarth.at(i), rotation, ephemerides).x();
      for(UInt k=0; k<point.size(); k++)
      {
        disp.at(k).at(i).x() += Ax(3*k+0);
        disp.at(k).at(i).y() += Ax(3*k+1);
        disp.at(k).at(i).z() += Ax(3*k+2);
      }
    }
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/

Matrix TidesBase::deformationMatrix(const std::vector<Vector3d> &point, const std::vector<Double> &gravity,
                                    const Vector &hn, const Vector &ln, Double GM, Double R, UInt maxDegree)
{
  try
  {
    Matrix A(3*point.size(), (maxDegree+1)*(maxDegree+1));

    for(UInt k=0; k<point.size(); k++)
    {
      Vector3d up = normalize(point.at(k));

      Matrix Cnm, Snm;
      SphericalHarmonics::CnmSnm(1/R * point.at(k), maxDegree+1, Cnm, Snm);

      // 0. order
      for(UInt n=0; n<=maxDegree; n++)
      {
        Double wm0 = sqrt((n+1.)*(n+1.));
        Double wp1 = sqrt((n+1.)*(n+2.)) / sqrt(2.0);
        Double Cm0 = wm0*Cnm(n+1,0);
        Double Cp1 = wp1*Cnm(n+1,1); Double Sp1 = wp1*Snm(n+1,1);

        Double   Vn     = GM/R * Cnm(n,0);
        Vector3d gradVn = GM/(2*R) * sqrt((2*n+1.)/(2*n+3.)) * Vector3d(-2*Cp1, -2*Sp1, -2*Cm0);


        Vector3d disp = (hn(n)/gravity.at(k)*Vn) * up // vertical
                      + (ln(n)/gravity.at(k)) * (gradVn-inner(gradVn,up)*up); // horizontal

        A(3*k+0, n*n) = disp.x();
        A(3*k+1, n*n) = disp.y();
        A(3*k+2, n*n) = disp.z();
      }

      // other orders
      for(UInt m=1; m<=maxDegree; m++)
        for(UInt n=m; n<=maxDegree; n++)
        {
          Double wm1 = sqrt((n-m+1.)*(n-m+2.)) * ((m==1) ? sqrt(2.0) : 1.0);
          Double wm0 = sqrt((n-m+1.)*(n+m+1.));
          Double wp1 = sqrt((n+m+1.)*(n+m+2.));
          Double Cm1 = wm1*Cnm(n+1,m-1);  Double Sm1 = wm1*Snm(n+1,m-1);
          Double Cm0 = wm0*Cnm(n+1,m  );  Double Sm0 = wm0*Snm(n+1,m  );
          Double Cp1 = wp1*Cnm(n+1,m+1);  Double Sp1 = wp1*Snm(n+1,m+1);

          Double   Vn     = GM/R * Cnm(n,m);
          Vector3d gradVn = GM/(2*R) * sqrt((2*n+1.)/(2*n+3.)) * Vector3d(Cm1-Cp1, -Sm1-Sp1, -2*Cm0);

          Vector3d disp = (hn(n)/gravity.at(k)*Vn) * up // vertical
                        + (ln(n)/gravity.at(k)) * (gradVn-inner(gradVn,up)*up); // horizontal

          A(3*k+0, n*n+2*m-1) = disp.x();
          A(3*k+1, n*n+2*m-1) = disp.y();
          A(3*k+2, n*n+2*m-1) = disp.z();

          Vn     = GM/R * Snm(n,m);
          gradVn = GM/(2*R) * sqrt((2*n+1.)/(2*n+3.)) * Vector3d(Sm1-Sp1, Cm1+Cp1, -2*Sm0);

          disp = (hn(n)/gravity.at(k)*Vn) * up // vertical
               + (ln(n)/gravity.at(k)) * (gradVn-inner(gradVn,up)*up); // horizontal

          A(3*k+0, n*n+2*m) = disp.x();
          A(3*k+1, n*n+2*m) = disp.y();
          A(3*k+2, n*n+2*m) = disp.z();
        }
    } // for(k=point)

    return A;
  }
  catch(std::exception &e)
  {
    GROOPS_RETHROW(e)
  }
}

/***********************************************/
/***********************************************/
