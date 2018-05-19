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

#ifndef _DSwhoTool_h_
#define _DSwhoTool_h_

class CDShowTool
{
public:
   HRESULT Connect(IGraphBuilder *, IBaseFilter *, IBaseFilter *);
   HRESULT ConnectDirect(IGraphBuilder *, IBaseFilter *, IBaseFilter *, AM_MEDIA_TYPE *);
   int GetPinCount(IBaseFilter *, PIN_DIRECTION);
   IPin* GetPin(IBaseFilter *, PIN_DIRECTION, int);
   HRESULT Disconnect(IGraphBuilder *, IBaseFilter *, IBaseFilter *);

   IBaseFilter *AddFilter(IGraphBuilder *, CLSID);
   IBaseFilter *AddSourceFilter(IGraphBuilder *, CLSID, WCHAR *);
   IBaseFilter *AddWriteFilter(IGraphBuilder *, CLSID, WCHAR *);



};

#endif _DSwhoTool_h_
