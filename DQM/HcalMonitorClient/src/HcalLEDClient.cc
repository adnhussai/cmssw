#include <DQM/HcalMonitorClient/interface/HcalLEDClient.h>

HcalLEDClient::HcalLEDClient(const ParameterSet& ps, MonitorUserInterface* mui){
  
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();

  mui_ = mui;

   ///HB ieta/iphi/depths
  etaMin[0]=1; etaMax[0]=16;
  phiMin[0]=1; phiMax[0]=71;
  depMin[0]=1; depMax[0]=2;

  ///HE ieta/iphi/depths
  etaMin[1]=16; etaMax[1]=29;
  phiMin[1]=1; phiMax[1]=71;
  depMin[1]=1; depMax[1]=3;
  
  ///HO ieta/iphi/depths
  etaMin[2]=1; etaMax[2]=15;
  phiMin[2]=1; phiMax[2]=71;
  depMin[2]=1; depMax[2]=1;

  ///HF ieta/iphi/depths
  etaMin[3]=29; etaMax[3]=41;
  phiMin[3]=1; phiMax[3]=71;
  depMin[3]=1; depMax[3]=1;



  for(int i=0; i<4; i++){    
    rms_shape[i]=0;
    mean_shape[i]=0;
    rms_time[i]=0;
    mean_time[i]=0;
    err_map_geo[i]=0;
    err_map_elec[i]=0;
    avg_shape[i] = 0;
    avg_time[i] = 0;
  }

  // cloneME switch
  cloneME_ = ps.getUntrackedParameter<bool>("cloneME", true);
  // verbosity switch
  verbose_ = ps.getUntrackedParameter<bool>("verbose", false);

  rms_thresh_ = ps.getUntrackedParameter<double>("LEDRMS_ErrThresh", 0.8);
  cout << "LED RMS error threshold set to " << rms_thresh_ << endl;
  
  mean_thresh_ = ps.getUntrackedParameter<double>("LEDMEAN_ErrThresh", 2.25);
  cout << "LED MEAN error threshold set to " << mean_thresh_ << endl;

  // DQM default process name
  process_ = ps.getUntrackedParameter<string>("processName", "HcalMonitor");
}

HcalLEDClient::HcalLEDClient(){
  
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();

  mui_ = 0;
    ///HB ieta/iphi/depths
  etaMin[0]=1; etaMax[0]=16;
  phiMin[0]=1; phiMax[0]=71;
  depMin[0]=1; depMax[0]=2;

  ///HE ieta/iphi/depths
  etaMin[1]=16; etaMax[1]=29;
  phiMin[1]=1; phiMax[1]=71;
  depMin[1]=1; depMax[1]=3;
  
  ///HO ieta/iphi/depths
  etaMin[2]=1; etaMax[2]=15;
  phiMin[2]=1; phiMax[2]=71;
  depMin[2]=1; depMax[2]=1;

  ///HF ieta/iphi/depths
  etaMin[3]=29; etaMax[3]=41;
  phiMin[3]=1; phiMax[3]=71;
  depMin[3]=1; depMax[3]=1;

  for(int i=0; i<4; i++){    
    rms_shape[i]=0;
    mean_shape[i]=0;
    rms_time[i]=0;
    mean_time[i]=0;
    err_map_geo[i]=0;
    err_map_elec[i]=0;
    avg_shape[i] = 0;
    avg_time[i] = 0;
  }

  // verbosity switch
  verbose_ = false;

}

HcalLEDClient::~HcalLEDClient(){
  this->cleanup();
}

void HcalLEDClient::beginJob(const EventSetup& eventSetup){
  
  if ( verbose_ ) cout << "HcalLEDClient: beginJob" << endl;
  //  eventSetup.get<HcalDbRecord>().get(conditions_);
  
   // get the hcal mapping
  edm::ESHandle<HcalDbService> pSetup;
  eventSetup.get<HcalDbRecord>().get( pSetup );
  readoutMap_=pSetup->getHcalMapping();

  ievt_ = 0;
  jevt_ = 0;
  this->setup();
  this->subscribe();
  this->resetME();
  return;
}

void HcalLEDClient::beginRun(void){

  if ( verbose_ ) cout << "HcalLEDClient: beginRun" << endl;

  jevt_ = 0;
  this->setup();
  this->subscribe();
  this->resetME();
  return;
}

void HcalLEDClient::endJob(void) {

  if ( verbose_ ) cout << "HcalLEDClient: endJob, ievt = " << ievt_ << endl;

  //  this->unsubscribe();
  this->cleanup();
  return;
}

