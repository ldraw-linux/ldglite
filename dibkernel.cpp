/*---------------------------------------------------------------------------------
	dibkernel.cpp, last modified July 4 1998 by hansm@mip.ou.dk
	René Dencker Eriksen (http://www.imada.ou.dk/~edr) and
	Hans-Martin Brændmose Jensen (http://www.mip.ou.dk/~hansm)
---------------------------------------------------------------------------------*/

#include "stdafx.h"		//standard precompiled headers
#include "dibkernel.h"	//dibkernel header

//default RGB color table for 8 bit color depth
const BYTE def256pal_r[256] = {0,128,0,128,0,128,0,192,192,166,64,96,128,160,192,
	224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,
	192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,
	160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,
	128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,
	96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,
	64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,
	32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,
	224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,
	192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,
	160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,128,160,192,224,0,32,64,96,
	128,160,255,160,128,255,0,255,0,255,0,255};
const BYTE def256pal_g[256] = {0,0,128,128,0,0,128,192,220,202,32,32,32,32,32,32,
	64,64,64,64,64,64,64,64,96,96,96,96,96,96,96,96,128,128,128,128,128,128,128,
	128,160,160,160,160,160,160,160,160,192,192,192,192,192,192,192,192,224,224,
	224,224,224,224,224,224,0,0,0,0,0,0,0,0,32,32,32,32,32,32,32,32,64,64,64,64,
	64,64,64,64,96,96,96,96,96,96,96,96,128,128,128,128,128,128,128,128,160,160,
	160,160,160,160,160,160,192,192,192,192,192,192,192,192,224,224,224,224,224,
	224,224,224,0,0,0,0,0,0,0,0,32,32,32,32,32,32,32,32,64,64,64,64,64,64,64,64,
	96,96,96,96,96,96,96,96,128,128,128,128,128,128,128,128,160,160,160,160,160,
	160,160,160,192,192,192,192,192,192,192,192,224,224,224,224,224,224,224,224,
	0,0,0,0,0,0,0,0,32,32,32,32,32,32,32,32,64,64,64,64,64,64,64,64,96,96,96,96,
	96,96,96,96,128,128,128,128,128,128,128,128,160,160,160,160,160,160,160,160,
	192,192,192,192,192,192,251,160,128,0,255,255,0,0,255,255};
const BYTE def256pal_b[256] = {0,0,0,0,128,128,128,192,192,240,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
	192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
	192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,240,164,128,
	0,0,0,255,255,255,255};

// file read errors
#define IO_NO_ERROR           0
#define IO_OS_ERROR          -1
#define IO_READHDR_ERROR     -2
#define IO_INVALID_ERROR     -3
#define IO_BADFORMAT_ERROR   -4
#define IO_COMPRESSED_ERROR  -5
#define IO_NODIBLOADED_ERROR -6


IMPLEMENT_SERIAL(CDib,CObject,0);


CFloatPoint::CFloatPoint(double x,double y)
{
	CFloatPoint::x = x; CFloatPoint::y = y;
}

CFloatPoint::~CFloatPoint() {
}

CPoint CFloatPoint::Round() {
	return CPoint(round(x),round(y));
}

CPoint CFloatPoint::Int() {
	return CPoint((int)x,(int)y);
}

int round(const double f)
{
	if (f<0) {
		return (int)(f - 0.5);
	}
	else {
		return (int)(f + 0.5);
	}
}


CDib::CDib() //default constructor
{
	m_ptOrigo = CPoint(0,0);	//default origo
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_hPalette = NULL;
	m_pImageMatrix = NULL;
	Empty();
}

CDib::CDib(CDib& dib) //copy constructor
{
//	m_ptOrigo = CPoint(0,0);	//default origo
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_hPalette = NULL;
	m_pImageMatrix = NULL;
	Empty();
	*this = dib;
}

CDib::CDib(CSize size, int nBitCount, BOOL bInitialize)
{
	m_ptOrigo = CPoint(0,0);	//default origo
	m_hPalette = NULL;
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_pImageMatrix = NULL;
	Empty();
	ComputePaletteSize(nBitCount);
	m_lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*m_nColorTableEntries];
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = 0; //online help: may be set to 0 for BI_RGB bitmaps
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();
	m_lpImage=(LPBYTE) new char[m_dwSizeImage];
	if (bInitialize) {
		//make default palette from def256pal_x constants
		int i;
		switch (nBitCount) {
			case 1:
				SetPaletteEntry(0,0);					//black pal. entry
				SetPaletteEntry(1,RGB(255,255,255));	//white pal. entry
				break;
			case 8:
				for (i=0; i<=255; i++) {
					SetPaletteEntry(i,RGB(def256pal_r[i],def256pal_g[i],def256pal_b[i]));
				}
				break;
			case 24:
				break;
			}

		memset(m_lpImage, 0, m_dwSizeImage); //initialize image bits to zero
	}
	MakePalette();
}

CDib::CDib(const CDib& dib,CRect rect)
{	//copy area of interest from dib

    int headerSize = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*dib.m_nColorTableEntries;
    //copy header
    m_lpBMIH = (LPBITMAPINFOHEADER) new char[headerSize];
    memcpy(m_lpBMIH,dib.m_lpBMIH,headerSize);
	//dimensions
	m_lpBMIH->biWidth=rect.Width();
	m_lpBMIH->biHeight=rect.Height();
	m_lpBMIH->biSizeImage = 0; //online help: may be set to 0 for BI_RGB bitmaps
    //compute member variables
    ComputeMetrics();
    ComputePaletteSize(dib.m_lpBMIH->biBitCount);
    MakePalette();
	m_ptOrigo = CPoint(0,0); //default origo
    //copy bits
    m_lpImage = (LPBYTE)new char[m_dwSizeImage];
	for(int y=rect.top; y<=rect.bottom; y++){
		for(int x=rect.left; x<=rect.right; x++){
			SetPixel(CPoint(x-rect.left,y-rect.top),dib.GetPixel(CPoint(x,y)));
		}
	}
	m_pImageMatrix = NULL;
}

CDib& CDib::operator=(const CDib& dib)
{
	ASSERT(this != &dib);	//beware of dib=dib

	if (dib.DibLoaded()) {
		Empty(); //clear left operand (dib we are copying to)

		int headerSize = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*dib.m_nColorTableEntries;
		//copy header
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[headerSize];
		memcpy(m_lpBMIH,dib.m_lpBMIH,headerSize);
		//copy member variables
		ComputeMetrics();
		ComputePaletteSize(dib.m_lpBMIH->biBitCount);
		MakePalette();
		m_ptOrigo = dib.m_ptOrigo;
		m_FileInfo = dib.m_FileInfo;
		//copy bits
		m_lpImage = (LPBYTE)new char[dib.m_dwSizeImage];
		memcpy(m_lpImage,dib.m_lpImage,dib.m_dwSizeImage);
		m_pImageMatrix = NULL;
	}
	return *this;
}

CDib::~CDib()
{
	Empty(); //clean up memory
}

CSize CDib::GetDimensions() const
{	
	if (!DibLoaded()) return CSize(0,0);
	return CSize((int)m_lpBMIH->biWidth,(int)m_lpBMIH->biHeight);
}

