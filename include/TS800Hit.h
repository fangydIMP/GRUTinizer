#ifndef TSEIGHTHUNDRADHIT_H
#define TSEIGHTHUNDRADHIT_H

#include <TObject.h>
#include <TClass.h>
#include <iostream>

#include <TDetectorHit.h>

#define MAXCRDC 513
#define CRDC_XSlope 2.54
#define CRDC_XOffset -281.94
#define CRDC_YSlope 2.54
#define CRDC_YOffset -281.94

class TS800Channel : public TDetectorHit {
  public:
    TS800Channel()                     { Clear();         }
    TS800Channel(short value)          { Set(value); }
    TS800Channel(unsigned short value) { Set(value); }
    ~TS800Channel()                    {}

    void Set(short value)          { fValue = (unsigned short)value; }
    void Set(unsigned short value) { fValue = value; }

    virtual short GetId()      const { return (((fValue)&0xf000)>>12);  }
    short GetValue()   const { return fValue&0x0fff;  }

    virtual void Clear(Option_t *opt="")       { TDetectorHit::Clear(opt); fValue = 0; }
    virtual void Print(Option_t *opt="") const { printf("[%i] = %i\n",GetId(),GetValue());}
    virtual void Copy(TObject &obj)      const { TDetectorHit::Copy(obj); ((TS800Channel&)obj).fValue = fValue; }

    virtual int  Charge() const { return GetValue(); }

  private:
    //void Init() { Class()->IgnoreTObjectStreamer(true); }
    short fValue;

  ClassDef(TS800Channel,1);
};

class TTrigger : public TDetectorHit {
  public:
    TTrigger();
    ~TTrigger();

    void SetRegistr(unsigned short reg) { fregistr=reg; }
    void SetS800Source(short sou)       { fs800source=sou; }
    void SetExternalSource1(short sou)  { fexternalsource1=sou; }
    void SetExternalSource2(short sou)  { fexternalsource2=sou; }
    void SetSecondarySource(short sou)  { fsecondarysource=sou; }

    unsigned short GetRegistr() { return fregistr; }
    short GetS800Source()       { return fs800source; }
    short GetExternalSource1()  { return fexternalsource1; }
    short GetExternalSource2()  { return fexternalsource2; }
    short GetSecondarySource()  { return fsecondarysource; }

    virtual void Copy(TObject &)         const;
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

  private:
    virtual int Charge() const { return 0; }
        
    unsigned short fregistr;
    short fs800source;
    short fexternalsource1;
    short fexternalsource2;
    short fsecondarysource;

  ClassDef(TTrigger,1)
};


class TTof : public TDetectorHit { // S800 Time of Flight
  public:
    TTof();
    ~TTof();

    void SetRF(short rf)                { frf=rf; }
    void SetOBJ(short obj)              { fobj=obj; }
    void SetXFP(short xfp)              { fxfp=xfp; }
    void SetSI(short si)                { fsi=si; }
    void SetTacOBJ(short obj)           { ftac_obj=obj; }
    void SetTacXFP(short xfp)           { ftac_xfp=xfp; }

    short GetRF()                         { return frf;}
    short GetOBJ()                        { return fobj;}
    short GetXFP()                        { return fxfp;}
    short GetSI()                         { return fsi;}
    short GetTacOBJ()                     { return ftac_obj;}
    short GetTacXFP()                     { return ftac_xfp;}
    

    virtual void Copy(TObject &)         const;
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

  private:
    virtual int Charge() const { return 0; }
        
    short frf;
    short fobj;
    short fxfp;
    short fsi;
    short ftac_obj;
    short ftac_xfp;

  ClassDef(TTof,1);
};

class TCrdc : public TDetectorHit {
  public:
    TCrdc();
    ~TCrdc();

    short GetId()    { return fId;   }
    short GetAnode() { return anode; }
    short GetTime()  { return time;  }

    int  Size()        { return sample.size(); }
    int  GetNSamples() { return sample.size(); }

