const AMOVIESETUP_MEDIATYPE sudInputPinTypes[]=
{
 { &MEDIATYPE_Video, &CLSID_xvid },
 { &MEDIATYPE_Video, &CLSID_XVID },
 { &MEDIATYPE_Video, &CLSID_divx },
 { &MEDIATYPE_Video, &CLSID_DIVX },
 { &MEDIATYPE_Video, &CLSID_div3 },
 { &MEDIATYPE_Video, &CLSID_DIV3 },
 { &MEDIATYPE_Video, &CLSID_dx50 },
 { &MEDIATYPE_Video, &CLSID_DX50 },
 { &MEDIATYPE_Video, &CLSID_mp43 },
 { &MEDIATYPE_Video, &CLSID_MP43 },
 { &MEDIATYPE_Video, &CLSID_mp42 },
 { &MEDIATYPE_Video, &CLSID_MP42 },
 { &MEDIATYPE_Video, &CLSID_mp41 },
 { &MEDIATYPE_Video, &CLSID_MP41 },
 { &MEDIATYPE_Video, &CLSID_mpg4 },
 { &MEDIATYPE_Video, &CLSID_MPG4 },
 { &MEDIATYPE_Video, &CLSID_h263 },
 { &MEDIATYPE_Video, &CLSID_H263 },
 { &MEDIATYPE_Video, &CLSID_blz0 },
 { &MEDIATYPE_Video, &CLSID_BLZ0 },
 { &MEDIATYPE_Video, &CLSID_wmv1 },
 { &MEDIATYPE_Video, &CLSID_WMV1 },
#ifdef FF__MPEG
 { &MEDIATYPE_Video, &CLSID_MPEG1Packet },
 { &MEDIATYPE_Video, &CLSID_MPEG1Payload },
#endif
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2 },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32},
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24},
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555},
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565},
#ifdef FF__MPEG2
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG2_VIDEO},
#endif
};

const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[]=
{
 { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL }
};

static AMOVIESETUP_PIN psudPins[]=
{
 {
  L"Input",           // String pin name
  FALSE,              // Is it rendered
  FALSE,              // Is it an output
  FALSE,              // Allowed none
  FALSE,              // Allowed many
  &CLSID_NULL,        // Connects to filter
  L"Output",          // Connects to pin
  sizeof(sudInputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  &sudInputPinTypes[0]    // The pin details
 },
 {
  L"Output",          // String pin name
  FALSE,              // Is it rendered
  TRUE,               // Is it an output
  FALSE,              // Allowed none
  FALSE,              // Allowed many
  &CLSID_NULL,        // Connects to filter
  L"Input",           // Connects to pin
  sizeof(sudOutputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  sudOutputPinTypes   // The pin details
 }
};

const AMOVIESETUP_FILTER sudXvidDecoder=
{
 &CLSID_FFDSHOW,            // Filter CLSID
 FFDSHOW_NAME_L,            // Filter name
 #ifdef  FF__MPEG
  0x40000002,
 #else
  MERIT_PREFERRED+1+1,    // Its merit - stupid hack, but Windows Media filters use it too
 #endif
 sizeof(psudPins) / sizeof(AMOVIESETUP_PIN), // Number of pins
 psudPins                // Pin details
};

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance
CFactoryTemplate g_Templates[] =
{
 {
  FFDSHOW_NAME_L,
  &CLSID_FFDSHOW,
  TffDecoder::CreateInstance,
  NULL,
  &sudXvidDecoder
 },
 {
  FFDSHOW_NAME_L L"ffdshow",
  &CLSID_TFFDSHOWPAGE,
  TffdshowPage::CreateInstance
 }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);

STDAPI DllRegisterServer()
{
 return AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer()
{
 return AMovieDllRegisterServer2( FALSE );
}
