
#include "TRuntimeObjects.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <string>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TCagra.h"
#include "TGrandRaiden.h"

#define BAD_NUM -441441

#include "TANLEvent.h"
//#include "GValue.h"

#define PRINT(x) std::cout << #x" = " << x << std::endl
#define STR(x) #x << " = " <<() x

using namespace std;


string name;
stringstream stream;


// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {



  TCagra* cagra = obj.GetDetector<TCagra>();
  TGrandRaiden* gr = obj.GetDetector<TGrandRaiden>();

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  if (cagra && gr) {
    for (auto& hit : *gr) {

      auto& rcnp = hit.GR();
      auto grtime = hit.GetTimestamp();

      auto x = rcnp.GR_X(0);
      auto Ex = x*0.0109738+7.65621;

      // coincidence rate
      static ULong_t first_timestamp = grtime;
      if (first_timestamp) {
        auto rate = (grtime-first_timestamp)/1e8;
        //cout << grtime << " " << first_timestamp << endl;
        obj.FillHistogram("COIN","Rate",3000,0,30000, rate);
      }

      // coincidence time difference
      for (auto& caghit : *cagra) {

        auto cagratime = caghit.Timestamp();
	auto tdiff = cagratime-grtime;
        obj.FillHistogram("COIN","Diff",1000,-1000,1000,cagratime-grtime);
        stream.str("");
        stream << "TimeDiff_" << caghit.GetBoardID() << "_" <<caghit.GetChannel();
        obj.FillHistogram("COIN",stream.str().c_str(),1200,-300,800,tdiff);
	if ( (tdiff > 235) && (tdiff < 245) ){

	  stream.str("");
          stream << "Ge_"<< caghit.GetBoardID() << "_" << caghit.GetChannel();
          obj.FillHistogram("COIN_Raw",stream.str(),10000,0,10000,caghit.Charge());
	  stream.str("");
          stream << "Ge_" << caghit.GetBoardID() << "_" << caghit.GetChannel();
          obj.FillHistogram("COIN_Calibrated",stream.str(),10000,0,10000,caghit.GetEnergy());

	  stream.str("");
          stream << "Ex_Ge_" << caghit.GetBoardID() << "_" << caghit.GetChannel();
          obj.FillHistogram("COIN_Calibrated",stream.str(),1000,0,20,Ex,2500,0,10,caghit.GetEnergy()*0.0010552+0.0636885);

          stream.str("");
          stream << "Ex_CAGRACorrected_" << caghit.GetBoardID() << "_" << caghit.GetChannel();
          obj.FillHistogram("COIN_Calibrated", stream.str().c_str(),1000,0,20,Ex,2500,0,10000,caghit.GetCorrectedEnergy(caghit.GetBaseSample()));
	}

      }


    }


    // int totalhits = 0;
    // for (auto& hit : *gr) { totalhits++; }
    // for (int i=0; i < totalhits; i++) {
    //   obj.FillHistogram("nCoin",4,0,1,0);
    // }

    // static int ncoin = 0;
    // ncoin+=totalhits;
  }


  if (gr) {

    for (auto& hit : *gr) {

      auto& rcnp = hit.GR();
      auto adc = rcnp.GR_ADC();


      if (rcnp.GR_MYRIAD(0) != BAD_NUM) {
        obj.FillHistogram("Timing","MyriadTimestamp",10000,1e9,5e12,hit.GetTimestamp());
      }

      static ULong_t prev_ts = 0;
      if (prev_ts) {
        obj.FillHistogram("Timing","GR_EventPeriod",5000,100,50000,hit.GetTimestamp()-prev_ts);
      }
      prev_ts = hit.GetTimestamp();

      auto rf = rcnp.GR_RF(0);
      if (rf != BAD_NUM) {

        obj.FillHistogram("GR","GR_RF",1000,0,0,rf);
      }

      if (rcnp.GR_ADC()) {
        auto& adc = *rcnp.GR_ADC();
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_ADC" << i;
          obj.FillHistogram("GR",stream.str().c_str(), 1000,0,2000, adc[i]);
        }
        obj.FillHistogram("GR","MeanPlastE1", 2000,0,2000, hit.GetMeanPlastE1());
        obj.FillHistogram("GR","MeanPlastE2", 2000,0,2000, hit.GetMeanPlastE2());
        if (rf != BAD_NUM) {
          obj.FillHistogram("GR","dE1_RF",1000,0,0,rf,2000,0,2000, hit.GetMeanPlastE1());
          obj.FillHistogram("GR","dE2_RF",1000,0,0,rf,2000,0,2000, hit.GetMeanPlastE2());
          obj.FillHistogram("GR","dE2_dE1",2000,0,2000, hit.GetMeanPlastE2(),2000,0,2000, hit.GetMeanPlastE1());

        }
      }
      if (rcnp.GR_TDC()) {
        auto& tdc = *rcnp.GR_TDC();
        for (int i=0; i<4; i++) {
          stream.str(""); stream << "GR_TDC" << i;
          obj.FillHistogram("GR",stream.str().c_str(), 1000,-40000,40000, tdc[i]);
        }
        obj.FillHistogram("GR","MeanPlastPos1", 1000, 0, 40000, hit.GetMeanPlastPos1());
        obj.FillHistogram("GR","MeanPlastPos2", 1000, 0, 40000, hit.GetMeanPlastPos2());
      }
      if (rcnp.QTC_LEADING_TDC()) {
        auto& qtc_leading = *rcnp.QTC_LEADING_TDC();
        auto& qtc_leading_chan = *rcnp.QTC_LEADING_CH();
        auto x = rcnp.GR_X(0);
	auto Ex = x*0.0109738+7.65621;
	double Egamma;

        for (int i=0; i< qtc_leading_chan.size(); i++) {
          int channum = qtc_leading_chan[i];
          stream.str(""); stream << "LaBrLeading" << channum;
          obj.FillHistogram("GR",stream.str().c_str(), 10000,-40000, 40000, qtc_leading[i]);

	  /* cut on prompt timing peak */

          if (qtc_leading[i]>=-5100 && qtc_leading[i] <=-4300) {

            obj.FillHistogram("GR_Prompt","RayID",64,-16,48, rcnp.GR_RAYID(0));
            if (rcnp.GR_RAYID(0) == 0) { // if track reconstruction successfull
              obj.FillHistogram("GR_Prompt","GR_X",1200,-600,600, rcnp.GR_X(0));
              obj.FillHistogram("GR_Prompt","GR_Y",200,-100,100, rcnp.GR_Y(0));
              obj.FillHistogram("GR_Prompt","GR_Theta",100,-1,1, rcnp.GR_TH(0)); // need to learn
              obj.FillHistogram("GR_Prompt","GR_Phi",100,-1,1, rcnp.GR_PH(0)); // from hist.def
              obj.FillHistogram("GR_Prompt","X_TH",1200,-600,600,rcnp.GR_X(0),1000,-1,1,rcnp.GR_TH(0));

              obj.FillHistogram("GR_Prompt","GR_Theta_Phi",100,-1,1, rcnp.GR_TH(0),100,-1,1, rcnp.GR_PH(0)); // need to learn
              obj.FillHistogram("GR_Prompt","GR_X_Y",1200,-600,600, rcnp.GR_X(0),200,-100,100, rcnp.GR_Y(0));
              obj.FillHistogram("GR_Prompt","GR_X_cal",1000,0,20, Ex);

            }

            // this needs to be moved out of the loop over qtc_chan I think  - Chris
	    for (auto const& labr_hit : hit.GetLaBr()) {
	      int channum = labr_hit.channel;
	      stream.str(""); stream << "LaBrWidth_LEGate" << channum;
              obj.FillHistogram("GR_Prompt",stream.str().c_str(), 10000, -5000, 15000, labr_hit.width);
	      if (channum == 1) {
		Egamma = labr_hit.width*0.00190458-1.27177;
                obj.FillHistogram("GR_Prompt","X_LaBrWidth1",1200,-600,600,rcnp.GR_X(0),10000,-5000,15000,labr_hit.width);
                obj.FillHistogram("GR_Prompt","LaBr1_cal", 1000,0,20,Egamma);
		if ((Ex > Egamma-0.150)&&(Ex < Egamma+0.150)) {
                  obj.FillHistogram("GR_Prompt","LaBr1_cal_GS", 1000,0,20,labr_hit.width*0.00181-1.0887);
		}
                obj.FillHistogram("GR_Prompt","x_LaBr1_cal",1200,-600,600,x,1000,0,20,Egamma);
                obj.FillHistogram("GR_Prompt","Ex_LaBr1_cal",1000,-0,20,Ex,1000,0,20,Egamma);
                obj.FillHistogram("GR_Prompt","Ex",1000,-0,20,Ex);
                obj.FillHistogram("GR_Prompt","LaBr1_cal_rf", 1000,0,20,Egamma,1000,-5500,4000,qtc_leading[i]-rf);

	      }

	      if (channum == 3) {
		Egamma = labr_hit.width*0.00168037-0.931571;
                obj.FillHistogram("GR_Prompt","LaBr2_cal", 1000,0,20,Egamma);
                obj.FillHistogram("GR_Prompt","X_LaBrWidth2",1200,-600,600,rcnp.GR_X(0),10000,-5000,15000,labr_hit.width);
                obj.FillHistogram("GR_Prompt","Ex_LaBr2_cal",1000,-0,20,Ex,1000,0,20,Egamma);
                obj.FillHistogram("GR_Prompt","LaBr2_cal_rf", 1000,0,20,Egamma,1000,-5500,4000,qtc_leading[i]-rf);
		if ((Ex > Egamma-0.150)&&(Ex < Egamma+0.150)) {
                  obj.FillHistogram("GR_Prompt","LaBr2_cal_GS", 1000,0,20,labr_hit.width*0.00181-1.0887);
		}
	      }

	      if (rcnp.GR_TH(0)<0.04){
                obj.FillHistogram("GR_Prompt","Gamma_cal_cutTheta", 1000,0,20,Egamma);
		if ((Ex > Egamma-0.150)&&(Ex < Egamma+0.150)) {
                  obj.FillHistogram("GR_Prompt","Gamma_cal_cutTheta_GS", 1000,0,20,labr_hit.width*0.00181-1.0887);
		}
		//if (Ex > 9. && Ex < 10.) {
		  //obj.FillHistogram("Gamma_cal_Ex_9650", 1000,0,10,labr_hit.width*0.00181-1.0887);
                //}
                //		obj.FillHistogram("x_Egamma_prompt",1200,-600,600,x,1000,0,20,Egamma);
                //	obj.FillHistogram("Ex_Egamma_prompt",1000,-0,20,Ex,1000,0,20,Egamma);
                obj.FillHistogram("GR_Prompt","Ex_TH_cutTheta",1000,0,20,Ex,1000,-1,1,rcnp.GR_TH(0));
	      }

	    }
	  }

        }
      }

      for (auto const& labr_hit : hit.GetLaBr()) {
        int channum = labr_hit.channel;
        stream.str(""); stream << "LaBrWidth" << channum;
        obj.FillHistogram("GR",stream.str().c_str(), 10000, -5000, 15000, labr_hit.width);
        stream.str(""); stream << "LaBrWidth_cal" << channum;
        obj.FillHistogram("GR",stream.str().c_str(), 1000,0,20,labr_hit.width*0.00181-1.0887);

        if (rcnp.GR_X(0) != BAD_NUM) {
          obj.FillHistogram("GR","X_LaBr",1200,-600,600,rcnp.GR_X(0),10000,-5000,15000,labr_hit.width);
	  if (channum == 1){
            obj.FillHistogram("GR","X_LaBr_cal",1000,-0,20,rcnp.GR_X(0)*0.0109+7.6324,1000,0,20,labr_hit.width*0.00181-1.0887);
	  }
        }

      }
      obj.FillHistogram("GR","RayID",64,-16,48, rcnp.GR_RAYID(0));
      if (rcnp.GR_RAYID(0) == 0) { // if track reconstruction successfull
        obj.FillHistogram("GR","GR_X",1200,-600,600, rcnp.GR_X(0));
        obj.FillHistogram("GR","GR_X_cal",1000,0,20, rcnp.GR_X(0)*0.0109+7.6324);
        obj.FillHistogram("GR","GR_Y",200,-100,100, rcnp.GR_Y(0));
        obj.FillHistogram("GR","GR_Theta",100,-1,1, rcnp.GR_TH(0)); // need to learn
        obj.FillHistogram("GR","GR_Phi",100,-1,1, rcnp.GR_PH(0)); // from hist.def
        obj.FillHistogram("GR","X_TH",1200,-600,600,rcnp.GR_X(0),1000,-1,1,rcnp.GR_TH(0));
      }

      //   obj.FillHistogram("GR_RF",1000,0,0,rf);
      //   auto first = TMath::Sqrt(adc[0]*adc[1]);
      //   auto second = TMath::Sqrt(adc[2]*adc[3]);
      //   obj.FillHistogram("pid_1",500,0,0,rf,500,0,0,first);
      //   obj.FillHistogram("pid_2",500,0,0,rf,500,0,0,second);
      // }
    }
  }

  if(cagra) {

    for (auto& hit : *cagra) {

      auto boardid = hit.GetBoardID();
      auto chan = hit.GetChannel();

      stream.str("");
      stream << "Ge_" << boardid << "_" << chan;
      obj.FillHistogram("CAGRA_Raw", stream.str().c_str(),10000,0,10000,hit.Charge());

      stream.str("");
      stream << "Ge_" << boardid << "_" << chan;
      if (boardid == 102) {
        auto labr_E = hit.GetTraceEnergy(0,57,60,60+57);
        //if (nevent % 10000 == 0) { cout << labr_E << endl; }
        obj.FillHistogram("CAGRA_Calibrated",stream.str().c_str(),10000,0,10000,labr_E);
      } else {
        obj.FillHistogram("CAGRA_Calibrated",stream.str().c_str(),10000,0,10000,hit.GetEnergy());
      }

      stream.str("");
      stream << "Ge_PZ_" << boardid << "_" << chan;
      obj.FillHistogram("CAGRA_Corrected", stream.str().c_str(),10000,0,10000,hit.GetCorrectedEnergy(0));
      stream.str("");
      stream << "Ge_PZ_AsymBL_" << boardid << "Cal" << chan;
      obj.FillHistogram("CAGRA_Corrected", stream.str().c_str(),10000,0,10000,hit.GetCorrectedEnergy(hit.GetBaseSample()));

      Double_t prerise_base = hit.GetPreRise()/TANLEvent::GetShapingTime();

      stream.str("");
      stream << "E_BL" << boardid << "_" << chan;
      obj.FillHistogram("CAGRA_Baseline", stream.str().c_str(),1000,0,10000,hit.Charge(),1000,0,3000,prerise_base);

      stream.str("");
      stream << "E_BL_scale" << boardid << "_" << chan;
      obj.FillHistogram("CAGRA_Baseline", stream.str().c_str(),1000,0,10000,hit.Charge()-(1.0/-11.21)*prerise_base,1000,0,3000,prerise_base);

      stream.str("");
      stream << "E_cor_BL" << boardid << "_" << chan;
      obj.FillHistogram("CAGRA_Baseline", stream.str().c_str(),1000,0,10000,hit.GetCorrectedEnergy(hit.GetBaseSample()),1000,0,3000,prerise_base);



    }

  }



  if(numobj!=list->GetSize())
    list->Sort();


}
