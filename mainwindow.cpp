#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cstdio>

#define PID_STREAM_TYPE_VIDEO_AVC       (0x1B)
#define DTV_MUX_PID_VIDEO               (0x1011)
#define PMT_STREAM_TYPE_IDX             (17)
#define DTV_MUX_PMT_SEC_LEN_IDX         (6)
#define DTV_MUX_STREAM_ID_VIDEO         (0xE0)
#define DTV_MUX_INVALID_PTS             (0xFFFFFFFFFFFFFFFFLL)
#define DTV_MUX_TS_SYNC_BYTE            (0x47)
#define DTV_MUX_PES_HEADER_LEN          (14)
#define DTV_MUX_PES_HEADER_LEN_WO_PTS	(9)
#define DTV_MUX_TS_PKT_SIZE				(188)
#define DTV_MUX_TS_PKT_HDR_SIZE			(4)

static unsigned char PAT[188] =
{
    0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0xB0, 0x0D, 0x00, 0x01, 0xC1, 0x00, 0x00, 0x00, 0x01, 0xE1,
    0x00, 0xE8, 0xF9, 0x5E, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static unsigned char PMT[188] =
{
    0x47, 0x41, 0x00, 0x10, 0x00, 0x02, 0xB0, 0x00, 0x00, 0x01, 0xC1, 0x00, 0x00, 0xFF, 0xFF, 0xF0,
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static unsigned int crc_table[256] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
    0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
    0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD, 0x4C11DB70, 0x48D0C6C7,
    0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3,
    0x709F7B7A, 0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
    0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58, 0xBAEA46EF,
    0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB,
    0xCEB42022, 0xCA753D95, 0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1,
    0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
    0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4,
    0x0808D07D, 0x0CC9CDCA, 0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE,
    0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08,
    0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC,
    0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6,
    0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A, 0xE0B41DE7, 0xE4750050,
    0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
    0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637,
    0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 0x4F040D56, 0x4BC510E1,
    0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5,
    0x3F9B762C, 0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
    0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E, 0xF5EE4BB9,
    0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD,
    0xCDA1F604, 0xC960EBB3, 0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7,
    0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
    0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2,
    0x470CDD2B, 0x43CDC09C, 0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
    0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E,
    0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A,
    0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0,
    0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C, 0xE3A1CBC1, 0xE760D676,
    0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
    0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,
    0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::parseVideoData(int dataSz)
{
    unsigned int PayloadSz;
    while(dataSz > 0)
    {
        pData = (ptr+1);
        PayloadSz = ((*(++ptr))<<24)|((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
        pData[0] = 0x0;
        pData[1] = 0x0;
        pData[2] = 0x0;
        pData[3] = 0x01;
        xEs2TsPacketizeEx(PayloadSz+4);
        ptr += PayloadSz;
    }
    ++ptr;

#if 0
    char pts[5];
    char stcode[4] = {0x0, 0x0, 0x0, 0x1};
    while(dataSz > 0)
    {
        unsigned int PayloadSz = ((*(++ptr))<<24)|((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
        std::cout<<"Payload Size: "<<PayloadSz<<std::endl;
        out.writeRawData((const char*)pes_header,9);
        {
            pts[0] = (0x2<<4|((_Timestamp&0xC0000000)>>29)|0x1)&(0xF7);
            pts[1] = (_Timestamp&0x3FC00000)>>22;
            pts[2] = ((_Timestamp&0x3F8000)>>15<<1)|0x1;
            pts[3] = (_Timestamp&0x7F80)>>14;
            pts[4] = ((_Timestamp&0x7F)<<1)|0x1;
            out.writeRawData((const char*)pts,5);
        }
        out.writeRawData((const char*)stcode,4);
        ++ptr;
        out.writeRawData((const char*)ptr,PayloadSz);
        ptr += (PayloadSz-1);

        dataSz = dataSz - PayloadSz - 4;
        std::cout<<"Data Remain Size: "<<dataSz<<std::endl;
    }
    ++ptr;
#endif
}

unsigned int GL_CRC32(unsigned char *pBuffer, unsigned int dSize)
{
    register unsigned int i;
    unsigned int crc = 0xFFFFFFFF;

    for (i = 0; i < dSize; i++) {
        crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *pBuffer++) & 0xFF];
    }

    return crc;
}

void MainWindow::generatePSI()
{
    //PAT
    memcpy(outbuf_cur, PAT, 188);
    unsigned char* pPAT = outbuf_cur;
    pPAT[3] = *pCC;
    *pCC = (*pCC + 1) & 0xF;
    outbuf_cur += 188;


    //PMT
    memcpy(outbuf_cur, PMT, 188);
    unsigned char* pPMT = outbuf_cur;
    PMT[3] = *pCC;
    *pCC = (*pCC + 1) & 0xF;

    int dIdx = PMT_STREAM_TYPE_IDX;
    pPMT[dIdx++] = PID_STREAM_TYPE_VIDEO_AVC;
    pPMT[dIdx++] = 0xE0 | ((DTV_MUX_PID_VIDEO >> 8) & 0x1F);
    pPMT[dIdx++] = DTV_MUX_PID_VIDEO & 0xFF;
    pPMT[dIdx++] = 0xF0;
    pPMT[dIdx++] = 0x00;

    int dSecLen = dIdx + 4 - 8;
    pPMT[DTV_MUX_PMT_SEC_LEN_IDX] = 0xB0 | ((dSecLen & 0xF00) >> 8);
    pPMT[DTV_MUX_PMT_SEC_LEN_IDX+1] = dSecLen & 0xFF;

    unsigned int dCrc32 = GL_CRC32(&(pPMT[5]), dIdx - 5);
    pPMT[dIdx++] = (dCrc32 >> 24) & 0xFF;
    pPMT[dIdx++] = (dCrc32 >> 16) & 0xFF;
    pPMT[dIdx++] = (dCrc32 >> 8) & 0xFF;
    pPMT[dIdx++] = dCrc32 & 0xFF;
    outbuf_cur += 188;
}

void MainWindow::xEs2TsPacketizeEx(unsigned int dChunkSize)
{
    unsigned char bPUSI = 1;
    unsigned char bAFCtrl = 0;
    int dIdx = 0;
    unsigned int dCopyLen = 0;
    unsigned char* pBuf = outbuf_cur;

    unsigned int pid = DTV_MUX_PID_VIDEO;
    unsigned int bStreamID = DTV_MUX_STREAM_ID_VIDEO;
    int dPesHdrLen = (dPTS != DTV_MUX_INVALID_PTS) ? DTV_MUX_PES_HEADER_LEN : DTV_MUX_PES_HEADER_LEN_WO_PTS;
    bAFCtrl = dChunkSize >= (DTV_MUX_TS_PKT_SIZE - DTV_MUX_TS_PKT_HDR_SIZE - (bPUSI ? dPesHdrLen : 0)) ? 0x1 : 0x3;

    while (dChunkSize > 0)
    {
        dIdx = 0;
        pBuf[dIdx++] = DTV_MUX_TS_SYNC_BYTE;
        pBuf[dIdx++] = (((bPUSI) ? 0x40 : 0x0) | (pid >> 8)) & 0xFF;
        pBuf[dIdx++] = (pid & 0xFF);
        pBuf[dIdx++] = ((bAFCtrl << 4) & 0xF0) | (*pCC & 0xF);
        *pCC = (*pCC + 1) & 0xF;

        if (bAFCtrl & 0x2)
        {
            unsigned char bAFLen = DTV_MUX_TS_PKT_SIZE - DTV_MUX_TS_PKT_HDR_SIZE - dChunkSize - 1;

            bAFLen -= bPUSI ? dPesHdrLen : 0;
            pBuf[dIdx++] = bAFLen & 0xFF;

            if (bAFLen > 0)
            {
                pBuf[dIdx++] = 0;
                bAFLen -= 1;
                if (bAFLen)
                {
                    memset(pBuf+6, 0xFF, bAFLen);
                    dIdx += bAFLen;
                }
            }
        }

        if (bPUSI) // add PES header
        {
            int dPesLen = dChunkSize + ((dPTS != DTV_MUX_INVALID_PTS) ? 8 : 3);
            pBuf[dIdx++] = 0x00;
            pBuf[dIdx++] = 0x00;
            pBuf[dIdx++] = 0x01;
            pBuf[dIdx++] = bStreamID;
            if (dPesLen <= 65535)
            {
                pBuf[dIdx++] = (dPesLen & 0xFF00) >> 8;
                pBuf[dIdx++] = (dPesLen) & 0xFF;
            }
            else
            {
                pBuf[dIdx++] = 0x0;
                pBuf[dIdx++] = 0x0;
            }

            pBuf[dIdx++] = 0x80;

            if (dPTS != DTV_MUX_INVALID_PTS)
            {
                pBuf[dIdx++] = 0x80;	// PTS_DTS_flags = 10b
                pBuf[dIdx++] = 0x05;	// PES_header_data_length

                //BYTE *pPTS = &(pBuf[dIdx]);
                pBuf[dIdx++] = 0x20 | ((dPTS & 0x100000000LL) ? 0x8 : 0x0) | (((dPTS & 0xC0000000) >> 30) << 1) | 0x1;	// PTS[32..30]
                pBuf[dIdx++] = ((dPTS & 0x3FC00000) >> 22);
                pBuf[dIdx++] = (((dPTS & 0x3F8000) >> 15) << 1) | 0x1;
                pBuf[dIdx++] = ((dPTS & 0x7F80) >> 7);
                pBuf[dIdx++] = ((dPTS & 0x7F) << 1) | 0x1;
            }
            else
            {
                pBuf[dIdx++] = 0x00;	// PTS_DTS_flags = 00b
                pBuf[dIdx++] = 0x00;	// PES_header_data_length
            }

            if ((dExtHdrSz) && (pExtHdr != NULL))
            {
                memcpy(pBuf+dIdx, pExtHdr, dExtHdrSz);
                dChunkSize -= dExtHdrSz;
                dIdx += dExtHdrSz;
            }

            bPUSI = 0;
        }

        dCopyLen = DTV_MUX_TS_PKT_SIZE - dIdx;

        if (dCopyLen) // copy video es data
        {
            memcpy(pBuf, pData, dCopyLen);
            pData += dCopyLen;
        }
        dChunkSize -= dCopyLen;
    }
}

void MainWindow::parseAVCDecoderConfigurationRecord()
{
    generatePSI();

    unsigned int Version = *(++ptr);
    unsigned int Profile = *(++ptr);
    unsigned int ProfileCompact = *(++ptr);
    unsigned int Level = *(++ptr);
    unsigned int NumBytePresentNaluSize = *(++ptr);
    NumBytePresentNaluSize &= 0x3;
    NumBytePresentNaluSize++;

    //SPS
    unsigned int SpsNum = *(++ptr);
    SpsNum &= 0x1F;
    unsigned int SpsSize = (*(++ptr))<<8|(*(++ptr));
    std::cout<<"SPS Size: "<<SpsSize<<std::endl;
    //out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to SPS NALU Type
    //out.writeRawData((const char*)ptr,SpsSize);
    pbSPS[0] = 0x0;
    pbSPS[1] = 0x0;
    pbSPS[2] = 0x0;
    pbSPS[3] = 0x01;
    memcpy(pbSPS+4, ptr, SpsSize);
    dExtHdrSz = SpsSize+4;
    pExtHdr = pbSPS;
    xEs2TsPacketizeEx(dExtHdrSz);
    ptr += SpsSize;


    //PPS
    unsigned int PpsNum = *ptr;
    unsigned int PpsSize = (*(++ptr))<<8|(*(++ptr));
    //out.writeRawData((const char*)tsHeader,4);
    std::cout<<"PPS Size: "<<PpsSize<<std::endl;
    //out.writeRawData((const char*)seq_pes_header,13);
    ++ptr; //point to PPS NALU Type
    //out.writeRawData((const char*)ptr,PpsSize);
    pbPPS[0] = 0x0;
    pbPPS[1] = 0x0;
    pbPPS[2] = 0x0;
    pbPPS[3] = 0x01;
    memcpy(pbPPS+4, ptr, SpsSize);
    dExtHdrSz = PpsSize+4;
    pExtHdr = pbPPS;
    xEs2TsPacketizeEx(dExtHdrSz);
    ptr += PpsSize;

}

void MainWindow::parseFlvSHeader(QDataStream &in)
{
    bsbuf  = new unsigned char[6<<20];
    ptr = bsbuf;
    outbuf_head  = new unsigned char[10<<20];
    outbuf_cur = outbuf_head;

    in.readRawData((char*)bsbuf, 6<<20);
    _CC = 0;
    pCC = &_CC;

    // FLV
    if(bsbuf[0] == 0x46 && bsbuf[1] == 0x4C && bsbuf[2] == 0x56)
        ui->textBrowser->append("I'm FLV!!");
    else
        ui->textBrowser->append("NOT FLV GG!!");

    // Header Size
    unsigned int DataOffset = (bsbuf[5]<<24)|(bsbuf[6]<<16)|(bsbuf[7]<<8)|(bsbuf[8]);
    QString str = QString::number(DataOffset);
    ui->textBrowser->append("DataOffset = "+str);

    ptr += 9; // Header
    ptr += 4; // PreviousTagSize0 always be 0

    int n = 0;
    while(n < 500)
    {
        if(*ptr == 18)
        {
            ui->textBrowser->append("\nScript Data!");
            unsigned int scriptDataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(scriptDataSz));
            ptr += 8 + scriptDataSz;
        }
        else if(*ptr == 9)
        {
            ui->textBrowser->append("\nVideo Data!");
            unsigned int DataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(DataSz));

            dPTS = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr))|((*(++ptr))<<24);
            ui->textBrowser->append("Timestamp: " + QString::number(dPTS));

            unsigned int StreamID = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("StreamID: " + QString::number(StreamID));

            unsigned int FrameType = (*(++ptr))&0xF0;
            unsigned int CodecID = (*ptr)&0xF;
            unsigned int AvcPacketType = *(++ptr);
            unsigned int CompositionTime = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            if(AvcPacketType == 0) // sequence
                parseAVCDecoderConfigurationRecord();
            else if(AvcPacketType == 1) // NALU
                parseVideoData(DataSz-5);
            //ptr += 8 + DataSz;
        }
        else if(*ptr == 8)
        {
            ui->textBrowser->append("\nAudio Data!");
            unsigned int DataSz = ((*(++ptr))<<16)|((*(++ptr))<<8)|(*(++ptr));
            ui->textBrowser->append("Data size: " + QString::number(DataSz));
            ptr += 8 + DataSz;
        }
        ptr += 4;
        n++;
    }


}

void MainWindow::open(QString& FileName)
{
    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    QFile outFile(FileName+".ts");
    outFile.open(QIODevice::WriteOnly);
    QDataStream out(&outFile);


    parseFlvSHeader(in);



}

void MainWindow::on_pushButton_clicked()
{
    QString FileName;
    FileName = QFileDialog::getOpenFileName(this, "Open a Transport Stream File");
    if(!FileName.isEmpty())
    {
        ui->lineEdit->setText(FileName);
        ui->lineEdit_2->setText(FileName+".ts");
    }

    open(FileName);

}