BOOL CDib::Draw(CDC* pDC,CRect rcDest,CPoint ptSrc)
/*-----------------------------------------------------------------------------
pDC:	Device context pointer to do output to.
rcDest:	Rectangle on DC to do output to.
ptSrc:	Coordinate of the lower-left corner of the DIB to output into rcDest.
-----------------------------------------------------------------------------*/
{
	//check for valid DIB handle
	if (!DibLoaded() /*|| (m_hPalette == NULL)*/) return FALSE;

	BOOL bSuccess=FALSE; //success/fail flag
	HPALETTE hOldPal=NULL; //previous palette (our DIB's palette is m_hPalette)
	CSize dibsize = GetDimensions(); //get DIB's dimensions

	//select as background since we have already realized in forground if needed
	hOldPal = ::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	if (pDC->IsPrinting()) //printer DC
	{
		//get size of printer page (in pixels)
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);
		//get printer pixels per inch
		int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
		int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

		// Best Fit case -- create a rectangle which preserves
		// the DIB's aspect ratio, and fills the page horizontally.
		// The formula in the "->bottom" field below calculates the Y
		// position of the printed bitmap, based on the size of the
		// bitmap, the width of the page, and the relative size of
		// a printed pixel (cyInch / cxInch).
		rcDest.top = rcDest.left = 0;
		rcDest.bottom = (int)(((double)dibsize.cy*cxPage*cyInch)/
			((double)dibsize.cx*cxInch));
		rcDest.right = cxPage;
	}
	//make sure to use the stretching mode best for color pictures
	::SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	//determine whether to call StretchDIBits or SetDIBitsToDevice
	if (dibsize == rcDest.Size()) {
		bSuccess = ::SetDIBitsToDevice(
			pDC->GetSafeHdc(),		//handle of device context
			rcDest.left,			//x-coordinate of upper-left corner of dest. rect.
			rcDest.top,				//y-coordinate of upper-left corner of dest. rect.
			rcDest.Width(),			//source rectangle width
			rcDest.Height(),		//source rectangle height
			ptSrc.x,				//x-coordinate of lower-left corner of source rect.
			ptSrc.y,				//y-coordinate of lower-left corner of source rect.
			0,						//first scan line in array
			rcDest.Height(),		//number of scan lines
			m_lpImage,				//address of array with DIB bits
			(LPBITMAPINFO)m_lpBMIH,	//address of structure with bitmap info.
			DIB_RGB_COLORS);		//RGB or palette indices
	}
	else {
		bSuccess = ::StretchDIBits(
			pDC->GetSafeHdc(),		//handle of device context
			rcDest.left,			//x-coordinate of upper-left corner of dest. rect.
			rcDest.top,				//y-coordinate of upper-left corner of dest. rect.
			rcDest.Width(),			//width of destination rectangle
			rcDest.Height(),		//height of destination rectangle
			ptSrc.x,				//x-coordinate of upper-left corner of source rect.
			ptSrc.y,				//y-coordinate of upper-left corner of source rect.
			m_lpBMIH->biWidth,		//width of source rectangle
			m_lpBMIH->biHeight,		//height of source rectangle
			m_lpImage,				//address of bitmap bits
			(LPBITMAPINFO)m_lpBMIH,	//address of bitmap data
			DIB_RGB_COLORS,			//usage
			SRCCOPY);				//raster operation code
	}
	//reselect old palette
	if (hOldPal != NULL) {
		::SelectPalette(pDC->GetSafeHdc(),hOldPal,TRUE);
	}
    return bSuccess;
}

BOOL CDib::MakePalette()
{	// makes a logical palette (m_hPalette) from the DIB's color table
	// this palette will be selected and realized prior to drawing the DIB

	if (m_nColorTableEntries == 0) return FALSE;
	if (m_hPalette != NULL) ::DeleteObject(m_hPalette);
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		m_nColorTableEntries * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = m_nColorTableEntries;
	LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
	for(int i = 0; i < m_nColorTableEntries; i++) {
		pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		pDibQuad++;
	}
	m_hPalette = ::CreatePalette(pLogPal);
	delete pLogPal;
	return TRUE;
}	

int CDib::Read(CFile* pFile) //CFile ptr passed by the documents serialize function
{
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. read image

	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	TRY {
		m_FileInfo.FileName=pFile->GetFileName();
		m_FileInfo.FilePath=pFile->GetFilePath();
		nCount = pFile->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER))
			return IO_READHDR_ERROR;
		if(bmfh.bfType != 0x4d42) // 'BM'
			return IO_INVALID_ERROR;
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		nCount = pFile->Read(m_lpBMIH, nSize); // info hdr & color table
		//check if Windows 3.x or Windows NT BMP
		if (m_lpBMIH->biSize!=40)
			return IO_BADFORMAT_ERROR;
		//check if compressed
		if ((m_lpBMIH->biCompression==BI_RLE8) || (m_lpBMIH->biCompression==BI_RLE4))
			return IO_COMPRESSED_ERROR;
		ComputeMetrics(); //sets m_dwSizeImage etc.
		m_lpImage = (LPBYTE) new char[m_dwSizeImage];
		nCount = pFile->Read(m_lpImage,m_dwSizeImage); //image bits only
	}
	CATCH (CFileException,e) {
		CString msg;
		msg.Format("%s could not be opened (OS error information =  %d)",
			m_FileInfo.FileName,e->m_cause);
		AfxMessageBox(msg);
		return IO_OS_ERROR;
	}
	END_CATCH
	ComputePaletteSize(m_lpBMIH->biBitCount);
	MakePalette();
	return IO_NO_ERROR;
}

void CDib::Read(CString filename)
{
	Serialize(CArchive(&CFile(filename,CFile::modeRead),CArchive::load));
	TRACE("DIB read from file %s\n",filename);
}

int CDib::Write(CFile* pFile) //CFile ptr passed by the documents serialize function
{
	if (!DibLoaded()) return IO_NODIBLOADED_ERROR;

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSize =  sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*m_nColorTableEntries;
	bmfh.bfSize = nSize+sizeof(BITMAPFILEHEADER);
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
			sizeof(RGBQUAD)*m_nColorTableEntries;
	TRY {
		m_FileInfo.FileName=pFile->GetFileName();
		m_FileInfo.FilePath=pFile->GetFilePath();
		pFile->Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		pFile->Write((LPVOID) m_lpBMIH,nSize);
		pFile->Write((LPVOID) m_lpImage,m_dwSizeImage);
	}
	CATCH (CFileException,e) {
		CString msg;
		msg.Format("%s could not be opened (OS error information =  %d)",
			m_FileInfo.FileName,e->m_cause);
		AfxMessageBox(msg);
		return IO_OS_ERROR;
	}
	END_CATCH
	return IO_NO_ERROR;
}

