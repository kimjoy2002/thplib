/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE DShowTool SOURCE CODE.                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE DShowTool SOURCE CODE IS (C) COPYRIGHT 2008 Cocha            *
 * http://sourceforge.jp/projects/directshow/                       *
 *                                                                  *
 ********************************************************************/

#include <streams.h>
#include "DShowTool.h"

#define SAFE_RELEASE(p) { if((p) != NULL) { (p)->Release(); (p)=NULL; } }

// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： Connect
// 説明　：	pFilter1の出力ピンとpFilter2の入力ピンを繋ぎます。(出力ピン、入力ピンは1個しか無いとは限らないので総当りで接続する)
// 戻り値： S_OK 成功。
//          E_POINTER 引数が不正です。
//　　　　　E_FAIL ピンの取得に失敗した。
//　　　　　VFW_S_PARTIAL_RENDER このムービーにサポートされないフォーマットのストリームが含まれている。
//　　　　　E_ABORT 操作が中止された。 
//　　　　　E_POINTER NULL ポインタ引数。 
//　　　　　VFW_E_CANNOT_CONNECT 接続を確立する中間フィルタの組み合わせが見つからなかった。 
//　　　　　VFW_E_NOT_IN_GRAPH フィルタ グラフに存在しないオブジェクトに要求された関数を実行できない。 
// 補足　： VFW_S_PARTIAL_RENDERは、例えば映像ファイルで映像デコードフィルタが無いけど音声デコードフィルタがある場合に
//          ソースフィルタと音声レンダリングフィルタを繋いだ場合になる。(ピン接続自体は成功)
// ----------------------------------------------------------------------------------------------------------------------------
HRESULT CDShowTool::Connect(IGraphBuilder *pGraphBuilder, IBaseFilter *pFilter1, IBaseFilter *pFilter2)
{
   CheckPointer(pGraphBuilder, E_POINTER);
   CheckPointer(pFilter1, E_POINTER);
   CheckPointer(pFilter2, E_POINTER);


   HRESULT hr;
   IPin *pInPin = NULL;
   IPin *pOutPin = NULL;
   int nInputPinCount, nOutputPinCount;

   // pFilter1の出力ピンのピン数を取得する
   nOutputPinCount = this->GetPinCount(pFilter1, PINDIR_OUTPUT);
   if(nOutputPinCount == 0) return E_FAIL;

   // pFilter2の入力ピンのピン数を取得する
   nInputPinCount  = this->GetPinCount(pFilter2, PINDIR_INPUT);
   if(nInputPinCount == 0) return E_FAIL;

   for(int i=0;i<nOutputPinCount;i++)
   {
      for(int j=0;j<nInputPinCount;j++)
      {
         pOutPin = this->GetPin(pFilter1, PINDIR_OUTPUT, i);
         if(pOutPin == NULL) return E_FAIL;

         pInPin = this->GetPin(pFilter2, PINDIR_INPUT, j);
         if(pInPin == NULL)
         {
            SAFE_RELEASE(pOutPin);
            return E_FAIL;;
         }

         hr = pGraphBuilder->Connect(pOutPin, pInPin);

         SAFE_RELEASE(pInPin);
         SAFE_RELEASE(pOutPin);

         if (hr == S_OK)
           return S_OK;

         if (hr == VFW_S_PARTIAL_RENDER)
           return VFW_S_PARTIAL_RENDER;
      }
   }

   SAFE_RELEASE(pInPin);
   SAFE_RELEASE(pOutPin);

   return VFW_E_CANNOT_CONNECT;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： ConnectDirect
// 説明　：	pFilter1の出力ピンとpFilter2の入力ピンをpAmMediaTypeのメディアタイプで直接繋ぎます。pAmMediaTypeはNULL指定可能です。
//          (出力ピン、入力ピンは1個しか無いとは限らないので総当りで接続する)
// 戻り値： S_OK 成功。
//          E_POINTER 引数が不正です。
//　　　　　VFW_E_ALREADY_CONNECTED ピンは既に接続されている。 
//　　　　　VFW_E_NO_ACCEPTABLE_TYPES 許可されたメディア タイプを見つけることができなかった。 
//　　　　　VFW_E_NOT_STOPPED フィルタがアクティブで、ピンが動的再接続をサポートしていない。 
//　　　　　VFW_E_TYPE_NOT_ACCEPTED 指定したメディア タイプは許可されない。 
// ----------------------------------------------------------------------------------------------------------------------------
HRESULT CDShowTool::ConnectDirect(IGraphBuilder *pGraphBuilder, IBaseFilter *pFilter1, IBaseFilter *pFilter2, AM_MEDIA_TYPE *pAmMediaType)
{
   CheckPointer(pGraphBuilder, E_POINTER);
   CheckPointer(pFilter1, E_POINTER);
   CheckPointer(pFilter2, E_POINTER);


   HRESULT hr;
   int nInputPinCount, nOutputPinCount;
   IPin *pInPin = NULL;
   IPin *pOutPin = NULL;

   // pFilter1の出力ピンのピン数を取得する
   nOutputPinCount = this->GetPinCount(pFilter1, PINDIR_OUTPUT);
   if(nOutputPinCount == 0) return E_FAIL;

   // pFilter2の入力ピンのピン数を取得する
   nInputPinCount  = this->GetPinCount(pFilter2, PINDIR_INPUT);
   if(nInputPinCount == 0) return E_FAIL;

   for(int i=0;i<nOutputPinCount;i++)
   {
      for(int j=0;j<nInputPinCount;j++)
      {
         pOutPin = this->GetPin(pFilter1, PINDIR_OUTPUT, i);
         if(pOutPin == NULL) return E_FAIL;

         pInPin = this->GetPin(pFilter2, PINDIR_INPUT, j);
         if(pInPin == NULL)
         {
            SAFE_RELEASE(pOutPin);
            return E_FAIL;
         }

         hr = pGraphBuilder->ConnectDirect(pOutPin, pInPin, pAmMediaType);

         SAFE_RELEASE(pInPin);
         SAFE_RELEASE(pOutPin);

         if (hr == S_OK)
           return S_OK;
      }
   }

   SAFE_RELEASE(pInPin);
   SAFE_RELEASE(pOutPin);

   return VFW_E_CANNOT_CONNECT;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： GetPinCount
// 説明　：	pFilterのpinDirectionの向きのピンの数を返します
// 戻り値： ピンの数。
// ----------------------------------------------------------------------------------------------------------------------------
int CDShowTool::GetPinCount(IBaseFilter *pFilter, PIN_DIRECTION pinDirection)
{
   CheckPointer(pFilter, 0);

   HRESULT hr;
   int nPinCount;
   IEnumPins *pEnumPins = NULL;
   IPin *pPin = NULL;

   nPinCount = 0;

   hr = pFilter->EnumPins(&pEnumPins);
   if (hr != NO_ERROR) return 0;

   hr = pEnumPins->Reset();

   while(pEnumPins->Next(1, &pPin, 0) == S_OK)
   {
      PIN_DIRECTION dir;
      pPin->QueryDirection(&dir);

      if (dir == pinDirection)
         nPinCount++;

      SAFE_RELEASE(pPin);
   }

   SAFE_RELEASE(pPin);
   SAFE_RELEASE(pEnumPins);

   return nPinCount;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： GetPin
// 説明　：	pFilterのpinDirectionの向きのnPinNumber番目のピンのポインタを返します(nPinNumberは0から始まる)
// 戻り値： ピンのポインタ。ピンの取得に失敗した場合はNULLを返す
// ----------------------------------------------------------------------------------------------------------------------------
IPin* CDShowTool::GetPin(IBaseFilter *pFilter, PIN_DIRECTION pinDirection, int nPinNumber)
{
   CheckPointer(pFilter, NULL);

   int hr, count;
   IEnumPins *pEnumPins = NULL;
   IPin *pPin = NULL;

   count = 0;

   hr = pFilter->EnumPins(&pEnumPins);
   if (hr != NO_ERROR) return NULL;

   pEnumPins->Reset();

   while(pEnumPins->Next(1, &pPin, 0) == S_OK)
   {
      PIN_DIRECTION dir;
      pPin->QueryDirection(&dir);

      if (dir == pinDirection)
      {
         if(count == nPinNumber)
         {
            SAFE_RELEASE(pEnumPins);
            return pPin;
         }

         count++;
      }

      SAFE_RELEASE(pPin);
   }

   SAFE_RELEASE(pPin);
   SAFE_RELEASE(pEnumPins);

   return NULL;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： Disconnect
// 説明　：	pFilter1とpFilter2のピン接続を解除します
// 戻り値： S_OK 成功。
//          E_POINTER 引数が不正です。
//       　 S_FALSE ピンが接続されていない。
//        　VFW_E_NOT_STOPPED フィルタがアクティブで、ピンは動的再接続をサポートしていない 
// ----------------------------------------------------------------------------------------------------------------------------
HRESULT CDShowTool::Disconnect(IGraphBuilder *pGraphBuilder, IBaseFilter *pFilter1, IBaseFilter *pFilter2)
{
   CheckPointer(pGraphBuilder, E_POINTER);
   CheckPointer(pFilter1, E_POINTER);
   CheckPointer(pFilter2, E_POINTER);

   HRESULT hr;
   IPin *pOutPin = NULL;
   IPin *pInPin = NULL;

   pOutPin = GetPin(pFilter1, PINDIR_OUTPUT, 0);
   pInPin = GetPin(pFilter2, PINDIR_INPUT, 0);

   hr = pGraphBuilder->Disconnect(pOutPin);
   if(hr == S_OK)
   {
      hr = pGraphBuilder->Disconnect(pInPin);
   }

   SAFE_RELEASE(pOutPin);
   SAFE_RELEASE(pInPin);

   return hr;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： AddFilter
// 説明　：	clsidのフィルタをpGraphBuilderに登録する。
// 戻り値： フィルタのポインタ。失敗した場合はNULLを返す
// ----------------------------------------------------------------------------------------------------------------------------
IBaseFilter* CDShowTool::AddFilter(IGraphBuilder *pGraphBuilder, CLSID clsid)
{
   CheckPointer(pGraphBuilder, NULL);

   HRESULT hr;
   IBaseFilter *pBaseFilter = NULL;

   hr = ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pBaseFilter);
   if(pBaseFilter == NULL) return NULL;

   hr = pGraphBuilder->AddFilter(pBaseFilter, NULL);
   if(hr != NO_ERROR)
   {
      SAFE_RELEASE(pBaseFilter);
      return NULL;
   }

   return pBaseFilter;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： AddSourceFilter
// 説明　：	clsidのソースフィルタをpGraphBuilderに登録し、wpFileNameのファイル名を指定します。
// 戻り値： ソースフィルタのポインタ。失敗した場合はNULLを返す
// ----------------------------------------------------------------------------------------------------------------------------
IBaseFilter* CDShowTool::AddSourceFilter(IGraphBuilder *pGraphBuilder, CLSID clsid, WCHAR *wpFileName)
{
   CheckPointer(pGraphBuilder, NULL);

   HRESULT hr;
   IBaseFilter *pBaseFilter = NULL;
   IFileSourceFilter *pFileSourceFilter = NULL;

   pBaseFilter = this->AddFilter(pGraphBuilder, clsid);
   if(pBaseFilter == NULL)
      return NULL;

   pBaseFilter->QueryInterface(IID_IFileSourceFilter, (void**)&pFileSourceFilter);
   if(pFileSourceFilter == NULL)
   {
      pGraphBuilder->RemoveFilter(pBaseFilter);
      SAFE_RELEASE(pBaseFilter);
      return NULL;
   }

   hr = pFileSourceFilter->Load(wpFileName, NULL);
   if(FAILED(hr))
   {
      SAFE_RELEASE(pFileSourceFilter);
      pGraphBuilder->RemoveFilter(pBaseFilter);
      SAFE_RELEASE(pBaseFilter);
      return NULL;
   }

   SAFE_RELEASE(pFileSourceFilter);

   return pBaseFilter;
}
// ----------------------------------------------------------------------------------------------------------------------------
// 関数名： AddWriteFilter
// 説明　：	clsidのライトフィルタをpGraphBuilderに登録し、wpFileNameのファイル名を指定します。
// 戻り値： ライトフィルタのポインタ。失敗した場合はNULLを返す
// ----------------------------------------------------------------------------------------------------------------------------
IBaseFilter* CDShowTool::AddWriteFilter(IGraphBuilder *pGraphBuilder, CLSID clsid, WCHAR *wpFileName)
{
   CheckPointer(pGraphBuilder, NULL);

   HRESULT hr;
   IBaseFilter *pBaseFilter = NULL;
   IFileSinkFilter *pFileSinkFilter = NULL;

   pBaseFilter = this->AddFilter(pGraphBuilder, clsid);
   if(pBaseFilter == NULL)
      return NULL;

   pBaseFilter->QueryInterface(IID_IFileSinkFilter, (void**)&pFileSinkFilter);
   if(pFileSinkFilter == NULL)
   {
      SAFE_RELEASE(pBaseFilter);
      return NULL;
   }

   hr = pFileSinkFilter->SetFileName(wpFileName, NULL);
   if(FAILED(hr))
   {
      SAFE_RELEASE(pFileSinkFilter);
      SAFE_RELEASE(pBaseFilter);
      return NULL;
   }

   SAFE_RELEASE(pFileSinkFilter);

   return pBaseFilter;
}
// ----------------------------------------------------------------------------------------------------------------------------