void HcalLEDClient::endRun(void) {

  if ( verbose_ ) cout << "HcalLEDClient: endRun, jevt = " << jevt_ << endl;

  //  this->resetME();
  //  this->unsubscribe();
  this->cleanup();
  return;
}

void HcalLEDClient::setup(void) {
  return;
}

void HcalLEDClient::cleanup(void) {
  if( cloneME_ ){
    for(int i=0; i<4; i++){
      if(rms_shape[i]) delete rms_shape[i];
      if(mean_shape[i]) delete mean_shape[i];
      if(rms_time[i]) delete rms_time[i];
      if(mean_time[i]) delete mean_time[i];
      if(err_map_geo[i]) delete err_map_geo[i];
      if(err_map_elec[i]) delete err_map_elec[i];
      if(avg_shape[i]) delete avg_shape[i];
      if(avg_time[i]) delete avg_time[i];
    }
  }
  for(int i=0; i<4; i++){    
    rms_shape[i]=0;
    mean_shape[i]=0;
    rms_time[i]=0;
    mean_time[i]=0;
    err_map_geo[i]=0;
    err_map_elec[i]=0;
    avg_shape[i] = 0;
    avg_time[i] = 0;
  }

  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();
  return;
}

void HcalLEDClient::subscribe(void){

  if ( verbose_ ) cout << "HcalLEDClient: subscribe" << endl;
  if(mui_) mui_->subscribe("*/HcalMonitor/LEDMonitor/*");
  return;
}

void HcalLEDClient::subscribeNew(void){
  if(mui_) mui_->subscribeNew("*/HcalMonitor/LEDMonitor/*");
  return;
}

void HcalLEDClient::unsubscribe(void){

  if ( verbose_ ) cout << "HcalLEDClient: unsubscribe" << endl;
  if(mui_) mui_->unsubscribe("*/HcalMonitor/LEDMonitor/*");
  return;
}

void HcalLEDClient::errorOutput(){
  if(!mui_) return;
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  
  for (map<string, string>::iterator testsMap=dqmQtests_.begin(); testsMap!=dqmQtests_.end();testsMap++){
    string testName = testsMap->first;
    string meName = testsMap->second;
    MonitorElement* me = mui_->get(meName);
    if(me){
      if (me->hasError()){
	vector<QReport*> report =  me->getQErrors();
	dqmReportMapErr_[meName] = report;
      }
      if (me->hasWarning()){
	vector<QReport*> report =  me->getQWarnings();
	dqmReportMapWarn_[meName] = report;
      }
      if(me->hasOtherReport()){
	vector<QReport*> report = me->getQOthers();
	dqmReportMapOther_[meName] = report;
      }
    }
  }
  printf("LED Task: %d errs, %d warnings, %d others\n",dqmReportMapErr_.size(),dqmReportMapWarn_.size(),dqmReportMapOther_.size());

  return;
}