void CDib::Write(CString filename, BOOL overwrite)
{
	CFileStatus status;
	if (CFile::GetStatus(filename,status)) { //0 if file does not exist
		if (overwrite) {
			TRACE("Warning! Overwriting: ");
		}
		else {
			CString msg;
			msg.Format("Replace existing file %s?",filename);
			if (AfxMessageBox(msg,MB_YESNO) == IDNO) {
				return;
			}
		}
	}
	TRY { //be sure we delete the file if save fails
		Serialize(CArchive(&CFile(filename,CFile::modeCreate|CFile::modeWrite),CArchive::store));
		TRACE("DIB wrote to file %s\n",filename);
	}
	CATCH_ALL(e) {
		CFile::Remove(filename);
	}
	END_CATCH_ALL
}

void CDib::Serialize(CArchive& ar)
{
	int fileError;
	CObject::Serialize(ar);	//always call base class Serialize
	ar.Flush();
	if (ar.IsStoring()) {
		if (fileError = Write(ar.GetFile())) {
			switch (fileError) {
				case IO_NODIBLOADED_ERROR:
					AfxMessageBox("Bitmap is empty!");
					break;
				case IO_OS_ERROR: //user already informed
					break;
			    default:	 
					AfxMessageBox("Unknown IO error during write");
					break;
			}
			AfxThrowUserException();
		}
	}
	else {
		if (fileError = Read(ar.GetFile())) {
			switch (fileError) {
				case IO_READHDR_ERROR:
					AfxMessageBox("Error reading header");
					break;
				case IO_INVALID_ERROR:
					AfxMessageBox("Invalid bitmap file");
					break;
				case IO_BADFORMAT_ERROR:
					AfxMessageBox("Is not a Windows 3.x or Windows NT BMP (OS/2 BMP?)");
					break;
				case IO_COMPRESSED_ERROR:
					AfxMessageBox("Compressed bitmaps not supported");
					break;
				case IO_OS_ERROR: //user already informed
					break;
			    default:
					AfxMessageBox("Unknown IO error during read");
					break;
			}
			Empty();
			AfxThrowUserException();
		}
	}
}

int CDib::GetSizeImage()
{
	return m_dwSizeImage;
}

int CDib::GetSizeHeader()
{
	return sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*m_nColorTableEntries;
}

DWORD CDib::GetPixel(CPoint pt) const
{
	CPoint ptReal = pt+m_ptOrigo;
	register BYTE* adr;

	if ((ptReal.x>=0)&&(ptReal.x<m_lpBMIH->biWidth)&&
		(ptReal.y>=0)&&(ptReal.y<m_lpBMIH->biHeight))
		switch(m_lpBMIH->biBitCount) {
		case 1:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth+(ptReal.x>>3);
			return (((*adr)&(0x80>>(ptReal.x&0x7)))!=0);
		case 8:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth+ptReal.x;
			return (*adr)&0xff;
		case 24:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth
				+ptReal.x+ptReal.x+ptReal.x;
			return ((*(DWORD*)adr)&0xffffff);
		default:
			AfxMessageBox("GetPixel: only implemented for 1,8 and 24 bit per pixels");
			return DIBERROR;
		}
	return DIBERROR;
}
	
DWORD CDib::SetPixel(CPoint pt, DWORD color)
{
	CPoint ptReal = pt + m_ptOrigo;
	register unsigned int pixnum;
	register BYTE* adr;

	if((ptReal.x>=0)&&(ptReal.x<m_lpBMIH->biWidth)&&
		(ptReal.y>=0)&&(ptReal.y<m_lpBMIH->biHeight))
		switch(m_lpBMIH->biBitCount) {
		case 1:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth+(ptReal.x>>3);
			pixnum = ptReal.x & 0x7;
			color ? (*adr |= (0x80>>pixnum)) : (*adr &= (0xff7f>>pixnum));
			break;
		case 8:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth+ptReal.x;
			*adr = (BYTE)color;
			break;
		case 24:
			adr = m_lpImage+(m_lpBMIH->biHeight-ptReal.y-1)*m_nByteWidth
				+ptReal.x+ptReal.x+ptReal.x;
#if 0 // causes memory fault on some sizes of images
			*((DWORD*)adr) = ((*((DWORD*)adr)) & 0xff000000) | (color & 0x00ffffff);
#else
			*adr++ =  color & 0x000000ff;
			*adr++ = (color & 0x0000ff00)>>8;
			*adr   = (color & 0x00ff0000)>>16;
#endif
			break;
		default:
			AfxMessageBox("SetPixel: only implemented for 1,8 and 24 bit per pixels");
			return DIBERROR;
		}
	return DIBERROR;
}

void CDib::Empty()
{
	// clean up whatever is allocated for the DIB
	if(m_lpBMIH != NULL) {
		delete [] m_lpBMIH;
	}
	if(m_lpImage != NULL){
		delete [] m_lpImage;
	}
	if(m_hPalette != NULL) ::DeleteObject(m_hPalette);
	if (m_pImageMatrix != NULL){
		delete [] m_pImageMatrix;
	}
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_hPalette = NULL;
	m_pImageMatrix=NULL;
	m_lpvColorTable = NULL;
	m_nColorTableEntries = 0;
	m_dwSizeImage = 0;
}

void CDib::SetOrigo(CPoint ptOrigo)
{
	m_ptOrigo = ptOrigo;
}

BOOL CDib::SetPaletteEntry(BYTE bEntry, DWORD dwValue)
{
	switch (m_lpBMIH->biBitCount) {
		case 1:
			if (bEntry>1){
				AfxMessageBox("SetPaletteEntry: palette entry out of range");
				ASSERT(FALSE);
				return FALSE;
			}
			*((DWORD*)((BYTE*)m_lpvColorTable+bEntry*sizeof(RGBQUAD)))=dwValue;
			return TRUE;
		case 8:
			*((DWORD*)((BYTE*)m_lpvColorTable+bEntry*sizeof(RGBQUAD)))=dwValue;
			return TRUE;
		case 24:
			AfxMessageBox("SetPaletteEntry: image with 24 bits per pixel has no palette");
			ASSERT(FALSE);
			return FALSE;
		default:
			AfxMessageBox("CDib only supports 1, 8 and 24 bits per pixel");
			ASSERT(FALSE);
			return FALSE;
	}
	//MakePalette() ?????
}

DWORD CDib::GetPaletteEntry(BYTE bEntry)
{
	switch (m_lpBMIH->biBitCount) {
		case 1:
			if (bEntry>1){
				AfxMessageBox("GetPaletteEntry: palette entry out of range");
				ASSERT(FALSE);
				return DIBERROR;
			}
			return *((DWORD*)((BYTE*)m_lpvColorTable+bEntry*sizeof(RGBQUAD)));
		case 8:
			return *((DWORD*)((BYTE*)m_lpvColorTable+bEntry*sizeof(RGBQUAD)));
		case 24:
			AfxMessageBox("GetPaletteEntry: image with 24 bits per pixel has no palette");
			ASSERT(FALSE);
			return DIBERROR;
		default:
			AfxMessageBox("CDib only supports 1, 8 and 24 bits per pixel");
			ASSERT(FALSE);
			return DIBERROR;
	}
}