    void SetId(short id)    { fId = id;  }
    void SetAnode(short an) {anode = an; }
    void SetTime(short ti)  {time = ti;  }

    void AddPoint(int chan,int samp,int dat) { channel.push_back(chan);  
                                               sample.push_back(samp);   
                                               data.push_back(dat);    } 
    int GetChannel(int i)    { if(i>=Size()) return -1; return channel.at(i);    } 
    int GetSample(int i)     { if(i>=Size()) return -1; return sample.at(i);     } 
    int GetData(int i)       { if(i>=Size()) return -1; return data.at(i);       } 

    int GetWidth();

    float GetDispersiveX()      { return (GetPad()*CRDC_XSlope+CRDC_XOffset); }
    float GetNonDispersiveY()   { return (GetPad()*CRDC_YSlope+CRDC_YOffset); }
    

    float GetPad();

    void LoadInverseMap() {}

    virtual void Copy(TObject&) const;
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

  private:
    virtual int Charge() const { return 0; }
    short fId;
    std::vector<int> channel;
    std::vector<int> sample;
    std::vector<int> data;

    unsigned short anode;
    unsigned short time;

  ClassDef(TCrdc,1)
};


class TScintillator : public TDetectorHit {
  public:
    TScintillator();
    ~TScintillator();

    void SetID(int id)         { fID=id; }
    void SetdE_Up(float de)    { fdE_up = de; }
    void SetdE_Down(float de)  { fdE_down = de; }
    void SetTime_Up(float t)   { fTime_up = t; }
    void SetTime_Down(float t) { fTime_down = t; }

    int GetID()          { return fID; }
    float GetdE_Up()     { return fdE_up; }
    float GetdE_Down()   { return fdE_down; }
    float GetTime_Up()   { return fTime_up; }
    float GetTime_Down() { return fTime_down; }

    virtual void Copy(TObject&) const;
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

  private:
    virtual int Charge() const { return 0; }
    int fID;
    float fdE_up;
    float fdE_down;
    float fTime_up;
    float fTime_down;
    
    ClassDef(TScintillator,1)
};

class TIonChamber : public TDetectorHit {
  public:
    TIonChamber();
    ~TIonChamber();
    
    void Set(int ch, int data); // { fChan.push_back(ch); fData.push_back(data); }

    int GetChannel(int i) const { if(i>=Size()) return -1; return fChan.at(i); }
    int GetData(int i)    const { if(i>=Size()) return -1; return fData.at(i); }
    int Size() const { return fChan.size(); }
    float GetdE(); 
    
    virtual void Copy(TObject&) const;
    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");

  private:
    virtual int Charge() const { return 0; }
    
    std::vector<int> fChan;
    std::vector<int> fData;
    //float            fdE;   //!          Average of all fData.
    ClassDef(TIonChamber,1)
};






// ----------- ?? OLD ?? -----------


class TTOFHit :  public TS800Channel {
  public:
    TTOFHit()  {  }
    TTOFHit(const TTOFHit &tof)  { tof.Copy(*this); }
    TTOFHit(short value) : TS800Channel(value)          {  }
    TTOFHit(unsigned short value) : TS800Channel(value) {  }
    //TTOFHit(char *data,int size);
    ~TTOFHit() {  }

    bool IsRF_tdc( ) {return GetId() == 0x000c;}
    bool IsOBJ_tdc() {return GetId() == 0x000d;}
    bool IsOBJ_tac() {return GetId() == 0x0005;}
    bool IsXFP_tac() {return GetId() == 0x000e;}
    bool IsXFP_tdc() {return GetId() == 0x0004;}
    bool IsSI_tdc()  {return GetId() == 0x000f;}

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt);              }
    virtual void Print(Option_t *opt="") const { printf("TOF");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);             }

  ClassDef(TTOFHit,1);
};


