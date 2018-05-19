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
// �֐����F Connect
// �����@�F	pFilter1�̏o�̓s����pFilter2�̓��̓s�����q���܂��B(�o�̓s���A���̓s����1���������Ƃ͌���Ȃ��̂ő�����Őڑ�����)
// �߂�l�F S_OK �����B
//          E_POINTER �������s���ł��B
//�@�@�@�@�@E_FAIL �s���̎擾�Ɏ��s�����B
//�@�@�@�@�@VFW_S_PARTIAL_RENDER ���̃��[�r�[�ɃT�|�[�g����Ȃ��t�H�[�}�b�g�̃X�g���[�����܂܂�Ă���B
//�@�@�@�@�@E_ABORT ���삪���~���ꂽ�B 
//�@�@�@�@�@E_POINTER NULL �|�C���^�����B 
//�@�@�@�@�@VFW_E_CANNOT_CONNECT �ڑ����m�����钆�ԃt�B���^�̑g�ݍ��킹��������Ȃ������B 
//�@�@�@�@�@VFW_E_NOT_IN_GRAPH �t�B���^ �O���t�ɑ��݂��Ȃ��I�u�W�F�N�g�ɗv�����ꂽ�֐������s�ł��Ȃ��B 
// �⑫�@�F VFW_S_PARTIAL_RENDER�́A�Ⴆ�Ήf���t�@�C���ŉf���f�R�[�h�t�B���^���������ǉ����f�R�[�h�t�B���^������ꍇ��
//          �\�[�X�t�B���^�Ɖ��������_�����O�t�B���^���q�����ꍇ�ɂȂ�B(�s���ڑ����̂͐���)
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

   // pFilter1�̏o�̓s���̃s�������擾����
   nOutputPinCount = this->GetPinCount(pFilter1, PINDIR_OUTPUT);
   if(nOutputPinCount == 0) return E_FAIL;

   // pFilter2�̓��̓s���̃s�������擾����
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
// �֐����F ConnectDirect
// �����@�F	pFilter1�̏o�̓s����pFilter2�̓��̓s����pAmMediaType�̃��f�B�A�^�C�v�Œ��ڌq���܂��BpAmMediaType��NULL�w��\�ł��B
//          (�o�̓s���A���̓s����1���������Ƃ͌���Ȃ��̂ő�����Őڑ�����)
// �߂�l�F S_OK �����B
//          E_POINTER �������s���ł��B
//�@�@�@�@�@VFW_E_ALREADY_CONNECTED �s���͊��ɐڑ�����Ă���B 
//�@�@�@�@�@VFW_E_NO_ACCEPTABLE_TYPES �����ꂽ���f�B�A �^�C�v�������邱�Ƃ��ł��Ȃ������B 
//�@�@�@�@�@VFW_E_NOT_STOPPED �t�B���^���A�N�e�B�u�ŁA�s�������I�Đڑ����T�|�[�g���Ă��Ȃ��B 
//�@�@�@�@�@VFW_E_TYPE_NOT_ACCEPTED �w�肵�����f�B�A �^�C�v�͋�����Ȃ��B 
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

   // pFilter1�̏o�̓s���̃s�������擾����
   nOutputPinCount = this->GetPinCount(pFilter1, PINDIR_OUTPUT);
   if(nOutputPinCount == 0) return E_FAIL;

   // pFilter2�̓��̓s���̃s�������擾����
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
// �֐����F GetPinCount
// �����@�F	pFilter��pinDirection�̌����̃s���̐���Ԃ��܂�
// �߂�l�F �s���̐��B
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
// �֐����F GetPin
// �����@�F	pFilter��pinDirection�̌�����nPinNumber�Ԗڂ̃s���̃|�C���^��Ԃ��܂�(nPinNumber��0����n�܂�)
// �߂�l�F �s���̃|�C���^�B�s���̎擾�Ɏ��s�����ꍇ��NULL��Ԃ�
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
// �֐����F Disconnect
// �����@�F	pFilter1��pFilter2�̃s���ڑ����������܂�
// �߂�l�F S_OK �����B
//          E_POINTER �������s���ł��B
//       �@ S_FALSE �s�����ڑ�����Ă��Ȃ��B
//        �@VFW_E_NOT_STOPPED �t�B���^���A�N�e�B�u�ŁA�s���͓��I�Đڑ����T�|�[�g���Ă��Ȃ� 
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
// �֐����F AddFilter
// �����@�F	clsid�̃t�B���^��pGraphBuilder�ɓo�^����B
// �߂�l�F �t�B���^�̃|�C���^�B���s�����ꍇ��NULL��Ԃ�
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
// �֐����F AddSourceFilter
// �����@�F	clsid�̃\�[�X�t�B���^��pGraphBuilder�ɓo�^���AwpFileName�̃t�@�C�������w�肵�܂��B
// �߂�l�F �\�[�X�t�B���^�̃|�C���^�B���s�����ꍇ��NULL��Ԃ�
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
// �֐����F AddWriteFilter
// �����@�F	clsid�̃��C�g�t�B���^��pGraphBuilder�ɓo�^���AwpFileName�̃t�@�C�������w�肵�܂��B
// �߂�l�F ���C�g�t�B���^�̃|�C���^�B���s�����ꍇ��NULL��Ԃ�
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