BOOL CDib::CopyPalette(const CDib& srcdib)
{	//copy palette from srcdib

	ASSERT(m_nColorTableEntries == srcdib.m_nColorTableEntries); //colpal not same size
	switch (m_lpBMIH->biBitCount) {
		case 1:
		case 8:
			memcpy(m_lpvColorTable,srcdib.m_lpvColorTable,
				m_nColorTableEntries*sizeof(RGBQUAD));
//			MakePalette() ????
			return TRUE;
		case 24:
			AfxMessageBox("CopyPalette: image with 24 bits per pixel has no palette");
			ASSERT(FALSE);
			return FALSE;
		default:
			AfxMessageBox("CDib only supports 1, 8 and 24 bits per pixel");
			ASSERT(FALSE);
			return FALSE;
	}
}

BOOL CDib::ConvertDepth8to1(BYTE cThreshold)
{	// Converts an 8 bits per pixel image to 1 bit per pixel

	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Cannot convert depth, source is not 8 bits per pixel");
		return FALSE;
	}
	LPBITMAPINFOHEADER lpBMIH1 =
		(LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD)];
	memcpy(lpBMIH1,m_lpBMIH,sizeof(BITMAPINFOHEADER));
	int nWidth=m_lpBMIH->biWidth;
	int nHeight=m_lpBMIH->biHeight;
	int bytewidth1 = (nWidth+31)/32*4;
	LPBYTE pImg8=m_lpImage;
	LPBYTE pImg1=(LPBYTE) new char[bytewidth1*nHeight];
	LPBYTE lpImage1=pImg1;
	int aligndw=(bytewidth1-(nWidth+7)/8)+((nWidth&0x7)?1:0); //double word alignment 
	int y=0;
	while (y<nHeight) {
		int x=0;
		BYTE cBit=0x80;
		while (x<nWidth) {
			*pImg1 = ((*pImg8)>cThreshold)?(cBit|(*pImg1)):((~cBit)&(*pImg1));
			if (!(cBit>>=1)) {
				cBit=0x80;
				pImg1++;
			}
			pImg8++;
			x++; 
		}
		pImg1+=aligndw;
		if (nWidth!=m_nByteWidth){ //if zero padded
			pImg8+=(m_nByteWidth-nWidth);
		}
		y++;
	}
	//clear 8 bpp image
	Empty();
	//set dib object to new 1 bpp picture
	m_lpBMIH=lpBMIH1;
	m_lpBMIH->biBitCount=1;
	m_lpBMIH->biClrUsed=0;
	ComputePaletteSize(m_lpBMIH->biBitCount);
	m_lpBMIH->biClrImportant=m_nColorTableEntries;
	m_lpBMIH->biSizeImage=0;
	ComputeMetrics();
	m_lpBMIH->biSizeImage=m_dwSizeImage;
	m_lpImage=lpImage1;
	SetPaletteEntry(0,0);					//black pal. entry
	SetPaletteEntry(1,RGB(255,255,255));	//white pal. entry
	MakePalette();
	return TRUE;
}

BOOL CDib::ConvertDepth1to8()
{	// Converts an 1 bits per pixel image to 8 bit per pixel

	if (!DibLoaded() || (m_lpBMIH->biBitCount!=1)){
		AfxMessageBox("Cannot convert depth, source is not 1 bits per pixel");
		return FALSE;
	}
    LPBITMAPINFOHEADER lpBMIH8 = (LPBITMAPINFOHEADER) new
		char[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
	memcpy(lpBMIH8,m_lpBMIH,sizeof(BITMAPINFOHEADER));
	int nWidth=m_lpBMIH->biWidth;
	int nHeight=m_lpBMIH->biHeight;
	int bytewidth8;
	int bytewidth1 = (nWidth+31)/32*4;
	int aligndw=(bytewidth1-(nWidth+7)/8)+((nWidth&0x7)?1:0);
	//double word alignment
	bytewidth8 = ((nWidth%4)!=0) ? nWidth+(4-nWidth%4) : nWidth;
	LPBYTE pImg1=m_lpImage;
	LPBYTE pImg8=(LPBYTE)  new char[bytewidth8*nHeight];
	LPBYTE lpImage8=pImg8;
	int y=nHeight-1;
	while (y>=0){
		int x=0;
		while (x<nWidth){
			*lpImage8 = (GetPixel(CPoint(x,y))==1) ? 255 : 0;
			lpImage8++;
			x++; 
		}
		if(bytewidth8!=nWidth){
			lpImage8+=(bytewidth8-nWidth);
		}
		y--;
	}
	//clear 1 bpp image
	Empty();
	//set dib object to new 8 bpp picture
	m_lpBMIH=lpBMIH8;
	m_lpBMIH->biBitCount=8;
	m_lpBMIH->biClrUsed=0;
	ComputePaletteSize(m_lpBMIH->biBitCount);
	m_lpBMIH->biClrImportant=m_nColorTableEntries;
	m_lpBMIH->biSizeImage=0;
	ComputeMetrics();
	m_lpBMIH->biSizeImage=m_dwSizeImage;
	m_lpImage=pImg8;
	for(int i=0; i<=255; i++){
		SetPaletteEntry(i,RGB(i,i,i));
	}
	MakePalette();
	return TRUE;
}

int CDib::FindThreshold(int width)
{	//Calls SmoothHist until there is only one minimum found (not including
	// the ends)
	
	CDWordArray Hist;
	Hist.SetSize(m_nColorTableEntries); //array[0 - (m_nColorTableEntries-1)]
	BYTE** pImage = GetImageMatrix();   //access image as matrix

	for (int y=0; y<m_lpBMIH->biHeight; y++){
		for (int x=0; x<m_lpBMIH->biWidth; x++){
			Hist[pImage[y][x]]++;
		}
	}
	int threshold;
	threshold = FindSingleMin(Hist);
	while (threshold < 0) {
		SmoothHist(Hist,width);
		threshold = FindSingleMin(Hist);
	}
	return threshold;
}

BOOL CDib::Rotate(double theta, BOOL centerOrigo, BOOL enlarge)
{	//rotate DIB by theta radians. If enlarge is TRUE the size of the rotated
	//DIB will be enlarged. The point of rotation will be m_ptOrigo;

	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Implemented for 8 bpp only!");
		return FALSE;
	}

	CSize dim = GetDimensions();
	SetOrigo(centerOrigo);
	CRect newrect = CalcNewCanvas(dim,theta,enlarge);
	CDib newdib = CDib(CSize(newrect.Size()+CSize(1,1)),m_lpBMIH->biBitCount,FALSE);
	newdib.CopyPalette(*this);

	int newpix;
	BYTE** pImage = GetImageMatrix();
	BYTE** pNewImage = newdib.GetImageMatrix();
	for (int x=newrect.left; x<=newrect.right; x++) {
		for (int y=newrect.top; y<=newrect.bottom; y++) {
			CFloatPoint fptFrom = FromPoint(CPoint(x,y),theta);
			CPoint ptFrom = fptFrom.Int();
			if ((ptFrom.x>=0) && (ptFrom.x<dim.cx) &&
				(ptFrom.y>=0) && (ptFrom.y<dim.cy)) {
				double a = fptFrom.x-floor(fptFrom.x);
				double b = fptFrom.y-floor(fptFrom.y);
				if ((ptFrom.x==(dim.cx-1)) || (ptFrom.y==(dim.cy-1))) {
					newpix = (int)((1.0-a)*(1.0-b)*(double)(pImage[ptFrom.y][ptFrom.x])
						+ (1.0-a)*b*GetBorderPixel(pImage,ptFrom.x,ptFrom.y+1)
						+ a*(1.0-b)*GetBorderPixel(pImage,ptFrom.x+1,ptFrom.y)
						+ a*b*GetBorderPixel(pImage,ptFrom.x+1,ptFrom.y+1));
				}
				else {
					newpix = (int)((1.0-a)*(1.0-b)*(double)(pImage[ptFrom.y][ptFrom.x])
						+ (1.0-a)*b*(double)(pImage[ptFrom.y+1][ptFrom.x])
						+ a*(1.0-b)*(double)(pImage[ptFrom.y][ptFrom.x+1])
						+ a*b*(double)(pImage[ptFrom.y+1][ptFrom.x+1]));
				}
				pNewImage[y-newrect.top][x-newrect.left]=(BYTE)newpix;
			}
		}
	}
	Empty();
	*this = newdib;
	return TRUE;
}