class THodoHit : public TS800Channel {
  public:
    THodoHit() { Clear(); }
    THodoHit(short chan,short value) { SetChannel(chan); Set(value); }
    ~THodoHit() { };

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt); fChannel=-1; }
    virtual void Print(Option_t *opt="") const { printf("HODO"); }
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj); ((THodoHit&)obj).fChannel = fChannel; }

    void SetChannel(short chan) { fChannel = chan;  }
    short GetId()      const { return GetChannel(); }
    short GetChannel() const { return fChannel;     }

  private:
    short fChannel;


  ClassDef(THodoHit,1)
};


class TFPScint :  public TS800Channel {
  public:
    TFPScint()  {  }
    TFPScint(const TFPScint &fpscint)  { fpscint.Copy(*this); }
    TFPScint(short charge,short time):TS800Channel(charge),fTime((unsigned short)time) {  }
    TFPScint(unsigned short charge,unsigned short time):TS800Channel(charge),fTime(time) {  }
    //TFPScint(char *data,int size);
    ~TFPScint() {  }

    void SetCharge(short chg) { Set(chg);     }
    void SetTime(short time)  { fTime = time; }



    bool IsE1up()   {return GetId() == 0x0000;}
    bool IsE1down() {return GetId() == 0x0001;}
    bool IsE2up()   {return GetId() == 0x0002;}
    bool IsE2down() {return GetId() == 0x0003;}
    bool IsE3up()   {return GetId() == 0x0004;}
    bool IsE3down() {return GetId() == 0x0005;}

    short GetTime() const { return fTime&0x0fff; }

    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt); fTime = 0;   }
    virtual void Print(Option_t *opt="") const { printf("FPScint");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);  }
  private:

   unsigned short fTime;
  ClassDef(TFPScint,1);
};

/*
class TIonChamber : public TS800Channel {
  public:
    TIonChamber() { }
    TIonChamber(const TIonChamber &ion) { ion.Copy(*this); }
    TIonChamber(short value):TS800Channel(value) { }
   
    virtual void Clear(Option_t *opt="")       { TS800Channel::Clear(opt);    }
    virtual void Print(Option_t *opt="") const { printf("Ion Chamber");TS800Channel::Print(opt);}
    virtual void Copy(TObject &obj)      const { TS800Channel::Copy(obj);  }
  private:
    ClassDef(TIonChamber,1)

};
*/

class TCrdcPad : public TDetectorHit {
 
  public:
    TCrdcPad(); 
    TCrdcPad(int chan); 
    ~TCrdcPad();
    
    virtual Int_t Charge() const; 

    
    void SetChannel(short chan)         { fChannel = chan; }
    void SetPoint(short sample,short value) { 
      if(fNumSamples >= MAXCRDC){
        printf("fNumSamples = %i\n",fNumSamples); fflush(stdout); 
      }
      fSample[fNumSamples] = sample; 
      fValue[fNumSamples] = value; 
      fNumSamples++;  

    }
    //int  GetPoint(int sample)           { if(fTrace.count(sample)) return fTrace.at(sample); else return 0; }  
    short GetChannel()                  { return fChannel; }

    virtual void Clear(Option_t *opt="");       
    virtual void Print(Option_t *opt="") const; 
    virtual void Copy(TObject &obj)      const; 

  private:
    short fChannel;
    short fNumSamples;
    short fSample[MAXCRDC];
    short fValue[MAXCRDC];
    //std::map <int,int> fTrace;
  


  ClassDef(TCrdcPad,1)
};


class TS800Hit : public TDetectorHit {
  public:
    TS800Hit()   {  }
    ~TS800Hit()  {  }
    virtual Int_t Charge() const { return 0; }

    virtual void Clear(Option_t *opt ="")       { TDetectorHit::Clear(opt); }
    virtual void Print(Option_t *opt ="") const {  }
    virtual void Copy(TObject &obj)       const { TDetectorHit::Copy(obj);  }

  private:

  ClassDef(TS800Hit,1);
};

#endif