void HcalLEDClient::getErrors(map<string, vector<QReport*> > outE, map<string, vector<QReport*> > outW, map<string, vector<QReport*> > outO){

  this->errorOutput();
  outE.clear(); outW.clear(); outO.clear();

  for(map<string, vector<QReport*> >::iterator i=dqmReportMapErr_.begin(); i!=dqmReportMapErr_.end(); i++){
    outE[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapWarn_.begin(); i!=dqmReportMapWarn_.end(); i++){
    outW[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapOther_.begin(); i!=dqmReportMapOther_.end(); i++){
    outO[i->first] = i->second;
  }

  return;
}

void HcalLEDClient::report(){
   if(!mui_) return;
  if ( verbose_ ) cout << "HcalLEDClient: report" << endl;
  this->setup();

  char name[256];
  sprintf(name, "%sHcalMonitor/LEDMonitor/LED Task Event Number",process_.c_str());
  MonitorElement* me = mui_->get(name);
  if ( me ) {
    string s = me->valueString();
    ievt_ = -1;
    sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
    if ( verbose_ ) cout << "Found '" << name << "'" << endl;
  }
  getHistograms();
  return;
}

void HcalLEDClient::getHistograms(){
  if(!mui_) return;
  char name[256];
  for(int i=0; i<4; i++){
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HO";
    if(i==3) type = "HF";

    sprintf(name,"LEDMonitor/%s/%s Average Pulse Shape",type.c_str(),type.c_str());      
    avg_shape[i] = getHisto(name, process_,mui_,verbose_,cloneME_);
    sprintf(name,"LEDMonitor/%s/%s Average Pulse Time",type.c_str(),type.c_str());      
    avg_time[i] = getHisto(name, process_,mui_,verbose_,cloneME_);
    
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape RMS Values",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meShapeRMS  = mui_->get(name);
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape Mean Values",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meShapeMean  = mui_->get(name);
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Time RMS Values",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meTimeRMS  = mui_->get(name);
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Time Mean Values",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meTimeMean  = mui_->get(name);
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Geo Error Map",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meGeoErr  = mui_->get(name);
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Elec Error Map",process_.c_str(),type.c_str(),type.c_str());
    MonitorElement* meElecErr  = mui_->get(name);

    if(!meShapeRMS || !meShapeMean) return;
    if(!meTimeRMS || !meTimeMean) return;
    mui_->softReset(meShapeRMS); mui_->softReset(meShapeMean);
    mui_->softReset(meTimeRMS); mui_->softReset(meTimeMean);
    mui_->softReset(meGeoErr); mui_->softReset(meElecErr);

    for(int ieta=-etaMax[i]; ieta<=etaMax[i]; ieta++){
      if(abs(ieta)<etaMin[i]) continue;
      for(int iphi=phiMin[i]; iphi<=phiMax[i]; iphi++){
	for(int depth=depMin[i]; depth<=depMax[i]; depth++){
	  sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape ieta=%d iphi=%d depth=%d",
		  process_.c_str(), type.c_str(),type.c_str(),ieta,iphi,depth);  
	  MonitorElement* me = mui_->get(name);
	  if(me){
	    meShapeRMS->Fill(me->getRMS());
	    meShapeMean->Fill(me->getMean());
	    if(me->getRMS()<rms_thresh_ || me->getMean()>mean_thresh_){
	      HcalSubdetector subdet = HcalBarrel;
	      if(i==1) subdet = HcalOuter;
	      else if(i==2) subdet = HcalForward;
	      else if(i==3) subdet = HcalEndcap;
	      
	      HcalDetId id(subdet,ieta,iphi,depth);
	      HcalElectronicsId eid = readoutMap_->lookup(id);
	      meGeoErr->Fill(ieta,iphi);
	      meElecErr->Fill(eid.readoutVMECrateId(),eid.htrSlot());	      
	    }
	  }
	  
	  sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Time ieta=%d iphi=%d depth=%d",process_.c_str(),
		  type.c_str(),type.c_str(),ieta,iphi,depth);  
	  me = mui_->get(name);
	  if(me){
	    meTimeRMS->Fill(me->getRMS());
	    meTimeMean->Fill(me->getMean());
	  }	  
	}
      }
    }
    
    rms_shape[i]=getHisto(meShapeRMS,verbose_,cloneME_);
    mean_shape[i]=getHisto(meShapeMean,verbose_,cloneME_);
    rms_time[i]=getHisto(meTimeRMS,verbose_,cloneME_);
    mean_time[i]=getHisto(meTimeMean,verbose_,cloneME_);
    err_map_geo[i]=getHisto2(meGeoErr,verbose_,cloneME_);
    err_map_elec[i]=getHisto2(meElecErr,verbose_,cloneME_);
  }

  return;
}

void HcalLEDClient::analyze(void){
  
  jevt_++;
  int updates = 0;
  if(mui_) mui_->getNumUpdates();
  if ( (updates % 10) == 0 ) {
    if ( verbose_ ) cout << "HcalLEDClient: " << updates << " updates" << endl;
  }
  
  return;
}