BYTE** CDib::GetImageMatrix()
{
	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Cannot create matrix - not 8 bits per pixel");
		return NULL;
	}
	else if (m_pImageMatrix == NULL){ //check if matrix already exists
		m_pImageMatrix = (BYTE**) new BYTE*[m_lpBMIH->biHeight];
		for(int y=(m_lpBMIH->biHeight-1);y>=0;y--){
			m_pImageMatrix[(m_lpBMIH->biHeight-1)-y] =
				(BYTE*)(m_lpImage+y*m_nByteWidth);
		}
	}
	return m_pImageMatrix;
}

BOOL CDib::CopyToClipboard()
{	//Before calling CopyToClipboard, an application must open the clipboard
	//  by using the OpenClipboard function.

	if (!DibLoaded()) return FALSE;

	// Clean clipboard of contents, and copy the DIB.
	HGLOBAL h = ::GlobalAlloc(GHND,GetSizeHeader()+GetSizeImage());
	if (h != NULL) {
		void* lp = ::GlobalLock(h);
		//copy header and palette
		memcpy(lp,m_lpBMIH,GetSizeHeader());
		//copy image bits
		memcpy((LPBYTE)lp+GetSizeHeader(),m_lpImage,GetSizeImage());
		::GlobalUnlock(h);
		EmptyClipboard();
		SetClipboardData(CF_DIB,h);
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

BOOL CDib::PasteFromClipboard()
{	//Before calling PasteFromClipboard, an application must open the clipboard
	//  by using the OpenClipboard function.

	HGLOBAL hNewDIB = (HGLOBAL)::GetClipboardData(CF_DIB);
	if (hNewDIB != NULL) {
		Empty();	//free the old DIB
		LPBITMAPINFOHEADER lpNewBMIH = m_lpBMIH = (LPBITMAPINFOHEADER)::GlobalLock(hNewDIB);
		ComputePaletteSize(lpNewBMIH->biBitCount);
		int headersize = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*m_nColorTableEntries;

		m_lpBMIH = (LPBITMAPINFOHEADER) new char[headersize];
	    //copy header and palette
		memcpy(m_lpBMIH,lpNewBMIH,headersize);
		ComputeMetrics();
		MakePalette();

		m_lpImage=(LPBYTE) new char[m_dwSizeImage];
		//copy image bits
		memcpy(m_lpImage,(LPBYTE)lpNewBMIH+headersize,m_dwSizeImage);
		
		::GlobalUnlock(hNewDIB);
		m_ptOrigo = CPoint(0,0);	//default origo

//		EmptyClipboard();	//free handles to data in the clipboard
		CloseClipboard();	//enable other windows to access the clipboard
		return TRUE;
	}
	return FALSE;
}

BOOL CDib::DibLoaded() const
{
	return (m_lpBMIH!=NULL);
}

void CDib::Scale(double factorX, double factorY)
{
	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Implemented for 8 bpp only!");
		return;
	}
	if ((factorX<0) || (factorY<0)){
		AfxMessageBox("Cannot scale image with negative factor");
		ASSERT(FALSE);
		return;
	}
	int nNewWidth = (int) ceil(m_lpBMIH->biWidth*factorX);
	int nNewHeight = (int) ceil(m_lpBMIH->biHeight*factorY);
	int addOffset = (4-(nNewWidth%4))%4;
	int nSizeImage = (nNewWidth + addOffset)*nNewHeight;
	LPBYTE lpNewImage = (LPBYTE)malloc(nSizeImage);
	double a, b;
	CFloatPoint fptFrom;
	WORD wFromX,wFromY;
	BYTE** ImageMatrix=GetImageMatrix();
	int nYValue,nXValue, YIndexInNewIm;
	// calculate new image pixels excluding borders to the right and at bottom 
	for(nYValue=0;nYValue<(nNewHeight-(int)ceil(factorY));nYValue++){ // -factorY = do not calculate at border
		YIndexInNewIm=(nNewHeight-1)*(nNewWidth+addOffset)-(nYValue*(nNewWidth+addOffset));
		for(nXValue=0;nXValue<(nNewWidth-(int)ceil(factorX));nXValue++){ // -factorX = do not calculate at border
			fptFrom.x = nXValue/factorX;
			fptFrom.y = nYValue/factorY;
			// Finds interpolated value (bilinear)
			a = fptFrom.x-floor(fptFrom.x);
			b = fptFrom.y-floor(fptFrom.y);
			wFromX = int (fptFrom.x);
			wFromY = int (fptFrom.y);
			lpNewImage[YIndexInNewIm + nXValue] =
				(BYTE)((1.0-a)*(1.0-b)*(double)ImageMatrix[wFromY][wFromX]
				+ (1.0-a)*b*(double)ImageMatrix[wFromY+1][wFromX]
				+ a*(1.0-b)*(double)ImageMatrix[wFromY][wFromX+1]
				+ a*b*(double)ImageMatrix[wFromY+1][wFromX+1]);
		}
	}
	// Calculate right border in new image (only interpolates in y-direction)
	fptFrom.x = double (m_lpBMIH->biWidth-1);
	wFromX = int (fptFrom.x);
	a = fptFrom.x-floor(fptFrom.x);
	for(nYValue=0;nYValue<(nNewHeight-(int)ceil(factorY));nYValue++){ // -factorY = do not calculate lower left corner
		YIndexInNewIm=(nNewHeight-1)*(nNewWidth+addOffset)-(nYValue*(nNewWidth+addOffset));
		for(nXValue=(nNewWidth-(int)ceil(factorX));nXValue<(nNewWidth);nXValue++){ // -factorX = do not calculate at border
			fptFrom.y = nYValue/factorY;
			// Finds interpolated value (bilinear)
			b = fptFrom.y-floor(fptFrom.y);
			wFromX = (int)floor(fptFrom.x);
			wFromY = (int)floor(fptFrom.y);
			lpNewImage[YIndexInNewIm + nXValue] =
				(BYTE)((1.0-a)*(1.0-b)*(double)ImageMatrix[wFromY][wFromX]
				+ (1.0-a)*b*(double)ImageMatrix[wFromY+1][wFromX]
				+ a*(1.0-b)*(double)ImageMatrix[wFromY][wFromX] // indexing out of image - uses ptFrom-value
				+ a*b*(double)ImageMatrix[wFromY][wFromX]); // indexing out of image - uses ptFrom-value
		}
	}
	// Calculate lower border in new image (only interpolates in x-direction)
	fptFrom.y = double (m_lpBMIH->biHeight-1);
	wFromY = int (fptFrom.y);
	b = fptFrom.y-floor(fptFrom.y);
	for(nYValue=(nNewHeight-(int)ceil(factorY));nYValue<nNewHeight;nYValue++){
		YIndexInNewIm=(nNewHeight-1)*(nNewWidth+addOffset)-(nYValue*(nNewWidth+addOffset));
		for(nXValue=0;nXValue<(nNewWidth-(int)ceil(factorX));nXValue++){ // -factorX = do not calculate lower left corner
			fptFrom.x = nXValue/factorX;
			// Finds interpolated value (bilinear)
			a = fptFrom.x-floor(fptFrom.x);
			wFromX = (int)floor(fptFrom.x);
			lpNewImage[YIndexInNewIm + nXValue] =
				(BYTE)((1.0-a)*(1.0-b)*(double)ImageMatrix[wFromY][wFromX]
				+ (1.0-a)*b*(double)ImageMatrix[wFromY][wFromX] // indexing out of image - uses ptFrom-value
				+ a*(1.0-b)*(double)ImageMatrix[wFromY][wFromX+1]
				+ a*b*(double)ImageMatrix[wFromY][wFromX]); // indexing out of image - uses ptFrom-value
		}
	}
	// still missing lower right rectangle, simply copies old corner pixel
	for(nYValue=(nNewHeight-(int)ceil(factorY));nYValue<nNewHeight;nYValue++){
		YIndexInNewIm=(nNewHeight-1)*(nNewWidth+addOffset)-(nYValue*(nNewWidth+addOffset));
		for(nXValue=(nNewWidth-(int)ceil(factorX));nXValue<nNewWidth;nXValue++){
			lpNewImage[YIndexInNewIm + nXValue] = (BYTE) ImageMatrix[m_lpBMIH->biHeight-1][m_lpBMIH->biWidth-1];
		}
	}
	//copy scaled image to CDib object
	LPBYTE temp = m_lpImage;
	m_lpImage = lpNewImage;
	free(temp);
	m_lpBMIH->biWidth=nNewWidth;
	m_lpBMIH->biHeight=nNewHeight;
	m_lpBMIH->biSizeImage=0; // So that ComputeMetrics() computes m_dwSizeImage variable
	ComputeMetrics();
	if (m_pImageMatrix != NULL){ // An old pointer is no longer valid
		delete [] m_pImageMatrix;
	}
	m_pImageMatrix = NULL;
}

BOOL CDib::DecreaseGrayTones(BYTE byColors)
{	// Inserts "byColors" (defaults 5) colors at the end of the palette. Colors are
	// from the def256pal constants in top of this file
	// byColors max value is 254, best results for byColors<=245

	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Cannot decrease gray tones, source is not 8 bits per pixel");
		return FALSE;
	}
	DWORD rgb=GetPaletteEntry(250);
	if ((GetRValue(rgb)!=250) || (GetGValue(rgb)!=250) || (GetBValue(rgb)!=250)){
		AfxMessageBox("Not a correct grayscale image");
		return FALSE;
	}
	float fDecreaseFactor=(float)((255-byColors)/(float) 255);
	BYTE byCount;
	for(byCount=0; byCount<=(255-byColors);byCount++){
		SetPaletteEntry(byCount,RGB((BYTE)(byCount/fDecreaseFactor+0.5),
			(BYTE)(byCount/fDecreaseFactor+0.5),(BYTE)(byCount/fDecreaseFactor+0.5)));
	}
	BYTE** DibMatrix=GetImageMatrix();
	for(WORD wYCount=0; wYCount<m_lpBMIH->biHeight;wYCount++){
		for(WORD wXCount=0; wXCount<m_lpBMIH->biWidth;wXCount++){
			DibMatrix[wYCount][wXCount]
				= (BYTE)((float)DibMatrix[wYCount][wXCount]*fDecreaseFactor);
		}
	}
	for (byCount=255; byCount>(255-byColors);byCount--){
		SetPaletteEntry(byCount,RGB(def256pal_r[(byCount-1)],
			def256pal_g[(byCount-1)],def256pal_b[(byCount-1)]));
	}
	return TRUE;
}

