// --- fourcc --- 
#define FOURCC_XVID mmioFOURCC('X','V','I','D') 
#define FOURCC_xvid mmioFOURCC('x','v','i','d')
#define FOURCC_DIVX mmioFOURCC('D','I','V','X') 
#define FOURCC_divx mmioFOURCC('d','i','v','x')
#define FOURCC_DIV3 mmioFOURCC('D','I','V','3') 
#define FOURCC_div3 mmioFOURCC('d','i','v','3')
#define FOURCC_DX50 mmioFOURCC('D','X','5','0') 
#define FOURCC_dx50 mmioFOURCC('d','x','5','0')
#define FOURCC_MP41 mmioFOURCC('M','P','4','1') 
#define FOURCC_mp41 mmioFOURCC('m','p','4','1')
#define FOURCC_MP42 mmioFOURCC('M','P','4','2') 
#define FOURCC_mp42 mmioFOURCC('m','p','4','2')
#define FOURCC_MP43 mmioFOURCC('M','P','4','3') 
#define FOURCC_mp43 mmioFOURCC('m','p','4','3')
#define FOURCC_H263 mmioFOURCC('H','2','6','3') 
#define FOURCC_h263 mmioFOURCC('h','2','6','3')
#define FOURCC_WMV1 mmioFOURCC('W','M','V','1') 
#define FOURCC_wmv1 mmioFOURCC('w','m','v','1')

// --- media uids --- 
DEFINE_GUID(CLSID_xvid, 0x64697678, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_XVID, 0x44495658, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_divx, 0x78766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIVX, 0x58564944, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_div3, 0x33766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIV3, 0x33564944, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_dx50, 0x30357864, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50, 0x30355844, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp43, 0x3334706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP43, 0x3334504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp42, 0x3234706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP42, 0x3234504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_mp41, 0x3134706D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP41, 0x3134504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_h263, 0x33363268, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_H263, 0x33363248, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_wmv1, 0x31766d77, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_WMV1, 0x31564d57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

DEFINE_GUID(CLSID_MPEG1Packet , 0xe436eb80, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
DEFINE_GUID(CLSID_MPEG1Payload, 0xe436eb81, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
                           
const AMOVIESETUP_MEDIATYPE sudInputPinTypes[] =
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
  { &MEDIATYPE_Video, &CLSID_h263 },
  { &MEDIATYPE_Video, &CLSID_H263 },
#ifdef FF__WMV1  
  { &MEDIATYPE_Video, &CLSID_wmv1 },
  { &MEDIATYPE_Video, &CLSID_WMV1 },
#endif  
#ifdef FF__MPEG
  { &MEDIATYPE_Video, &CLSID_MPEG1Packet },  
  { &MEDIATYPE_Video, &CLSID_MPEG1Payload },
#endif  
 };

const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[] =
{
 { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL }
};

static AMOVIESETUP_PIN psudPins[] =
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

const AMOVIESETUP_FILTER sudXvidDecoder =
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
