#ifndef MACRO_G4HCALOUTREF_C
#define MACRO_G4HCALOUTREF_C

#include <GlobalVariables.C>
#include <QA.C>

#include <g4calo/HcalRawTowerBuilder.h>
#include <g4calo/RawTowerDigitizer.h>

#include <g4ohcal/PHG4OHCalSubsystem.h>

#include <g4detectors/PHG4HcalCellReco.h>
#include <g4detectors/PHG4OuterHcalSubsystem.h>

#include <g4eval/CaloEvaluator.h>

#include <g4main/PHG4Reco.h>

#include <calowaveformsim/CaloWaveformSim.h>

#include <calobase/TowerInfoDefs.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <caloreco/RawClusterBuilderGraph.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawTowerCalibration.h>

#include <simqa_modules/QAG4SimulationCalorimeter.h>

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libg4calo.so)
R__LOAD_LIBRARY(libCaloWaveformSim.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libg4ohcal.so)
R__LOAD_LIBRARY(libsimqa_modules.so)

namespace Enable
{
  bool HCALOUT = false;
  bool HCALOUT_ABSORBER = false;
  bool HCALOUT_OVERLAPCHECK = false;
  bool HCALOUT_CELL = false;
  bool HCALOUT_TOWER = false;
  bool HCALOUT_CLUSTER = false;
  bool HCALOUT_EVAL = false;
  bool HCALOUT_QA = false;
  bool HCALOUT_OLD = false;
  bool HCALOUT_RING = false;
  bool HCALOUT_G4Hit = true;
  bool HCALOUT_TOWERINFO = false;
  int HCALOUT_VERBOSITY = 0;
}  // namespace Enable

namespace G4HCALOUT
{
  double outer_radius = 269.317 + 5;
  double size_z = 639.240 + 10;
  double phistart = NAN;
  double tower_emin = NAN;
  int light_scint_model = -1;
  int tower_energy_source = -1;

  // Digitization (default photon digi):
  RawTowerDigitizer::enu_digi_algorithm TowerDigi = RawTowerDigitizer::kSimple_photon_digitization;
  // directly pass the energy of sim tower to digitized tower
  // kNo_digitization
  // simple digitization with photon statistics, single amplitude ADC conversion and pedestal
  // kSimple_photon_digitization
  // digitization with photon statistics on SiPM with an effective pixel N, ADC conversion and pedestal
  // kSiPM_photon_digitization

  enum enu_HCalOut_clusterizer
  {
    kHCalOutGraphClusterizer,
    kHCalOutTemplateClusterizer
  };

  bool useTowerInfoV2 = true;

  //! template clusterizer, RawClusterBuilderTemplate, as developed by Sasha Bazilevsky
  enu_HCalOut_clusterizer HCalOut_clusterizer = kHCalOutTemplateClusterizer;
  //! graph clusterizer, RawClusterBuilderGraph
  // enu_HCalOut_clusterizer HCalOut_clusterizer = kHCalOutGraphClusterizer;
}  // namespace G4HCALOUT

// Init is called by G4Setup.C
void HCalOuterInit()
{
  BlackHoleGeometry::max_radius = std::max(BlackHoleGeometry::max_radius, G4HCALOUT::outer_radius);
  BlackHoleGeometry::max_z = std::max(BlackHoleGeometry::max_z, G4HCALOUT::size_z / 2.);
  BlackHoleGeometry::min_z = std::min(BlackHoleGeometry::min_z, -G4HCALOUT::size_z / 2.);
}