BOOL CDib::Blur(BYTE byBlurFactor)
{	// blurs image with a mask of size (byBlurFactor*byBlurFactor)

	if (!DibLoaded() || (m_lpBMIH->biBitCount!=8)) {
		AfxMessageBox("Cannot blur image, source is not 8 bits per pixel");
		return FALSE;
	}
	CDib sourceDib=*this;
//[hansm 3.7.98] sourceDib.EnlargeCanvas(2*byBlurFactor,2*byBlurFactor);
	sourceDib.EnlargeCanvas(CRect(byBlurFactor,byBlurFactor,byBlurFactor,byBlurFactor));
	BYTE** sourceMatrix=sourceDib.GetImageMatrix();
	BYTE** destMatrix=GetImageMatrix();
	int nLocalXC,nLocalYC;
	WORD wSumPixels;
	WORD wNrOfPixels=(byBlurFactor*2+1)*(byBlurFactor*2+1);
	for(WORD wXCount=byBlurFactor; wXCount<(m_lpBMIH->biWidth+byBlurFactor);wXCount++){
		for(WORD wYCount=byBlurFactor; wYCount<(m_lpBMIH->biHeight+byBlurFactor);wYCount++){
			wSumPixels=0;
			for(nLocalXC=(wXCount-byBlurFactor);
			nLocalXC<=(wXCount+byBlurFactor);nLocalXC++){
				for(nLocalYC=(wYCount-byBlurFactor);
				nLocalYC<=(wYCount+byBlurFactor);nLocalYC++){
					wSumPixels += sourceMatrix[nLocalYC][nLocalXC];
				}
			}
			destMatrix[wYCount-byBlurFactor][wXCount-byBlurFactor]=wSumPixels/wNrOfPixels;
		}
	}
	return TRUE;
}