void HcalLEDClient::createTests(){
  if(!mui_) return;
  
  char meTitle[250], name[250];    
  vector<string> params;

  if(verbose_) printf("Creating LED tests...\n");
  
  for(int i=0; i<4; i++){
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HO";
    if(i==3) type = "HF";

    sprintf(meTitle,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape RMS Values",process_.c_str(),type.c_str(),type.c_str());
    sprintf(name,"%s LED Shape RMS Values: X-Range",type.c_str());
    if( dqmQtests_.find(name) == dqmQtests_.end() ){	
      MonitorElement* me = mui_->get(meTitle);
      if(me){
	dqmQtests_[name]=meTitle;	  
	params.clear();
	params.push_back(meTitle); params.push_back(name);  //hist and test titles
	params.push_back("0.75"); params.push_back("0.5");  //warn, err probs
	char low[20];
	sprintf(low,"%.2f\n",rms_thresh_);
	params.push_back(low); params.push_back("2");  //xmin, xmax
	createXRangeTest(mui_, params);
      }
    }
    
    sprintf(meTitle,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape Mean Values",process_.c_str(),type.c_str(),type.c_str());
    sprintf(name,"%s LED Shape RMS Values: X-Range",type.c_str());
    if( dqmQtests_.find(name) == dqmQtests_.end() ){	
      MonitorElement* me = mui_->get(meTitle);
      if(me){
	dqmQtests_[name]=meTitle;	  
	params.clear();
	params.push_back(meTitle); params.push_back(name);  //hist and test titles
	params.push_back("0.75"); params.push_back("0.5");  //warn, err probs
	char high[20];
	sprintf(high,"%.2f\n",mean_thresh_);
	params.push_back("0"); params.push_back(high);  //xmin, xmax
	createXRangeTest(mui_, params);
      }
    }    
  }

  return;
}

void HcalLEDClient::resetME(){
  if(!mui_) return;
  Char_t name[150];    
  MonitorElement* me;
  
  for(int i=0; i<4; i++){
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HO"; 
    if(i==3) type = "HF"; 
    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s Average Pulse Shape",process_.c_str(),type.c_str(),type.c_str());
    me = mui_->get(name);
    if(me) mui_->softReset(me);

    sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s Average Pulse Time",process_.c_str(),type.c_str(),type.c_str());
    me = mui_->get(name);
    if(me) mui_->softReset(me);
    
    for(int ieta=-42; ieta<42; ieta++){
      for(int iphi=0; iphi<72; iphi++){
	for(int depth=0; depth<4; depth++){
	  sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Shape ieta=%d iphi=%d depth=%d",
		  process_.c_str(), type.c_str(),type.c_str(),ieta,iphi,depth);  
	  me = mui_->get(name);
	  if(me) mui_->softReset(me);

	  sprintf(name,"%sHcalMonitor/LEDMonitor/%s/%s LED Time ieta=%d iphi=%d depth=%d",process_.c_str(),
		  type.c_str(),type.c_str(),ieta,iphi,depth);  
	  me = mui_->get(name);
	  if(me) mui_->softReset(me);
	}
      }
    }
  }
  return;
}

void HcalLEDClient::htmlOutput(int run, string htmlDir, string htmlName){
  
  cout << "Preparing HcalLEDClient html output ..." << endl;
  string client = "LEDMonitor";
  htmlErrors(htmlDir,client,process_,mui_,dqmReportMapErr_,dqmReportMapWarn_,dqmReportMapOther_);

  ofstream htmlFile;
  htmlFile.open((htmlDir + htmlName).c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << endl;
  htmlFile << "<html>  " << endl;
  htmlFile << "<head>  " << endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << endl;
  htmlFile << " http-equiv=\"content-type\">  " << endl;
  htmlFile << "  <title>Monitor: Hcal LED Task output</title> " << endl;
  htmlFile << "</head>  " << endl;
  htmlFile << "<style type=\"text/css\"> td { font-weight: bold } </style>" << endl;
  htmlFile << "<body>  " << endl;
  htmlFile << "<br>  " << endl;
  htmlFile << "<h2>Run:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << run << "</span></h2>" << endl;
  htmlFile << "<h2>Monitoring task:&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">Hcal LEDs</span></h2> " << endl;

  htmlFile << "<h2>Events processed:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << ievt_ << "</span></h2>" << endl;

  htmlFile << "<hr>" << endl;
  htmlFile << "<table border=1><tr>" << endl;
  if(hasErrors())htmlFile << "<td bgcolor=red><a href=\"LEDMonitorErrors.html\">Errors in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Errors</td>" << endl;
  if(hasWarnings()) htmlFile << "<td bgcolor=yellow><a href=\"LEDMonitorWarnings.html\">Warnings in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Warnings</td>" << endl;
  if(hasOther()) htmlFile << "<td bgcolor=aqua><a href=\"LEDMonitorMessages.html\">Messages in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Messages</td>" << endl;
  htmlFile << "</tr></table>" << endl;
  htmlFile << "<hr>" << endl;
  
  htmlFile << "<h2><strong>Hcal LED Histograms</strong></h2>" << endl;
   htmlFile << "<h3>" << endl;
  htmlFile << "<a href=\"#HB_Plots\">HB Plots </a></br>" << endl;
  htmlFile << "<a href=\"#HE_Plots\">HE Plots </a></br>" << endl;
  htmlFile << "<a href=\"#HO_Plots\">HO Plots </a></br>" << endl;
  htmlFile << "<a href=\"#HF_Plots\">HF Plots </a></br>" << endl;
  htmlFile << "</h3>" << endl;
  htmlFile << "<hr>" << endl;

  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;

   for(int i=0; i<4; i++){
     string type = "HB";
     if(i==1) type = "HE"; 
     if(i==2) type = "HO"; 
     if(i==3) type = "HF"; 
     
     htmlFile << "<tr align=\"left\">" << endl;  
     htmlFile << "<td>&nbsp;&nbsp;&nbsp;<a name=\""<<type<<"_Plots\"><h3>" << type << " Histograms</h3></td></tr>" << endl;
     
     htmlFile << "<tr align=\"left\">" << endl;
     histoHTML2(err_map_geo[i],"iEta","iPhi", 92, htmlFile,htmlDir);
     histoHTML2(err_map_elec[i],"VME Crate ID","HTR Slot", 100, htmlFile,htmlDir);
     htmlFile << "</tr>" << endl;
     
     htmlFile << "<tr align=\"left\">" << endl;
     histoHTML(avg_shape[i],"Average Pulse Shape","Events", 92, htmlFile,htmlDir);
     histoHTML(avg_time[i],"Average Pulse Time","Events", 100, htmlFile,htmlDir);
     htmlFile << "</tr>" << endl;

     htmlFile << "<tr align=\"left\">" << endl;
     histoHTML(rms_shape[i],"Shape RMS Value","Events", 92, htmlFile,htmlDir);
     histoHTML(mean_shape[i],"Shape Mean Value","Events", 100, htmlFile,htmlDir);
     htmlFile << "</tr>" << endl;

     htmlFile << "<tr align=\"left\">" << endl;
     histoHTML(rms_time[i],"Time RMS Value","Events", 92, htmlFile,htmlDir);
     histoHTML(mean_time[i],"Time Mean Value","Events", 100, htmlFile,htmlDir);
     htmlFile << "</tr>" << endl;
   }

  htmlFile << "</table>" << endl;
  htmlFile << "<br>" << endl;

  // html page footer
  htmlFile << "</body> " << endl;
  htmlFile << "</html> " << endl;

  htmlFile.close();

  return;
}


void HcalLEDClient::loadHistograms(TFile* infile){

  TNamed* tnd = (TNamed*)infile->Get("DQMData/HcalMonitor/LEDMonitor/LED Task Event Number");
  if(tnd){
    string s =tnd->GetTitle();
    ievt_ = -1;
    sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
  }
  char name[256];
  for(int i=0; i<4; i++){
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HO"; 
    if(i==3) type = "HF";


    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s Average Pulse Shape",type.c_str(),type.c_str());      
    avg_shape[i] = (TH1F*)infile->Get(name);
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s Average Pulse Time",type.c_str(),type.c_str());      
    avg_time[i] = (TH1F*)infile->Get(name);
    
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Shape RMS Values",type.c_str(),type.c_str());
    rms_shape[i]=(TH1F*)infile->Get(name);
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Shape Mean Values",type.c_str(),type.c_str());
    mean_shape[i]=(TH1F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Time RMS Values",type.c_str(),type.c_str());
    rms_time[i]=(TH1F*)infile->Get(name);
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Time Mean Values",type.c_str(),type.c_str());
    mean_time[i]=(TH1F*)infile->Get(name);
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Geo Error Map",type.c_str(),type.c_str());
    err_map_geo[i]=(TH2F*)infile->Get(name);
    sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Elec Error Map",type.c_str(),type.c_str());
    err_map_elec[i]=(TH2F*)infile->Get(name);


    for(int ieta=-etaMax[i]; ieta<=etaMax[i]; ieta++){
      if(abs(ieta)<etaMin[i]) continue;
      for(int iphi=phiMin[i]; iphi<=phiMax[i]; iphi++){
	for(int depth=depMin[i]; depth<=depMax[i]; depth++){
	  sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Shape ieta=%d iphi=%d depth=%d",
		  type.c_str(),type.c_str(),ieta,iphi,depth);  
	  TH1F* h = (TH1F*)infile->Get(name);
	  if(h){
	    rms_shape[i]->Fill(h->GetRMS());
	    mean_shape[i]->Fill(h->GetMean());
	  }
	  
	  sprintf(name,"DQMData/HcalMonitor/LEDMonitor/%s/%s LED Time ieta=%d iphi=%d depth=%d",
		  type.c_str(),type.c_str(),ieta,iphi,depth);  
	  h = (TH1F*)infile->Get(name);
	  if(h){
	    rms_time[i]->Fill(h->GetRMS());
	    mean_time[i]->Fill(h->GetMean());
	  }	  
	}
      }
    }


  }

  return;
}
