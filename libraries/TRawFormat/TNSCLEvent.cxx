#include "TNSCLEvent.h"

#include <cassert>

Int_t TNSCLEvent::GetBodyHeaderSize() const {
  Int_t output = *((Int_t*)(GetBody()+0));
  if(output == 0) {
    return 4;  // If only the body header size is present, it is listed as 0.
  } else {
    return output;
  }
}

long TNSCLEvent::GetTimestamp() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Long_t*)(GetBody()+4));
  } else {
    return -1;
  }
}

Int_t TNSCLEvent::GetSourceID() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Int_t*)(GetBody()+12));
  } else {
    return -1;
  }
}

Int_t TNSCLEvent::GetBarrierType() const {
  if(GetBodyHeaderSize() > 4) {
    return *((Int_t*)(GetBody()+16));
  } else {
    return -1;
  }
}

int TNSCLEvent::IsBuiltData() const {
  static int is_built_data = -1;
  if(is_built_data != -1) {
    return is_built_data;
  }

  unsigned int sourceid = *((unsigned int*)(GetPayload() + sizeof(Long_t)));
  unsigned int barrier  = *((unsigned int*)(GetPayload() + sizeof(Long_t) + 2*sizeof(Int_t)));

  is_built_data = ((sourceid < 10) &&
                   (barrier == 0));

  return is_built_data;
}

const char* TNSCLEvent::GetPayload() const {
  return (GetBody() + GetBodyHeaderSize());
}

TSmartBuffer TNSCLEvent::GetPayloadBuffer() const {
  return fBody.BufferSubset(GetBodyHeaderSize());
}

Int_t TNSCLEvent::GetPayloadSize() const {
  return (GetBodySize()-GetBodyHeaderSize());
}

// Seconds since the previous scaler read
Int_t TNSCLScaler::GetIntervalStartOffset(){
  return *(Int_t*)(GetPayload() + 0);
}

// Seconds since beginning of run
Int_t TNSCLScaler::GetIntervalEndOffset(){
  return *(Int_t*)(GetPayload() + 4);
}

// Time when writing to disk
time_t TNSCLScaler::GetUnixTime(){
  return *(Int_t*)(GetPayload() + 8);
}

// Interval (seconds) between each scaler packet
Int_t TNSCLScaler::GetIntervalDivisor(){
  return *(Int_t*)(GetPayload() + 12);
}

// Number of integers to follow.
Int_t TNSCLScaler::GetScalerCount(){
  return *(Int_t*)(GetPayload() + 16);
}

// Are the scalers reset after each read
Int_t TNSCLScaler::IsIncremental(){
  return *(Int_t*)(GetPayload() + 20);
}

Int_t TNSCLScaler::GetScalerValue(size_t scaler_num){
  assert(scaler_num < GetScalerCount());

  return *(Int_t*)(GetPayload() + 24 + 4*scaler_num);
}

TNSCLFragment::TNSCLFragment(TSmartBuffer& buf){
  memcpy((char*)&fNSCLFragmentHeader, buf.GetData(), sizeof(fNSCLFragmentHeader));
  buf = buf.BufferSubset(sizeof(fNSCLFragmentHeader));

  memcpy((char*)fNSCLEvent.GetRawHeader(), buf.GetData(), sizeof(TRawEvent::RawHeader));
  buf = buf.BufferSubset(sizeof(TRawEvent::RawHeader));

  fNSCLEvent.SetFileType(kFileType::NSCL_EVT);

  fNSCLEvent.SetData(buf.BufferSubset(0,fNSCLEvent.GetBodySize()));
  buf = buf.BufferSubset(fNSCLEvent.GetBodySize());
}


Long_t       TNSCLFragment::GetFragmentTimestamp()     const{
  return fNSCLFragmentHeader.timestamp;
}

Int_t        TNSCLFragment::GetFragmentSourceID()      const{
  return fNSCLFragmentHeader.sourceid;
}

Int_t        TNSCLFragment::GetFragmentPayloadSize()   const{
  return fNSCLFragmentHeader.payload_size;
}

Int_t        TNSCLFragment::GetFragmentBarrier()       const{
  return fNSCLFragmentHeader.barrier;
}

TNSCLBuiltRingItem::TNSCLBuiltRingItem(TNSCLEvent& event)
  : fEvent(event) {
  assert(kNSCLEventType(event.GetEventType()) == kNSCLEventType::PHYSICS_EVENT);
}

TNSCLFragment& TNSCLBuiltRingItem::GetFragment(size_t fragnum) {
  BuildFragments();
  return fragments.at(fragnum);
}

Int_t TNSCLBuiltRingItem::Size() const {
  BuildFragments();
  return fragments.size();
}

size_t TNSCLBuiltRingItem::NumFragments() const {
  BuildFragments();
  return fragments.size();
}

Int_t TNSCLBuiltRingItem::GetBuiltRingItemSize() const {
  return *(Int_t*)fEvent.GetPayloadBuffer().GetData();
}

void TNSCLBuiltRingItem::BuildFragments() const {
  if(fragments.size()){
    return;
  }

  // Skip past the size of the fragments
  TSmartBuffer buf = fEvent.GetPayloadBuffer().BufferSubset(sizeof(Int_t));

  // Loop through, extracting each fragment
  while(buf.GetSize()){
    // TNSCLFragment constructor advances the buffer
    fragments.emplace_back(buf);
  }
}