BOOL CDib::EnlargeCanvas(CRect addRect, BYTE newBorderVal)
{
	if (!DibLoaded() || !((m_lpBMIH->biBitCount==1) || (m_lpBMIH->biBitCount==8))) {
		AfxMessageBox("Enlarge canvas implemented for 1 & 8 bpp, only");
		return FALSE;
	}

	//save old image header values
	CSize dim = GetDimensions();
	LPBYTE pBitsSave = m_lpImage;
	int oldByteWidth = m_nByteWidth;

	//calculate and set new header entries
	m_lpBMIH->biWidth += addRect.TopLeft().x + addRect.BottomRight().x;
	m_lpBMIH->biHeight += addRect.BottomRight().y + addRect.TopLeft().y; //remember image are flipped
	m_nByteWidth = (m_lpBMIH->biBitCount*m_lpBMIH->biWidth+31)/32 * 4;
	m_dwSizeImage = m_lpBMIH->biSizeImage = m_nByteWidth*m_lpBMIH->biHeight;
	m_lpImage = (LPBYTE)new char[m_dwSizeImage];
	memset(m_lpImage,newBorderVal,m_dwSizeImage); //initialize bits
	
	//copy the bits
	LPBYTE pOldImage = pBitsSave;
	LPBYTE pImage = m_lpImage + m_nByteWidth*addRect.BottomRight().y; //remember image are flipped
	if (m_lpBMIH->biBitCount == 1) {
		#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
		newBorderVal = (newBorderVal&1 == WHITE)?255:0;
		int nXByteOldImage = dim.cx >> 3; //#bytes used by raster in source image
		int nShifts = addRect.TopLeft().x & 7; //our start bit
		pImage += addRect.TopLeft().x >> 3; //move to start byte
		for (int n=0; n<dim.cy; n++) {
			LPBYTE pIm=pImage,pOldIm=pOldImage;
			*pIm++ = MAKEWORD(*pOldIm,newBorderVal) >> nShifts; //first raster byte
			int m=0;
			while (m++ < nXByteOldImage + 1) {
				*pIm++ = (BYTE)((SWAPWORD(*(WORD*)pOldIm)) >> nShifts);
				pOldIm++;
			}
			//fix the zero padded bytes (at end of line)
			int zeroBits = 8 - ((addRect.TopLeft().x + dim.cx) & 7); //#zeros used for zero padding
			int endByte = ((addRect.TopLeft().x + dim.cx) >> 3) - (addRect.TopLeft().x >> 3);
			*(pImage+endByte)=HIBYTE(MAKEWORD(newBorderVal,*(pImage+endByte)>>zeroBits)<<zeroBits);
			if (8*endByte < m_lpBMIH->biWidth) *(pImage+endByte+1) = newBorderVal;

			//go to next raster
			pImage += m_nByteWidth;
			pOldImage += oldByteWidth;
		}
	}
	else if (m_lpBMIH->biBitCount == 8) {
		pImage += addRect.TopLeft().x;
		for (int n=0; n<dim.cy; n++) {
			memcpy(pImage,pOldImage,dim.cx);
			pImage += m_nByteWidth;
			pOldImage += oldByteWidth;
		}
	}

	//clean up memory
	delete pBitsSave; //free memory occupied by old image bits
	if (m_pImageMatrix) delete m_pImageMatrix; //matrix pointer array is now invalid
	m_pImageMatrix = NULL;
	return TRUE;
}

BOOL CDib::DrawLine(CPoint ptStart,CPoint ptEnd,DWORD color){
	if (!DibLoaded()) {
		AfxMessageBox("Can not draw line - no image loaded");
		return FALSE;
	}
	BOOL bDrawOutside=FALSE;
	int nWidth=m_lpBMIH->biWidth;
	int nHeight=m_lpBMIH->biHeight;
	if ((ptStart.x>=nWidth) || (ptStart.y>=nHeight) || (ptStart.x<0) || (ptStart.y<0) ||
		(ptEnd.x>=nWidth) || (ptEnd.y>=nHeight) || (ptEnd.x<0) || (ptEnd.y<0)){
		bDrawOutside=TRUE;
	}
	CPoint ptVector = (ptEnd-ptStart);
	CPoint Offs=ptStart; // offset
	// make sure the count direction is positive
	if (((abs(ptVector.x)>=abs(ptVector.y)) && (ptVector.x<0)) ||
        ((abs(ptVector.y)>abs(ptVector.x)) && (ptVector.y<0))){
		ptVector = (ptStart+(-ptEnd));
		Offs=ptEnd;
	}
	double dTheta=atan2((double)ptVector.y,(double)ptVector.x);
	if ((bDrawOutside) || (m_lpBMIH->biBitCount!=8)){ // always use SetPixel
		CPoint ptPlot;
		if (abs(ptVector.x)>=abs(ptVector.y)){
			// count in x-direction
			double dXFact=tan(dTheta);
			for(int nX=0; nX<=ptVector.x; nX++){
				ptPlot = CPoint(nX,(int) (nX*dXFact+((dXFact>0)?0.5:-0.5)));
				ptPlot += Offs; // Add offset
				SetPixel(ptPlot,color);
			}
		}
		else{ // count in y-direction
			double dXFact=tan(pi/2-dTheta);
			for(int nY=0; nY<=ptVector.y; nY++){
				ptPlot = CPoint((int) (nY*dXFact+((dXFact>0)?0.5:-0.5)),nY);
				ptPlot += Offs; // Add offset
				SetPixel(ptPlot,color);
			}
		}
	}
	else{ // bDrawOutside=FALSE and biBitCount=8 -> use matrix
		BYTE** pImage = GetImageMatrix();
		if (abs(ptVector.x)>=abs(ptVector.y)){
			// count in x-direction
			double dXFact=tan(dTheta);
			for(int nX=0; nX<=ptVector.x; nX++){
				pImage[(int) (nX*dXFact+((dXFact>0)?0.5:-0.5))+Offs.y][nX+Offs.x]=(BYTE) color;
			}
		}
		else{ // count in y-direction
			double dXFact=tan(pi/2-dTheta);
			for(int nY=0; nY<=ptVector.y; nY++){
				pImage[(nY+Offs.y)][(int) (nY*dXFact+((dXFact>0)?0.5:-0.5))+Offs.x]=(BYTE) color;
			}
		}
	}
	if (bDrawOutside){
		return FALSE; // one of the points were out of range
	}
	return TRUE; // both points inside image
}

BOOL CDib::Invert()
{
	if (!DibLoaded()) {
		AfxMessageBox("No image loaded");
		return FALSE;
	}

	int bytesUsed = (m_lpBMIH->biBitCount*m_lpBMIH->biWidth + 7) >> 3;
	LPBYTE pBits = m_lpImage;
	for (int n=0; n<m_lpBMIH->biHeight; n++) { //for each raster
		for (int m=0; m<bytesUsed; m++) { //for each used byte
			*pBits++ = ~*pBits;
		}
		pBits += m_nByteWidth - bytesUsed; //skip the bytes used by zero padding
	}
	return TRUE;
}

#ifdef _DEBUG
void CDib::TraceHeader()
{
	TRACE("BITMAPINFOHEADER:\n");
	if (DibLoaded()) {
		TRACE("\tbiSize:\t\t\t\t%d\n",m_lpBMIH->biSize);
		TRACE("\tbiWidth:\t\t\t%d\n",m_lpBMIH->biWidth);
		TRACE("\tbiHeight:\t\t\t%d\n",m_lpBMIH->biHeight);
		TRACE("\tbiPlanes:\t\t\t%d\n",m_lpBMIH->biPlanes);
		TRACE("\tbiBitCount:\t\t\t%d\n",m_lpBMIH->biBitCount);
		TRACE("\tbiCompression:\t\t%d\n",m_lpBMIH->biCompression);
		TRACE("\tbiSizeImage:\t\t%d\n",m_lpBMIH->biSizeImage);
		TRACE("\tbiXPelsPerMeter:\t%d\n",m_lpBMIH->biXPelsPerMeter);
		TRACE("\tbiYPelsPerMeter:\t%d\n",m_lpBMIH->biYPelsPerMeter);
		TRACE("\tbiClrUsed:\t\t\t%d\n",m_lpBMIH->biClrUsed);
		TRACE("\tbiClrImportant:\t\t%d\n",m_lpBMIH->biClrImportant);
		TRACE("\t(m_nByteWidth:\t\t%d)\n",m_nByteWidth);
		TRACE("\t(m_dwSizeImage:\t\t%d)\n",m_dwSizeImage);
	}
	else {
		TRACE("\tno image loaded.\n");
	}
}