double HCalOuter(PHG4Reco *g4Reco,
                 double radius,
                 const int crossings)
{
  bool AbsorberActive = Enable::ABSORBER || Enable::HCALOUT_ABSORBER;
  bool OverlapCheck = Enable::OVERLAPCHECK || Enable::HCALOUT_OVERLAPCHECK;
  int verbosity = std::max(Enable::VERBOSITY, Enable::HCALOUT_VERBOSITY);

  PHG4DetectorSubsystem *hcal = nullptr;
  //  Mephi Maps
  //  Maps are different for old/new but how to set is identical
  //  here are the ones for the old outer hcal since the new maps do not exist yet
  //  use hcal->set_string_param("MapFileName",""); to disable map
  //  hcal->set_string_param("MapFileName",std::string(getenv("CALIBRATIONROOT")) + "/HCALOUT/tilemap/oHCALMaps092021.root");
  //  hcal->set_string_param("MapHistoName","hCombinedMap");

  if (Enable::HCALOUT_OLD)
  {
    hcal = new PHG4OuterHcalSubsystem("HCALOUT");
    // hcal->set_double_param("inner_radius", 183.3);
    //-----------------------------------------
    // the light correction can be set in a single call
    // hcal->set_double_param("light_balance_inner_corr", NAN);
    // hcal->set_double_param("light_balance_inner_radius", NAN);
    // hcal->set_double_param("light_balance_outer_corr", NAN);
    // hcal->set_double_param("light_balance_outer_radius", NAN);
    // hcal->set_double_param("magnet_cutout_radius", 195.31);
    // hcal->set_double_param("magnet_cutout_scinti_radius", 195.96);
    // hcal->SetLightCorrection(NAN,NAN,NAN,NAN);
    //-----------------------------------------
    // hcal->set_double_param("outer_radius", G4HCALOUT::outer_radius);
    // hcal->set_double_param("place_x", 0.);
    // hcal->set_double_param("place_y", 0.);
    // hcal->set_double_param("place_z", 0.);
    // hcal->set_double_param("rot_x", 0.);
    // hcal->set_double_param("rot_y", 0.);
    // hcal->set_double_param("rot_z", 0.);
    // hcal->set_double_param("scinti_eta_coverage", 1.1);
    // hcal->set_double_param("scinti_gap", 0.85);
    // hcal->set_double_param("scinti_gap_neighbor", 0.1);
    // hcal->set_double_param("scinti_inner_radius",183.89);
    // hcal->set_double_param("scinti_outer_radius",263.27);
    // hcal->set_double_param("scinti_tile_thickness", 0.7);
    // hcal->set_double_param("size_z", G4HCALOUT::size_z);
    // hcal->set_double_param("steplimits", NAN);
    // hcal->set_double_param("tilt_angle", -11.23);

    // hcal->set_int_param("light_scint_model", 1);
    // hcal->set_int_param("magnet_cutout_first_scinti", 8);
    // hcal->set_int_param("ncross", 0);
    // hcal->set_int_param("n_towers", 64);
    // hcal->set_int_param("n_scinti_plates_per_tower", 5);
    // hcal->set_int_param("n_scinti_tiles", 12);

    // hcal->set_string_param("material", "Steel_1006");
  }
  else
  {
    hcal = new PHG4OHCalSubsystem("HCALOUT");
    if (Enable::HCALOUT_RING)
    {
      std::string gdmlfile_no_ring = std::string(getenv("CALIBRATIONROOT")) + "/HcalGeo/OuterHCalAbsorberTiles_merged.gdml";
      hcal->set_string_param("GDMPath", gdmlfile_no_ring);
    }
    // hcal->set_string_param("GDMPath", "mytestgdml.gdml"); // try other gdml file
    // common setting with tracking, we likely want to move to the cdb with this
    hcal->set_string_param("IronFieldMapPath", G4MAGNET::magfield_OHCAL_steel);
    hcal->set_double_param("IronFieldMapScale", G4MAGNET::magfield_rescale);
  }

  if (G4HCALOUT::light_scint_model >= 0)
  {
    hcal->set_int_param("light_scint_model", G4HCALOUT::light_scint_model);
  }
  // hcal->set_int_param("field_check", 1); // for validating the field in HCal
  hcal->SetActive();
  hcal->SuperDetector("HCALOUT");
  if (AbsorberActive)
  {
    hcal->SetAbsorberActive();
  }
  hcal->OverlapCheck(OverlapCheck);
  if (!std::isfinite(G4HCALOUT::phistart))
  {
    if (Enable::HCALOUT_OLD)
    {
      G4HCALOUT::phistart = 0.026598397;  // offet in phi (from zero) extracted from geantinos
    }
    else
    {
      G4HCALOUT::phistart = 0.0240615415;  // offet in phi (from zero) extracted from geantinos
    }
  }
  hcal->set_int_param("saveg4hit", Enable::HCALOUT_G4Hit);
  hcal->set_double_param("phistart", G4HCALOUT::phistart);
  g4Reco->registerSubsystem(hcal);

  if (!Enable::HCALOUT_OLD)
  {
    // HCal support rings, approximated as solid rings
    // note there is only one ring on either side, but to allow part of the ring inside the HCal envelope two rings are used
    const double inch = 2.54;
    const double support_ring_outer_radius = 74.061 * inch;
    const double innerradius = 56.188 * inch;
    const double hcal_envelope_radius = 182.423 - 5.;
    const double support_ring_z = 175.375 * inch / 2.;
    const double support_ring_dz = 4. * inch;
    const double z_rings[] =
        {-support_ring_z, support_ring_z};
    PHG4CylinderSubsystem *cyl;
    PHG4CylinderSubsystem *cylout;

    for (int i = 0; i < 2; i++)
    {
      // rings outside of HCal envelope
      cyl = new PHG4CylinderSubsystem("HCAL_SPT_N1", i);
      cyl->set_double_param("place_z", z_rings[i]);
      cyl->SuperDetector("HCALIN_SPT");
      cyl->set_double_param("radius", innerradius);
      cyl->set_int_param("lengthviarapidity", 0);
      cyl->set_double_param("length", support_ring_dz);
      cyl->set_string_param("material", "G4_Al");
      cyl->set_double_param("thickness", hcal_envelope_radius - 0.1 - innerradius);
      cyl->set_double_param("start_phi_rad", 1.867);
      cyl->set_double_param("delta_phi_rad", 5.692);
      cyl->OverlapCheck(Enable::OVERLAPCHECK);
      if (AbsorberActive)
      {
        cyl->SetActive();
      }
      g4Reco->registerSubsystem(cyl);

      // rings inside outer HCal envelope
      // only use if we want to use the old version of the ring instead of the gdml implementation
      if (Enable::HCALOUT_RING)
      {
        cylout = new PHG4CylinderSubsystem("HCAL_SPT_N1", i + 2);
        cylout->set_double_param("place_z", z_rings[i]);
        cylout->SuperDetector("HCALIN_SPT");
        cylout->set_double_param("radius", hcal_envelope_radius + 0.1);  // add a mm to avoid overlaps
        cylout->set_int_param("lengthviarapidity", 0);
        cylout->set_double_param("length", support_ring_dz);
        cylout->set_string_param("material", "G4_Al");
        cylout->set_double_param("thickness", support_ring_outer_radius - (hcal_envelope_radius + 0.1));
        cylout->set_double_param("start_phi_rad", 1.867);
        cylout->set_double_param("delta_phi_rad", 5.692);
        if (AbsorberActive)
        {
          cylout->SetActive();
        }
        cylout->SetMotherSubsystem(hcal);
        cylout->OverlapCheck(OverlapCheck);
        g4Reco->registerSubsystem(cylout);
      }
    }
  }

  radius = hcal->get_double_param("outer_radius");

  radius += no_overlapp;

  return radius;
}

