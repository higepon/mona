// LZMADecoder.cpp

#include "StdAfx.h"

#include "LZMADecoder.h"
#include "../../../Common/Defs.h"

namespace NCompress {
namespace NLZMA {

HRESULT CDecoder::Init(ISequentialInStream *inStream,
    ISequentialOutStream *outStream)
{
  _rangeDecoder.Init(inStream);
  _outWindowStream.Init(outStream);
  { 
  for(int i = 0; i < kNumStates; i++)
  {
    for (UInt32 j = 0; j <= _posStateMask; j++)
    {
      _isMatch[i][j].Init();
      _isRep0Long[i][j].Init();
    }
    _isRep[i].Init();
    _isRepG0[i].Init();
    _isRepG1[i].Init();
    _isRepG2[i].Init();
  }
  }
  { 
    for (UInt32 i = 0; i < kNumLenToPosStates; i++)
    _posSlotDecoder[i].Init();
  }
  { 
    for(UInt32 i = 0; i < kNumFullDistances - kEndPosModelIndex; i++)
      _posDecoders[i].Init();
  }
  _posAlignDecoder.Init();
  _lenDecoder.Init(_posStateMask + 1);
  _repMatchLenDecoder.Init(_posStateMask + 1);
  _literalDecoder.Init();
  return S_OK;
}

STDMETHODIMP CDecoder::CodeReal(ISequentialInStream *inStream,
    ISequentialOutStream *outStream, 
    const UInt64 *, const UInt64 *outSize,
    ICompressProgressInfo *progress)
{
  Init(inStream, outStream);
  CDecoderFlusher flusher(this);

  CState state;
  state.Init();
  bool peviousIsMatch = false;
  Byte previousByte = 0;
  UInt32 rep0 = 0, rep1 = 0, rep2 = 0, rep3 = 0;

  UInt64 nowPos64 = 0;
  UInt64 size = (outSize == NULL) ? (UInt64)(Int64)(-1) : *outSize;
  while(nowPos64 < size)
  {
    UInt64 nextPos = MyMin(nowPos64 + (1 << 18), size);
    while(nowPos64 < nextPos)
    {
      #ifdef _NO_EXCEPTIONS
      if (_rangeDecoder.Stream.ErrorCode != S_OK)
        return _rangeDecoder.Stream.ErrorCode;
      #endif
      if (_rangeDecoder.Stream.WasFinished())
        return S_FALSE;
      UInt32 posState = UInt32(nowPos64) & _posStateMask;
      if (_isMatch[state.Index][posState].Decode(&_rangeDecoder) == 0)
      {
        state.UpdateChar();
        if(peviousIsMatch)
        {
          Byte matchByte = _outWindowStream.GetByte(rep0);
          previousByte = _literalDecoder.DecodeWithMatchByte(&_rangeDecoder, 
              UInt32(nowPos64), previousByte, matchByte);
          peviousIsMatch = false;
        }
        else
          previousByte = _literalDecoder.DecodeNormal(&_rangeDecoder, 
              UInt32(nowPos64), previousByte);
        _outWindowStream.PutByte(previousByte);
        nowPos64++;
      }
      else             
      {
        peviousIsMatch = true;
        UInt32 len;
        if(_isRep[state.Index].Decode(&_rangeDecoder) == 1)
        {
          if(_isRepG0[state.Index].Decode(&_rangeDecoder) == 0)
          {
            if(_isRep0Long[state.Index][posState].Decode(&_rangeDecoder) == 0)
            {
              if (nowPos64 == 0)
                return S_FALSE;
              state.UpdateShortRep();
              previousByte = _outWindowStream.GetByte(rep0);
              _outWindowStream.PutByte(previousByte);
              nowPos64++;
              continue;
            }
          }
          else
          {
            UInt32 distance;
            if(_isRepG1[state.Index].Decode(&_rangeDecoder) == 0)
              distance = rep1;
            else 
            {
              if (_isRepG2[state.Index].Decode(&_rangeDecoder) == 0)
                distance = rep2;
              else
              {
                distance = rep3;
                rep3 = rep2;
              }
              rep2 = rep1;
            }
            rep1 = rep0;
            rep0 = distance;
          }
          len = _repMatchLenDecoder.Decode(&_rangeDecoder, posState) + kMatchMinLen;
          state.UpdateRep();
        }
        else
        {
          rep3 = rep2;
          rep2 = rep1;
          rep1 = rep0;
          len = kMatchMinLen + _lenDecoder.Decode(&_rangeDecoder, posState);
          state.UpdateMatch();
          UInt32 posSlot = _posSlotDecoder[GetLenToPosState(len)].Decode(&_rangeDecoder);
          if (posSlot >= kStartPosModelIndex)
          {
            UInt32 numDirectBits = (posSlot >> 1) - 1;
            rep0 = ((2 | (posSlot & 1)) << numDirectBits);

            if (posSlot < kEndPosModelIndex)
              rep0 += NRangeCoder::ReverseBitTreeDecode(_posDecoders + 
                  rep0 - posSlot - 1, &_rangeDecoder, numDirectBits);
            else
            {
              rep0 += (_rangeDecoder.DecodeDirectBits(
                  numDirectBits - kNumAlignBits) << kNumAlignBits);
              rep0 += _posAlignDecoder.ReverseDecode(&_rangeDecoder);
            }
          }
          else
            rep0 = posSlot;
        }
        if (rep0 >= nowPos64 || rep0 >= _dictionarySizeCheck)
        {
          if (rep0 == (UInt32)(-1) && size == (UInt64)(Int64)(-1))
          {
            flusher.NeedFlush = false;
            return Flush();
          }
          return S_FALSE;
        }
        _outWindowStream.CopyBlock(rep0, len);
        nowPos64 += len;
        previousByte = _outWindowStream.GetByte(0);
        #ifdef _NO_EXCEPTIONS
        if (_outWindowStream.ErrorCode != S_OK)
          return _outWindowStream.ErrorCode;
        #endif
      }
    }
    if (progress != NULL)
    {
      UInt64 inSize = _rangeDecoder.GetProcessedSize();
      RINOK(progress->SetRatioInfo(&inSize, &nowPos64));
    }
  }
  flusher.NeedFlush = false;
  return Flush();
}

STDMETHODIMP CDecoder::Code(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress)
{
  #ifndef _NO_EXCEPTIONS
  try 
  { 
  #endif
    return CodeReal(inStream, outStream, inSize, outSize, progress); 
  #ifndef _NO_EXCEPTIONS
  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(const CLZOutWindowException &e)  { return e.ErrorCode; }
  catch(...) { return S_FALSE; }
  #endif
}

STDMETHODIMP CDecoder::SetDecoderProperties(ISequentialInStream *inStream)
{
  UInt32 processesedSize;
  Byte properties[5];
  RINOK(inStream->Read(properties, sizeof(properties), &processesedSize));
  if (processesedSize != sizeof(properties))
    return E_INVALIDARG;
  int lc = properties[0] % 9;
  Byte remainder = (Byte)(properties[0] / 9);
  int lp = remainder % 5;
  int pb = remainder / 5;
  if (pb > NLength::kNumPosStatesBitsMax)
    return E_INVALIDARG;
  _posStateMask = (1 << pb) - 1;
  UInt32 dictionarySize = 0;
  for (int i = 0; i < 4; i++)
    dictionarySize += ((UInt32)(properties[1 + i])) << (i * 8);
  _dictionarySizeCheck = MyMax(dictionarySize, UInt32(1));
  UInt32 blockSize = MyMax(_dictionarySizeCheck, UInt32(1 << 12));
  if (!_outWindowStream.Create(blockSize))
    return E_OUTOFMEMORY;
  if (!_literalDecoder.Create(lp, lc))
    return E_OUTOFMEMORY;
  if (!_rangeDecoder.Create(1 << 20))
    return E_OUTOFMEMORY;
  return S_OK;
}

STDMETHODIMP CDecoder::GetInStreamProcessedSize(UInt64 *value)
{
  *value = _rangeDecoder.GetProcessedSize();
  return S_OK;
}

}}