void CDib::TracePalette()
{
	TRACE("BITMAP PALETTE:\n");
	if (DibLoaded()) {
		RGBQUAD* pColTable = (RGBQUAD*)m_lpvColorTable;
		for (int n=0; n<m_nColorTableEntries; n++) {
			TRACE("\t%d%-5cRGB=(%d,%d,%d)\n",n,':',
				pColTable->rgbBlue,pColTable->rgbGreen,pColTable->rgbRed);
			pColTable++;
		}
	}
	else {
		TRACE("\tno image loaded.\n");
	}
}
#endif

/*--------------------------------------------------------------------
   private helper functions
--------------------------------------------------------------------*/

void CDib::ComputePaletteSize(int nBitCount)
{
	if (m_lpBMIH==NULL || m_lpBMIH->biClrUsed == 0) {
		switch(nBitCount) {
			case 1:
				m_nColorTableEntries = 2;
				break;
			case 4:
				m_nColorTableEntries = 16;
				break;
			case 8:
				m_nColorTableEntries = 256;
				break;
			case 24:
				m_nColorTableEntries = 0;
				break;
			default:
				AfxMessageBox("CDib only supports 1, 8 and 24 bits per pixel");
				ASSERT(FALSE);
		}
	}
	else {
		m_nColorTableEntries = m_lpBMIH->biClrUsed;
	}
	ASSERT(m_nColorTableEntries <= 256); 
}

void CDib::ComputeMetrics()
{
	m_dwSizeImage = m_lpBMIH->biSizeImage;
	if(m_dwSizeImage == 0) {
		DWORD dwBytes = ((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) / 32;
		if(((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * m_lpBMIH->biHeight; // no compression
	}
	m_lpvColorTable = (LPBYTE) m_lpBMIH + sizeof(BITMAPINFOHEADER);
	m_nByteWidth = (((m_lpBMIH->biWidth)*(m_lpBMIH->biBitCount) + 31) / 32 * 4);
}

int CDib::FindSingleMin(const CDWordArray& Hist)
{	//If only one minimum (not including the ends) is found its intensity
	//is returned, otherwise -1 is returned

	int color=0, min;

	while ((color<m_nColorTableEntries-1) && (Hist[color]<=Hist[color+1])){
		color++;
	}
	while ((color<m_nColorTableEntries-1) && (Hist[color]>=Hist[color+1])){
		color++;
	}
	min = color;
	while ((color<m_nColorTableEntries-1) && (Hist[color]<=Hist[color+1])){
		color++;
	}
	while ((color<m_nColorTableEntries-1) && (Hist[color]>=Hist[color+1])){
		color++;
	}
	if (color < m_nColorTableEntries-1) { //more than one minimum
		min = -1;
	}
	return min;
}

void CDib::SmoothHist(CDWordArray& Hist, int width)
{	//Smoothes the histogram to remove local variations by changing each value
	//to the mean value of itself at its width-1 neighbours (width must be odd)

	CDWordArray NewHist;
	NewHist.SetSize(Hist.GetSize());
	int hw=(width-1)/2;

	for (int color=0; color<=m_nColorTableEntries-1; color++){
		DWORD sum=0;
		int count=0;
		for (int i=color-hw; i<=color+hw; i++){
			if ((i >= 0) && (i < m_nColorTableEntries)){
				sum += Hist[i];
				count++;
			}
		}
		NewHist[color] = sum/count;
	}
	Hist.Copy(NewHist);
}

CFloatPoint CDib::FromPoint(CPoint pt, double theta)
{	//finds point to get pixel in target-to-source rotation

	CFloatPoint fpt;

	fpt.x = (pt.x-m_fptRotOrigo.x)*cos(theta)-(pt.y-m_fptRotOrigo.y)*sin(theta)
		   + m_fptRotOrigo.x;
	fpt.y = (pt.x-m_fptRotOrigo.x)*sin(theta)+(pt.y-m_fptRotOrigo.y)*cos(theta)
		   + m_fptRotOrigo.y;
	return fpt;
}

CFloatPoint CDib::ToPoint(CPoint pt, double theta)
{	//finds where point maps to in source-to-target rotation

	CFloatPoint fpt;

	fpt.x = (pt.x-m_fptRotOrigo.x)*cos(theta)+(pt.y-m_fptRotOrigo.y)*sin(theta)
		   + m_fptRotOrigo.x;
	fpt.y = (m_fptRotOrigo.x-pt.x)*sin(theta)+(pt.y-m_fptRotOrigo.y)*cos(theta)
		   + m_fptRotOrigo.y;
	return fpt;
}

CRect CDib::CalcNewCanvas(const CSize& dim, double theta, BOOL enlarge)
{
	if (enlarge) {
		CFloatPoint ul = ToPoint(CPoint(0,0),theta);
		CFloatPoint ur = ToPoint(CPoint(dim.cx-1,0),theta);
		CFloatPoint lr = ToPoint(dim - CPoint(1,1),theta);
		CFloatPoint ll = ToPoint(CPoint(0,dim.cy-1),theta);
		CPoint ptMin(round(min(0,min(min(ul.x,ur.x),min(lr.x,ll.x)))),
					  round(min(0,min(min(ul.y,ur.y),min(lr.y,ll.y)))));
		CPoint ptMax(round(max(dim.cx-1,max(max(ul.x,ur.x),max(lr.x,ll.x)))),
					  round(max(dim.cy-1,max(max(ul.y,ur.y),max(lr.y,ll.y)))));
		return CRect(ptMin,ptMax);
	}
	else {
		return CRect(CPoint(0,0),dim - CPoint(1,1));
	}
}

void CDib::SetOrigo(BOOL centerOrigo)
{
	if (centerOrigo) {
		CSize dim = GetDimensions();
		m_fptRotOrigo.x=(double)(dim.cx-1)/2;
		m_fptRotOrigo.y=(double)(dim.cy-1)/2;
	}
	else {
		m_fptRotOrigo.x=(double)m_ptOrigo.x;
		m_fptRotOrigo.y=(double)m_ptOrigo.y;
	}
}

double CDib::GetBorderPixel(BYTE** pImage, int x, int y)
{
	CSize dim = GetDimensions();
	if ((x==dim.cx) && (y==dim.cy)) {
		return (double)pImage[y-1][x-1];
	}
	else if (x==dim.cx) {
		return (double)pImage[y][x-1];
	}
	else {
		return (double)pImage[y-1][x];
	}
}