void HCALOuter_Cells()
{
  if (!Enable::HCALOUT_G4Hit) return;
  int verbosity = std::max(Enable::VERBOSITY, Enable::HCALOUT_VERBOSITY);

  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4HcalCellReco *hc = new PHG4HcalCellReco("HCALOUT_CELLRECO");
  hc->Detector("HCALOUT");
  hc->Verbosity(verbosity);
  // check for energy conservation - needs modified "infinite" timing cuts
  // 0-999999999
  //  hc->checkenergy();
  // timing cuts with their default settings
  // hc->set_double_param("tmin",0.);
  // hc->set_double_param("tmax",60.0);
  // or all at once:
  // hc->set_timing_window(0.0,60.0);
  // this sets all cells to a fixed energy for debugging
  // hc->set_fixed_energy(1.);
  se->registerSubsystem(hc);

  return;
}

void HCALOuter_Towers()
{
  int verbosity = std::max(Enable::VERBOSITY, Enable::HCALOUT_VERBOSITY);
  Fun4AllServer *se = Fun4AllServer::instance();
  // build the raw tower anyways for the geom nodes
  if (Enable::HCALOUT_G4Hit)
  {
    HcalRawTowerBuilder *TowerBuilder = new HcalRawTowerBuilder("HcalOutRawTowerBuilder");
    TowerBuilder->Detector("HCALOUT");
    TowerBuilder->set_sim_tower_node_prefix("SIM");
    if (!isfinite(G4HCALOUT::phistart))
    {
      if (Enable::HCALOUT_OLD)
      {
        G4HCALOUT::phistart = 0.026598397;  // offet in phi (from zero) extracted from geantinos
      }
      else
      {
        G4HCALOUT::phistart = 0.0240615415;  // offet in phi (from zero) extracted from geantinos
      }
    }
    TowerBuilder->set_double_param("phistart", G4HCALOUT::phistart);
    if (isfinite(G4HCALOUT::tower_emin))
    {
      TowerBuilder->set_double_param("emin", G4HCALOUT::tower_emin);
    }
    if (G4HCALOUT::tower_energy_source >= 0)
    {
      TowerBuilder->set_int_param("tower_energy_source", G4HCALOUT::tower_energy_source);
    }
    // this sets specific decalibration factors
    // for a given cell
    // TowerBuilder->set_cell_decal_factor(1,10,0.1);
    // for a whole tower
    // TowerBuilder->set_tower_decal_factor(0,10,0.2);
    // TowerBuilder->set_cell_decal_factor(1,10,0.1);
    // TowerBuilder->set_tower_decal_factor(0,10,0.2);
    TowerBuilder->Verbosity(verbosity);
    se->registerSubsystem(TowerBuilder);
  }
  if (!Enable::HCALOUT_TOWERINFO)
  {
    // From 2016 Test beam sim
    RawTowerDigitizer *TowerDigitizer = new RawTowerDigitizer("HcalOutRawTowerDigitizer");
    TowerDigitizer->Detector("HCALOUT");
    //  TowerDigitizer->set_raw_tower_node_prefix("RAW_LG");
    TowerDigitizer->set_digi_algorithm(G4HCALOUT::TowerDigi);
    TowerDigitizer->set_pedstal_central_ADC(0);
    TowerDigitizer->set_pedstal_width_ADC(1);  // From Jin's guess. No EMCal High Gain data yet! TODO: update
    TowerDigitizer->set_photonelec_ADC(16. / 5.);
    TowerDigitizer->set_photonelec_yield_visible_GeV(16. / 5 / (0.2e-3));
    TowerDigitizer->set_zero_suppression_ADC(-0);  // no-zero suppression
    TowerDigitizer->Verbosity(verbosity);
    if (!Enable::HCALOUT_G4Hit) TowerDigitizer->set_towerinfo(RawTowerDigitizer::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
    se->registerSubsystem(TowerDigitizer);

    const double visible_sample_fraction_HCALOUT = 3.38021e-02;  // /gpfs/mnt/gpfs04/sphenix/user/jinhuang/prod_analysis/hadron_shower_res_nightly/./G4Hits_sPHENIX_pi-_eta0_16GeV.root_qa.rootQA_Draw_HCALOUT_G4Hit.pdf

    RawTowerCalibration *TowerCalibration = new RawTowerCalibration("HcalOutRawTowerCalibration");
    TowerCalibration->Detector("HCALOUT");
    TowerCalibration->set_usetowerinfo_v2(G4HCALOUT::useTowerInfoV2);

    //  TowerCalibration->set_raw_tower_node_prefix("RAW_LG");
    //  TowerCalibration->set_calib_tower_node_prefix("CALIB_LG");
    TowerCalibration->set_calib_algorithm(RawTowerCalibration::kSimple_linear_calibration);
    if (G4HCALOUT::TowerDigi == RawTowerDigitizer::kNo_digitization)
    {
      // 0.033 extracted from electron sims (edep(scintillator)/edep(total))
      TowerCalibration->set_calib_const_GeV_ADC(1. / 0.033);
    }
    else
    {
      TowerCalibration->set_calib_const_GeV_ADC(0.2e-3 / visible_sample_fraction_HCALOUT);
    }
    TowerCalibration->set_pedstal_ADC(0);
    TowerCalibration->Verbosity(verbosity);
    if (!Enable::HCALOUT_G4Hit) TowerCalibration->set_towerinfo(RawTowerCalibration::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
    se->registerSubsystem(TowerCalibration);
  }
  // where I use waveformsim
  else
  {
    CaloWaveformSim *caloWaveformSim = new CaloWaveformSim();
    caloWaveformSim->set_detector_type(CaloTowerDefs::HCALOUT);
    caloWaveformSim->set_detector("HCALOUT");
    caloWaveformSim->set_nsamples(12);
    caloWaveformSim->set_pedestalsamples(12);
    caloWaveformSim->set_timewidth(0.2);
    caloWaveformSim->set_peakpos(6);
    // caloWaveformSim->Verbosity(2);
    // caloWaveformSim->set_noise_type(CaloWaveformSim::NOISE_NONE);
    se->registerSubsystem(caloWaveformSim);

    CaloTowerBuilder *ca2 = new CaloTowerBuilder();
    ca2->set_detector_type(CaloTowerDefs::HCALOUT);
    ca2->set_nsamples(12);
    ca2->set_dataflag(false);
    ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ca2->set_builder_type(CaloTowerDefs::kWaveformTowerSimv1);
    ca2->set_softwarezerosuppression(true, 30);
    se->registerSubsystem(ca2);

    CaloTowerStatus *statusHCALOUT = new CaloTowerStatus("HCALOUTSTATUS");
    statusHCALOUT->set_detector_type(CaloTowerDefs::HCALOUT);
    statusHCALOUT->set_time_cut(2);
    se->registerSubsystem(statusHCALOUT);

    CaloTowerCalib *calibOHCal = new CaloTowerCalib("HCALOUTCALIB");
    calibOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
    calibOHCal->set_outputNodePrefix("TOWERINFO_CALIB_");
    se->registerSubsystem(calibOHCal);
  }

  return;
}

void HCALOuter_Clusters()
{
  int verbosity = std::max(Enable::VERBOSITY, Enable::HCALOUT_VERBOSITY);

  Fun4AllServer *se = Fun4AllServer::instance();

  if (G4HCALOUT::HCalOut_clusterizer == G4HCALOUT::kHCalOutTemplateClusterizer)
  {
    RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("HcalOutRawClusterBuilderTemplate");
    ClusterBuilder->Detector("HCALOUT");
    ClusterBuilder->SetCylindricalGeometry();  // has to be called after Detector()
    ClusterBuilder->Verbosity(verbosity);
    if (!Enable::HCALOUT_G4Hit || Enable::HCALOUT_TOWERINFO) ClusterBuilder->set_UseTowerInfo(1);  // just use towerinfo
    se->registerSubsystem(ClusterBuilder);
  }
  else if (G4HCALOUT::HCalOut_clusterizer == G4HCALOUT::kHCalOutGraphClusterizer)
  {
    RawClusterBuilderGraph *ClusterBuilder = new RawClusterBuilderGraph("HcalOutRawClusterBuilderGraph");
    ClusterBuilder->Detector("HCALOUT");
    ClusterBuilder->Verbosity(verbosity);
    // if (!Enable::HCALOUT_G4Hit) ClusterBuilder->set_UseTowerInfo(1);  // just use towerinfo
    se->registerSubsystem(ClusterBuilder);
  }
  else
  {
    std::cout << "HCALOuter_Clusters - unknown clusterizer setting!" << std::endl;
    exit(1);
  }

  return;
}

void HCALOuter_Eval(const std::string &outputfile, int start_event = 0)
{
  int verbosity = std::max(Enable::VERBOSITY, Enable::HCALOUT_VERBOSITY);

  Fun4AllServer *se = Fun4AllServer::instance();

  CaloEvaluator *eval = new CaloEvaluator("HCALOUTEVALUATOR", "HCALOUT", outputfile);
  eval->set_event(start_event);
  eval->Verbosity(verbosity);
  eval->set_use_towerinfo(Enable::HCALOUT_TOWERINFO);
  se->registerSubsystem(eval);

  return;
}

void HCALOuter_QA()
{
  int verbosity = std::max(Enable::QA_VERBOSITY, Enable::HCALOUT_VERBOSITY);

  Fun4AllServer *se = Fun4AllServer::instance();
  QAG4SimulationCalorimeter *qa = new QAG4SimulationCalorimeter("HCALOUT");
  if (Enable::HCALOUT_TOWERINFO) qa->set_flags(QAG4SimulationCalorimeter::kProcessTowerinfo);
  qa->Verbosity(verbosity);
  se->registerSubsystem(qa);

  return;
}

#endif
